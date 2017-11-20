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
// RF


// use this to test without actual network and client and server
//#define RF_NETWORK_SIMULATION

// uset his to use RadioHeada
#define RF_RADIO_HEAD

#if defined(RF_RADIO_HEAD)
#include <RH_NRF24.h>
#include <RHReliableDatagram.h>
#include <SPI.h> // Not actualy used but needed to compile

#if defined(MODULE_IS_SERVER)
RH_NRF24 RF_DRIVER(RF_CE_PIN, RF_CSN_PIN);
#endif

#if defined(MODULE_IS_CLIENT)
RH_NRF24 RF_DRIVER;
#endif

#define RF_BROADCAST_ADDRESS RH_BROADCAST_ADDRESS
#define RF_BROADCAST_HARD_RETRANSMIT_COUNT 5
#define RH_RELIABLE_TIMEOUT 100
#define RH_RETRANSMIT_COUNT 2
#define RF_HARD_RETRANSMIT_COUNT 2
#define RF_HARD_RETRANSMIT_DELAY 10

#if not defined(RF_NETWORK_SIMULATION)
#if defined(MODULE_IS_SERVER)
RHReliableDatagram RF_MANAGER = RHReliableDatagram(RF_DRIVER, 0);
#endif
#if defined(MODULE_IS_CLIENT)
RHReliableDatagram RF_MANAGER = RHReliableDatagram(RF_DRIVER, MODULE_HANDSHAKE_DELAY_INDEX);
#endif
#endif

#define RF_MAX_MESSAGE_LEN RH_NRF24_MAX_MESSAGE_LEN
#endif

#if defined(RF_NETWORK_SIMULATION)
#define RF_BROADCAST_ADDRESS 255
#define RF_MAX_MESSAGE_LEN 30
uint8_t RF_SIMULATION_BUF[RF_MAX_MESSAGE_LEN];
uint8_t RF_SIMULATION_SIZE;
uint8_t RF_SIMULATION_BUF_SLAVE[RF_MAX_MESSAGE_LEN];
uint8_t RF_SIMULATION_SIZE_SLAVE;
uint8_t RF_SIMULATION_BUF_SLAVE_SLAVE[RF_MAX_MESSAGE_LEN];
uint8_t RF_SIMULATION_SIZE_SLAVE_SLAVE;
#endif

uint8_t RF_LAST_RECEIVED_BUF[RF_MAX_MESSAGE_LEN];
uint8_t RF_LAST_RECEIVED_SIZE;

#if defined(MODULE_IS_CLIENT)
uint32_t RF_LAST_OUR_SERVER_PACKET_TIME;
//5 min
#define RF_MASTER_RESET_TIMEOUT 300000
#endif

inline void RF_INIT()
{
  RF_LAST_RECEIVED_SIZE = 0;

#if defined(MODULE_IS_CLIENT)
  RF_LAST_OUR_SERVER_PACKET_TIME = millis();
#endif

#if not defined(RF_NETWORK_SIMULATION)

#if defined(RF_RADIO_HEAD)

  RF_DRIVER.init();

  // use this if you want to have different speed / power then default
  if (!RF_DRIVER.setRF(RH_NRF24::DataRate::DataRate250kbps, RH_NRF24::TransmitPower::TransmitPower0dBm))
  {
    LOG64_SET(F("RF: INIT RadioHead - Cannot set rate and power"));
    LOG64_NEW_LINE;
  }

  RF_MANAGER.setRetries(RH_RETRANSMIT_COUNT);
  RF_MANAGER.setTimeout(RH_RELIABLE_TIMEOUT);
  if (!RF_MANAGER.init())
  {
    LOG64_SET(F("RF: INIT RadioHead FAILED!"));
    LOG64_NEW_LINE;
  }

  RF_MANAGER.resetRetransmissions();
  RF_DRIVER.available(); // trurns the driver ON
#endif
#endif

#if defined(RF_NETWORK_SIMULATION)
  RF_SIMULATION_SIZE = 0;
  RF_SIMULATION_SIZE_SLAVE = 0;
  RF_SIMULATION_SIZE_SLAVE_SLAVE = 0;
#endif

  LOG64_SET(F("RF: INIT"));
  LOG64_NEW_LINE;
}

