#include <micrortps/client/core/serialization/xrce_subheader.h>

//==================================================================
//                             PUBLIC
//==================================================================
void serialize_submessage_header(MicroBuffer* mb, uint8_t id, uint8_t flags, uint16_t length)
{
    (void) serialize_uint8_t(mb, id);
    (void) serialize_uint8_t(mb, flags);
    (void) serialize_endian_uint16_t(mb, LITTLE_ENDIANNESS, length);
}

void deserialize_submessage_header(MicroBuffer* mb, uint8_t* id, uint8_t* flags, uint16_t* length)
{
    (void) deserialize_uint8_t(mb, id);
    (void) deserialize_uint8_t(mb, flags);
    (void) deserialize_endian_uint16_t(mb, LITTLE_ENDIANNESS, length);
}
