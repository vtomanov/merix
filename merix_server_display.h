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
// Server display methods

///////////WIRING///////////////////////////////////////////////////////////////////
/*
  DISPLAY I2C->ARDUINO MEGA2560
  =====================
  VCC->VCC
  GND->GND
  SCL->SCL (pin 21)
  SDA->SDA (pin 20)
*/
////////////////////////////////////////////////////////////////////////////////////


#if defined(MODULE_IS_SERVER)

// use his is you wnat to simulate the display 20x4 in the log
//#define SERVER_DISPLAY_SIMULATION

#if defined(SERVER_DISPLAY_SIMULATION)
char SERVER_DISPLAY_SIMULATION_CACHE[4][21];
#endif

#if not defined(SERVER_DISPLAY_SIMULATION)
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Set the LCD I2C address
#endif

#define SERVER_DISPLAY_REFRESH_TIMEOUT  1000
uint32_t SERVER_DISPLAY_REFRESH_LAST_EXECUTE;

#define SERVER_DISPLAY_A24_TIME_TO_TOGGLE_TIMEOUT  5000
uint32_t SERVER_DISPLAY_A24_TIME_TO_TOGGLE_LAST_EXECUTE;
#define SERVER_DISPLAY_PERCENT_TO_DISCHARGE (50.0f)
#define SERVER_DISPLAY_PERCENT_TO_CHARGE (90.0f)
uint8_t SERVER_DISPLAY_A24_TIME_TO_TOGGLE;
uint8_t SERVER_DISPLAY_A24_TIME_TO_TOGGLE_PUBLISHED;
uint8_t SERVER_DISPLAY_TIME_TYPE;
float SERVER_DISPLAY_TIME;
uint8_t SERVER_DISPLAY_TIME_TYPE_PUBLISHED;
float SERVER_DISPLAY_TIME_PUBLISHED;

#define SERVER_DISPLAY_DATA_TIMEOUT  5000
uint32_t SERVER_DISPLAY_DATA_LAST_EXECUTE;
uint8_t SERVER_DISPLAY_INDEX;
uint8_t SERVER_DISPLAY_INDEX_PUBLISHED;

float SERVER_DISPLAY_AMPS;
float SERVER_DISPLAY_VOLTS;
uint8_t SERVER_DISPLAY_PERCENT;
float SERVER_DISPLAY_AH_AVAILABLE ;
float SERVER_DISPLAY_LAST24;

float SERVER_DISPLAY_AMPS_PUBLISHED;
float SERVER_DISPLAY_VOLTS_PUBLISHED;
uint8_t SERVER_DISPLAY_PERCENT_PUBLISHED;
float SERVER_DISPLAY_AH_AVAILABLE_PUBLISHED ;
float SERVER_DISPLAY_LAST24_PUBLISHED;

bool SERVER_DISPLAY_BACKLIGHT_ON;

#define SERVER_DISPLAY_ATTACHED_TIMEOUT  1000
uint32_t SERVER_DISPLAY_ATTACHED_LAST_EXECUTE;
uint8_t SERVER_DISPLAY_ATTACHED_INDEX;

//float SERVER_DISPLAY_REFRESH_AMPS_KALMAN_X;
//float SERVER_DISPLAY_REFRESH_AMPS_KALMAN_P;
//float SERVER_DISPLAY_REFRESH_AMPS_KALMAN_Q;
//float SERVER_DISPLAY_REFRESH_AMPS_KALMAN_R;
//
//float SERVER_DISPLAY_REFRESH_VOLTS_KALMAN_X;
//float SERVER_DISPLAY_REFRESH_VOLTS_KALMAN_P;
//float SERVER_DISPLAY_REFRESH_VOLTS_KALMAN_Q;
//float SERVER_DISPLAY_REFRESH_VOLTS_KALMAN_R;