inline bool RF_SEND_DATA(uint8_t buf[], uint8_t size, uint8_t to)
{
  bool ret = false;

#if not defined(RF_NETWORK_SIMULATION)

#if defined(RF_RADIO_HEAD)
  if (size >= RF_DRIVER.maxMessageLength())
#endif
  {
    LOG64_SET(F("RF: SEND : MESSAGE BIGGER THEN MAXIMUM SUPPORTED : SIZE["));
    LOG64_SET(size);
    LOG64_SET(F("MAXIMUM SUPPORTED["));
#if defined(RF_RADIO_HEAD)
    LOG64_SET(RF_DRIVER.maxMessageLength());
#endif
    LOG64_SET(F("]"));
    LOG64_NEW_LINE;
  }

#if defined(RF_RADIO_HEAD)

  if (to == RF_BROADCAST_ADDRESS)
  {
    for (uint8_t r = 0; r < RF_BROADCAST_HARD_RETRANSMIT_COUNT; r++)
    {
      RF_MANAGER.resetRetransmissions();
      RF_MANAGER.sendtoWait(buf, size, to);
      delay(RF_HARD_RETRANSMIT_DELAY);
    }
  }
  else
  {
    for (uint8_t r = 0; r < RF_HARD_RETRANSMIT_COUNT; r++)
    {
      RF_MANAGER.resetRetransmissions();
      if (!RF_MANAGER.sendtoWait(buf, size, to))
      {
        LOG64_SET(F("RF: SEND : MESSAGE HAS NOT BEEN RECEIVED : SIZE["));
        LOG64_SET(size);
        LOG64_SET(F("] RETRIES["));
        LOG64_SET(RF_MANAGER.retries());
        LOG64_SET(F("] : HARD RETRANSMIT ["));
        LOG64_SET(r);
        LOG64_SET(F("]"));
        LOG64_NEW_LINE;
      }
      else
      {
        ret = true;
        break;
      }

      delay(RF_HARD_RETRANSMIT_DELAY);
    }
  }
#endif

  LOG64_SET(F("RF: SEND SIZE["));
  LOG64_SET(size);
  LOG64_SET(F("]"));
  LOG64_NEW_LINE;

#endif

#if defined(RF_NETWORK_SIMULATION)
  if (RF_SIMULATION_SIZE == 0)
  {
    RF_SIMULATION_SIZE = size;
    memcpy(RF_SIMULATION_BUF, buf, size);

    ret = true;
  }
  else if (RF_SIMULATION_SIZE_SLAVE == 0)
  {
    RF_SIMULATION_SIZE_SLAVE = size;
    memcpy(RF_SIMULATION_BUF_SLAVE, buf, size);

    ret = true;
  }
  else if (RF_SIMULATION_SIZE_SLAVE_SLAVE == 0)
  {
    RF_SIMULATION_SIZE_SLAVE_SLAVE = size;
    memcpy(RF_SIMULATION_BUF_SLAVE_SLAVE, buf, size);

    ret = true;
  }
  else
  {
    return false;
  }
#endif

  return ret;
}

inline bool RF_CMP(uint8_t buf[], uint8_t & size, uint8_t buf_tmp[], uint8_t & size_tmp)
{
  if (size != size_tmp)
  {
    return false;
  }

  return (memcmp(buf, buf_tmp, size) == 0);
}

