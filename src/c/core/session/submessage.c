#include <micrortps/client/core/session/submessage.h>
#include <micrortps/client/core/serialization/xrce_subheader.h>

#define FLAG_ENDIANNESS 1

//==================================================================
//                             PUBLIC
//==================================================================
bool write_submessage_header(mcMicroBuffer* mb, uint8_t submessage_id, uint16_t length, uint8_t flags)
{
    mc_align_to(mb, 4);
    mb->endianness = MC_MACHINE_ENDIANNESS;
    flags = (uint8_t)(flags | mb->endianness);
    mc_serialize_submessage_header(mb, submessage_id, flags, length);

    return mc_micro_buffer_remaining(mb) >= length;
}

bool read_submessage_header(mcMicroBuffer* mb, uint8_t* submessage_id, uint16_t* length, uint8_t* flags, uint8_t** payload_it)
{
    if(*payload_it != NULL)
    {
        mb->iterator = *payload_it + *length;
    }

    mc_align_to(mb, 4);
    bool ready_to_read = mc_micro_buffer_remaining(mb) >= SUBHEADER_SIZE;
    if(ready_to_read)
    {
        mc_deserialize_submessage_header(mb, submessage_id, flags, length);

        uint8_t endiannes_flag = *flags & FLAG_ENDIANNESS;
        *flags = (uint8_t)(*flags & ~endiannes_flag);
        mb->endianness = endiannes_flag ? MC_LITTLE_ENDIANNESS : MC_BIG_ENDIANNESS;

        ready_to_read = mc_micro_buffer_remaining(mb) >= *length;
        *payload_it = mb->iterator;
    }
    return ready_to_read;
}

size_t submessage_padding(size_t length)
{
    return (length % SUBHEADER_SIZE != 0) ? SUBHEADER_SIZE - (length % SUBHEADER_SIZE) : 0;
}