#if defined(SERVER_DISPLAY_SIMULATION)
inline void SERVER_DISPLAY_PRINT_FINISH()
{
  LOG64_NEW_LINE;
  LOG64_SET(F("DISPLAY LINE 0:"));
  LOG64_SET(SERVER_DISPLAY_SIMULATION_CACHE[0]);
  LOG64_NEW_LINE;
  LOG64_SET(F("DISPLAY LINE 1:"));
  LOG64_SET(SERVER_DISPLAY_SIMULATION_CACHE[1]);
  LOG64_NEW_LINE;
  LOG64_SET(F("DISPLAY LINE 2:"));
  LOG64_SET(SERVER_DISPLAY_SIMULATION_CACHE[2]);
  LOG64_NEW_LINE;
  LOG64_SET(F("DISPLAY LINE 3:"));
  LOG64_SET(SERVER_DISPLAY_SIMULATION_CACHE[3]);
  LOG64_NEW_LINE;
  LOG64_NEW_LINE;
}
#endif

inline void SERVER_DISPLAY_CLEAR()
{
#if not defined(SERVER_DISPLAY_SIMULATION)
  lcd.clear();
#else
  for (int j = 0; j < 4; j++)
  {
    for (int i = 0; i < 20; i++)
    {
      SERVER_DISPLAY_SIMULATION_CACHE[j][i] = ' ';
    }
  }
#endif

}


inline void SERVER_DISPLAY_PRINT(uint8_t col, uint8_t row, String s)
{
#if not defined(SERVER_DISPLAY_SIMULATION)
  lcd.setCursor ( col, row );
  lcd.print(s.c_str());
#else
  for (int i = col; i < (col + s.length()); i++)
  {
    SERVER_DISPLAY_SIMULATION_CACHE[row][i] = s.c_str()[i - col];
  }
#endif
}

inline void SERVER_DISPLAY_RESET()
{

  //  SERVER_DISPLAY_REFRESH_AMPS_KALMAN_X = MERIX_NOT_AVAILABLE;
  //  SERVER_DISPLAY_REFRESH_AMPS_KALMAN_P = 0.0f;
  //  SERVER_DISPLAY_REFRESH_AMPS_KALMAN_Q = 0.125f;
  //  SERVER_DISPLAY_REFRESH_AMPS_KALMAN_R = 4.0f;
  //
  //  SERVER_DISPLAY_REFRESH_VOLTS_KALMAN_X = MERIX_NOT_AVAILABLE;
  //  SERVER_DISPLAY_REFRESH_VOLTS_KALMAN_P = 0.0f;
  //  SERVER_DISPLAY_REFRESH_VOLTS_KALMAN_Q = 0.125f;
  //  SERVER_DISPLAY_REFRESH_VOLTS_KALMAN_R = 4.0f ;

#if not defined(SERVER_DISPLAY_SIMULATION)
  lcd.clear();
#endif

#if defined(SERVER_DISPLAY_SIMULATION)
  for (int j = 0; j < 4; j++)
  {
    for (int i = 0; i < 20; i++)
    {
      SERVER_DISPLAY_SIMULATION_CACHE[j][i] = ' ';
    }

    SERVER_DISPLAY_SIMULATION_CACHE[j][21] = 0x0;
  }

#endif

  String v = String(Version_Major_Minor_Revision);
  for (int i = 0; v.length() < 20; i++)
  {
    if ((i % 2) == 0)
    {
      v = ' ' + v;
    }
    else
    {
      v += ' ';
    }
  }


  SERVER_DISPLAY_PRINT(0, 1, v);

#if defined(SERVER_DISPLAY_SIMULATION)
  SERVER_DISPLAY_PRINT_FINISH();
#endif

  SERVER_DISPLAY_AMPS = MERIX_NOT_AVAILABLE;
  SERVER_DISPLAY_VOLTS = MERIX_NOT_AVAILABLE;
  SERVER_DISPLAY_PERCENT = 255;
  SERVER_DISPLAY_AH_AVAILABLE = MERIX_NOT_AVAILABLE;
  SERVER_DISPLAY_LAST24 = 0.0f;

  SERVER_DISPLAY_AMPS_PUBLISHED = MERIX_NOT_AVAILABLE;
  SERVER_DISPLAY_VOLTS_PUBLISHED = MERIX_NOT_AVAILABLE;
  SERVER_DISPLAY_PERCENT_PUBLISHED = 255;
  SERVER_DISPLAY_AH_AVAILABLE_PUBLISHED = MERIX_NOT_AVAILABLE;
  SERVER_DISPLAY_LAST24_PUBLISHED = 0.0f;

  SERVER_DISPLAY_DATA_LAST_EXECUTE = millis();
  SERVER_DISPLAY_INDEX = 0;
  SERVER_DISPLAY_INDEX_PUBLISHED = MAX_CLIENTS;

  SERVER_DISPLAY_REFRESH_LAST_EXECUTE = millis();

  SERVER_DISPLAY_ATTACHED_LAST_EXECUTE = millis();
  SERVER_DISPLAY_ATTACHED_INDEX = 0;

  SERVER_DISPLAY_A24_TIME_TO_TOGGLE_LAST_EXECUTE = millis();
  SERVER_DISPLAY_A24_TIME_TO_TOGGLE = 0;
  SERVER_DISPLAY_A24_TIME_TO_TOGGLE_PUBLISHED = 0;
  SERVER_DISPLAY_TIME_TYPE = 0;
  SERVER_DISPLAY_TIME = 0.0f;
  SERVER_DISPLAY_TIME_TYPE_PUBLISHED = 0;
  SERVER_DISPLAY_TIME_PUBLISHED = 0.0f;

  SERVER_DISPLAY_BACKLIGHT_ON = true;

  LOG64_SET(F("SERVER_DISPLAY: RESET"));
  LOG64_NEW_LINE;
}

