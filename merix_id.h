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
// Generate ID for this board if not generated

#include <EEPROM.h>

uint16_t ID_ID;
#if defined(MODULE_SLAVE_ENABLED)
uint16_t ID_SLAVE_ID;
#endif
uint16_t ID_SERVER_ID;

#if defined(MODULE_IS_CLIENT)
uint16_t ID_DELAY;
#endif

// handshake delays based on prime numbers
const uint16_t ID_PRIME_DELAY[MAX_CLIENTS] PROGMEM = {0,
                                                      1000,
                                                      3000,
                                                      5000,
                                                      7000,
                                                      9000,
                                                      11000,
                                                      13000,
                                                      15000,
                                                      17000
                                                     };


inline void ID_SET_SERVER_ID(uint16_t server_id)
{
  ID_SERVER_ID = server_id;
  EEPROM.put(EEPROM_ADDRESS_SERVER_ID, ID_SERVER_ID);

  LOG64_SET(F("ID: SET_SERVER_ID: "));
  LOG64_SET(F("SERVER_ID["));
  LOG64_SET(ID_SERVER_ID);
  LOG64_SET(F("]"));
  LOG64_NEW_LINE;

}

inline void ID_REINIT()
{
  LOG64_SET(F("ID: REINIT"));
  LOG64_NEW_LINE;

  randomSeed(analogRead(0));

  ID_ID = (uint16_t)random(0xFFFFE);
  EEPROM.put(EEPROM_ADDRESS_ID, ID_ID);
#if defined(MODULE_SLAVE_ENABLED)
  ID_SLAVE_ID = (uint16_t)random(0xFFFFE);
  EEPROM.put(EEPROM_ADDRESS_SLAVE_ID, ID_SLAVE_ID);
#endif
#if defined(MODULE_IS_CLIENT)
  ID_SET_SERVER_ID(0xFFFF);
#endif
#if defined(MODULE_IS_SERVER)
  ID_SET_SERVER_ID(ID_ID);
#endif

#if defined(MODULE_IS_CLIENT)
  ID_DELAY = (uint16_t)pgm_read_word(&(ID_PRIME_DELAY[MODULE_HANDSHAKE_DELAY_INDEX]));
#endif
}

inline void ID_INIT()
{

  EEPROM.get(EEPROM_ADDRESS_ID, ID_ID);
#if defined(MODULE_SLAVE_ENABLED)
  EEPROM.get(EEPROM_ADDRESS_SLAVE_ID, ID_SLAVE_ID);
#endif
  EEPROM.get(EEPROM_ADDRESS_SERVER_ID, ID_SERVER_ID);

  if (ID_ID == 0xFFFF)
  {
    // get first time ID
    ID_REINIT();
  }
#if defined(MODULE_IS_CLIENT)
  else
  {
    ID_DELAY = (uint16_t)pgm_read_word(&(ID_PRIME_DELAY[MODULE_HANDSHAKE_DELAY_INDEX]));
  }
#endif;

  LOG64_SET(F("ID: INIT: ID["));
  LOG64_SET(ID_ID);
#if defined(MODULE_SLAVE_ENABLED)
  LOG64_SET(F("] SLAVE_ID["));
  LOG64_SET(ID_SLAVE_ID);
#endif
#if defined(MODULE_IS_CLIENT)
  LOG64_SET(F("] DELAY["));
  LOG64_SET(ID_DELAY);
#endif
  LOG64_SET(F("]"));
  LOG64_NEW_LINE;
}

