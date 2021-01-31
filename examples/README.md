# JSON client
* Umožňuje vzdialene pripojiť sa na Ethernet termostat po HTTP protokole (80) v rovnakej LAN sieti
* GET requestom stiahne obsah súboru **/get_data.json**, ktorý distribuuje WiFi termostat
* JSON string po stiahnutí mikrokontróler deserializuje a pripraví ho na parsovanie hodnôt
* Štandardná dĺžka stringu do 80 znakov, pre Arduino s Ethernetom znížený JSON objekt na 128 bajtov, pre ESP platformy použitý štandardný buffer 512 bajtov

**Vyparsuje hodnoty a vypíše ich na UART:**
* Nastavená hysteréza
* Nastavená cieľová teplota
* Aktuálne nameraná teplota

![JSON client - Ethernet termostat](https://i.imgur.com/4Cm5z8o.png)

# MQTT / MQTTS client
* Umožňuje vzdialene pripojiť sa na Ethernet termostat po HTTP protokole (80) v rovnakej LAN sieti
* GET requestom stiahne obsah súboru **/get_data.json**, ktorý distribuuje WiFi termostat
* JSON string po stiahnutí mikrokontróler deserializuje a pripraví ho na parsovanie hodnôt

**Vyparsuje hodnoty a vypíše ich na UART:**
* Nastavená hysteréza
* Nastavená cieľová teplota
* Aktuálne nameraná teplota

* Hodnoty klient Publishuje na MQTT Broker Iot Industries Slovakia https://mqtt.iotindustries.sk/sk/
* Hodnoty sa zapisujú pod hlavný topic termostat
# Cieľové topicy
* **termostat/hysteresis** - pre hysterézu
* **termostat/actual_temp** - pre aktuálne nameranú teplotu
* **termostat/target_temp** - pre cieľovú (referenčnú) teplotu
* Klient má prihlásený odber na topic termostat/#, ktorý mu vráti všetky dáta, ktoré na MQTT Broker publishoval (overuje zapísanie informácie na MQTT Broker)
* MQTT klient komunikuje s MQTT brokrom cez port 1883 - nezbezpečený MQTT port, MQTTS klient cez port 8883 - šifrovaný MQTTS port.
* ESP32 využíva pre šifrované spojenie Root CA certifikát, ESP32 odtlačok certifikátu MQTT Brokra (vyžaduje častejší RENEW)
* **UPOZORNENIE: MQTT Broker IoT Industries Slovakia, ktorý sa vo vzorovej implementácii používa je verejný a tak môžu byť dáta zmenené, prepísané, čítané akýmkoľvek používateľom! Ak do svojho mikrokontroléru nahrá firmvér JSON klienta bez zmeny akýkoľvek iný používateľ, bude vám dané dáta v preddefinovanom topicu prepisovať a rovnako aj vy jemu. Je tak vhodné pri vlastnej implementácii zvoliť iný topic, kam budú dáta odosielané.**

# Bloková schéma MQTT klienta
![MQTT client - Bloková schéma](https://i.imgur.com/m2O62dn.jpg)
# HiveMQ client - odber k topicu termostat/#
![Hive MQ client Subscribe na topic termostat - websocket](https://i.imgur.com/5BtdmFH.png)