inline void SERVER_DISPLAY_INIT()
{
#if not defined(SERVER_DISPLAY_SIMULATION)
  // Switch on the backlight

  lcd.begin(20, 4);              // initialize the lcd
  lcd.home();                    // go home
#endif

  SERVER_DISPLAY_RESET();

  LOG64_SET(F("SERVER_DISPLAY: INIT"));
  LOG64_NEW_LINE;

}


inline void SERVER_DISPLAY_TOGLE_BACKLIGHT()
{
  if (SERVER_DISPLAY_BACKLIGHT_ON)
  {
    SERVER_DISPLAY_BACKLIGHT_ON = false;
    lcd.noBacklight();
  }
  else
  {
    SERVER_DISPLAY_BACKLIGHT_ON = true;
    lcd.backlight();
  }
}

inline void SERVER_DISPLAY_()
{
  if (SERVER_STATE == SERVER_STATE_HANDSHAKE)
  {
    if (DO_EXECUTE(millis(), SERVER_DISPLAY_ATTACHED_LAST_EXECUTE, SERVER_DISPLAY_ATTACHED_TIMEOUT))
    {
      SERVER_DISPLAY_ATTACHED_LAST_EXECUTE = millis();

      int16_t left_sec = (SERVER_STATE_HANDSHAKE_TIMEOUT - (millis() - SERVER_STATE_HANDSHAKE_LAST_EXECUTE)) / 1000;
      if (left_sec < 0)
      {
        left_sec = 0;
      }

      String s = String(F("["));
      s += left_sec;
      s += F("]S");
      for (; s.length() < 11; )
      {
        s = ' ' + s;
      }

      SERVER_DISPLAY_PRINT(9, 0, s);

      if ((SERVER_DISPLAY_ATTACHED_INDEX < MAX_CLIENTS) && (SERVER_STORE_CLIENT_ID[SERVER_DISPLAY_ATTACHED_INDEX] != 0xFFFF))
      {

        //01234567890123456789
        //ATTACHED:     [120S]
        //        [WATERMAKER]
        //ID:
        //             [65535]


        SERVER_DISPLAY_PRINT(0, 0, F("ATTACHED"));

        String n = String(SERVER_STORE_CLIENT_NAME[SERVER_DISPLAY_ATTACHED_INDEX]);
        n += ']';
        n = '[' + n;
        for (; n.length() < 20; )
        {
          n = ' ' + n;
        }
        SERVER_DISPLAY_PRINT(0, 1, n);
        // 01234567890123456789
        SERVER_DISPLAY_PRINT(0, 2, F("ID:                 "));

        String d = String(SERVER_STORE_CLIENT_ID[SERVER_DISPLAY_ATTACHED_INDEX]);
        d += ']';
        d = '[' + d;
        for (; d.length() < 20; )
        {
          d = ' ' + d;
        }
        SERVER_DISPLAY_PRINT(0, 3, d);

        SERVER_DISPLAY_ATTACHED_INDEX++;
      }
#if defined(SERVER_DISPLAY_SIMULATION)
      SERVER_DISPLAY_PRINT_FINISH();
#endif
    }

    return;
  }

  if (DO_EXECUTE(millis(), SERVER_DISPLAY_A24_TIME_TO_TOGGLE_LAST_EXECUTE, SERVER_DISPLAY_A24_TIME_TO_TOGGLE_TIMEOUT))
  {
    SERVER_DISPLAY_A24_TIME_TO_TOGGLE_LAST_EXECUTE = millis();

    if (SERVER_DISPLAY_A24_TIME_TO_TOGGLE == 0)
    {
      SERVER_DISPLAY_A24_TIME_TO_TOGGLE = 1;
    }
    else
    {
      SERVER_DISPLAY_A24_TIME_TO_TOGGLE = 0;
    }

    LOG64_SET(F("DISPLAY: TOGGLE["));
    LOG64_SET(SERVER_DISPLAY_A24_TIME_TO_TOGGLE);
    LOG64_SET(F("]"));
    LOG64_NEW_LINE;

  }


  bool data_changed = false;
  if (DO_EXECUTE(millis(), SERVER_DISPLAY_REFRESH_LAST_EXECUTE, SERVER_DISPLAY_REFRESH_TIMEOUT))
  {

    SERVER_DISPLAY_REFRESH_LAST_EXECUTE = millis();

    if (SERVER_STATE == SERVER_STATE_WORK)
    {

#if defined(SERVER_DISPLAY_SIMULATION)
      bool printed = false;
#endif

      //12.45V -324.14A 100%
      //01234567890123456789
      //TO50%:   20H 15M 03S
      // or
      //A:-123AH 24/H:-124AH

      if (SERVER_DISPLAY_VOLTS != SERVER_DISPLAY_VOLTS_PUBLISHED)
      {
        SERVER_DISPLAY_VOLTS_PUBLISHED = SERVER_DISPLAY_VOLTS;

        String v = (SERVER_DISPLAY_VOLTS <= 0) ? String(F("--.--")) : String(SERVER_DISPLAY_VOLTS , 2);
        v += 'V';
        for (; v.length() < 6; )
        {
          v = ' ' + v;
        }

        SERVER_DISPLAY_PRINT(0, 0, v);

        data_changed = true;

#if defined(SERVER_DISPLAY_SIMULATION)
        printed = true;
#endif
      }

      if (SERVER_DISPLAY_AMPS != SERVER_DISPLAY_AMPS_PUBLISHED)
      {
        SERVER_DISPLAY_AMPS_PUBLISHED = SERVER_DISPLAY_AMPS;

        String a = (SERVER_DISPLAY_AMPS <= MERIX_NOT_AVAILABLE) ? String(F("---.--")) : String(SERVER_DISPLAY_AMPS , 2);
        a += 'A';
        for (; a.length() < 9; )
        {
          a = ' ' + a;
        }

        SERVER_DISPLAY_PRINT(6, 0, a);

        data_changed = true;

#if defined(SERVER_DISPLAY_SIMULATION)
        printed = true;
#endif
      }

      // need to be displayed always as to recover from gauge switched off
      {
        SERVER_DISPLAY_PERCENT_PUBLISHED = SERVER_DISPLAY_PERCENT;

        String p = ((SERVER_DISPLAY_VOLTS <= 0) || (SERVER_DISPLAY_AMPS <= MERIX_NOT_AVAILABLE)) ? String(F("---")) :   String(SERVER_DISPLAY_PERCENT);
        p += '%';
        for (; p.length() < 5; )
        {
          p = ' ' + p;
        }

        SERVER_DISPLAY_PRINT(15, 0, p);
#if defined(SERVER_DISPLAY_SIMULATION)
        printed = true;
#endif
      }

      if (SERVER_DISPLAY_A24_TIME_TO_TOGGLE == 0)
      {

        if ((SERVER_DISPLAY_LAST24 != SERVER_DISPLAY_LAST24_PUBLISHED) || (SERVER_DISPLAY_A24_TIME_TO_TOGGLE_PUBLISHED != SERVER_DISPLAY_A24_TIME_TO_TOGGLE))
        {
          SERVER_DISPLAY_LAST24_PUBLISHED = SERVER_DISPLAY_LAST24;

          String l = String(SERVER_DISPLAY_LAST24, 0);
          l += "AH";
          for (; l.length() < 6; )
          {
            l = ' ' + l;
          }
          SERVER_DISPLAY_PRINT(8, 1, " 24/H:");
          SERVER_DISPLAY_PRINT(14, 1, l);
#if defined(SERVER_DISPLAY_SIMULATION)
          printed = true;
#endif
        }

        if ((SERVER_DISPLAY_AH_AVAILABLE != SERVER_DISPLAY_AH_AVAILABLE_PUBLISHED) || (SERVER_DISPLAY_A24_TIME_TO_TOGGLE_PUBLISHED != SERVER_DISPLAY_A24_TIME_TO_TOGGLE))
        {
          SERVER_DISPLAY_AH_AVAILABLE_PUBLISHED = SERVER_DISPLAY_AH_AVAILABLE;

          String h = String((SERVER_DISPLAY_AH_AVAILABLE <= 0.0f) ? 0 : round(SERVER_DISPLAY_AH_AVAILABLE));
          h += "AH";
          for (; h.length() < 6; )
          {
            h = ' ' + h;
          }
          SERVER_DISPLAY_PRINT(0, 1, "A:");
          SERVER_DISPLAY_PRINT(2, 1, h);
#if defined(SERVER_DISPLAY_SIMULATION)
          printed = true;
#endif
        }

        SERVER_DISPLAY_A24_TIME_TO_TOGGLE_PUBLISHED = SERVER_DISPLAY_A24_TIME_TO_TOGGLE;
      }
      else
      {
        if ((SERVER_DISPLAY_TIME_TYPE != SERVER_DISPLAY_TIME_TYPE_PUBLISHED) || (SERVER_DISPLAY_TIME != SERVER_DISPLAY_TIME_PUBLISHED) || (SERVER_DISPLAY_A24_TIME_TO_TOGGLE_PUBLISHED != SERVER_DISPLAY_A24_TIME_TO_TOGGLE))
        {
          if ((SERVER_STORE_AH.GET() != MERIX_NOT_AVAILABLE) && (SERVER_DISPLAY_AMPS != MERIX_NOT_AVAILABLE))
          {
            SERVER_DISPLAY_TIME_TYPE_PUBLISHED = SERVER_DISPLAY_TIME_TYPE;
            SERVER_DISPLAY_TIME_PUBLISHED = SERVER_DISPLAY_TIME;

            float time = (SERVER_DISPLAY_TIME_TYPE == 0) ?
                         SERVER_BATTERY_DATA_TO_DISCHARGE(SERVER_DISPLAY_AMPS * (-1) , SERVER_DISPLAY_PERCENT_TO_DISCHARGE) :
                         SERVER_BATTERY_DATA_TO_CHARGE(SERVER_DISPLAY_AMPS * SERVER_STORE_DATA_LOSS_COEF, SERVER_DISPLAY_PERCENT_TO_CHARGE);

            SERVER_DISPLAY_TIME = time;

            uint8_t h  = (uint8_t)time;
            float minutesRemainder = (time - ((float)h)) * 60.0f;
            uint8_t m = (uint8_t)minutesRemainder;
            float secondsRemainder = (minutesRemainder - ((float)m)) * 60.0f;
            uint8_t s = (uint8_t)secondsRemainder;

            String t = String(s);
            t += "S";
            for (; t.length() < 3; )
            {
              t = '0' + t;
            }
            t = String(m) + F("M ") + t;
            for (; t.length() < 7; )
            {
              t = '0' + t;
            }
            t = String(h) + F("H ") + t;
            for (; t.length() < 14; )
            {
              t = ' ' + t;
            }
            String b = String(F("TO"));
            b += (SERVER_DISPLAY_TIME_TYPE == 0) ? (uint8_t)SERVER_DISPLAY_PERCENT_TO_DISCHARGE : (uint8_t)SERVER_DISPLAY_PERCENT_TO_CHARGE;
            b += F("%:");
            SERVER_DISPLAY_PRINT(0, 1, b);
            SERVER_DISPLAY_PRINT(6, 1, t);

#if defined(SERVER_DISPLAY_SIMULATION)
            printed = true;
#endif
            SERVER_DISPLAY_A24_TIME_TO_TOGGLE_PUBLISHED = SERVER_DISPLAY_A24_TIME_TO_TOGGLE;
          }
        }
      }

#if defined(SERVER_DISPLAY_SIMULATION)
      if (printed)
      {
        SERVER_DISPLAY_PRINT_FINISH();
      }
#endif
    }

    bool do_data = false;
    if ((do_data = DO_EXECUTE(millis(), SERVER_DISPLAY_DATA_LAST_EXECUTE, SERVER_DISPLAY_DATA_TIMEOUT)) || data_changed)
    {
      if (do_data)
      {
        SERVER_DISPLAY_DATA_LAST_EXECUTE = millis();
      }

      //01234567890123456789
      //WATERMAKER        :3
      //12.45V -324.14A 100%

      uint8_t index_found = 255;
      if (do_data)
      {

        if ((++SERVER_DISPLAY_INDEX) >= MAX_CLIENTS)
        {
          SERVER_DISPLAY_INDEX = 0;
        }

        for (uint8_t i = SERVER_DISPLAY_INDEX; i < MAX_CLIENTS; i++)
        {
          if (SERVER_STORE_CLIENT_ID[i] != 0xFFFF)
          {
            index_found = i;
            break;
          }
        }

        if (index_found == 255)
        {
          for (uint8_t i = 0; i < SERVER_DISPLAY_INDEX; i++)
          {
            if (SERVER_STORE_CLIENT_ID[i] != 0xFFFF)
            {
              index_found = i;
              break;
            }
          }
        }
      }
      //    LOG64_SET(index_found);
      //    LOG64_NEW_LINE;

      if ((index_found != 255) || (data_changed))
      {
        if (index_found != 255)
        {
          SERVER_DISPLAY_INDEX = index_found;
          SERVER_DISPLAY_INDEX_PUBLISHED = SERVER_DISPLAY_INDEX;
        }

        String n = String(SERVER_STORE_CLIENT_NAME[SERVER_DISPLAY_INDEX]);
        for (; n.length() < 18; )
        {
          n += ' ';
        }
        n += ':';
        n += (SERVER_STORE_CLIENT_SEQ[SERVER_DISPLAY_INDEX] % 10);
        SERVER_DISPLAY_PRINT(0, 2, n);

        LOG64_SET("DISP:");
        LOG64_SET(SERVER_STORE_CLIENT_TYPE[SERVER_DISPLAY_INDEX]);
        LOG64_NEW_LINE;
        
        if ((SERVER_STORE_CLIENT_TYPE[SERVER_DISPLAY_INDEX] != 3) && (SERVER_STORE_CLIENT_TYPE[SERVER_DISPLAY_INDEX] != 4))
        {
          String v = (SERVER_STORE_CLIENT_VOLTS[SERVER_DISPLAY_INDEX] <= 0) ? String(F("--.--")) : String(SERVER_STORE_CLIENT_VOLTS[SERVER_DISPLAY_INDEX] , 2);
          v += 'V';
          for (; v.length() < 6; )
          {
            v = ' ' + v;
          }
          SERVER_DISPLAY_PRINT(0, 3, v);
        }
        else
        {
          SERVER_DISPLAY_PRINT(0, 3, String(F("      ")));
        }

        if (SERVER_STORE_CLIENT_TYPE[SERVER_DISPLAY_INDEX] != 2)
        {
          String a = (SERVER_STORE_CLIENT_AMPS[SERVER_DISPLAY_INDEX] <= MERIX_NOT_AVAILABLE) ? String(F("---.--")) : String(SERVER_STORE_CLIENT_AMPS[SERVER_DISPLAY_INDEX] , 2);
          a += 'A';
          for (; a.length() < 9; )
          {
            a = ' ' + a;
          }
          SERVER_DISPLAY_PRINT(6, 3, a);
        }
        else
        {
          SERVER_DISPLAY_PRINT(6, 3, String(F("         ")));
        }

        if ((SERVER_STORE_CLIENT_TYPE[SERVER_DISPLAY_INDEX] == 0) || (SERVER_STORE_CLIENT_TYPE[SERVER_DISPLAY_INDEX] == 4))
        {
          FLOAT_FLOAT percent_total = SERVER_STORE_TOTAL_PER_CLIENT[SERVER_DISPLAY_INDEX];
          if ((SERVER_STORE_TOTAL_DISCHARGED.GET() == 0.0f) && (SERVER_STORE_TOTAL_DISCHARGED.GET_LO() == 0.0f))
          {
            percent_total = FLOAT_FLOAT(100.0f);
          }
          else
          {
            percent_total.DIV(SERVER_STORE_TOTAL_DISCHARGED);
            percent_total.MUL(FLOAT_FLOAT(100.0f));
          }

          String p = String((abs(percent_total.GET()) > 100.0f) ? (uint8_t)100 : (uint8_t)round(abs(percent_total.GET())));
          p += '%';
          for (; p.length() < 5; )
          {
            p = ' ' + p;
          }

          SERVER_DISPLAY_PRINT(15, 3, p);
        }
        else
        {
          SERVER_DISPLAY_PRINT(15, 3, String(F("     ")));
        }

#if defined(SERVER_DISPLAY_SIMULATION)
        SERVER_DISPLAY_PRINT_FINISH();
#endif

      }
    }
  }
}

