/*|-----------------------------------------------------------|*/
/*|HTTP webserver - Ethernet termostat - ESP32 + PHY LAN8720A |*/
/*|Kompatibilný ESPlan z Laskakitu                            |*/
/*|Autor: Martin Chlebovec                                    |*/
/*|EMAIL: martinius96@gmail.com                               |*/
/*|DONATE: paypal.me/chlebovec                                |*/
/*|Arduino Core 3.0.10 (December 2024                         |*/
/*|-----------------------------------------------------------|*/

#define ETH_PHY_TYPE ETH_PHY_LAN8720
#define ETH_PHY_ADDR  0
#define ETH_PHY_MDC   23
#define ETH_PHY_MDIO  18
#define ETH_PHY_POWER -1
#define ETH_CLK_MODE  ETH_CLOCK_GPIO17_OUT

#define ETH_NRST_PIN 5 //FOR RESETTING

#include <ETH.h>
#include <WebServer.h>
#include <ESPmDNS.h>
const char *host = "esplan-termostat";
WebServer server(80);
static bool eth_connected = false;
#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 32 //GPIO32
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensorsA(&oneWire);
const int rele = 33; //GPIO33
unsigned long cas = 0;
String stav = "VYP";
float teplota;
long day = 86400000; // 86400000 milliseconds in a day
long hour = 3600000; // 3600000 milliseconds in an hour
long minute = 60000; // 60000 milliseconds in a minute
long second =  1000; // 1000 milliseconds in a second
float rezim;

void onEvent(arduino_event_id_t event) {
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      Serial.println("ETH Started");
      // The hostname must be set after the interface is started, but needs
      // to be set before DHCP, so set it from the event handler thread.
      ETH.setHostname("esp32-ethernet");
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected");
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.print("ETH MAC: ");
      Serial.print(ETH.macAddress());
      Serial.print(", IPv4: ");
      Serial.print(ETH.localIP());
      if (ETH.fullDuplex()) {
        Serial.print(", FULL_DUPLEX");
      }
      Serial.print(", ");
      Serial.print(ETH.linkSpeed());
      Serial.println("Mbps");
      eth_connected = true;
      if (MDNS.begin(host))
      {
        MDNS.addService("http", "tcp", 80);
        Serial.println("MDNS responder started");
        Serial.print("You can now connect to http://");
        Serial.print(host);
        Serial.println(".local");
      }
      break;
    case ARDUINO_EVENT_ETH_LOST_IP:
      Serial.println("ETH Lost IP");
      eth_connected = false;
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      eth_connected = false;
      break;
    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      eth_connected = false;
      break;
    default: break;
  }
}


boolean isFloat(String tString) {
  String tBuf;
  boolean decPt = false;

  if (tString.charAt(0) == '+' || tString.charAt(0) == '-') tBuf = &tString[1];
  else tBuf = tString;

  for (int x = 0; x < tBuf.length(); x++)
  {
    if (tBuf.charAt(x) == '.' || tBuf.charAt(x) == ',') {
      if (decPt) return false;
      else decPt = true;
    }
    else if (tBuf.charAt(x) < '0' || tBuf.charAt(x) > '9') return false;
  }
  return true;
}

void writeString(char add, float data)
{
  EEPROM.put(add, (data * 1000));
  EEPROM.commit();
}


float read_String(char add)
{
  float payload = 0;
  float data = EEPROM.get(add, payload);
  return (data / 1000);
}

