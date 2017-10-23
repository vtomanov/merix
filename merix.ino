

/**
   USE OF THIS SOFTWARE IS GOVERNED BY THE TERMS AND CONDITIONS
   OF THE LICENSE STATEMENT AND LIMITED WARRANTY FURNISHED WITH
   THE PRODUCT.
   <p/>
   IN PARTICULAR, YOU WILL INDEMNIFY AND HOLD B2N LTD., ITS
   RELATED COMPANIES AND ITS SUPPLIERS, HARMLESS FROM AND AGAINST ANY
   CLAIMS OR LIABILITIES ARISING OUT OF THE USE, REPRODUCTION, OR
   DISTRIBUTION OF YOUR PROGRAMS, INCLUDING ANY CLAIMS OR LIABILITIES
   ARISING OUT OF OR RESULTING FROM THE USE, MODIFICATION, OR
   DISTRIBUTION OF PROGRAMS OR FILES CREATED FROM, BASED ON, AND/OR
   DERIVED FROM THIS SOURCE CODE FILE.
*/

/*
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
*/

#include <avr/pgmspace.h>

#include "merix_float_float.h"

#define MERIX_NOT_AVAILABLE -1000.0f

// <BEGIN> SETTINGS THAT NEED TO BE SET BEFORE FINAL COMPILE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//////////////////////////////////////////////////////////////////////////////////
// PLEASE SET CLIENT or SERVER HERE

//#define MODULE_IS_CLIENT
#define MODULE_IS_SERVER

//////////////////////////////////////////////////////////////////////////////////
// PLEASE SET VERSION and REVISION HERE

#define Version_Major_Minor_Revision F("Ver.#0.0.376")

//////////////////////////////////////////////////////////////////////////////////
// PLEASE SET MODULE NAME e.g. Main Consumers, Bow Thruster, Inverter, Watermaker ( name can be max 18 symbols)

#if defined(MODULE_IS_CLIENT)

// if slave is enabled - make sure you have set : MODULE_SLAVE_NAME, MODULE_SLAVE_INCLUDE, MODULE_SLAVE_INDEX ( expected to be MODULE_HANDSHAKE_DELAY_INDEX + 1) 
//#define MODULE_SLAVE_ENABLED

//                     123456789012345
#define MODULE_NAME F("HOUSE CONSUMERS")


#if defined(MODULE_SLAVE_ENABLED)
#define MODULE_SLAVE_NAME F("START BATTERY")
#endif

//////////////////////////////////////////////////////////////////////////////////
// PLEASE SET INCLUDE or EXCLUDE the module from consumption calculation ( 1 - include )
#define MODULE_INCLUDE 1

// TYPE 0 = volts and amps and ah, TYPE 1 - volts and amps, TYPE 2 - only volts, TYPE 3 ony amps, TYPE 4 amps and ah
#define MODULE_TYPE 0

#if defined(MODULE_SLAVE_ENABLED)
#define MODULE_SLAVE_INCLUDE 0
#define MODULE_SLAVE_TYPE 2
#endif

//////////////////////////////////////////////////////////////////////////////////
// PLEASE SET DELAY INDEX ( for every module should be differnt) for clients - Index[1-9], for server Index = 0
#define MODULE_HANDSHAKE_DELAY_INDEX 1

#if defined(MODULE_SLAVE_ENABLED)
#define MODULE_SLAVE_INDEX 2
#endif

#endif

#if defined(MODULE_IS_SERVER)

//////////////////////////////////////////////////////////////////////////////////
// PLEASE SET TOTAL ( inlcuding all battery banks capacity) in AH- float type
#define SERVER_BATTERY_CAPACITY  400.0f

//////////////////////////////////////////////////////////////////////////////////
// PLEASE SET MINIMUM CONSUMPTION to be used for fully charged test based on voltage and current consumption
#define SERVER_BATTERY_MIN_CONSUMPTION_WHEN_FULLY_CHARGED  1.0f