//// Kalman filtering
//// if x <= MERIX_NOT_AVAILABLE then assumed not intialized yet
//// based on : http://interactive-matter.eu/blog/2009/12/18/filtering-sensor-data-with-a-kalman-filter/
//inline void SERVER_DISPLAY_KALMAN(float & x, float & p, float & q, float & r, float & in_out)
//{
//  if (x <= MERIX_NOT_AVAILABLE)
//  {
//    x = in_out;
//    return;
//  }
//
//  // peform Kalman
//  p = p + q;
//  float k = p / (p + r);
//  x = x + k * (in_out - x);
//
//  p = (1.0f - k) * p;
//  in_out = x;
//
//}


inline void SERVER_DISPLAY_REFRESH(float amps, float volts, uint8_t percent, float ah_available, float last24[24])
{
  // perform kalman
  //  SERVER_DISPLAY_KALMAN(SERVER_DISPLAY_REFRESH_AMPS_KALMAN_X,
  //                        SERVER_DISPLAY_REFRESH_AMPS_KALMAN_P,
  //                        SERVER_DISPLAY_REFRESH_AMPS_KALMAN_Q,
  //                        SERVER_DISPLAY_REFRESH_AMPS_KALMAN_R,
  //                        amps);
  //
  //  SERVER_DISPLAY_KALMAN(SERVER_DISPLAY_REFRESH_VOLTS_KALMAN_X,
  //                        SERVER_DISPLAY_REFRESH_VOLTS_KALMAN_P,
  //                        SERVER_DISPLAY_REFRESH_VOLTS_KALMAN_Q,
  //                        SERVER_DISPLAY_REFRESH_VOLTS_KALMAN_R,
  //                        volts);

  SERVER_DISPLAY_AMPS = amps;
  SERVER_DISPLAY_VOLTS = volts;
  SERVER_DISPLAY_PERCENT = percent;
  SERVER_DISPLAY_AH_AVAILABLE = ah_available;

  if (amps < 0)
  {
    SERVER_DISPLAY_TIME_TYPE = 0;
  }
  else
  {
    SERVER_DISPLAY_TIME_TYPE = 1;
  }

    LOG64_SET(F("DISPLAY: REFRESH: AMPS["));
    LOG64_SET(amps);
    LOG64_SET(F("] VOLTS["));
    LOG64_SET(volts);
    LOG64_SET(F("] %["));
    LOG64_SET(percent);
    LOG64_SET(F("] AH["));
    LOG64_SET(ah_available);
    LOG64_SET(F("] 24["));
    LOG64_NEW_LINE;

  float l24 = 0.0f;
  for (int i = 0; i < 24; i++)
  {
    //LOG64_SET(SERVER_STORE_LAST24[i]);
    l24 += SERVER_STORE_LAST24[i];
  }
  SERVER_DISPLAY_LAST24 = l24;

  //LOG64_SET(F("] 24["));
  //LOG64_SET(SERVER_DISPLAY_LAST24);
  //LOG64_SET(F("]"));
  //LOG64_NEW_LINE;

}

#endif

