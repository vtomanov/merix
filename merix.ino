

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

#define Version_Major_Minor_Revision F("Ver.#0.0.367")

//////////////////////////////////////////////////////////////////////////////////
// PLEASE SET MODULE NAME e.g. Main Consumers, Bow Thruster, Inverter, Watermaker ( name can be max 18 symbols)
//                     123456789012345 
#define MODULE_NAME F("HOUSE CONSUMERS")

//////////////////////////////////////////////////////////////////////////////////
// PLEASE SET INCLUDE or EXCLUDE the module from consumption calculation ( 1 - include )
#define MODULE_INCLUDE 1

//////////////////////////////////////////////////////////////////////////////////
// PLEASE SET DELAY INDEX ( for every module should be differnt) for clients - Index[1-9], for server Index = 0
#define MODULE_HANDSHAKE_DELAY_INDEX 1

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


//////////////////////////////////////////////////////////////////////////////////
// GLOBALS
#define MAX_CLIENTS 10

///////////////////////////////////////////////////////////////////////////////////
// LOG constants
#define LOG64_ENABLED
//#define EEPROM_LOG64_ENABLED

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
#include "merix_client_sensors.h"
#include "merix_rf.h"
#include "merix_server_reset.h"



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
               (SERVER_STORE_CLIENT_ID[SERVER_STORE_DATA_REQUEST_INDEX] == 0xFFFF) && (SERVER_STORE_DATA_REQUEST_INDEX < MAX_CLIENTS);
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


