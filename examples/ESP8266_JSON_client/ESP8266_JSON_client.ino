/*|-------------------------------------------------|*/
/*|Projekt: JSON client - ESP8266                   |*/
/*|Popis: Pripojenie k WiFi termostatu v LAN sieti  |*/
/*|stiahnutie JSON dat, vyparsovanie hodnot, vypis  |*/
/*|Autor: Martin Chlebovec                          |*/
/*|E-mail: martinius96@gmail.com                    |*/
/*|Revízia: 1. Januar 2021                          |*/
/*|-------------------------------------------------|*/

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
const char* ssid = "MOJE SSID"; //SSID vasej WiFi siete
const char* password = "MOJE HESLO"; //HESLO vasej WiFi siete
const char* host = "192.168.1.XX"; //IP adresa WiFi termostatu
const int httpPort = 80;
WiFiClient client;
unsigned long timer = 0;
int interval = 15000;
void setup() {
  Serial.begin(115200); //rychlost seriovej linky
  Serial.println();
  Serial.print("Pripajanie na WiFi siet: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password); //pripoj sa na wifi siet s heslom
  while (WiFi.status() != WL_CONNECTED) { //pokial sa nepripojime na wifi opakuj pripajanie a spustaj funkcie pre ovladanie v offline rezime
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wifi pripojene s IP:");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
  }
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if ((millis() - timer) >= interval || timer == 0) {
    timer = millis();
    client.stop();
    if (client.connect(host, httpPort)) {
      String url = "/get_data.json";
      Serial.println("Pripojenie uspesne, nacitavam JSON data");
      client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: NodeMCU\r\n" + "Connection: close\r\n\r\n");
      while (client.connected()) {
        String line = client.readStringUntil('\n'); //HTTP HEADER
        //Serial.println(line);
        if (line == "\r") {
          break;
        }
      }
      DynamicJsonDocument doc(512);
      String line = client.readString(); //PAYLOAD
      Serial.println(line);
      deserializeJson(doc, line);
      JsonObject obj = doc.as<JsonObject>();
      float hystereza = obj[String("Hysteresis")];
      float cielova_teplota = obj[String("Target_Temperature")];
      float actual_temperature = obj[String("Actual_Temperature")];
      Serial.print("Hystereza: ");
      Serial.println(hystereza);
      Serial.print("Cielova teplota: ");
      Serial.println(cielova_teplota);
      Serial.print("Namerana (aktualna) teplota: ");
      Serial.println(actual_temperature);
    } else if (!client.connect(host, httpPort)) {
      Serial.println("Nepodarilo sa pripojenie k termostatu, ani nacitanie JSON data");
    }
  }
}
