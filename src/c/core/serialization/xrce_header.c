#include <micrortps/client/core/serialization/xrce_header.h>

void serialize_message_header(MicroBuffer* mb, uint8_t session_id, uint8_t stream_id, uint16_t seq_num, const uint8_t* key)
{
    (void) serialize_uint8_t(mb, session_id);
    (void) serialize_uint8_t(mb, stream_id);
    (void) serialize_endian_uint16_t(mb, LITTLE_ENDIANNESS, seq_num);
    if(SESSION_ID_WITHOUT_CLIENT_KEY > session_id)
    {
        (void) serialize_array_uint8_t(mb, key, CLIENT_KEY_SIZE);
    }
}

void deserialize_message_header(MicroBuffer* mb, uint8_t* session_id, uint8_t* stream_id, uint16_t* seq_num, uint8_t* key)
{
    (void) deserialize_uint8_t(mb, session_id);
    (void) deserialize_uint8_t(mb, stream_id);
    (void) deserialize_endian_uint16_t(mb, LITTLE_ENDIANNESS, seq_num);
    if(SESSION_ID_WITHOUT_CLIENT_KEY > *session_id)
    {
        (void) deserialize_array_uint8_t(mb, key, CLIENT_KEY_SIZE);
    }
}
