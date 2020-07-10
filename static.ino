/*|----------------------------------------------------------|*/
/*|HTTP webserver - FORM - HTML - PROCESSING - EEPROM        |*/
/*|AUTHOR: Martin Chlebovec                                  |*/
/*|EMAIL: martinius96@gmail.com                              |*/
/*|WEBSITE: https://arduino.php5.sk                          |*/
/*|----------------------------------------------------------|*/
#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensorsA(&oneWire);
const int rele = 6;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //physical mac address
byte ip[] = { 192, 168, 4, 1 }; // STATICKA IP V LAN SIETI
byte subnet[] = { 255, 255, 255, 0 }; //subnet mask
EthernetServer server(80); //server port
const char terminator1[2] = " ";
const char terminator3[2] = "?";
const char terminator4[2] = "&";
const char terminator5[2] = "=";
unsigned long cas = 0;
float teplota;
String stav;

void writeString(char add, String data)
{
  int _size = data.length();
  int i;
  for (i = 0; i < _size; i++)
  {
    EEPROM.write(add + i, data[i]);
  }
  EEPROM.write(add + _size, '\0'); //Add termination null character for String Data
}


String read_String(char add)
{
  int i;
  char data[100]; //Max 100 Bytes
  int len = 0;
  unsigned char k;
  k = EEPROM.read(add);
  while (k != '\0' && len < 500) //Read until null character
  {
    k = EEPROM.read(add + len);
    data[len] = k;
    len++;
  }
  data[len] = '\0';
  return String(data);
}

void setup() {
  String a = read_String(10);
  String b = read_String(100);
  if (a == "" || a == NULL) {
    writeString(10, "0.25");
  }
  if (b == "" || b == NULL) {
    writeString(100, "20.25");
  }
  sensorsA.begin();
  pinMode(rele, OUTPUT);
  digitalWrite(rele, HIGH);
  Serial.begin(115200);
  sensorsA.requestTemperatures();
  delay(2000);
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.println("Ready");
  Serial.println("Ethernet shield na IP:");
  Serial.println(Ethernet.localIP());
}

