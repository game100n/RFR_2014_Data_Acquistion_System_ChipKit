Rutgers Formula Racing 2014 Data Acquisition System
Arduino 8-bit AVR Based Computer Data System

Storage: SD card over SPI
Data type: Comma separated values

Sensors:
- 1 x Pololu MinIMU-9 v2 accelerometer/gyroscope/magnetometer
- 4 x Shock position potentiometers (not coded as of v0.1a)
- 1 x Steering position sensor (not coded as of v0.1a)

--------------------------------------------------
Changelog:

Version 0.1 alpha
- Initial commit
- MinIMU-9 v2 logging, comma separated values
- Proof of concept program, operation checked ok
- 5 samples per second
- Serial command in-system file clearing, logging pause/unpause, calibration