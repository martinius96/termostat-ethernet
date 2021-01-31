/*|----------------------------------------------------------|*/
/*|Projekt: JSON client - Arduino + Ethernet ENC28J60 - MQTT |*/
/*|Popis: Pripojenie k Ethernet termostatu v LAN sieti       |*/
/*|stiahnutie JSON dat, vyparsovanie hodnot, vypis dat,      |*/
/*|Publish na MQTT Broker IoT Industries Slovakia            |*/
/*|Autor: Martin Chlebovec                                   |*/
/*|E-mail: martinius96@gmail.com                             |*/
/*|Revízia: 31. Januar 2021                                  |*/
/*|----------------------------------------------------------|*/

#include <UIPEthernet.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
const char* host = "192.168.1.XX"; //IP adresa Ethernet termostatu
const char* mqtt_server = "mqtt.iotindustries.sk";
const int httpPort = 80;
byte mac[] = { 0xAA, 0xCA, 0xCC, 0x81, 0xBB, 0xAC };
IPAddress ip(192, 168, 1, 254); //staticke pridelenie IP
EthernetClient klient;
unsigned long timer = 0;
int interval = 15000;
EthernetClient espClient; //pre MQTT
PubSubClient client(espClient);

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print(F("Message arrived on topic: "));
  Serial.print(topic);
  Serial.print(F(". Message: "));
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print(F("Attempting MQTT connection..."));
    // Attempt to connect
    if (client.connect("ENC28J60_Client_Termostat")) {
      Serial.println(F("connected"));
      // Subscribe
      client.subscribe("termostat/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(F(" try again in 5 seconds"));
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200); //rychlost seriovej linky
  Serial.println();
  if (Ethernet.begin(mac) == 0) {
    Ethernet.begin(mac); //nechame pridelenie na DHCP server
    // Ethernet.begin(mac, ip); //nastavime manualne
  }
  Serial.print(F("DHCP assigned IP: "));
  Serial.println(Ethernet.localIP());
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  if (!client.connected()) {
    reconnect();
  }
  Serial.println(F("Ready"));
}

void loop() {
  if ((millis() - timer) >= interval || timer == 0) {
    timer = millis();
    klient.stop();
    if (klient.connect(host, httpPort)) {
      String url = F("/get_data.json");
      Serial.println(F("Pripojenie uspesne, nacitavam JSON data"));
      klient.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: ENC28J60\r\n" + "Connection: close\r\n\r\n");
      while (klient.connected()) {
        String line = klient.readStringUntil('\n'); //HTTP HEADER
        //Serial.println(line);
        if (line == "\r") {
          break;
        }
      }
      DynamicJsonDocument doc(128);
      String line = klient.readString(); //PAYLOAD
      Serial.println(line);
      deserializeJson(doc, line);
      JsonObject obj = doc.as<JsonObject>();
      float hystereza = obj[String("Hysteresis")];
      float cielova_teplota = obj[String("Target_Temperature")];
      float actual_temperature = obj[String("Actual_Temperature")];
      Serial.print(F("Hystereza: "));
      Serial.println(hystereza);
      char pole_hysteresis[32];
      dtostrf(hystereza, 1, 2, pole_hysteresis);
      client.publish("termostat/hysteresis", pole_hysteresis);
      Serial.print(F("Cielova teplota: "));
      Serial.println(cielova_teplota);
      char pole_cielova_teplota[32];
      dtostrf(cielova_teplota, 1, 2, pole_cielova_teplota);
      client.publish("termostat/target_temp", pole_cielova_teplota);
      Serial.print(F("Namerana (aktualna) teplota: "));
      Serial.println(actual_temperature);
      char pole_actual_teplota[32];
      dtostrf(actual_temperature, 1, 2, pole_actual_teplota);
      client.publish("termostat/actual_temp", pole_actual_teplota);
    } else if (!klient.connect(host, httpPort)) {
      Serial.println(F("Nepodarilo sa pripojenie k termostatu, ani nacitanie JSON data"));
    }
  }
}