void handleRoot() {
  int days = millis() / day ;                                //number of days
  unsigned int hours = (millis() % day) / hour;                       //the remainder from days division (in milliseconds) divided by hours, this gives the full hours
  unsigned int minutes = ((millis() % day) % hour) / minute ;         //and so on...
  unsigned int seconds = (((millis() % day) % hour) % minute) / second;
  String stranka = F("<!DOCTYPE html>");
  stranka += F("<html>");
  stranka += F("<head>");
  stranka += F("<meta charset='utf-8'>");
  stranka += F("<meta name='author' content='Martin Chlebovec'>");
  stranka += F("<meta http-equiv='Refresh' content='30'; />");
  stranka += F("<meta name='viewport' content='width=device-width, initial-scale=1'>");
  stranka += F("<link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-QWTKZyjpPEjISv5WaRU9OFeRpok6YctnYmDr5pNlyT2bRjXh0JMhjY6hW+ALEwIH\" crossorigin=\"anonymous\">");
  stranka += F("<script src=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/js/bootstrap.bundle.min.js\" integrity=\"sha384-YvpcrYf0tY3lHB60NNkmXc5s9fDVZLESaAA55NDzOxhy9GkcIdslK1eN7N6jIeHz\" crossorigin=\"anonymous\"></script>");
  stranka += F("<script type='text/javascript'>");
  stranka += F("var timeleft = 30;");
  stranka += F("var downloadTimer = setInterval(function(){");
  stranka += F("if(timeleft <= 0){");
  stranka += F("clearInterval(downloadTimer);");
  stranka += F("document.getElementById(\"countdown\").innerHTML = \"Reštart...\";");
  stranka += F("} else {");
  stranka += F("document.getElementById(\"countdown\").innerHTML = timeleft + \" sekúnd do refreshu\";");
  stranka += F("}");
  stranka += F("timeleft -= 1;");
  stranka += F("}, 1000);");
  stranka += F("</script>");
  stranka += F("<title>PHY Ethernet termostat - ESP32</title>");
  stranka += F("</head>");
  stranka += F("<body>");
  stranka += F("<center><h3>ESPlan - Ethernet termostat (Bootstrap 5.3.3):</h3>");
  if (rezim == 0.00) {
    stranka += F("<form action='/action.html' method='post'>");
    stranka += "<b>Referenčná teplota:</b><br><input type='text' id='fname' name='fname' min='5' max='50' step='0.25' value=" + String(read_String(10)) + "><br>";
    stranka += "<b>Hysteréza:</b><br><input type='text' id='fname2' name='fname2' min='0' max='10' step='0.25' value=" + String(read_String(100)) + "><br>";
    stranka += F("<input type='submit' class='btn btn-success' value='Zapísať'>");
    stranka += F("</form>");
    stranka += F("<a href='manual.html' class='btn btn-primary' role='button'>Manuálny režim</a><hr>");
  } else if (rezim == 1.00) {
    if (stav == "ZAP") {
      stranka += F("<a href='vyp.html' class='btn btn-danger' role='button'>Vypnúť</a><br>");
    }
    if (stav == "VYP") {
      stranka += F("<a href='zap.html' class='btn btn-success' role='button'>Zapnúť</a><br>");
    }
    stranka += F("<a href='automat.html' class='btn btn-primary' role='button'>Automatický režim</a><hr>");
  }
  if (stav == "ZAP") {
    stranka += F("<b><font color='green'>Výstup: Zapnutý</font></b>");
  }
  if (stav == "VYP") {
    stranka += F("<b><font color='red'>Výstup: Vypnutý</font></b>");
  }
  stranka += F("<div id=\"countdown\"></div>");
  stranka += F("<b>Aktuálna teplota senzora DS18B20:</b> ");
  stranka += String(teplota);
  stranka += F(" °C");
  stranka += F("<hr>");
  stranka += F("<b>Uptime: </b>");
  stranka += String(days);
  stranka += F("d");
  stranka += F(" ");
  stranka += String(hours);
  stranka += F("h");
  stranka += F(" ");
  stranka += String(minutes);
  stranka += F("m");
  stranka += F(" ");
  stranka += String(seconds);
  stranka += F("s");
  stranka += F("</center>");
  stranka += F("</body>");
  stranka += F("</html>");
  server.send(200, "text/html", stranka);
}

void handleBody() {
  if (server.hasArg("fname")) {
    String target_temp = server.arg("fname");
    // float cielova_teplota = target_temp.toFloat();

    if (isFloat(target_temp)) {
      float cielova_teplota = target_temp.toFloat();
      writeString(10, cielova_teplota);
    } else {
      Serial.println(F("Do input pola cielovej teploty nebolo vlozene cislo!"));
      Serial.println(F("Zapis zakazany!"));
    }
  }
  if (server.hasArg("fname2")) {
    String hysteresis = server.arg("fname2");
    if (isFloat(hysteresis)) {
      float hystereza = hysteresis.toFloat();
      writeString(100, hystereza);
    } else {
      Serial.println(F("Do input pola hysterezy nebolo vlozene cislo!"));
      Serial.println(F("Zapis zakazany!"));
    }
  }
  String stranka = F("<!DOCTYPE html>");
  stranka += F("<html>");
  stranka += F("<head>");
  stranka += F("<meta charset='utf-8'>");
  stranka += F("<meta http-equiv='Refresh' content='5; url=/' />");
  stranka += F("<title>Ethernet termostat - ESP32 - spracovanie riadiach dát</title>");
  stranka += F("</head>");
  stranka += F("<body>");
  stranka += F("<center><h3>Server prijal data z formulára:</h3>");
  stranka += "<li><b>Referenčná teplota: </b>" + String(read_String(10)) + " °C</li>";
  stranka += "<li><b>Hysteréza: </b>" + String(read_String(100)) + " °C</li>";
  stranka += F("<b>Presmerovanie... Prosím čakajte</b></center>");
  stranka += F("</body>");
  stranka += F("</html>");
  server.send(200, "text/html", stranka);
}

void handleGet() {
  String stranka = "{\n";
  stranka += F("\"Hysteresis\":");
  stranka += String(read_String(100));
  stranka += F(",\n");
  stranka += F("\"Target_Temperature\":");
  stranka += String(read_String(10));
  stranka += F(",\n");
  stranka += F("\"Actual_Temperature\":");
  stranka += String(teplota) + "\n";
  stranka += F("}\n");
  server.send(200, "application/json", stranka);
}

