# Termostat - Arduino + Ethernet Wiznet W5100 / W5500
* Termostat postavený na platforme Arduino s Ethernetom
* Rozšírený popis k projektu, schéma zapojenia, dokumentácia: https://martinius96.github.io/termostat-ethernet/
* Arduino funguje v režime HTTP webservera, kde ponúka na HTML stránkach komplexný prehľad aktuálne nameraných a platných údajov
* Umožňuje zmeniť údaje o cieľovej - referenčnej teplote a hysteréze, ktorá sa používa pre riadenie vykurovania
* Údaje sú zapísané do EEPROM pamäte, ktorá má životnosť až 100-tisíc prepisov
* Webserver je prístupný z domácej LAN siete
* Arduino riadi na základe navolenej hysterézy a cieľovej teploty výstup - relé, ktoré spína signál pre kotol
* Programová logika sa vykonáva nezávisle na webserveri (nevyžaduje sa keep-alive spojenie, alebo pozornosť používateľa)
* **Projekt nerieši výkonovú reguláciu kotla!**
* Pre meranie teploty Arduino využíva senzor Dallas DS18B20 na OneWire zbernici v parazitnom / normálnom zapojení (použité v schéme zapojenia)
* **Arduino na UART vypíše pridelenú (respektíve statickú) IP adresu**
* **Pri Ethernet module W5500 je nutné pripojiť aj vývod D2 Arduina na INT vývod Ethernet modulu** 
* Projekt môže fungovať celoročne, aj ako Ethernet teplomer v prípade, že je odpojený výstup ku riadeniu kotla.

| Názov súboru        | Shield           | IPv4           |
| ------------- |:-------------:| ------------- |
| dynamic_W5100.hex     | W5100 | priradená cez DHCP (obsahuje i maintain pre predĺženie lease time / vyžiadanie novej IP) |
| dynamic_W5500.hex      | W5500      | priradená cez DHCP (obsahuje i maintain pre predĺženie lease time / vyžiadanie novej IP) |
| static_W5100_1_254.hex | W5100      | 192.168.1.254 |
| static_W5100_4_1.hex | W5100      | 192.168.4.1 |
| static_W5500_1_254.hex | W5500      | 192.168.1.254 |
| static_W5500_4_1.hex | W5500      | 192.168.4.1 |
# Nahratie programu do Arduina cez avrdude (vzorovo pre COM21 a program sketch.ino.hex):
* avrdude -CC: avrdude.conf -v -patmega328p -carduino -PCOM21 -b115200 -D -Uflash:w:sketch.ino.hex:i
# Schéma zapojenia (Uno, Nano, Mega)
![Termostat - Ethernet - Arduino](https://i.imgur.com/GgJrAOj.png)
**Dôležité informácie:**
* Serial monitor: 115200 baud/s
* Údaje o hysteréze a referenčnej teplote uložené v EEPROM pamäti

**HTML stránky bežiace na Arduine:**
* **/** - root stránka obsahujúca formulár, aktuálny výpis logického výstupu pre relé, teplotu
* **/action.html** - spracúvava hodnoty z formulára, zapisuje ich do EEPROM pamäte, presmeruje používateľa späť na root stránku
* **/get_data.json** - distribuuje dáta o aktuálnej teplote, referenčnej teplote a hysteréze v JSON formáte

**Rozšírená verzia tohto projektu obsahuje:**
* Manuálny režim pre relé (neobmedzená doba, natvrdo ZAP/VYP)
* Watchdog timer
* Dostupné senzory SHT21, SHT31, DHT22, BME280, BMP280 a iné
* Režim chladenia
* Ovládanie a konfigurácia po RS232 / UART nezávisle na Ethernete
* PID regulácia teploty pre termostat
* Možnosť využitia platforiem ESP8266, ESP32 pre termostat

**Rozšírené informácie k projektu v článku:**
* http://deadawp.blog.sector.sk/blogclanok/13223/izbovy-termostat-arduino-ethernet.htm

# Screenshoty webaplikácie + výstup Serial monitoru
![Ethernet termostat vypnutý - webserver - Arduino](https://i.imgur.com/9EOOqlW.png)
![Ethernet termostat zapnutý - webserver - Arduino](https://i.imgur.com/bnm7EAj.png)
![Ethernet termostat- presmerovanie používateľa po odoslaní dát do formulára](https://i.imgur.com/k9J9DFG.png)
![UART - RS232 výstup termostatu - Arduino](https://i.imgur.com/qQ74dpi.png)
![JSON output - Ethernet termostat - Arduino](https://chiptron.cz/images/articles/IzbovyTermostat/json.jpg)

# JSON klienti - sťahovanie dát z termostatu
* Programové implementácia pre klientov na platforme Arduino, ESP8266, ESP32, ktorí sa dokážu pripojiť k WiFi termostatu
* Dokážu načítať dáta, ktoré termostat distribuuje - hysteréza, cieľová teplota, nameraná teplota na podstránke /get_data.json
* Dáta z JSON formátu klient deserializuje, vyparsuje pre ďalšie použitie, archiváciu, upload do MySQL databázy, cloud
* Možnosť na základe dát riadiť perifériu (solenoid radiátora, ohrev, ventilátor, notifikácie o teplote pre Android, iOS zariadenia)
* Pripájanie JSON klienta sa realizuje každých 15 sekúnd k termostatu cez websocket
* V rozšírenej implementácii JSON klienta o MQTT sa dáta posielajú na dostupný free MQTT Broker - IoT Industries Slovakia
* Dáta sa Publishujú do hlavného topicu termostat, pričom je každá entita rozdelená subtopicom
* Subtopicy sú: hysteresis, actual_temp, target_temp
* JSON klient má k daným subtopicom prihlásený Subscribe prostredníctvom čítania hlavného topicu termostat/#
* **Tento MQTT Broker je verejný a tak môžu byť dáta zmenené, prepísané, čítané akýkoľvek používateľom služby**
* **Ak do svojho mikrokontroléru nahrá firmvér JSON klienta bez zmeny akýkoľvek iný používateľ, bude vám prepisovať dáta v preddefinovanom topicu**
* Možno prispôsobiť pre váš MQTT broker a systém inteligentnej domácnosti, kde môžete mať dáta z termostatu - Hassio, Domoticz, MQTT Mosquitto, Loxone
* Pre súkromný MQTT broker je možné využiť aj autentizáciu menom a heslom, viz. dokumentácia: https://pubsubclient.knolleary.net/api
* K dispozícii je aj firmvér pre JSON MQTT klientov s označením MQTTS - využívajú šifrované spojenie cez socket s MQTT brokerom
* Tento typ firmvéru je dostupný iba pre ESP8266 a ESP32. Arduino s Ethernetom nepodporuje šifrovanie
![JSON klient - Arduino, ESP8266, ESP32](https://i.imgur.com/UEnHDb2.png)