inline bool RF_RECEIVE_DATA(uint8_t buf[], uint8_t & size)
{
  // LOG64_SET(F("RF: RECEIVE_DATA: MILLIS["));
  // LOG64_SET(millis());
  // LOG64_SET(F("]"));
  // LOG64_NEW_LINE;

  uint8_t buf_tmp[RF_MAX_MESSAGE_LEN];
  uint8_t size_tmp = RF_MAX_MESSAGE_LEN;

#if not defined(RF_NETWORK_SIMULATION)

#if defined(RF_RADIO_HEAD)
  if (RF_MANAGER.recvfromAck(buf_tmp, &size_tmp))
#endif
  {

    if (RF_CMP(RF_LAST_RECEIVED_BUF, RF_LAST_RECEIVED_SIZE, buf_tmp, size_tmp))
    {
      LOG64_SET(F("RF: RECEIVE_DATA: DUPLICATE: SIZE["));
      LOG64_SET(size_tmp);
      LOG64_SET(F("]"));
      LOG64_NEW_LINE;

      return false;
    }

    MONITOR_UP();

    memcpy(RF_LAST_RECEIVED_BUF, buf_tmp, size_tmp);
    RF_LAST_RECEIVED_SIZE = size_tmp;

    memcpy(buf, buf_tmp, size_tmp);
    size = size_tmp;

    LOG64_SET(F("RF: RECEIVE_DATA: SIZE["));
    LOG64_SET(size_tmp);
    LOG64_SET(F("]"));
    LOG64_NEW_LINE;

    return true;
  }
#endif
#if defined(RF_NETWORK_SIMULATION)
  if (RF_SIMULATION_SIZE > 0)
  {
    size_tmp = RF_SIMULATION_SIZE;
    memcpy(buf_tmp, RF_SIMULATION_BUF, size_tmp);
    RF_SIMULATION_SIZE = 0;

    if (RF_CMP(RF_LAST_RECEIVED_BUF, RF_LAST_RECEIVED_SIZE, buf_tmp, size_tmp))
    {
      LOG64_SET(F("RF: RECEIVE_DATA: DUPLICATE: SIZE["));
      LOG64_SET(size_tmp);
      LOG64_SET(F("]"));
      LOG64_NEW_LINE;

      return false;
    }


    memcpy(RF_LAST_RECEIVED_BUF, buf_tmp, size_tmp);
    RF_LAST_RECEIVED_SIZE = size_tmp;

    memcpy(buf, buf_tmp, size_tmp);
    size = size_tmp;

    LOG64_SET(F("RF: RECEIVE_DATA: SIZE["));
    LOG64_SET(size_tmp);
    LOG64_SET(F("]"));
    LOG64_NEW_LINE;

    return true;
  }
  else if (RF_SIMULATION_SIZE_SLAVE > 0)
  {
    size_tmp = RF_SIMULATION_SIZE_SLAVE;
    memcpy(buf_tmp, RF_SIMULATION_BUF_SLAVE, size_tmp);
    RF_SIMULATION_SIZE_SLAVE = 0;

    if (RF_CMP(RF_LAST_RECEIVED_BUF, RF_LAST_RECEIVED_SIZE, buf_tmp, size_tmp))
    {
      LOG64_SET(F("RF: RECEIVE_DATA: DUPLICATE: SIZE["));
      LOG64_SET(size_tmp);
      LOG64_SET(F("]"));
      LOG64_NEW_LINE;

      return false;
    }

    memcpy(RF_LAST_RECEIVED_BUF, buf_tmp, size_tmp);
    RF_LAST_RECEIVED_SIZE = size_tmp;

    memcpy(buf, buf_tmp, size_tmp);
    size = size_tmp;

    LOG64_SET(F("RF: RECEIVE_DATA: SIZE["));
    LOG64_SET(size_tmp);
    LOG64_SET(F("]"));
    LOG64_NEW_LINE;

    return true;
  }
  else if (RF_SIMULATION_SIZE_SLAVE_SLAVE > 0)
  {
    size_tmp = RF_SIMULATION_SIZE_SLAVE_SLAVE;
    memcpy(buf_tmp, RF_SIMULATION_BUF_SLAVE_SLAVE, size_tmp);
    RF_SIMULATION_SIZE_SLAVE_SLAVE = 0;

    if (RF_CMP(RF_LAST_RECEIVED_BUF, RF_LAST_RECEIVED_SIZE, buf_tmp, size_tmp))
    {
      LOG64_SET(F("RF: RECEIVE_DATA: DUPLICATE: SIZE["));
      LOG64_SET(size_tmp);
      LOG64_SET(F("]"));
      LOG64_NEW_LINE;

      return false;
    }

    memcpy(RF_LAST_RECEIVED_BUF, buf_tmp, size_tmp);
    RF_LAST_RECEIVED_SIZE = size_tmp;

    memcpy(buf, buf_tmp, size_tmp);
    size = size_tmp;

    LOG64_SET(F("RF: RECEIVE_DATA: SIZE["));
    LOG64_SET(size_tmp);
    LOG64_SET(F("]"));
    LOG64_NEW_LINE;

    return true;
  }

#endif
  return false;
}

