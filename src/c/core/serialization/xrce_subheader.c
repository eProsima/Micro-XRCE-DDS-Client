#include <micrortps/client/core/serialization/xrce_subheader.h>

//==================================================================
//                             PUBLIC
//==================================================================
void serialize_submessage_header(ucdrBuffer* mb, uint8_t id, uint8_t flags, uint16_t length)
{
    (void) ucdr_serialize_uint8_t(mb, id);
    (void) ucdr_serialize_uint8_t(mb, flags);
    (void) ucdr_serialize_endian_uint16_t(mb, UCDR_LITTLE_ENDIANNESS, length);
}

void deserialize_submessage_header(ucdrBuffer* mb, uint8_t* id, uint8_t* flags, uint16_t* length)
{
    (void) ucdr_deserialize_uint8_t(mb, id);
    (void) ucdr_deserialize_uint8_t(mb, flags);
    (void) ucdr_deserialize_endian_uint16_t(mb, UCDR_LITTLE_ENDIANNESS, length);
}
