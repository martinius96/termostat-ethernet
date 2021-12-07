# Inštrukcie - Ethernet termostat firmvér
* Popis rozdielov jednotlivých firmvérov nájdete na: https://martinius96.github.io/termostat-ethernet/spustenie.html
* Každý firmvér je kompatibilný pre čip ATmega328P, ktorým sú vybavené aj mikrokontroléry Arduino Uno, Nano...
* Firmvér je dostupný v strojovom kóde pre tento ATmega čip a je rozdelený podľa Ethernet shieldu / modulu Wiznet
* Nástroj XLoader pre nahrávanie firmvéru do Arduino dosiek je dostupný na: https://www.hobbytronics.co.uk/arduino-xloader
* **V programe je nutné zvoliť firmvér a zmeniť COM port a použiť vhodný baudrate**
* Aktuálny port, kde je prihlásený mikrokontróler nájdete v Správcovi zariadení
* Zariadenie je najčastejšie označené ako USB-SERIAL CH340 v prípade klonov, alebo napr. Arduino Uno v prípade originálnov
* **Baudrate pre New Bootloader: 115200 baud/s, pre Old Bootloader: 57600 (niektoré verzie najmä Arduino Nano V3.0)**
* Kliknutím na Upload sa firmvér zapíše do flash pamäte mikrokontroléra
* Po nahratí firmvéru sa v spodnej časti rozhrania XLoader vypíše informácia o počte zapísaných bajtov do flash pamäte
* Termostat funguje okamžite po nahratí firmvéru.
* Pridelená IP adresa z DHCP služby je vypísaná na UART (115200 baud/s)

<p align="center">
  <img src="https://i.imgur.com/N1CHBoL.png" />
</p>

# Konfigurácia Ethernet termostatu
* Termostat nemá konfiguračné rozhranie pre sieť a jej nastavenie
* Na základe firmvéru je IP adresa priradená z DHCP poolu, alebo je nastavená statická
* Aktuálna IP adresa je v pravidelných intervaloch spolu s logikou termostatu vypísaná na UART rozhranie
<p align="center">
  <img src="https://i.imgur.com/r5jNZf5.png" />
</p>
