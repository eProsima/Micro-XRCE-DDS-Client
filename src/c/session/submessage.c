#include <micrortps/client/session/submessage.h>
#include <microcdr/microcdr.h>

#define FLAG_ENDIANNESS 1

void write_submessage_header(MicroBuffer* mb, SubmessageId submessage_id, uint16_t length, SubmessageFlags flags)
{
    align_to(mb, 4);
    mb->endianness = (flags & FLAG_ENDIANNESS) ? LITTLE_ENDIANNESS : BIG_ENDIANNESS;
    serialize_submessage_header(mb, submessage_id, flags, length);
}

bool read_submessage_header(MicroBuffer* mb, SubmessageId* submessage_id, uint16_t* length, SubmessageFlags* flags)
{
    align_to(mb, 4);
    bool ready_to_read = micro_buffer_remaining(mb) >= SUBHEADER_SIZE;
    if(ready_to_read)
    {
        deserialize_submessage_header(mb, (uint8_t*)submessage_id, (uint8_t*)flags, length);
        uint8_t endiannes_flag = *flags & FLAG_ENDIANNESS;
        *flags &= ~endiannes_flag;
        mb->endianness = endiannes_flag ? LITTLE_ENDIANNESS : BIG_ENDIANNESS;
    }
    return ready_to_read;
}