void loop() {
  if ((millis() - cas) >= 10000 || cas == 0) {
    cas = millis();
    teplota = sensorsA.getTempCByIndex(0);
    String referencia = read_String(10);
    String hystereza = read_String(100);
    float referencia_teplota = referencia.toFloat();
    float hystereza_teplota = hystereza.toFloat();
    float minus_hystereza_teplota = (-1 * hystereza_teplota);
    float rozdiel = referencia_teplota - teplota;
    if (rozdiel > hystereza_teplota) {
      Serial.println("RELE ZAP");
      stav = "ZAP";
      digitalWrite(rele, LOW);
    } else if (rozdiel < minus_hystereza_teplota) {
      Serial.println("RELE VYP");
      stav = "VYP";
      digitalWrite(rele, HIGH);
    }
    sensorsA.requestTemperatures();
  }
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        char str[line.length() + 1];
        line.toCharArray(str, line.length());
        char *method;
        char *request;
        method = strtok(str, terminator1);
        request = strtok(NULL, terminator1);
        if (String(request) == "/") {
          //HLAVNA ROOT HTTP STRANKA
          client.println(F("HTTP/1.1 200 OK"));
          client.println(F("Content-Type: text/html"));
          client.println();
          client.println(F("<!DOCTYPE html>"));
          client.println(F("<html>"));
          client.println(F("<head>"));
          client.println(F("<meta charset='utf-8'>"));
          client.println(F("<meta name='author' content='Martin Chlebovec'>"));
          client.println(F("<meta http-equiv='Refresh' content='10'; />"));
          client.println(F("<meta name='viewport' content='width=device-width, initial-scale=1'>"));
          client.println(F("<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/css/bootstrap.min.css'>"));
          client.println(F("<script type='text/javascript'>"));
          client.println(F("var timeleft = 10;"));
          client.println(F("var downloadTimer = setInterval(function(){"));
          client.println(F("if(timeleft <= 0){"));
          client.println(F("clearInterval(downloadTimer);"));
          client.println(F("document.getElementById(\"countdown\").innerHTML = \"Reštart...\";"));
          client.println(F("} else {"));
          client.println(F("document.getElementById(\"countdown\").innerHTML = timeleft + \" sekúnd do reštartu\";"));
          client.println(F("}"));
          client.println(F("timeleft -= 1;"));
          client.println(F("}, 1000);"));
          client.println(F("</script>"));
          client.println(F("<title>HTTP webserver - Arduino + Ethernet</title>"));
          client.println(F("</head>"));
          client.println(F("<body>"));
          client.println(F("<center><h3>Zadajte dáta pre termostat (budú uložené do EEPROM):</h3>"));
          client.println(F("<form action='/action.html' method='get'>"));
          client.println("<b>Referenčná teplota:</b><br><input type='number' id='fname' name='fname' min='5' max='50' step='0.25' value=" + read_String(10) + "><br>");
          client.println("<b>Hysteréza:</b><br><input type='number' id='fname2' name='fname2' min='0' max='10' step='0.25' value=" + read_String(100) + "><br>");
          client.println(F("<input type='submit' class='btn btn-success' value='Zapísať'>"));
          client.println(F("</form><hr>"));
          if (stav == "ZAP") {
            client.println(F("<b><font color='green'>Výstup: Zapnutý</font></b>"));
          }
          if (stav == "VYP") {
            client.println(F("<b><font color='red'>Výstup: Vypnutý</font></b>"));
          }
          client.println(F("<div id=\"countdown\"></div>"));
          client.print(F("Aktuálna teplota senzora DS18B20: "));
          client.print(teplota);
          client.println(F(" °C"));
          client.println(F("<h3>Autor: Martin Chlebovec - martinius96@gmail.com - https://arduino.php5.sk</h3>"));
          client.println(F("<h4>Verzia free-1.0.0 build 10. Jul 2020</h4>"));
          client.println(F("</center>"));
          client.println(F("</body>"));
          client.println(F("</html>"));
          delay(1);
          client.stop();
          client.flush();
        } else if (String(request) == "/get_data/") {
          //PODSTRANKA PRE VYCITANIE DAT (INYM MIKROKONTROLEROM)
          client.println(F("HTTP/1.1 200 OK"));
          client.println(F("Content-Type: text/html"));
          client.println();
          client.println(F("<!DOCTYPE html>"));
          client.println(F("<html>"));
          client.println(F("<head>"));
          client.println(F("<meta charset='utf-8'>"));
          client.println(F("<meta name='viewport' content='width=device-width, initial-scale=1'>"));
          client.println(F("<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/css/bootstrap.min.css'>"));
          client.println(F("<title>HTTP webserver - Arduino + Ethernet</title>"));
          client.println(F("</head>"));
          client.println(F("<body>"));
          client.println("<li>" + read_String(10) + "</li>");
          client.println("<li>" + read_String(100) + "</li>");
          client.println("</body>");
          client.println("</html>");
          delay(1);
          client.stop();
          client.flush();
        } else if (String(request) == "/favicon.ico") { //fix chybajuceho faviconu
          client.stop();
        } else {
          String myString = String(request);
          if (myString.startsWith("/action.html")) {
            char* parameter;
            char* value;
            char* hodnota1;
            char* hodnota2;
            parameter = strtok(request, terminator3);
            Serial.println(parameter);
            value = strtok(NULL, terminator3);
            hodnota1 = strtok(value, terminator4);
            hodnota2 = strtok(NULL, terminator4);
            char* H_1;
            char* H_2;
            strtok(hodnota1, terminator5);
            H_1 = strtok(NULL, terminator5);
            strtok(hodnota2, terminator5);
            H_2 = strtok(NULL, terminator5);
            writeString(10, String(H_1));
            writeString(100, String(H_2));
            client.println(F("HTTP/1.1 200 OK"));
            client.println(F("Content-Type: text/html"));
            client.println();
            client.println(F("<!DOCTYPE html>"));
            client.println(F("<html>"));
            client.println(F("<head>"));
            client.println(F("<meta charset='utf-8'>"));
            client.println(F("<meta http-equiv='Refresh' content='5; url=/' />"));
            client.println(F("<title>HTTP webserver - Arduino + Ethernet</title>"));
            client.println(F("</head>"));
            client.println(F("<body>"));
            client.println(F("<center><h3>Server prijal data z formulára:</h3>"));
            client.println("<li><b>Referenčná teplota: </b>" + String(H_1) + "</li>");
            client.println("<li><b>Hysteréza: </b>" + String(H_2) + "</li>");
            client.println(F("<b>Presmerovanie... Prosim cakajte</b></center>"));
            client.println(F("</body>"));
            client.println(F("</html>"));
            delay(1);
            client.stop();
            client.flush();
          }
        }
      }
    }
  }
}