inline void ID_GET_DATA_PACKET(uint8_t buf[], uint8_t & size, float amps, float volts, FLOAT_FLOAT ah, uint16_t ID = ID_ID)
{
  size = 0;

  uint16_t magic = OPER_MAGIC_GET();
  buf[size++] = ((uint8_t*)&magic)[0];
  buf[size++] = ((uint8_t*)&magic)[1];

  buf[size++] = OPER_DATA;
  buf[size++] = ((uint8_t*)&ID_SERVER_ID)[0];
  buf[size++] = ((uint8_t*)&ID_SERVER_ID)[1];

  buf[size++] = ((uint8_t*)&ID)[0];
  buf[size++] = ((uint8_t*)&ID)[1];

  memcpy((void*) & buf[size], &amps, sizeof(float));
  size += sizeof(float);
  memcpy((void*) & buf[size ], &volts, sizeof(float));
  size += sizeof(float);
  memcpy((void*) & buf[size], &ah, sizeof(FLOAT_FLOAT));
  size += sizeof(FLOAT_FLOAT);

  LOG64_SET(F("ID: GET_DATA_PACKET: OPER["));
  LOG64_SET(OPER_DATA);
  LOG64_SET(F("] SERVER_ID["));
  LOG64_SET(ID_SERVER_ID);
  LOG64_SET(F("] ID["));
  LOG64_SET(ID);
  LOG64_SET(F("] AMPS["));
  LOG64_SET(amps);
  LOG64_SET(F("] VOLTS["));
  LOG64_SET(volts);
  LOG64_SET(F("] AH["));
  LOG64_SET(ah.GET());
  LOG64_SET(F("]"));
  LOG64_NEW_LINE;
}

inline void ID_GET_DATA_REQUEST_PACKET(uint8_t buf[], uint8_t & size, uint16_t id)
{
  size = 0;

  uint16_t magic = OPER_MAGIC_GET();
  buf[size++] = ((uint8_t*)&magic)[0];
  buf[size++] = ((uint8_t*)&magic)[1];

  buf[size++] = OPER_DATA_REQUEST;

  buf[size++] = ((uint8_t*)&ID_SERVER_ID)[0];
  buf[size++] = ((uint8_t*)&ID_SERVER_ID)[1];

  buf[size++] = ((uint8_t*)&id)[0];
  buf[size++] = ((uint8_t*)&id)[1];

  LOG64_SET(F("ID: GET_DATA_REQUEST_PACKET: OPER["));
  LOG64_SET(OPER_DATA_REQUEST);
  LOG64_SET(F("] SERVER_ID["));
  LOG64_SET(ID_SERVER_ID);
  LOG64_SET(F("] ID["));
  LOG64_SET(id);
  LOG64_SET(F("]"));
  LOG64_NEW_LINE;
}