// Loss during charging is around 25%initially and then is get autoadjusted everytime the charging state is corrected based on voltage and settled battery
#define SERVER_BATTERY_DATA_LOSS_COEF_INIT 0.75f

// Over what voltage to test for fully charged based on amps consumtion
#define SERVER_BATTERY_FULLY_CHARGED_TEST_VOLTAGE 14.00f

#define CLIENT_MIN_CONSUMPTION  0.5f

#endif

///////////////////////////////////////////////////////////////////////////////////
// LOG constants
#define LOG64_ENABLED
//#define EEPROM_LOG64_ENABLED


// <END> SETTINGS THAT NEED TO BE SET BEFORE FINAL COMPILE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//////////////////////////////////////////////////////////////////////////////////
// GLOBALS
#define MAX_CLIENTS 10

#include <Log64.h>

#include "merix_eprom_address.h"
#include "merix_pins.h"

///////////////////////////////////////////////////////////////////////////////////
// DO EXECUTE test
// will return true only if the fuction is ready for execute

inline bool DO_EXECUTE(uint32_t loopCounter, uint32_t lastExecute, uint32_t timeout)
{
  uint64_t loopCounter_ = loopCounter;
  uint64_t lastExecute_ = lastExecute;
  uint64_t timeout_ = timeout;

  if (lastExecute_ > loopCounter_)
  {
    loopCounter_ += 0xFFFFFFFF;
  }

  if ((lastExecute_ + timeout_) < loopCounter_)
  {
    return true;
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////////
// Insert in a sorted array

inline void INSERT_SORT(float arr[], float val, uint8_t count)
{
  uint8_t idx = 0;

  //Find the place to insert the element
  for (; (idx < count) && (arr[idx] <= val); idx++)
  {
  }

  //Move backward elements
  for (uint8_t j = count; j > idx; j--)
  {
    arr[j] = arr[j - 1];
  }

  //Insert element
  arr[idx] = val;
}



uint32_t MONITOR_LAST_EXECUTE;
#define MONITOR_TIMEOUT  500

inline void INIT_MONITOR()
{
  pinMode(MONITOR_PIN, OUTPUT);
  MONITOR_DOWN();
  MONITOR_LAST_EXECUTE = millis();
  LOG64_SET(F("LED MONITOR: INIT"));
  LOG64_NEW_LINE;
}
inline void MONITOR_UP()
{
  digitalWrite(MONITOR_PIN, HIGH);
}

inline void MONITOR_DOWN()
{
  digitalWrite(MONITOR_PIN, LOW);
}


#include "merix_server_state.h"
#include "merix_server_store.h"
#include "merix_server_battery_data.h"
#include "merix_server_display.h"
#include "merix_operations.h"
#include "merix_id.h"
#include "merix_client_voltmeter.h"
#include "merix_client_ampermeter.h"
#include "merix_client_voltmeter_slave.h"
#include "merix_client_ampermeter_slave.h"
#include "merix_client_sensors.h"
#include "merix_rf.h"
#include "merix_server_reset.h"
#include "merix_server_button.h"



void setup()
{
  // Serial Log
  LOG64_INIT();

  // Print version
  LOG64_SET(Version_Major_Minor_Revision);
  LOG64_NEW_LINE;

  // Monitor PIN
  INIT_MONITOR();

#if defined(MODULE_IS_CLIENT)
  CLIENT_VOLTMETER_INIT();
  CLIENT_AMPERMETER_INIT();
  // Init client meters and calculations
  CLIENT_SENSORS_INIT();
#endif

#if defined(MODULE_IS_SERVER)
  //Server button
  SERVER_BUTTON_INIT();
  //Server display
  SERVER_DISPLAY_INIT();
  // Server state
  SERVER_STATE_INIT();
  //Server store
  SERVER_STORE_INIT();
#endif

  // ID
  ID_INIT();

  // RF
  RF_INIT();

}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Foreground main loop/process and communcation

void loop()
{

  if (DO_EXECUTE(millis(), MONITOR_LAST_EXECUTE, MONITOR_TIMEOUT))
  {
    MONITOR_LAST_EXECUTE = millis();
    MONITOR_DOWN();
  }


#if defined(MODULE_IS_SERVER)
  switch (SERVER_STATE)
  {
    case SERVER_STATE_RESET :
      {
        if (DO_EXECUTE(millis(), SERVER_STATE_REST_LAST_EXECUTE, SERVER_STATE_RESET_TIMEOUT))
        {
          RF_SEND_HANDSHAKE_REQUEST();
          SERVER_STATE_HANDSHAKE_LAST_EXECUTE = millis();
          SERVER_STATE = SERVER_STATE_HANDSHAKE;
        }

      }; break;
    case SERVER_STATE_HANDSHAKE :
      {

        if (DO_EXECUTE(millis(), SERVER_STATE_HANDSHAKE_LAST_EXECUTE, SERVER_STATE_HANDSHAKE_TIMEOUT))
        {
          bool have_registered = false;
          for (uint8_t i = 0; i < MAX_CLIENTS; i++)
          {
            if (SERVER_STORE_CLIENT_ID[i] != 0xFFFF)
            {
              have_registered = true;
              break;
            }
          }

          if (have_registered)
          {
            SERVER_STATE_WORK_LAST_EXECUTE = 0;

            SERVER_STATE = SERVER_STATE_WORK;

            SERVER_DISPLAY_CLEAR();

            LOG64_SET(F("LOOP: FROM STATE[HANDSHAKE} TO SATE[WORK]"));
            LOG64_NEW_LINE;
          }
          else
          {
            RF_SEND_HANDSHAKE_REQUEST();
            SERVER_STATE_HANDSHAKE_LAST_EXECUTE = millis();
            SERVER_STATE = SERVER_STATE_HANDSHAKE;

            SERVER_DISPLAY_RESET();

            LOG64_SET(F("LOOP: FROM STATE[HANDSHAKE} STAY TO SATE[HANDSHAKE] AS NO CLINETS ATTACHED"));
            LOG64_NEW_LINE;
          }
        }
      }; break;
    case SERVER_STATE_WORK :
      {
        if (DO_EXECUTE(millis(), SERVER_STATE_WORK_LAST_EXECUTE, SERVER_STATE_DATA_REQUEST_DELAY))
        {
          SERVER_STATE_WORK_LAST_EXECUTE = millis();

          if ((++SERVER_STORE_DATA_REQUEST_INDEX) >= MAX_CLIENTS)
          {
            SERVER_STORE_DATA_REQUEST_INDEX = 0;
          }

          for (;
               ((SERVER_STORE_CLIENT_ID[SERVER_STORE_DATA_REQUEST_INDEX] == 0xFFFF) || (SERVER_STORE_CLIENT_SLAVE[SERVER_STORE_DATA_REQUEST_INDEX] == 1))  && (SERVER_STORE_DATA_REQUEST_INDEX < MAX_CLIENTS);
               SERVER_STORE_DATA_REQUEST_INDEX++);


          if (SERVER_STORE_DATA_REQUEST_INDEX < MAX_CLIENTS)
          {
            LOG64_SET(F("LOOP: DO[WORK] : CLINET_INDEX["));
            LOG64_SET(SERVER_STORE_DATA_REQUEST_INDEX);
            LOG64_SET(F("]"));
            LOG64_NEW_LINE;

            RF_SEND_DATA_REQUEST(SERVER_STORE_CLIENT_ID[SERVER_STORE_DATA_REQUEST_INDEX], SERVER_STORE_DATA_REQUEST_INDEX + 1);
          }
        }
      }; break;
  }

  SERVER_STORE_();

  SERVER_DISPLAY_();

  SERVER_BUTTON_();

#endif

#if defined(MODULE_IS_CLIENT)
  // Pumpo client voltemeter
  CLIENT_VOLTMETER_();

  //Pump client ampermeter
  CLIENT_AMPERMETER_();

  //Pump client client meters and calculations
  CLIENT_SENSORS_();
#endif

  // try receive and process message
  RF_();

  //delay(10);
}


