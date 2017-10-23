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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Server store

#if defined(MODULE_IS_SERVER)

// forward definitions
inline void SERVER_DISPLAY_REFRESH(float amps, float volts, uint8_t percent, float ah_available, float last24[24]);
inline uint8_t SERVER_BATTERY_DATA_FROM_VOLTAGE(float voltage);

#define SERVER_STORE_TIMEOUT  10000
uint32_t SERVER_STORE_LAST_EXECUTE;

#define SERVER_STORE_LAST1_TIMEOUT  (60000)
//(1000 * 60 * 60)
uint32_t SERVER_STORE_LAST1_EXECUTE;

uint8_t SERVER_STORE_DATA_REQUEST_INDEX;

uint16_t SERVER_STORE_CLIENT_ID[MAX_CLIENTS];
uint8_t SERVER_STORE_CLIENT_INCLUDE[MAX_CLIENTS];
uint8_t SERVER_STORE_CLIENT_SLAVE[MAX_CLIENTS];
uint8_t SERVER_STORE_CLIENT_TYPE[MAX_CLIENTS];
uint8_t SERVER_STORE_CLIENT_SEQ[MAX_CLIENTS];
char SERVER_STORE_CLIENT_NAME[MAX_CLIENTS][20];

float SERVER_STORE_CLIENT_VOLTS[MAX_CLIENTS];
float SERVER_STORE_CLIENT_AMPS[MAX_CLIENTS];

bool SERVER_STORE_INITIALIZED;
FLOAT_FLOAT SERVER_STORE_AH;

#define SERVER_STORE_TOTAL_DISCHARGED_FOR_CLEAR (20000.0f)
FLOAT_FLOAT SERVER_STORE_TOTAL_DISCHARGED;
FLOAT_FLOAT SERVER_STORE_TOTAL_PER_CLIENT[MAX_CLIENTS];

float SERVER_STORE_LAST24[24];
FLOAT_FLOAT SERVER_STORE_LAST1;

float SERVER_STORE_DATA_LOSS_COEF;

// adjust based on voltage only if charging current/discharging current small for atleast 1min
#define SERVER_STORE_ADJUST_TIMEOUT  (60 * 1000)
uint32_t SERVER_STORE_ADJUST_LAST_EXECUTE;

inline void SERVER_STORE_INIT()
{
  SERVER_STORE_DATA_LOSS_COEF = SERVER_BATTERY_DATA_LOSS_COEF_INIT;
  SERVER_STORE_LAST_EXECUTE = millis();
  SERVER_STORE_LAST1_EXECUTE = millis();

  SERVER_STORE_INITIALIZED = false;
  SERVER_STORE_AH = FLOAT_FLOAT(MERIX_NOT_AVAILABLE);
  SERVER_STORE_DATA_REQUEST_INDEX = 0;
  SERVER_STORE_TOTAL_DISCHARGED = FLOAT_FLOAT(0.0f);
  SERVER_STORE_LAST1 = FLOAT_FLOAT(0.0f);;

  for (uint8_t i = 0; i < MAX_CLIENTS; i++)
  {
    SERVER_STORE_CLIENT_ID[i] = 0xFFFF;
    SERVER_STORE_CLIENT_SEQ[i] = 0;
    SERVER_STORE_CLIENT_VOLTS[i] = MERIX_NOT_AVAILABLE;
    SERVER_STORE_CLIENT_AMPS[i] = MERIX_NOT_AVAILABLE;
    SERVER_STORE_TOTAL_PER_CLIENT[i] = FLOAT_FLOAT(0.0f);
  }

  for (uint8_t i = 0; i < 24; i++)
  {
    SERVER_STORE_LAST24[i] = 0.0f;
  }

  SERVER_STORE_ADJUST_LAST_EXECUTE = 0;

  LOG64_SET(F("SERVER_STORE: INIT"));
  LOG64_NEW_LINE;
}

