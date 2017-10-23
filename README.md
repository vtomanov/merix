# merix
Advance battery monitoring system for boats , RV etc. 


Brief Capability Description:

- up to 9 measuring point ( easy extensible with just a configuration change if needed) 
- every measuring point can be included or excluded in the current consumption, current amps and volts calculation - for example if you have let say  one measuring point just after the battery bank and somewhere further down a measuring point which will measure the watermaker consumption - you do not want the watermaker to be included in the current calculations as it is already part of the first measuring point, but in the same time you do want to know how much the watermaker consumes and how much from the total consumption it is accountable for.
- measuring point communicate with the main/display module using radio/wirelessly
- calculate properly the in and out amps and adjust based on the battery voltage when appropriated
- calculate/adjust dynamically the loss coefficient as to get 1ah to discharge you actually need to charge more then 1ah - and the loss increases when batteries become older
- show the consumption for the last 24h
- shows how much time is left until the batteries are down to 50%
- shows how long it will take to charge the batteries to 90% 
- shows current AH available in the batteries
- shows charge state in %
- shows amps, volts and for how much of the discharge or charge every measuring point is accountable for
- screen is able to switch off the back light to prevent the annoying strong light source during sleep in the saloon
- able to initialize and connect new measuring points ( handshaking and binding with all available measuring points)
- reset of all measuring points and enables them for new binding with new main module or etc.
 

What The Display Show:

First line:

- average voltage across all measuring points
- total resulting current ( sum across all measuring point)
- proper calculation of  battery bank state of charge ( with  correction when volts are over 14 and no consumption or charge, when no consumption or charge for 1min also charge state correction based on voltage) and when charge state is corrected - auto calculation of lost coefficient while charging - e.g. when you charge there is a loss e.g. to have 1ah to discharge in average you need to charge 1.25ah)

Second line - flips between ( every 5 sec - configurable) :

- total AH available , and total consumption for the last 24h
and
- time to discharge to 50% ( if discharging) or time to charge to 90% if charging - assuming charge or discharge current continue to be the same ( the charging calculation is a proper one taking in account that after 75% charge the batteries gradually start accepting lower and lower current) 

Third and Fourth line  are related to  cycling between all measuring points

Third line - name of measuring point and :<sequence> of the update received from the sensor
Fourth - current voltage at this measuring point,  current amps at this measuring point, how much consumption this measuring point is accountable for in % from day one.

ELEMENTS:

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

 - module type and slave type
 
  TYPE 0 = volts and amps and ah, TYPE 1 - volts and amps, TYPE 2 - only volts, TYPE 3 ony amps, TYPE 4 amps and ah

  - slave - when one arduino with one RF can transport data for 2 sensors - slave and master - they can be from different type also


