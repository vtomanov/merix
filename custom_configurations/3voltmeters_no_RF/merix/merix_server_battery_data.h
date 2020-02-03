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
// Server battery constants

#if defined(MODULE_IS_SERVER)

// Constant gerenation code in Java
/***************************************************************************************************
  public static boolean isFull(List<double[]> data)
  {
  for (int i = 0; i < data.size() - 1; i++)
  {
    if ((((int)data.get(i)[0]) - 1) == ((int)data.get(i + 1)[0]))
    {
      continue;
    }

    return false;
  }

  return true;
  }

  public static void main(String[] args)
  {

  double[][] init = {{100.0, 12.78333333},
    {75.0, 12.55},
    {50.0, 12.28333333},
    {25.0, 12.0},
    {0.0, 11.8}
  };


  List<double[]> data = new ArrayList<double[]>();
  for (int i = 0; i < init.length; i++)
  {
    data.add(init[i]);
  }



  for (; !isFull(data);)
  {

    for (int i = 0; i < data.size() - 1; i++)
    {
      double newPercent = (((int)data.get(i)[0]) + ((int)data.get(i + 1)[0])) / 2 ;
      double newVoltage = ((data.get(i)[1]) + (data.get(i + 1)[1])) / 2 ;
      double[] newData = { newPercent, newVoltage};
      int index = Collections.binarySearch(data, newData , new Comparator<double[]>()
      {
        @Override
        public int compare(double[] o1, double[] o2)
        {
          return Double.compare(o2[0], o1[0]);
        }
      });

      if (index < 0)
      {
        data.add((index + 1) * (-1), newData);
      }
    }
  }

  System.out.println("const uint8_t SERVER_BATTERY_DATA_PERCENT[101] PROGMEM = {");
  for (int i = 0; i < data.size(); i++)
  {
    System.out.print((int) data.get(i)[0]);
    if (i < (data.size() - 1))
    {
      System.out.println(",");
    }
    else
    {
      System.out.println();
    }


  }
  System.out.println("};");

  System.out.println();

  System.out.println("const uint8_t SERVER_BATTERY_DATA_VOLTAGE[101][4] PROGMEM = {");
  for (int i = 0; i < data.size(); i++)
  {
    byte[] buf = new byte[4];
    ByteBuffer.wrap(buf).order(ByteOrder.LITTLE_ENDIAN).putFloat((float)data.get(i)[1]);
    System.out.print("{" + (buf[0] & 0xFF) + "," + (buf[1] & 0xFF) + "," + (buf[2] & 0xFF) + "," + (buf[3] & 0xFF) + "}");

    if (i < (data.size() - 1))
    {
      System.out.println(", //" + new DecimalFormat("#.0000").format(data.get(i)[1]));
    }
    else
    {
      System.out.println("  //" + new DecimalFormat("#.0000").format(data.get(i)[1]));
    }

  }
  System.out.println("};");
  }
***************************************************************************************************/


const uint8_t SERVER_BATTERY_DATA_PERCENT[101] PROGMEM = {
  100,
  99,
  98,
  97,
  96,
  95,
  94,
  93,
  92,
  91,
  90,
  89,
  88,
  87,
  86,
  85,
  84,
  83,
  82,
  81,
  80,
  79,
  78,
  77,
  76,
  75,
  74,
  73,
  72,
  71,
  70,
  69,
  68,
  67,
  66,
  65,
  64,
  63,
  62,
  61,
  60,
  59,
  58,
  57,
  56,
  55,
  54,
  53,
  52,
  51,
  50,
  49,
  48,
  47,
  46,
  45,
  44,
  43,
  42,
  41,
  40,
  39,
  38,
  37,
  36,
  35,
  34,
  33,
  32,
  31,
  30,
  29,
  28,
  27,
  26,
  25,
  24,
  23,
  22,
  21,
  20,
  19,
  18,
  17,
  16,
  15,
  14,
  13,
  12,
  11,
  10,
  9,
  8,
  7,
  6,
  5,
  4,
  3,
  2,
  1,
  0
};

