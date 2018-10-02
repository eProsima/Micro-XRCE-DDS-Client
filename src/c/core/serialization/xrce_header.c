#include <micrortps/client/core/serialization/xrce_header.h>

//==================================================================
//                             PUBLIC
//==================================================================
void serialize_message_header(mcBuffer* mb, uint8_t session_id, uint8_t stream_id, uint16_t seq_num, const uint8_t* key)
{
    (void) mc_serialize_uint8_t(mb, session_id);
    (void) mc_serialize_uint8_t(mb, stream_id);
    (void) mc_serialize_endian_uint16_t(mb, MC_LITTLE_ENDIANNESS, seq_num);
    if(SESSION_ID_WITHOUT_CLIENT_KEY > session_id)
    {
        (void) mc_serialize_array_uint8_t(mb, key, CLIENT_KEY_SIZE);
    }
}

void deserialize_message_header(mcBuffer* mb, uint8_t* session_id, uint8_t* stream_id, uint16_t* seq_num, uint8_t* key)
{
    (void) mc_deserialize_uint8_t(mb, session_id);
    (void) mc_deserialize_uint8_t(mb, stream_id);
    (void) mc_deserialize_endian_uint16_t(mb, MC_LITTLE_ENDIANNESS, seq_num);
    if(SESSION_ID_WITHOUT_CLIENT_KEY > *session_id)
    {
        (void) mc_deserialize_array_uint8_t(mb, key, CLIENT_KEY_SIZE);
    }
}
