# Instructions - WiFi thermostat firmware
* You can find a description of the functions of individual firmware at: https://martinius96.github.io/WiFi-termostat/en/spustenie.html 
* Each firmware folder contains the esptool.exe tool and a .bat file for automated firmware upload to the ESP8266 / ESP32 microcontroller
* **It is necessary to change the COM port in the .bat file (COM17 for ESP32 firmware is used as example)**
* The current port where the microcontroller is logged in can be found in the Device Manager
* The device is most often marked as CH340 device or CP210X depending on the used USB-UART converter
* After uploading the firmware, the command line window (CLI) will close automatically and the firmware is ready for use
* When uploading to the ESP32 microcontroller, it is often necessary to switch the microcontroller to Upload mode by holding down the BOOT button or setting GND to GPIO0

<p align="center">
  <img src="https://i.imgur.com/M0U6HkC.png" />
</p>
