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

**Možná práca s hodnotami:**
* Informatívny výpis na UART
* Možnosť Publishovať dáta na MQTT Broker, napríklad https://mqtt.iotindustries.sk/sk/, do aplikácie Blynk, GET / POST requestom nahrávať dáta do MySQL databázy pre vlastné vizualizácie (nie je súčasťou programových implementácii)
* Prebratie hodnoty a ovládanie iných periférii na základe načítaných hodnôt, napríklad odvetrávanie, ventilátor, klimatizácia a iné...
