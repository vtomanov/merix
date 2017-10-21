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
- DC Power Pigtail Male 5.5 * 2.1mm Cable Wire Plug 
(optional enclosure box)

The elements required for the server/display ( total ~$30)
- Arduino Mega
- IIC/I2C 2004 20X4 Character LCD Module Display
- NRF24L01+PA+LNA SMA Antenna Wireless Transceiver Communication Module 2.4G ( the one with the built-in antenna did not perform well)
- Socket Adapter plate Board For NRF24L01 ( required as connecting directly to 3.3v is not stable)
- Push button ( used for lcd backlight toggle and reset and master reset)
- Cables MALE TO FEMALE Jumper GPIO
- DC-DC LM2596HV Adjustable Step Down Buck Module Voltage Regulator MO
- DC Power Pigtail Male 5.5 * 2.1mm Cable Wire Plug 
(optional enclosure box)

helping instructions : 

- all specific pins ( non standard for the libraries ) can be seen in the file merix_pins.h
- most of the modlues has simulations - just uncomment the macro for the simulation in the beggining
- client adn server can be simulated together in one arduino mega - uncomment both in the main file( merix.ino)

#define MODULE_IS_CLIENT
#define MODULE_IS_SERVER

and in merix_rf.h

#define RF_NETWORK_SIMULATION

- to simulate the screen in the serial log  in merix_server_display.h uncomment :

#define SERVER_DISPLAY_SIMULATION



