# AS8579-capacitive-sensor

This repository contains an Arduino code to access the AS8579 capacitive sensor.
The sensor is read via SPI. Initialization before reading the I and Q values is necessary! 

So far, the code can initialize the sensor and do the "quick read" for the I and Q components for the SEN0 line. 
However, these I and Q components are not changing when touching the SEN0 line. 