#if defined(MODULE_IS_CLIENT)
inline void ID_GET_HANDSHAKE_PACKET(uint8_t buf[], uint8_t & size, uint8_t slave = 0)
{
  size = 0;

  uint16_t magic = OPER_MAGIC_GET();
  buf[size++] = ((uint8_t*)&magic)[0];
  buf[size++] = ((uint8_t*)&magic)[1];

  buf[size++] = OPER_HANDSHAKE;
  buf[size++] = ((uint8_t*)&ID_SERVER_ID)[0];
  buf[size++] = ((uint8_t*)&ID_SERVER_ID)[1];

  if (slave == 0)
  {
    buf[size++] = ((uint8_t*)&ID_ID)[0];
    buf[size++] = ((uint8_t*)&ID_ID)[1];

    buf[size++] = slave;

    buf[size++] = MODULE_INCLUDE;
    buf[size++] = MODULE_TYPE;
    buf[size++] = MODULE_DISPLAY_TYPE;
    buf[size++] = MODULE_HANDSHAKE_DELAY_INDEX; // address
    uint8_t len = (uint8_t)strlen_P((PGM_P)MODULE_NAME);
    buf[size++] = len;
    memcpy_P(& buf[size], (PGM_P)MODULE_NAME, len);
    size += len;

    LOG64_SET(F("ID: GET_HANDSHAKE_PACKET: OPER["));
    LOG64_SET(OPER_HANDSHAKE);
    LOG64_SET(F("] SERVER_ID["));
    LOG64_SET(ID_SERVER_ID);
    LOG64_SET(F("] ID["));
    LOG64_SET(ID_ID);
    LOG64_SET(F("] SLAVE["));
    LOG64_SET(slave);
    LOG64_SET(F("] INCLUDE["));
    LOG64_SET(MODULE_INCLUDE);
    LOG64_SET(F("] TYPE["));
    LOG64_SET(MODULE_TYPE);
    LOG64_SET(F("] NAME["));
    LOG64_SET(MODULE_NAME);
    LOG64_SET(F("]"));
    LOG64_NEW_LINE;
  }
#if defined(MODULE_SLAVE_ENABLED)
  else
  {
    buf[size++] = ((uint8_t*)&ID_SLAVE_ID)[0];
    buf[size++] = ((uint8_t*)&ID_SLAVE_ID)[1];

    buf[size++] = slave;

    buf[size++] = MODULE_SLAVE_INCLUDE;
    buf[size++] = MODULE_SLAVE_TYPE;
    buf[size++] = MODULE_SLAVE_DISPLAY_TYPE;
    buf[size++] = 255; // no index/address for slave
    uint8_t len = (uint8_t)strlen_P((PGM_P)MODULE_SLAVE_NAME);
    buf[size++] = len;
    memcpy_P(& buf[size], (PGM_P)MODULE_SLAVE_NAME, len);
    size += len;

    LOG64_SET(F("ID: GET_HANDSHAKE_PACKET: OPER["));
    LOG64_SET(OPER_HANDSHAKE);
    LOG64_SET(F("] SERVER_ID["));
    LOG64_SET(ID_SERVER_ID);
    LOG64_SET(F("] ID["));
    LOG64_SET(ID_SLAVE_ID);
    LOG64_SET(F("] SLAVE["));
    LOG64_SET(slave);
    LOG64_SET(F("] INCLUDE["));
    LOG64_SET(MODULE_SLAVE_INCLUDE);
    LOG64_SET(F("] TYPE["));
    LOG64_SET(MODULE_SLAVE_TYPE);
    LOG64_SET(F("] NAME["));
    LOG64_SET(MODULE_SLAVE_NAME);
    LOG64_SET(F("]"));
    LOG64_NEW_LINE;
  }
#endif



}
#endif

inline void ID_GET_HANDSHAKE_REQUEST_PACKET(uint8_t buf[], uint8_t & size)
{
  size = 0;

  uint16_t magic = OPER_MAGIC_GET();
  buf[size++] = ((uint8_t*)&magic)[0];
  buf[size++] = ((uint8_t*)&magic)[1];

  buf[size++] = OPER_HANDSHAKE_REQUEST;

  buf[size++] = ((uint8_t*)&ID_SERVER_ID)[0];
  buf[size++] = ((uint8_t*)&ID_SERVER_ID)[1];

  LOG64_SET(F("ID: GET_HANDSHAKE_REQUEST_PACKET: OPER["));
  LOG64_SET(OPER_HANDSHAKE_REQUEST);
  LOG64_SET(F("] SERVER_ID["));
  LOG64_SET(ID_SERVER_ID);
  LOG64_SET(F("]"));
  LOG64_NEW_LINE;
}

inline void ID_GET_RESET_REQUEST_PACKET(uint8_t buf[], uint8_t & size)
{
  size = 0;

  uint16_t magic = OPER_MAGIC_GET();
  buf[size++] = ((uint8_t*)&magic)[0];
  buf[size++] = ((uint8_t*)&magic)[1];

  buf[size++] = OPER_RESET_REQUEST;

  buf[size++] = ((uint8_t*)&ID_SERVER_ID)[0];
  buf[size++] = ((uint8_t*)&ID_SERVER_ID)[1];

  LOG64_SET(F("ID: GET_RESET_REQUEST_PACKET: OPER["));
  LOG64_SET(OPER_RESET_REQUEST);
  LOG64_SET(F("] SERVER_ID["));
  LOG64_SET(ID_SERVER_ID);
  LOG64_SET(F("]"));
  LOG64_NEW_LINE;
}