const uint8_t SERVER_BATTERY_DATA_VOLTAGE[101][4] PROGMEM = {
  {137, 136, 76, 65}, //12.7833
  {171, 106, 76, 65}, //12.7760
  {205, 76, 76, 65}, //12.7687
  {239, 46, 76, 65}, //12.7615
  {17, 17, 76, 65}, //12.7542
  {51, 243, 75, 65}, //12.7469
  {85, 213, 75, 65}, //12.7396
  {154, 153, 75, 65}, //12.7250
  {188, 123, 75, 65}, //12.7177
  {222, 93, 75, 65}, //12.7104
  {34, 34, 75, 65}, //12.6958
  {68, 4, 75, 65}, //12.6885
  {102, 230, 74, 65}, //12.6812
  {171, 170, 74, 65}, //12.6667
  {205, 140, 74, 65}, //12.6594
  {239, 110, 74, 65}, //12.6521
  {51, 51, 74, 65}, //12.6375
  {85, 21, 74, 65}, //12.6302
  {119, 247, 73, 65}, //12.6229
  {188, 187, 73, 65}, //12.6083
  {222, 157, 73, 65}, //12.6010
  {0, 128, 73, 65}, //12.5937
  {68, 68, 73, 65}, //12.5792
  {102, 38, 73, 65}, //12.5719
  {137, 8, 73, 65}, //12.5646
  {205, 204, 72, 65}, //12.5500
  {171, 170, 72, 65}, //12.5417
  {137, 136, 72, 65}, //12.5333
  {102, 102, 72, 65}, //12.5250
  {68, 68, 72, 65}, //12.5167
  {34, 34, 72, 65}, //12.5083
  {0, 0, 72, 65}, //12.5000
  {188, 187, 71, 65}, //12.4833
  {154, 153, 71, 65}, //12.4750
  {119, 119, 71, 65}, //12.4667
  {51, 51, 71, 65}, //12.4500
  {17, 17, 71, 65}, //12.4417
  {239, 238, 70, 65}, //12.4333
  {171, 170, 70, 65}, //12.4167
  {137, 136, 70, 65}, //12.4083
  {102, 102, 70, 65}, //12.4000
  {34, 34, 70, 65}, //12.3833
  {0, 0, 70, 65}, //12.3750
  {222, 221, 69, 65}, //12.3667
  {154, 153, 69, 65}, //12.3500
  {119, 119, 69, 65}, //12.3417
  {85, 85, 69, 65}, //12.3333
  {17, 17, 69, 65}, //12.3167
  {239, 238, 68, 65}, //12.3083
  {205, 204, 68, 65}, //12.3000
  {137, 136, 68, 65}, //12.2833
  {68, 100, 68, 65}, //12.2745
  {0, 64, 68, 65}, //12.2656
  {188, 27, 68, 65}, //12.2568
  {119, 247, 67, 65}, //12.2479
  {51, 211, 67, 65}, //12.2391
  {239, 174, 67, 65}, //12.2302
  {102, 102, 67, 65}, //12.2125
  {34, 66, 67, 65}, //12.2036
  {222, 29, 67, 65}, //12.1948
  {85, 213, 66, 65}, //12.1771
  {17, 177, 66, 65}, //12.1682
  {205, 140, 66, 65}, //12.1594
  {68, 68, 66, 65}, //12.1417
  {0, 32, 66, 65}, //12.1328
  {188, 251, 65, 65}, //12.1240
  {51, 179, 65, 65}, //12.1062
  {239, 142, 65, 65}, //12.0974
  {171, 106, 65, 65}, //12.0885
  {34, 34, 65, 65}, //12.0708
  {222, 253, 64, 65}, //12.0620
  {154, 217, 64, 65}, //12.0531
  {17, 145, 64, 65}, //12.0354
  {205, 108, 64, 65}, //12.0266
  {137, 72, 64, 65}, //12.0177
  {0, 0, 64, 65}, //12.0000
  {102, 230, 63, 65}, //11.9938
  {205, 204, 63, 65}, //11.9875
  {51, 179, 63, 65}, //11.9812
  {154, 153, 63, 65}, //11.9750
  {0, 128, 63, 65}, //11.9688
  {102, 102, 63, 65}, //11.9625
  {51, 51, 63, 65}, //11.9500
  {154, 25, 63, 65}, //11.9438
  {0, 0, 63, 65}, //11.9375
  {205, 204, 62, 65}, //11.9250
  {51, 179, 62, 65}, //11.9188
  {154, 153, 62, 65}, //11.9125
  {102, 102, 62, 65}, //11.9000
  {205, 76, 62, 65}, //11.8938
  {51, 51, 62, 65}, //11.8875
  {0, 0, 62, 65}, //11.8750
  {102, 230, 61, 65}, //11.8688
  {205, 204, 61, 65}, //11.8625
  {154, 153, 61, 65}, //11.8500
  {0, 128, 61, 65}, //11.8438
  {102, 102, 61, 65}, //11.8375
  {51, 51, 61, 65}, //11.8250
  {154, 25, 61, 65}, //11.8188
  {0, 0, 61, 65}, //11.8125
  {205, 204, 60, 65} //11.8000
};

inline uint8_t SERVER_BATTERY_DATA_FROM_VOLTAGE(float voltage)
{
  for (uint8_t i = 0; i < 101; i++)
  {
    float voltage_at_index;
    ((uint8_t *)&voltage_at_index)[0] = (uint8_t)pgm_read_byte(&(SERVER_BATTERY_DATA_VOLTAGE[i][0]));
    ((uint8_t *)&voltage_at_index)[1] = (uint8_t)pgm_read_byte(&(SERVER_BATTERY_DATA_VOLTAGE[i][1]));
    ((uint8_t *)&voltage_at_index)[2] = (uint8_t)pgm_read_byte(&(SERVER_BATTERY_DATA_VOLTAGE[i][2]));
    ((uint8_t *)&voltage_at_index)[3] = (uint8_t)pgm_read_byte(&(SERVER_BATTERY_DATA_VOLTAGE[i][3]));
    if (voltage >= voltage_at_index)
    {
      return (uint8_t)pgm_read_byte(&(SERVER_BATTERY_DATA_PERCENT[i]));
    }
  }

  return (uint8_t)pgm_read_byte(&(SERVER_BATTERY_DATA_PERCENT[100]));
}

