Rutgers Formula Racing 2014 Data Acquisition System
ChipKit 32-bit MIPS Based Computer Data System

Storage: SD card over SPI
Data type: Comma separated values

Sensors:
- 1 x Pololu MinIMU-9 v2 accelerometer/gyroscope/magnetometer
- 4 x Shock position potentiometers
- 1 x Steering position sensor

- 4 x Wheel speed (not coded as of 0.2 beta)
- 2 x Brake pressure ( not coded as of 0.2 beta)

--------------------------------------------------
Changelog:

Version 0.1 alpha
- Initial commit
- MinIMU-9 v2 logging, comma separated values
- Proof of concept program, operation checked ok
- 5 samples per second
- Serial command in-system file clearing, logging pause/unpause, calibration

Version 0.2 beta
Written to work on ChipKit Boards
- 4 x Shock position potentiometers
- 1 x Steering position sensor