# Instructions - Ethernet thermostat firmware
* Description of the differences between the individual firmware can be found at: https://martinius96.github.io/termostat-ethernet/en/spustenie.html
* Each firmware is compatible with the ATmega328P chip, which is also equipped with Arduino Uno, Nano microcontrollers...
* The firmware is available in the machine code for this ATmega chip and is divided according to the Ethernet shield / Wiznet module
* The XLoader tool for uploading firmware to Arduino boards is available at: https://www.hobbytronics.co.uk/arduino-xloader
* **In the program it is necessary to select the firmware and change the COM port and use a suitable baudrate**
* The current port where the microcontroller is logged in can be found in the Device Manager
* The device is most often marked as USB-SERIAL CH340 in the case of clones, or e.g. Arduino Uno in the case of originals
* Baudrate for New Bootloader: 115200 baud / s, for Old Bootloader: 57600 (some versions of Arduino Nano V3.0)
* Click Upload to write the firmware to the microcontroller
* After uploading the firmware, information on the number of bytes written to the flash memory is displayed at the bottom of the XLoader interface

<p align="center">
  <img src="https://i.imgur.com/N1CHBoL.png" />
</p>

# Ethernet thermostat configuration
* The thermostat does not have a configuration interface for the network and its settings
* Based on the firmware, the IP address is assigned from a DHCP pool or is set to static
* IP address is always displayed on HW UART -> 115200 baud / s
<p align="center">
  <img src="https://i.imgur.com/yufSKiq.png" />
</p>
