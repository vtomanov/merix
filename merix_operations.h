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
// Awailable operations

// General logic
// When switched ON server sends OPER_HANDSHAKE_REQUEST
// Clients replay with OPER_HANDSHAKE

// If button RESET is clicked on server - server sends OPER_RESET_REQUEST followed by OPER_HANDSHAKE_REQUEST


//counter
uint8_t OPER_MAGIC_COUNTER = 0;

//8606
inline uint16_t OPER_MAGIC_GET()
{
  uint16_t ret = 86;
  ret = ret << 8;
  ret += (OPER_MAGIC_COUNTER++);

  return ret;
}

// 8606
inline bool OPER_MAGIC_CHECK(uint16_t magic)
{
  uint8_t magic_8 = magic >> 8;
  return (magic_8 == 86);
}

#define OPER_NONE 0
#define OPER_HANDSHAKE 1
#define OPER_HANDSHAKE_REQUEST 2
#define OPER_RESET_REQUEST 3
#define OPER_DATA 4
#define OPER_DATA_REQUEST 5

struct OPER_BASE
{
  uint8_t seq;
  uint16_t server_id;
};

struct OPER_DATA_REQUEST_PACKET : OPER_BASE
{
  uint16_t id;
};

struct OPER_DATA_PACKET : OPER_BASE
{
  uint16_t id;
  float amps;
  float volts;
  FLOAT_FLOAT ah; // amps per hour till last data send
};

struct OPER_HANDSHAKE_PACKET : OPER_BASE
{
  uint16_t id;
  uint8_t slave;
  uint8_t include;
  uint8_t type;
  char name[21];
};

struct OPER_HANDSHAKE_REQUEST_PACKET : OPER_BASE
{
};

struct OPER_RESET_REQUEST_PACKET : OPER_BASE
{
};

union OPER_PACKET
{
  OPER_BASE base;
  OPER_HANDSHAKE_PACKET handshake_packet;
  OPER_HANDSHAKE_REQUEST_PACKET handshake_request_packet;
  OPER_RESET_REQUEST_PACKET reset_request_packet;
  OPER_DATA_REQUEST_PACKET data_request_packet;
  OPER_DATA_PACKET data_packet;

  OPER_PACKET() {};
};

inline uint8_t OPER_PARSE_PACKET(uint8_t buf[], OPER_PACKET & packet)
{
  uint8_t pos = 0;

  uint16_t magic;
  ((uint8_t*)&magic)[0] = buf[pos++];
  ((uint8_t*)&magic)[1] = buf[pos++];
  if (!OPER_MAGIC_CHECK(magic))
  {
    // not our packet
    return OPER_NONE;
  }

  uint8_t oper = buf[pos++];

  LOG64_SET(F("OPER: PARSE_PACKET: OPER["));
  LOG64_SET(oper);

  switch (oper)
  {
#if defined(MODULE_IS_CLIENT)
    case OPER_DATA_REQUEST :
      {
        packet.data_request_packet.seq = (uint8_t)magic;
        ((uint8_t*)&packet.data_request_packet.server_id)[0] = buf[pos++];
        ((uint8_t*)&packet.data_request_packet.server_id)[1] = buf[pos++];

        ((uint8_t*)&packet.data_request_packet.id)[0] = buf[pos++];
        ((uint8_t*)&packet.data_request_packet.id)[1] = buf[pos++];

        LOG64_SET(F("] SERVER_ID["));
        LOG64_SET(packet.data_request_packet.server_id);
        LOG64_SET(F("] ID["));
        LOG64_SET(packet.data_request_packet.id);

      }; break;
    case OPER_HANDSHAKE_REQUEST :
      {
        packet.handshake_request_packet.seq = (uint8_t)magic;
        ((uint8_t*)&packet.handshake_request_packet.server_id)[0] = buf[pos++];
        ((uint8_t*)&packet.handshake_request_packet.server_id)[1] = buf[pos++];

        LOG64_SET(F("] SERVER_ID["));
        LOG64_SET(packet.handshake_request_packet.server_id);

      }; break;

    case OPER_RESET_REQUEST :
      {
        packet.reset_request_packet.seq = (uint8_t)magic;
        ((uint8_t*)&packet.reset_request_packet.server_id)[0] = buf[pos++];
        ((uint8_t*)&packet.reset_request_packet.server_id)[1] = buf[pos++];

        LOG64_SET(F("] SERVER_ID["));
        LOG64_SET(packet.reset_request_packet.server_id);

      }; break;
#endif
#if defined(MODULE_IS_SERVER)
    case OPER_DATA :
      {
        packet.data_packet.seq = (uint8_t)magic;
        ((uint8_t*)&packet.data_packet.server_id)[0] = buf[pos++];
        ((uint8_t*)&packet.data_packet.server_id)[1] = buf[pos++];
        ((uint8_t*)&packet.data_packet.id)[0] = buf[pos++];
        ((uint8_t*)&packet.data_packet.id)[1] = buf[pos++];
        memcpy((void*)&packet.data_packet.amps, & buf[pos], sizeof(float));
        pos += sizeof(float);
        memcpy((void*)&packet.data_packet.volts, & buf[pos], sizeof(float));
        pos += sizeof(float);
        memcpy((void*)&packet.data_packet.ah, & buf[pos], sizeof(FLOAT_FLOAT));

        LOG64_SET(F("] SERVER_ID["));
        LOG64_SET(packet.data_packet.server_id);
        LOG64_SET(F("] ID["));
        LOG64_SET(packet.data_packet.id);
        LOG64_SET(F("] AMPS["));
        LOG64_SET(packet.data_packet.amps);
        LOG64_SET(F("] VOLTS["));
        LOG64_SET(packet.data_packet.volts);
        LOG64_SET(F("] AH["));
        LOG64_SET(packet.data_packet.ah.GET());

      }; break;
    case OPER_HANDSHAKE :
      {
        packet.handshake_packet.seq = (uint8_t)magic;
        ((uint8_t*)&packet.handshake_packet.server_id)[0] = buf[pos++];
        ((uint8_t*)&packet.handshake_packet.server_id)[1] = buf[pos++];
        ((uint8_t*)&packet.handshake_packet.id)[0] = buf[pos++];
        ((uint8_t*)&packet.handshake_packet.id)[1] = buf[pos++];
        packet.handshake_packet.slave = buf[pos++];
        packet.handshake_packet.include = buf[pos++];
        packet.handshake_packet.type = buf[pos++];
        uint8_t len = buf[pos++];
        memcpy(&packet.handshake_packet.name[0], & buf[pos], len);
        packet.handshake_packet.name[len] = 0;


        LOG64_SET(F("] SERVER_ID["));
        LOG64_SET(packet.handshake_packet.server_id);
        LOG64_SET(F("] ID["));
        LOG64_SET(packet.handshake_packet.id);
        LOG64_SET(F("] SLAVE["));
        LOG64_SET(packet.handshake_packet.slave);
        LOG64_SET(F("] INCLUDE["));
        LOG64_SET(packet.handshake_packet.include);
        LOG64_SET(F("] TYPE["));
        LOG64_SET(packet.handshake_packet.type);
        LOG64_SET(F("] NAME["));
        LOG64_SET(packet.handshake_packet.name);

      }; break;
#endif
  }

  LOG64_SET(F("]"));
  LOG64_NEW_LINE;

  return oper;
}