inline void RF_REINIT()
{
#if not defined(RF_NETWORK_SIMULATION)
  uint8_t buf[RF_MAX_MESSAGE_LEN];
  uint8_t size = RF_MAX_MESSAGE_LEN;
  RF_RECEIVE_DATA(buf, size);
#if defined(RF_RADIO_HEAD)
  RF_MANAGER.resetRetransmissions();
  RF_DRIVER.available(); // trurns the driver ON
#endif
#else
  RF_SIMULATION_SIZE = 0;
  RF_SIMULATION_SIZE_SLAVE = 0;
  RF_SIMULATION_SIZE_SLAVE_SLAVE = 0;
#endif
}

inline void RF_SEND_HANDSHAKE_REQUEST()
{

  uint8_t out_buf[RF_MAX_MESSAGE_LEN];
  uint8_t out_size = RF_MAX_MESSAGE_LEN;

  ID_GET_HANDSHAKE_REQUEST_PACKET(out_buf, out_size);

  RF_SEND_DATA(out_buf, out_size, RF_BROADCAST_ADDRESS);
}

inline void RF_SEND_RESET_REQUEST()
{
  uint8_t out_buf[RF_MAX_MESSAGE_LEN];
  uint8_t out_size = RF_MAX_MESSAGE_LEN;

  ID_GET_RESET_REQUEST_PACKET(out_buf, out_size);

  RF_SEND_DATA(out_buf, out_size, RF_BROADCAST_ADDRESS);
}


