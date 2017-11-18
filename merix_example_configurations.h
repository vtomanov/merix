/***********************************************************************************

ALFIL: Jeannea Sun Liberty 34

GENERICS:

- Two house battery banks connected in parallel
-- H_Bank(1) - stern starboard locker (150AH)
-- H_Bank(2) - bow under the bed (265AH)

- One starter/engine battery
-- S_Bank(1) - 20AH LiFePo4


- Consumers
-- H_Bank(2) - INVERTOR 2500W and BOW THRUSTER - 125A
-- H_Bank(1) - ALL ELSE 
--- After H_Bank(1) - WATERNAKER ( 9ah - max 30A temporary current)

- Producers (assumed total max 50A)
-- Solars (600W - max current 40A)
-- Windgen (400W - max current 30A)

CONFIGURATIONS

---------------------------------------------------
- Server (Arduino MEGA)

//#define MODULE_IS_CLIENT
#define MODULE_IS_SERVER

---------------------------------------------------
- H_Bank(2) - BOW ( Arduino UNO)


#define MODULE_IS_CLIENT
//#define MODULE_IS_SERVER

#define MODULE_SLAVE_ENABLED
//#define MODULE_SLAVE_SLAVE_ENABLED

#define MODULE_NAME F("INVERTER")
#define MODULE_SLAVE_NAME F("BOW THRUSTER")

#define MODULE_INCLUDE 1
#define MODULE_TYPE 0
#define MODULE_DISPLAY_TYPE 0

#define MODULE_SLAVE_INCLUDE 1
#define MODULE_SLAVE_TYPE 0
#define MODULE_SLAVE_DISPLAY_TYPE 0

#define MODULE_HANDSHAKE_DELAY_INDEX 3

#define CLIENT_AMPERMETER_PIN A0
#define CLIENT_VOLTMETER_PIN A1

#define CLIENT_VOLTMETER_SLAVE_PIN A4
#define CLIENT_AMPERMETER_SLAVE_PIN A1

// NOTICE THE USE OF SAME VOLTEMETER FOR BOTH

---------------------------------------------------

***********************************************************************************/
