start esptool.exe --chip esp32 --port COM17 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0xe000 boot_app0.bin 0x1000 bootloader_qio_80m.bin 0x10000 ESP32_PHY_thermostat_EN.ino.bin 0x8000 ESP32_PHY_thermostat_EN.ino.partitions.bin