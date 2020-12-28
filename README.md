# Termostat - Arduino + Ethernet W5100 / W5500
* Termostat postavený na platforme Arduino s Ethernetom
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

**Pri záujme o kúpu rozšírenej verzie / základnej verzie so zdrojovým kódom .ino:**
* martinius96@gmail.com

**Rozšírené informácie k projektu v článku:**
* http://deadawp.blog.sector.sk/blogclanok/13223/izbovy-termostat-arduino-ethernet.htm

# Screenshoty webaplikácie + výstup Serial monitoru
![Ethernet termostat vypnutý - webserver - Arduino](https://i.imgur.com/9EOOqlW.png)
![Ethernet termostat zapnutý - webserver - Arduino](https://i.imgur.com/bnm7EAj.png)
![Ethernet termostat- presmerovanie používateľa po odoslaní dát do formulára](https://i.imgur.com/k9J9DFG.png)
![UART - RS232 výstup termostatu - Arduino](https://i.imgur.com/qQ74dpi.png)
![JSON output - Ethernet termostat - Arduino](https://chiptron.cz/images/articles/IzbovyTermostat/json.jpg)