inline float SERVER_STORE_()
{
  if (DO_EXECUTE(millis(), SERVER_STORE_LAST_EXECUTE, SERVER_STORE_TIMEOUT))
  {
    SERVER_STORE_LAST_EXECUTE = millis();

    if (SERVER_STORE_TOTAL_DISCHARGED_FOR_CLEAR <= abs(SERVER_STORE_TOTAL_DISCHARGED.GET()) )
    {
      SERVER_STORE_TOTAL_DISCHARGED = FLOAT_FLOAT(0.0f);
      for (uint8_t i = 0; i < MAX_CLIENTS; i++)
      {
        SERVER_STORE_TOTAL_PER_CLIENT[i] = FLOAT_FLOAT(0.0f);
      }
      SERVER_STORE_LAST1 = FLOAT_FLOAT(0.0f);
      for (uint8_t i = 0; i < 24; i++)
      {
        SERVER_STORE_LAST24[i] = 0.0f;
      }
      SERVER_STORE_LAST1_EXECUTE = millis();
    }
  }

  if (DO_EXECUTE(millis(), SERVER_STORE_LAST1_EXECUTE, SERVER_STORE_LAST1_TIMEOUT))
  {

    //LOG64_SET(F("STORE: CALC 24: TOTAL["));
    //LOG64_SET(SERVER_STORE_TOTAL_DISCHARGED.GET());
    //LOG64_SET(F("] LAST1["));
    //LOG64_SET(SERVER_STORE_LAST1.GET());
    //LOG64_SET(F("] 24["));

    SERVER_STORE_LAST1_EXECUTE = millis();
    memcpy(&SERVER_STORE_LAST24[0], &SERVER_STORE_LAST24[1], sizeof(SERVER_STORE_LAST24) - sizeof(float));
    FLOAT_FLOAT last1 = FLOAT_FLOAT(SERVER_STORE_TOTAL_DISCHARGED);
    last1.SUB(SERVER_STORE_LAST1);
    SERVER_STORE_LAST24[23] = abs(last1.GET());
    SERVER_STORE_LAST1 = SERVER_STORE_TOTAL_DISCHARGED;

    //for (int i = 0; i < 24; i++)
    //{
    //  LOG64_SET(SERVER_STORE_LAST24[i]);
    //}

    //LOG64_SET(F("] LAST1["));
    //LOG64_SET(last1.GET());
    //LOG64_SET(F("]"));
    //LOG64_NEW_LINE;
  }
}

inline float SERVER_STORE_VOLTS()
{
  uint8_t count = 0;
  float ret = 0;

  for (uint8_t i = 0; i < MAX_CLIENTS; i++)
  {
    if (SERVER_STORE_CLIENT_ID[i] != 0xFFFF)
    {
      if (SERVER_STORE_CLIENT_VOLTS[i] > MERIX_NOT_AVAILABLE)
      {
        if (SERVER_STORE_CLIENT_INCLUDE[i] > 0)
        {
          count++;
          ret += SERVER_STORE_CLIENT_VOLTS[i];
        }
      }
    }
  }

  if (count > 0)
  {
    return (ret / ((float)count));
  }

  return MERIX_NOT_AVAILABLE;
}

inline float SERVER_STORE_AMPS()
{
  uint8_t count = 0;
  float ret = 0;

  for (uint8_t i = 0; i < MAX_CLIENTS; i++)
  {
    if (SERVER_STORE_CLIENT_ID[i] != 0xFFFF)
    {
      if (SERVER_STORE_CLIENT_AMPS[i] > MERIX_NOT_AVAILABLE)
      {
        if (SERVER_STORE_CLIENT_INCLUDE[i] > 0)
        {
          count++;
          ret += SERVER_STORE_CLIENT_AMPS[i];

        }
      }
    }
  }

  if (count > 0)
  {
    return ret;
  }

  return MERIX_NOT_AVAILABLE;
}

