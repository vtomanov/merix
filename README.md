# merix
Advance battery monitoring system for boats , RV etc. 

The system is based on Arduino Uno for monitoring gauges and Arduino Mega for the server/display side.

The elements required for the every gauge module are (total ~$25) :

- Arduino Uno
- WCS1800 Hall Current Sensor
- NRF24L01+PA+LNA SMA Antenna Wireless Transceiver Communication Module 2.4G ( the one with the built-in antenna did not perform well)
- Socket Adapter plate Board For NRF24L01 ( required as connecting directly to 3.3v is not stable)
- Cables MALE TO FEMALE Jumper GPIO 
- 25v Voltage Sensor Module
- DC-DC LM2596HV Adjustable Step Down Buck Module Voltage Regulator MO
- high power battery cable and lugs
(optional enclosure box)

The elements required for the server/display ( total ~$30)
- Arduino Mega
- IIC/I2C 2004 20X4 Character LCD Module Display
- NRF24L01+PA+LNA SMA Antenna Wireless Transceiver Communication Module 2.4G ( the one with the built-in antenna did not perform well)
- Socket Adapter plate Board For NRF24L01 ( required as connecting directly to 3.3v is not stable)
- Cables MALE TO FEMALE Jumper GPIO
- DC-DC LM2596HV Adjustable Step Down Buck Module Voltage Regulator MO
(optional enclosure box)
