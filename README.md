# AS8579-capacitive-sensor

This repository contains Arduino code to read the AS8579 self-capacitive sensor via SPI.

Firstly, the initialization according to the "SPI Configuration" in the datasheet is carried out. 
Secondly, the "quick read"-command to read the I and Q values can be performed.

To make the I and Q values useful additional settings (offset, PGA, ...) might have to be adjusted. 
The status register (0x20) helps to find wrong settings.