inline void SERVER_STORE_PROCESS_DATA(uint16_t id, float amps, float volts, uint8_t seq, FLOAT_FLOAT ah)
{

  //amps *= 3.8f;

  for (uint8_t i = 0; i < MAX_CLIENTS; i++)
  {
    if (SERVER_STORE_CLIENT_ID[i] == id)
    {
      SERVER_STORE_CLIENT_SEQ[i] = seq;
      SERVER_STORE_CLIENT_VOLTS[i] = volts;
      SERVER_STORE_CLIENT_AMPS[i] = (abs(amps) < CLIENT_MIN_CONSUMPTION) ? 0.0f : amps;

      if ((ah.GET() < 0) && (ah.GET() > MERIX_NOT_AVAILABLE))
      {
        SERVER_STORE_TOTAL_DISCHARGED.ADD(ah);
        SERVER_STORE_TOTAL_PER_CLIENT[i].ADD(ah);
      }

      if (!SERVER_STORE_INITIALIZED)
      {
        bool data_available = true;
        for (uint8_t j = 0; j < MAX_CLIENTS; j++)
        {
          if (SERVER_STORE_CLIENT_ID[j] != 0xFFFF)
          {
            if (SERVER_STORE_CLIENT_INCLUDE[i] > 0)
            {
              if (SERVER_STORE_CLIENT_VOLTS[j] <= MERIX_NOT_AVAILABLE)
              {
                data_available = false;
                break;
              }
              if (SERVER_STORE_CLIENT_AMPS[j] <= MERIX_NOT_AVAILABLE)
              {
                data_available = false;
                break;
              }
            }
          }
        }
        
        if (data_available)
        {
          float server_volts = SERVER_STORE_VOLTS();
          float server_amps = SERVER_STORE_AMPS();
          uint8_t server_percent = SERVER_BATTERY_DATA_FROM_VOLTAGE(server_volts);
          SERVER_STORE_AH = FLOAT_FLOAT(SERVER_BATTERY_CAPACITY * (((float)server_percent) / 100.0f));
          SERVER_STORE_INITIALIZED = true;

          SERVER_DISPLAY_REFRESH(server_amps, server_volts, server_percent, SERVER_STORE_AH.GET(), SERVER_STORE_LAST24);

          LOG64_SET(F("SERVER_STORE: PROCESS_DATA: STORE_INITIALIZED : AMPS["));
          LOG64_SET(server_amps);
          LOG64_SET(F("] VOLTS["));
          LOG64_SET(server_volts);
          LOG64_SET(F("] %["));
          LOG64_SET(server_percent);
          LOG64_SET(F("] AH["));
          LOG64_SET(SERVER_STORE_AH.GET());
          LOG64_SET(F("]"));
          LOG64_NEW_LINE;
        }
        return;
      }

      if (SERVER_STORE_CLIENT_INCLUDE[i] > 0)
      {

        float server_volts = SERVER_STORE_VOLTS();
        float server_amps = SERVER_STORE_AMPS();

        if (ah.GET() > MERIX_NOT_AVAILABLE)
        {
          //LOG64_SET(F("SERVER_STORE: AH BEFORE["));
          //LOG64_SET(String(SERVER_STORE_AH.GET(), 12));
          //LOG64_SET(String(SERVER_STORE_AH.GET_LO(), 12));
          //LOG64_SET(F("]"));
          //LOG64_NEW_LINE;
          if (ah.GET() > 0)
          {
            FLOAT_FLOAT tmp = FLOAT_FLOAT(ah);
            tmp.MUL(FLOAT_FLOAT(SERVER_STORE_DATA_LOSS_COEF));
            SERVER_STORE_AH.ADD(tmp);
          }
          else
          {

            SERVER_STORE_AH.ADD(ah);
          }

          //LOG64_SET(F("SERVER_STORE: AH AFTER["));
          //LOG64_SET(String(SERVER_STORE_AH.GET(), 12));
          //LOG64_SET(String(SERVER_STORE_AH.GET_LO(), 12));
          //LOG64_SET(F("]"));
          //LOG64_NEW_LINE;

          if (SERVER_STORE_AH.GET() > SERVER_BATTERY_CAPACITY)
          {
            SERVER_STORE_AH = FLOAT_FLOAT(SERVER_BATTERY_CAPACITY);
          }

          if (SERVER_STORE_AH.GET() < 0)
          {
            SERVER_STORE_AH = FLOAT_FLOAT(0.0f);
          }

          bool settled_amps = false;
          // logic for calclating has the charging been settled for small charge for period e.g. 1min
          if (abs(server_amps) <= SERVER_BATTERY_MIN_CONSUMPTION_WHEN_FULLY_CHARGED)
          {
            if (SERVER_STORE_ADJUST_LAST_EXECUTE == 0)
            {
              // first time we hit the case
              SERVER_STORE_ADJUST_LAST_EXECUTE = millis();
            }
            else
            {
              if (DO_EXECUTE(millis(), SERVER_STORE_ADJUST_LAST_EXECUTE, SERVER_STORE_ADJUST_TIMEOUT))
              {
                SERVER_STORE_LAST_EXECUTE = 0;
                settled_amps = true;
              }
            }
          }
          else
          {
            SERVER_STORE_LAST_EXECUTE = 0;
          }

          if (SERVER_STORE_AH.GET() < SERVER_BATTERY_CAPACITY)
          {
            if (SERVER_BATTERY_FULLY_CHARGED_TEST_VOLTAGE <= server_volts)
            {
              // test for fully charged
              if (settled_amps)
              {
                // adjust lost coef
                float old_coef = SERVER_STORE_DATA_LOSS_COEF;
                FLOAT_FLOAT new_coef = FLOAT_FLOAT(old_coef);
                new_coef.MUL(SERVER_STORE_AH);
                new_coef.DIV(FLOAT_FLOAT(SERVER_BATTERY_CAPACITY));
                SERVER_STORE_DATA_LOSS_COEF = new_coef.GET();
                if (SERVER_STORE_DATA_LOSS_COEF > 40.0f)
                {
                  // too big
                  SERVER_STORE_DATA_LOSS_COEF = 40.0f;
                }

                SERVER_STORE_AH = FLOAT_FLOAT(SERVER_BATTERY_CAPACITY);

                LOG64_SET(F("SERVER_STORE: PROCESS_DATA: CORRECTED TO FULLY CHARGED BASED ON VOLTS OVER ["));
                LOG64_SET(SERVER_BATTERY_FULLY_CHARGED_TEST_VOLTAGE);
                LOG64_SET(server_amps);
                LOG64_SET(F("]: VOLTS["));
                LOG64_SET(server_volts);
                LOG64_SET(F("] AH["));
                LOG64_SET(String(ah.GET(), 12));
                LOG64_SET(String(ah.GET_LO(), 12));
                LOG64_SET(F("] AH_TOTAL["));
                LOG64_SET(String(SERVER_STORE_AH.GET(), 12));
                LOG64_SET(String(SERVER_STORE_AH.GET_LO(), 12));
                LOG64_SET(F("] OLD_COEF["));
                LOG64_SET(old_coef);
                LOG64_SET(F("] NEW_COEF["));
                LOG64_SET(SERVER_STORE_DATA_LOSS_COEF);
                LOG64_SET(F("]"));
                LOG64_NEW_LINE;
              }
            }
          }

          // try state of charge correction based of small current
          if (settled_amps)
          {
            uint8_t server_percent = SERVER_BATTERY_DATA_FROM_VOLTAGE(server_volts);
            float server_ah = SERVER_BATTERY_CAPACITY * (((float)server_percent) / 100.0f);
            // adjust lost coef
            float old_coef = SERVER_STORE_DATA_LOSS_COEF;
            FLOAT_FLOAT new_coef = FLOAT_FLOAT(old_coef);
            new_coef.MUL(SERVER_STORE_AH);
            new_coef.DIV(FLOAT_FLOAT(SERVER_BATTERY_CAPACITY));
            SERVER_STORE_DATA_LOSS_COEF = new_coef.GET();
            if (SERVER_STORE_DATA_LOSS_COEF > 40.0f)
            {
              // too big
              SERVER_STORE_DATA_LOSS_COEF = 40.0f;
            }

            SERVER_STORE_AH = FLOAT_FLOAT(server_ah);

            LOG64_SET(F("SERVER_STORE: PROCESS_DATA: CORRECTED CHARGE STATE BASED ON VOLTS AND LOW CURRENT AMPS["));
            LOG64_SET(server_amps);
            LOG64_SET(F("]: VOLTS["));
            LOG64_SET(server_volts);
            LOG64_SET(F("] %["));
            LOG64_SET(server_percent);
            LOG64_SET(F("] AH["));
            LOG64_SET(String(ah.GET(), 12));
            LOG64_SET(String(ah.GET_LO(), 12));
            LOG64_SET(F("] AH_TOTAL["));
            LOG64_SET(String(SERVER_STORE_AH.GET(), 12));
            LOG64_SET(String(SERVER_STORE_AH.GET_LO(), 12));
            LOG64_SET(F("] OLD_COEF["));
            LOG64_SET(old_coef);
            LOG64_SET(F("] NEW_COEF["));
            LOG64_SET(SERVER_STORE_DATA_LOSS_COEF);
            LOG64_SET(F("]"));
            LOG64_NEW_LINE;
          }
        }


        //        LOG64_SET(F("SERVER_STORE: PROCESS_DATA : AMPS["));
        //        LOG64_SET(server_amps);
        //        LOG64_SET(F("]: VOLTS["));
        //        LOG64_SET(server_volts);
        //        LOG64_SET(F("] AH["));
        //        LOG64_SET(String(ah.GET(), 12));
        //        LOG64_SET(String(ah.GET_LO(), 12));
        //        LOG64_SET(F("] AH_TOTAL["));
        //        LOG64_SET(String(SERVER_STORE_AH.GET(), 12));
        //        LOG64_SET(String(SERVER_STORE_AH.GET_LO(), 12));
        //        LOG64_SET(F("] NEW_COEF["));
        //        LOG64_SET(SERVER_STORE_DATA_LOSS_COEF);
        //        LOG64_SET(F("]"));
        //        LOG64_NEW_LINE;

        SERVER_DISPLAY_REFRESH(server_amps, server_volts, (uint8_t)round((SERVER_STORE_AH.GET() / SERVER_BATTERY_CAPACITY) * 100.0f), SERVER_STORE_AH.GET(), SERVER_STORE_LAST24);
      }
      return;
    }
  }
}

#endif

