#include <micrortps/client/serialization/xrce_header.h>
#include <microcdr/microcdr.h>

void serialize_message_header(MicroBuffer* mb, const MessageHeader* input)
{
    (void) serialize_uint8_t(mb, input->session_id);
    (void) serialize_uint8_t(mb, input->stream_id);
    (void) serialize_endian_uint16_t(mb, LITTLE_ENDIANNESS, input->sequence_nr);
    if(SESSION_ID_WITHOUT_CLIENT_KEY > input->session_id)
    {
        (void) serialize_array_uint8_t(mb, input->key, CLIENT_KEY_SIZE);
    }
}

void deserialize_message_header(MicroBuffer* mb, MessageHeader* output)
{
    (void) deserialize_uint8_t(mb, &output->session_id);
    (void) deserialize_uint8_t(mb, &output->stream_id);
    (void) deserialize_endian_uint16_t(mb, LITTLE_ENDIANNESS, &output->sequence_nr);
    if(SESSION_ID_WITHOUT_CLIENT_KEY > output->session_id)
    {
        (void) deserialize_array_uint8_t(mb, output->key, CLIENT_KEY_SIZE);
    }
}
