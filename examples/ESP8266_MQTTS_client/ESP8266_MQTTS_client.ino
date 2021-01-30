  
/*|-----------------------------------------------------|*/
/*|Projekt: JSON client - ESP8266 - MQTTS               |*/
/*|Popis: Pripojenie k Ethernet termostatu v LAN sieti  |*/
/*|stiahnutie JSON dat, vyparsovanie hodnot, vypis      |*/
/*|Odoslanie dat do troch topicov na free MQTT          |*/
/*|Broker pre vývojárov - slovenský                     |*/
/*|Autor: Martin Chlebovec                              |*/
/*|E-mail: martinius96@gmail.com                        |*/
/*|Revízia: 29. Januar 2021                             |*/
/*|-----------------------------------------------------|*/

//#define ARDUINOJSON_USE_DOUBLE 1 //uncomment if overflow is at UART output (https://i.imgur.com/OZaRey0.png) --> Core 2.5.2 and older fix, at 2.7.4 stable not required
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
const char* ssid = "WIFI_NAME"; //SSID vasej WiFi siete
const char* password = "WIFI_PASSWORD"; //HESLO vasej WiFi siete
const char* host = "192.168.1.XX"; //IP adresa Ethernet termostatu
const char* mqtt_server = "mqtt.iotindustries.sk";
const int httpPort = 80;
WiFiClient klient; //pre HTTP
WiFiClientSecure   espClient; //pre MQTTS
PubSubClient client(espClient);
unsigned long timer = 0;
int interval = 15000;
const static char fingerprint[] PROGMEM = "32 35 82 68 C8 BC 5E 54 AF 69 08 42 32 A7 C6 F1 A5 CD 33 CB"; //SHA1 FINGERPRINT

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
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
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266_Client_Termostat")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("termostat/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200); //rychlost seriovej linky
  Serial.println();
  Serial.print("Pripajanie na WiFi siet: ");
  Serial.println(ssid);
  espClient.setFingerprint(fingerprint);
  WiFi.begin(ssid, password); //pripoj sa na wifi siet s heslom
  while (WiFi.status() != WL_CONNECTED) { //pokial sa nepripojime na wifi opakuj pripajanie a spustaj funkcie pre ovladanie v offline rezime
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wifi pripojene s IP:");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, 8883);
  client.setCallback(callback);
  if (!client.connected()) {
    reconnect();
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
  }
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  client.loop();
  if ((millis() - timer) >= interval || timer == 0) {
    timer = millis();
    klient.stop();
    if (klient.connect(host, httpPort)) {
      String url = "/get_data.json";
      Serial.println("Pripojenie uspesne, nacitavam JSON data");
      klient.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: NodeMCU\r\n" + "Connection: close\r\n\r\n");
      while (klient.connected()) {
        String line = klient.readStringUntil('\n'); //HTTP HEADER
        //Serial.println(line);
        if (line == "\r") {
          break;
        }
      }
      DynamicJsonDocument doc(512);
      String line = klient.readString(); //PAYLOAD
      Serial.println(line);
      deserializeJson(doc, line);
      JsonObject obj = doc.as<JsonObject>();
      float hystereza = obj[String("Hysteresis")];
      float cielova_teplota = obj[String("Target_Temperature")];
      float actual_temperature = obj[String("Actual_Temperature")];
      Serial.print("Hystereza: ");
      Serial.println(hystereza);
      char pole_hysteresis[32];
      dtostrf(hystereza, 1, 2, pole_hysteresis);
      client.publish("termostat/hysteresis", pole_hysteresis);
      Serial.print("Cielova teplota: ");
      Serial.println(cielova_teplota);
      char pole_cielova_teplota[32];
      dtostrf(cielova_teplota, 1, 2, pole_cielova_teplota);
      client.publish("termostat/target_temp", pole_cielova_teplota);
      Serial.print("Namerana (aktualna) teplota: ");
      Serial.println(actual_temperature);
      char pole_actual_teplota[32];
      dtostrf(actual_temperature, 1, 2, pole_actual_teplota);
      client.publish("termostat/actual_temp", pole_actual_teplota);
    } else if (!klient.connect(host, httpPort)) {
      Serial.println("Nepodarilo sa pripojenie k termostatu, ani nacitanie JSON data");
    }
  }
}
