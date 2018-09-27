#include <micrortps/client/core/serialization/xrce_subheader.h>

//==================================================================
//                             PUBLIC
//==================================================================
void mc_serialize_submessage_header(mcBuffer* mb, uint8_t id, uint8_t flags, uint16_t length)
{
    (void) mc_serialize_uint8_t(mb, id);
    (void) mc_serialize_uint8_t(mb, flags);
    (void) mc_serialize_endian_uint16_t(mb, MC_LITTLE_ENDIANNESS, length);
}

void mc_deserialize_submessage_header(mcBuffer* mb, uint8_t* id, uint8_t* flags, uint16_t* length)
{
    (void) mc_deserialize_uint8_t(mb, id);
    (void) mc_deserialize_uint8_t(mb, flags);
    (void) mc_deserialize_endian_uint16_t(mb, MC_LITTLE_ENDIANNESS, length);
}