// do not call with < 0 discharging
inline float SERVER_BATTERY_DATA_TO_DISCHARGE(float discharge_amps, float to_percent)
{
  float to = SERVER_STORE_AH.GET() - (SERVER_BATTERY_CAPACITY * to_percent / 100.0f);
  if (to <= 0.0f)
  {
    return 0.0f;
  }

  //LOG64_SET(F("BATTERY_DATA  : TO_DISCHARGE : AMPS["));
  //LOG64_SET(discharge_amps);
  //LOG64_SET(F("] %["));
  //LOG64_SET(to_percent);
  //LOG64_SET(F("] time["));
  //LOG64_SET(to / discharge_amps);
  //LOG64_SET(F("] AH["));
  //LOG64_SET(SERVER_STORE_AH.GET());
  //LOG64_SET(F("]"));
  //LOG64_NEW_LINE;

  return to / discharge_amps;
}
// the discharging rate should be with adjusted with the loss coef beforehand
// do not call with < 0 charging
inline float SERVER_BATTERY_DATA_TO_CHARGE(float charge_amps, float to_percent)
{
  float ret = 0.0f;

  //LOG64_SET(F("BATTERY_DATA  : TO_CHARGE : AMPS["));
  //LOG64_SET(charge_amps);
  //LOG64_SET(F("] %["));
  //LOG64_SET(to_percent);

  if (charge_amps < 1.0f)
  {
    ret = (SERVER_BATTERY_CAPACITY * to_percent / 100.0f) - SERVER_STORE_AH.GET();

    //LOG64_SET(F("] AH["));
    //LOG64_SET(SERVER_STORE_AH.GET());
    //LOG64_SET(F("]"));
    //LOG64_NEW_LINE;

    return (ret < 0) ? 0.0f : ret;
  }

  float capacity = SERVER_BATTERY_CAPACITY * to_percent / 100.0f;

  if (SERVER_STORE_AH.GET() <= (SERVER_BATTERY_CAPACITY * 0.7f))//the first 70% of the batt can be charged with constant high current
  {
    if (to_percent <= 70)//if we want to charge less then those 70% we can directly charge till our target with constant high current
    {
      ret = (capacity - SERVER_STORE_AH.GET()) / charge_amps; //the time needed to charge our target percent is the remaining capacity to charge divided by the current(constant high current)
      if (ret < 0.0f)
      {
        ret = 0.0f;
      }

      //LOG64_SET(F("] time["));
      //LOG64_SET(ret);
      //LOG64_SET(F("] AH["));
      //LOG64_SET(SERVER_STORE_AH.GET());
      //LOG64_SET(F("]"));
      //LOG64_NEW_LINE;

      return ret;
    }

    //we want to charge to more then 70% - so we can charge till 70% using this constant high current and the rest  using the cycle
    ret = ((SERVER_BATTERY_CAPACITY * 0.7f) - SERVER_STORE_AH.GET()) / charge_amps;
    capacity = capacity - (SERVER_BATTERY_CAPACITY * 0.7f);
    if (charge_amps > (capacity * 0.25f))
    {
      charge_amps = capacity * 0.25f;
    }
  }
  else
  {
    capacity = capacity - SERVER_STORE_AH.GET();

    if (capacity <= 0.0f)
    {
      //LOG64_SET(F("] time["));
      //LOG64_SET(0.0f);
      //LOG64_SET(F("] AH["));
      //LOG64_SET(SERVER_STORE_AH.GET());
      //LOG64_SET(F("]"));
      //LOG64_NEW_LINE;

      return 0.0f;
    }

    if (charge_amps > (capacity * 0.25f))
    {
      charge_amps = capacity * 0.25f;
    }

  }

  for (float charged = 0.0f, to_capacity = capacity; (charge_amps > 1.0f) || (charged <= to_capacity); )
  {
    ret += capacity * 0.7f / charge_amps;
    charged += (ret * charge_amps);
    capacity = capacity * 0.3f;
    if (charge_amps > (capacity * 0.25f))
    {
      charge_amps = capacity * 0.25f;
    }
  }

  //LOG64_SET(F("] time["));
  //LOG64_SET(ret);
  //LOG64_SET(F("] AH["));
  //LOG64_SET(SERVER_STORE_AH.GET());
  //LOG64_SET(F("]"));
  //LOG64_NEW_LINE;

  return ret;
}

#endif

