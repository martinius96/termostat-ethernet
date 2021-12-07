# Anleitung - Ethernet-Thermostat-Firmware
* Eine Beschreibung der Unterschiede der einzelnen Firmware finden Sie unter: https://martinius96.github.io/termostat-ethernet/de/spustenie.html
* Jede Firmware ist mit dem ATmega328P-Chip kompatibel, der auch mit Arduino Uno-Mikrocontrollern, Nano ...
* Firmware ist für diesen ATmega-Chip im Maschinencode verfügbar und ist nach Ethernet-Shield / Wiznet-Modul unterteilt
* XLoader-Tool zum Hochladen von Firmware auf Arduino-Boards ist verfügbar unter: https://www.hobbytronics.co.uk/arduino-xloader
* **Im Programm ist es notwendig, die Firmware auszuwählen und den COM-Port zu ändern und eine geeignete Baudrate zu verwenden**
* Den aktuellen Port, an dem der Mikrocontroller angemeldet ist, finden Sie im Gerätemanager
* Bei Klonen wird das Gerät meistens als USB-SERIAL CH340 gekennzeichnet, oder z.B. Arduino Uno bei Originalen
* Baudrate für neuen Bootloader: 115200 Baud/s, für alten Bootloader: 57600 (einige Versionen von Arduino Nano V3.0)
* Klicken Sie auf Hochladen, um die Firmware auf den Mikrocontroller zu schreiben
* Nach dem Hochladen der Firmware werden Informationen über die Anzahl der in den Flash-Speicher geschriebenen Bytes unten auf der XLoader-Schnittstelle geschrieben

<p align="center">
  <img src="https://i.imgur.com/N1CHBoL.png" />
</p>

# Konfiguration des Ethernet-Thermostats
* Der Thermostat hat keine Konfigurationsoberfläche für das Netzwerk und seine Einstellungen
* Basierend auf der Firmware wird die IP-Adresse aus einem DHCP-Pool zugewiesen oder auf statisch gesetzt
* IP-Adresse wird immer auf HW UART angezeigt -> 115200 Baud/s
<p align="center">
  <img src="https://i.imgur.com/6CYuMZZ.png" />
</p>
