#include <micrortps/client/core/session/submessage.h>
#include <micrortps/client/core/serialization/xrce_subheader.h>

#define FLAG_ENDIANNESS 1

//==================================================================
//                             PUBLIC
//==================================================================
bool write_submessage_header(MicroBuffer* mb, uint8_t submessage_id, uint16_t length, uint8_t flags)
{
    align_to(mb, 4);
    mb->endianness = MACHINE_ENDIANNESS;
    flags |= mb->endianness;
    serialize_submessage_header(mb, submessage_id, flags, length);

    return micro_buffer_remaining(mb) >= length;
}

bool read_submessage_header(MicroBuffer* mb, uint8_t* submessage_id, uint16_t* length, uint8_t* flags)
{
    align_to(mb, 4);
    bool ready_to_read = micro_buffer_remaining(mb) >= SUBHEADER_SIZE;
    if(ready_to_read)
    {
        deserialize_submessage_header(mb, submessage_id, flags, length);

        uint8_t endiannes_flag = *flags & FLAG_ENDIANNESS;
        *flags &= ~endiannes_flag;
        mb->endianness = endiannes_flag ? LITTLE_ENDIANNESS : BIG_ENDIANNESS;

        ready_to_read = micro_buffer_remaining(mb) >= *length;
    }
    return ready_to_read;
}

size_t submessage_padding(size_t length)
{
    return (length % 4 != 0) ? 4 - (length % 4) : 0;
}