void handleZAP() {
  stav = "ZAP";
  digitalWrite(rele, LOW);
  String stranka = F("<!DOCTYPE html>");
  stranka += F("<html>");
  stranka += F("<head>");
  stranka += F("<meta charset='utf-8'>");
  stranka += F("<meta http-equiv='Refresh' content='0; url=/' />");
  stranka += F("</head>");
  stranka += F("</html>");
  server.send(200, "text/html", stranka);
}

void handleAuto() {
  writeString(150, 0.00);
  rezim = read_String(150);
  String stranka = F("<!DOCTYPE html>");
  stranka += F("<html>");
  stranka += F("<head>");
  stranka += F("<meta charset='utf-8'>");
  stranka += F("<meta http-equiv='Refresh' content='0; url=/' />");
  stranka += F("</head>");
  stranka += F("</html>");
  server.send(200, "text/html", stranka);
}
void handleManual() {
  writeString(150, 1.00);
  rezim = read_String(150);
  String stranka = F("<!DOCTYPE html>");
  stranka += F("<html>");
  stranka += F("<head>");
  stranka += F("<meta charset='utf-8'>");
  stranka += F("<meta http-equiv='Refresh' content='0; url=/' />");
  stranka += F("</head>");
  stranka += F("</html>");
  server.send(200, "text/html", stranka);
}
void handleVYP() {
  stav = "VYP";
  digitalWrite(rele, HIGH);
  String stranka = F("<!DOCTYPE html>");
  stranka += F("<html>");
  stranka += F("<head>");
  stranka += F("<meta charset='utf-8'>");
  stranka += F("<meta http-equiv='Refresh' content='0; url=/' />");
  stranka += F("</head>");
  stranka += F("</html>");
  server.send(200, "text/html", stranka);
}
void setup() {
  Serial.begin(115200);
#if defined(ETH_NRST_PIN)
  pinMode(ETH_NRST_PIN, OUTPUT);
  digitalWrite(ETH_NRST_PIN, LOW);
  delay(500);
  digitalWrite(ETH_NRST_PIN, HIGH);
#endif
  Network.onEvent(onEvent);
  ETH.begin();
  delay(5000);
  EEPROM.begin(512);  //Initialize EEPROM
  float a = read_String(10);
  float b = read_String(100);
  float c = read_String(150);
  if (isnan(a)) {
    writeString(10, 20.25);
  }
  if (isnan(b)) {
    writeString(100, 0.25);
  }
  if (isnan(c)) {
    writeString(150, 0.00);
  }
  sensorsA.begin();
  pinMode(rele, OUTPUT);
  digitalWrite(rele, HIGH);
  sensorsA.requestTemperatures();
  delay(750);
  Serial.println(F("Ethernet termostat - Autor: Martin Chlebovec"));
  server.on("/", handleRoot);
  server.on("/get_data.json", handleGet);
  server.on("/automat.html", handleAuto);
  server.on("/manual.html", handleManual);
  server.on("/zap.html", handleZAP);
  server.on("/vyp.html", handleVYP);
  server.on("/action.html", HTTP_POST, handleBody);
  server.begin();
}

void loop() {
  if ((millis() - cas) >= 10000 || cas == 0) {
    cas = millis();
    teplota = sensorsA.getTempCByIndex(0);
    Serial.println();
    Serial.println(F("----------------------------------------------"));
    Serial.print(F("IP addresa ESP32 termostat: "));
    Serial.print(ETH.localIP());
    Serial.print(F(", pre pristup k termostatu navstivte http://"));
    Serial.print(ETH.localIP());
    Serial.println(F("/"));
    Serial.print("Alebo cez mDNS zaznam http://");
    Serial.print(host);
    Serial.println(".local");
    Serial.print(F("Free HEAP: "));
    Serial.print(ESP.getFreeHeap());
    Serial.println(F(" B"));
    Serial.print(F("Aktuálna teplota: "));
    Serial.print(String(teplota));
    Serial.println(F(" °C"));
    sensorsA.requestTemperatures();
    rezim = read_String(150);
    if (rezim == 0.00) {
      float cielova_teplota = read_String(10);
      float  hystereza = read_String(100);
      float minus_hystereza_teplota = (-1 * hystereza);
      float rozdiel = cielova_teplota - teplota; //21 - 20
      if (rozdiel > hystereza) {
        Serial.println(F("Kotol zapnuty"));
        stav = "ZAP";
        digitalWrite(rele, LOW);
      } else if (rozdiel < minus_hystereza_teplota) {
        Serial.println(F("Kotol vypnuty"));
        stav = "VYP";
        digitalWrite(rele, HIGH);
      } else {
        Serial.println(F("Rozdiel cielovej a aktuálnej teploty nie je nad, ani pod hysterezou. Stav vystupu sa nemeni."));
        Serial.print(F("Aktualny stav vystupu pre kotol: "));
        Serial.println(stav);
      }
    } else {
      Serial.print(F("Manualny rezim, stav vystupu: "));
      Serial.println(stav);
    }
  }
  if (eth_connected)
  {
    server.handleClient();
  }
  yield();
}
