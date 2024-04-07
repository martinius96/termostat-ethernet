# Inštrukcie - PHY Ethernet termostat firmvér
* Funkcionalita každého firmvéru termostatu je opísaná na: https://martinius96.github.io/WiFi-termostat/spustenie.html
* Každý priečinok s firmvérom obsahuje nástroj esptool.exe a .bat súbor pre automatizované nahratie firmvéru do mikrokontroléra (ESP8266 / ESP32)
* **V .bat súbore je nutné vykonať úpravu čísla COM portu, kde sa hlási mikrokontróler (vzorovo COM17 pre ESP32)**
* Aktuálny port, kde je prihlásený mikrokontróler nájdete v Správcovi zariadení
* Zariadenie je najčastejšie označené ako CH340 device, alebo CP210X v závislosti od použitého USB-UART prevodníka
* Po nahratí firmvéru sa okno príkazového riadku (CLI) automaticky zatvorí a firmvér je pripravený na použitie
* Pri nahrávaní do mikrokontroléru ESP32 je často nutné prepnúť mikrokontróler do Upload režimu držaním tlačidla BOOT, respektíve privedením GND (pulldown) na GPIO0

<p align="center">
  <img src="https://i.imgur.com/M0U6HkC.png" />
</p>
