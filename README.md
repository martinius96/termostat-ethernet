# Termostat - Arduino + Ethernet W5100
| Názov súboru        | Shield           | IPv4           |
| ------------- |:-------------:| ------------- |
| dynamic_W5100.hex     | W5100 | priradená cez DHCP |
| dynamic_W5500.hex      | W5500      | priradená cez DHCP |
| static_W5100_1_254.hex | W5100      | 192.168.1.254 |
| static_W5100_4_1.hex | W5100      | 192.168.4.1 |
| static_W5500_1_254.hex | W5500      | 192.168.1.254 |
| static_W5500_4_1.hex | W5500      | 192.168.4.1 |

# Nahratie programu do Arduina cez avrdude
* avrdude -CC: avrdude.conf -v -patmega328p -carduino -PCOM21 -b115200 -D -Uflash:w:sketch.ino.hex:i

# Dôležité informácie
* Serial monitor: 115200 baud/s
* Údaje o hysteréze a referenčnej teplote uložené v EEPROM pamäti