inline void RF_PROCESS(OPER_PACKET  & oper_packet, uint8_t oper)
{
#if defined(MODULE_IS_CLIENT)
  if (oper != OPER_NONE)
  {
    if (ID_SERVER_ID == oper_packet.base.server_id)
    {
      RF_LAST_OUR_SERVER_PACKET_TIME = millis();
    }
  }
#endif
  switch (oper)
  {
    case OPER_NONE :
      {
        LOG64_SET(F("RF: PROCESSING:  OPER_NONE"));
        LOG64_NEW_LINE;
      }; return;

#if defined(MODULE_IS_SERVER)
    case OPER_HANDSHAKE :
      {
        LOG64_SET(F("RF: PROCESSING:  OPER_HANDSHAKE"));
        LOG64_NEW_LINE;

        if (ID_SERVER_ID != oper_packet.handshake_packet.server_id)
        {
          LOG64_SET(F("RF: PROCESSING:  OPER_HANDSHAKE: HANDSHAKE NOT TO US"));
          LOG64_NEW_LINE;

          return;
        }

        for (uint8_t i = 0; i < MAX_CLIENTS; i++)
        {
          if (SERVER_STORE_CLIENT_ID[i] == 0xFFFF)
          {
            SERVER_STORE_CLIENT_ID[i] = oper_packet.handshake_packet.id;
            SERVER_STORE_CLIENT_SLAVE[i] = oper_packet.handshake_packet.slave;
            SERVER_STORE_CLIENT_INCLUDE[i] = oper_packet.handshake_packet.include;
            SERVER_STORE_CLIENT_TYPE[i] = oper_packet.handshake_packet.type;
            SERVER_STORE_CLIENT_DISPLAY[i] = oper_packet.handshake_packet.display;
            SERVER_STORE_CLIENT_INDEX[i] = oper_packet.handshake_packet.index;
            strcpy(SERVER_STORE_CLIENT_NAME[i], oper_packet.handshake_packet.name);

            //            LOG64_SET(F("RF: PROCESSING:  OPER_HANDSHAKE INDEX: "));
            //            LOG64_SET(i);
            //            LOG64_SET(F("] ID["));
            //            LOG64_SET(oper_packet.handshake_packet.id);
            //            LOG64_SET(F("] SLAVE["));
            //            LOG64_SET(oper_packet.handshake_packet.slave);
            //            LOG64_SET(F("] INCLUDE["));
            //            LOG64_SET(oper_packet.handshake_packet.include);
            //            LOG64_SET(F("] TYPE["));
            //            LOG64_SET(oper_packet.handshake_packet.type);
            //            LOG64_SET(F("] NAME["));
            //            LOG64_SET(oper_packet.handshake_packet.name);
            //            LOG64_SET(F("]"));
            //            LOG64_NEW_LINE;

            break;
          }
        }

      }; return;
    case OPER_DATA :
      {
        LOG64_SET(F("RF: PROCESSING:  OPER_DATA"));
        LOG64_NEW_LINE;

        if (ID_SERVER_ID != oper_packet.data_packet.server_id)
        {
          LOG64_SET(F("RF: PROCESSING:  OPER_DATA: DATA NOT TO US"));
          LOG64_NEW_LINE;

          return;
        }
        SERVER_STORE_PROCESS_DATA(oper_packet.data_packet.id, oper_packet.data_packet.amps, oper_packet.data_packet.volts, oper_packet.data_packet.seq, oper_packet.data_packet.ah);
      }; return;
#endif

#if defined(MODULE_IS_CLIENT)
    case OPER_HANDSHAKE_REQUEST :
      {
        LOG64_SET(F("RF: PROCESSING:  OPER_HANDSHAKE_REQUEST"));
        LOG64_NEW_LINE;

        if (ID_SERVER_ID == 0xFFFF)
        {
          ID_SET_SERVER_ID(oper_packet.handshake_request_packet.server_id);
        }

        if (ID_SERVER_ID != oper_packet.handshake_request_packet.server_id)
        {
          LOG64_SET(F("RF: PROCESSING:  OPER_HANDSHAKE_REQUEST : NOT FROM OUR SERVER"));
          LOG64_NEW_LINE;

          return;
        }

        delay(ID_DELAY);

        uint8_t out_buf[RF_MAX_MESSAGE_LEN];
        uint8_t out_size = RF_MAX_MESSAGE_LEN;

        ID_GET_HANDSHAKE_PACKET(out_buf, out_size);
        RF_SEND_DATA(out_buf, out_size, 0);

#if defined(MODULE_SLAVE_ENABLED)
        ID_GET_HANDSHAKE_PACKET(out_buf, out_size, 1);

        delay(RF_HARD_RETRANSMIT_DELAY);
        RF_SEND_DATA(out_buf, out_size, 0);
#endif

#if defined(MODULE_SLAVE_SLAVE_ENABLED)
        ID_GET_HANDSHAKE_PACKET(out_buf, out_size, 2);

        delay(RF_HARD_RETRANSMIT_DELAY);
        RF_SEND_DATA(out_buf, out_size, 0);
#endif
      }; return;
    case OPER_RESET_REQUEST :
      {
        LOG64_SET(F("RF: PROCESSING:  OPER_RESET_REQUEST"));
        LOG64_NEW_LINE;

        if (ID_SERVER_ID == oper_packet.reset_request_packet.server_id)
        {
          LOG64_SET(F("RF: PROCESSING:  EXECUTING RESET"));
          LOG64_NEW_LINE;

          ID_REINIT();
        }
        else
        {
          if (DO_EXECUTE(millis(), RF_LAST_OUR_SERVER_PACKET_TIME, RF_MASTER_RESET_TIMEOUT))
          {
            LOG64_SET(F("RF: PROCESSING:  RESET REQUEST NOT FROM OUR SERVER BUT MASTER RESET IS ENABLED"));
            LOG64_SET(F("RF: PROCESSING:  EXECUTING RESET"));
            LOG64_NEW_LINE;

            ID_REINIT();
          }
          else
          {
            LOG64_SET(F("RF: PROCESSING:  RESET REQUEST NOT FROM OUR SERVER"));
            LOG64_NEW_LINE;
          }
        }

      }; return;
    case OPER_DATA_REQUEST :
      {
        LOG64_SET(F("RF: PROCESSING:  OPER_DATA_REQUEST"));
        LOG64_NEW_LINE;

        if (ID_SERVER_ID != oper_packet.data_request_packet.server_id)
        {
          LOG64_SET(F("RF: PROCESSING:  OPER_DATA_REQUEST : NOT FROM OUR SERVER"));
          LOG64_NEW_LINE;

          return;
        }

        if (ID_ID != oper_packet.data_request_packet.id)
        {
          LOG64_SET(F("RF: PROCESSING:  OPER_DATA_REQUEST : NOT FOR US"));
          LOG64_NEW_LINE;

          return;
        }

        uint8_t out_buf[RF_MAX_MESSAGE_LEN];
        uint8_t out_size = RF_MAX_MESSAGE_LEN;

        float amps;
        float volts;
        FLOAT_FLOAT ah;

        float volts_slave;
        float amps_slave;
        FLOAT_FLOAT ah_slave;

        float volts_slave_slave;
        float amps_slave_slave;
        FLOAT_FLOAT ah_slave_slave;

        CLIENT_SENSORS_GET(amps, volts, ah, volts_slave, amps_slave, ah_slave, volts_slave_slave, amps_slave_slave, ah_slave_slave);

        //        LOG64_SET(F("RF: SENSORS_GET : AMPS["));
        //        LOG64_SET(amps);
        //        LOG64_SET(F("] VOLTS["));
        //        LOG64_SET(volts);
        //        LOG64_SET(F("] AH["));
        //        LOG64_SET(ah.GET());
        //
        //        LOG64_SET(F("] AMPS_SLAVE["));
        //        LOG64_SET(amps_slave);
        //        LOG64_SET(F("] VOLTS_SLAVE["));
        //        LOG64_SET(volts_slave);
        //        LOG64_SET(F("] AH_SLAVE["));
        //        LOG64_SET(ah_slave.GET());
        //
        //        LOG64_SET(F("] AMPS_SLAVE_SLAVE["));
        //        LOG64_SET(amps_slave_slave);
        //        LOG64_SET(F("] VOLTS_SLAVE_SLAVE["));
        //        LOG64_SET(volts_slave_slave);
        //        LOG64_SET(F("] AH_SLAVE_SLAVE["));
        //        LOG64_SET(ah_slave_slave.GET());
        //
        //        LOG64_SET(F("]"));
        //        LOG64_NEW_LINE;

        ID_GET_DATA_PACKET(out_buf, out_size, amps, volts, ah);
        RF_SEND_DATA(out_buf, out_size, 0);

#if defined(MODULE_SLAVE_ENABLED)
        ID_GET_DATA_PACKET(out_buf, out_size, amps_slave, volts_slave, ah_slave, ID_SLAVE_ID);

        delay(RF_HARD_RETRANSMIT_DELAY);
        RF_SEND_DATA(out_buf, out_size, 0);
#endif

#if defined(MODULE_SLAVE_SLAVE_ENABLED)
        ID_GET_DATA_PACKET(out_buf, out_size, amps_slave_slave, volts_slave_slave, ah_slave_slave, ID_SLAVE_SLAVE_ID);

        delay(RF_HARD_RETRANSMIT_DELAY);
        RF_SEND_DATA(out_buf, out_size, 0);
#endif

      }; return;
#endif
  }
}

