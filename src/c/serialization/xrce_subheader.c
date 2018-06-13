#include <micrortps/client/session/submessage.h>
#include <microcdr/microcdr.h>

void serialize_submessage_header(MicroBuffer* mb, const SubmessageHeader* input)
{
    (void) serialize_uint8_t(mb, input->id);
    (void) serialize_uint8_t(mb, input->flags);
    (void) serialize_endian_uint16_t(mb, LITTLE_ENDIANNESS, input->length);
}

void deserialize_submessage_header(MicroBuffer* mb, SubmessageHeader* output)
{
    (void) deserialize_uint8_t(mb, &output->id);
    (void) deserialize_uint8_t(mb, &output->flags);
    (void) deserialize_endian_uint16_t(mb, LITTLE_ENDIANNESS, &output->length);
}
