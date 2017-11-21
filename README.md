# merix

https://www.youtube.com/watch?v=_Ukt8E8NHLA&t=31s

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

WARNINGS !!!!!

  you need to point the antenas of the NRF24 to the sky as else you may encounter RF stability issues
  
 INSTRUCTIONS FOR RUNNING THE SIMULATION:

  file : merix.h
  uncomment both lines :

  #define MODULE_IS_CLIENT
  #define MODULE_IS_SERVER

  file: merix_client_ampermeter.h
  uncomment line:
  #define CLIENT_AMPERMETER_SIMULATION

  file: merix_client_voltmeter.h
  uncomment line:
  #define CLIENT_VOLTMETER_SIMULATION

  file: merix_server_display.h
  uncomment line :
  #define SERVER_DISPLAY_SIMULATION

  file : merix_rf.h
  uncomment line :
  #define RF_NETWORK_SIMULATION

  comment out line:
  #define RF_RADIO_HEAD

  compile for arduino Mega , upload  and all will be displayed in the serial screen - simulation will 
  have one client ("HOUSE CONSUMERS") which measure - volts, amps and consumption

  to add simulations for one slave attached to this measuring point also you need :

  file : merix.h
  uncomment line:
  #define MODULE_SLAVE_ENABLED

  file : merix_client_voltmeter_slave.h
  uncomment line:
  #define CLIENT_VOLTMETER_SLAVE_SIMULATION

  Compile, upload to arduino Mega and you will see in addition one slave ("START BATTER")  attached to the 
  measuring point which measures only Volts , changing slave type in merix.h  to #define MODULE_SLAVE_TYPE 0  
  will add the slave to measure amps also , changing #define MODULE_SLAVE_INCLUDE 1 - will include the slave 
  measurement as part of the calculations for battery sate, consummation usage etc.

  FYI - the idea for slave is if you have 2 measuring points very close to each other and you just 
  want to have two measuring but only one sender to the main module/server - e.g. my stater battery 
  and house bank 1 are very close.