inline void RF_SEND_DATA_REQUEST(uint16_t id, uint8_t to)
{
  uint8_t out_buf[RF_MAX_MESSAGE_LEN];
  uint8_t out_size = RF_MAX_MESSAGE_LEN;

  ID_GET_DATA_REQUEST_PACKET(out_buf, out_size, id);

  if (!RF_SEND_DATA(out_buf, out_size, to))
  {
    LOG64_SET(F("RF: DATA FREQUEST FAILED TO BE RECEIVED : SIMULATED WITH NULL VALUES"));
    LOG64_NEW_LINE;

    uint8_t out_buf[RF_MAX_MESSAGE_LEN];
    uint8_t out_size = RF_MAX_MESSAGE_LEN;

    float amps = MERIX_NOT_AVAILABLE;
    float volts = MERIX_NOT_AVAILABLE;
    FLOAT_FLOAT ah(0.0f);

    ID_GET_DATA_PACKET(out_buf, out_size, amps, volts, ah, id);

    OPER_PACKET oper_packet;
    uint8_t oper = OPER_PARSE_PACKET(out_buf, oper_packet);

    RF_PROCESS(oper_packet, oper);
  }
}


inline void RF_()
{
  uint8_t buf[RF_MAX_MESSAGE_LEN];
  uint8_t size = RF_MAX_MESSAGE_LEN;

  //LOG64_SET(F("RF: TRY RECEIVE DATA"));
  //LOG64_NEW_LINE;

  if (RF_RECEIVE_DATA(buf, size))
  {
    LOG64_SET(F("RF: DATA RECEIVED"));
    LOG64_NEW_LINE;

    OPER_PACKET oper_packet;
    uint8_t oper = OPER_PARSE_PACKET(buf, oper_packet);

    RF_PROCESS(oper_packet, oper);
  }
}



