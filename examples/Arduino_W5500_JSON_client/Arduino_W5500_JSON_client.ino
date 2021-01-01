/*|-------------------------------------------------|*/
/*|Projekt: JSON client - Arduino + Ethernet W5100  |*/
/*|Popis: Pripojenie k Ethernet termostatu v LAN    |*/
/*|sieti, stiahnutie JSON dat, deserializacia,      |*/
/*|vyparsovanie hodnot, vypis ziskanych dat na UART |*/
/*|Autor: Martin Chlebovec                          |*/
/*|E-mail: martinius96@gmail.com                    |*/
/*|Revízia: 1. Januar 2021                          |*/
/*|-------------------------------------------------|*/
//POZNAMKA: NEZABUDNITE PRI FYZICKOM MODULU S ARDUINOM PREPOJENI PRIPOJIT I INT (INTERRUPT) NA D2

#include <Ethernet2.h>
#include <ArduinoJson.h>
const char* host = "192.168.1.XX"; //IP adresa Ethernet termostatu
const int httpPort = 80;
byte mac[] = { 0xAA, 0xBB, 0xCC, 0x81, 0xBB, 0xAB };
IPAddress ip(192, 168, 1, 253); //staticke pridelenie IP
EthernetClient client;
unsigned long timer = 0;
int interval = 15000;
void setup() {
  Serial.begin(115200); //rychlost seriovej linky
  Serial.println();
  if (Ethernet.begin(mac) == 0) {
    Ethernet.begin(mac); //nechame pridelenie na DHCP server
    // Ethernet.begin(mac, ip); //nastavime manualne
  }
  Serial.print("  DHCP assigned IP ");
  Serial.println(Ethernet.localIP());
  Serial.println("Ready");
}

void loop() {
  if ((millis() - timer) >= interval || timer == 0) {
    timer = millis();
    client.stop();
    if (client.connect(host, httpPort)) {
      String url = "/get_data.json";
      Serial.println("Pripojenie uspesne, nacitavam JSON data");
      client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: W5500\r\n" + "Connection: close\r\n\r\n");
      while (client.connected()) {
        String line = client.readStringUntil('\n'); //HTTP HEADER
        //Serial.println(line);
        if (line == "\r") {
          break;
        }
      }
      DynamicJsonDocument doc(128);
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
