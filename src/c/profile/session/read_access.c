#include <micrortps/client/profile/session/read_access.h>
#include <micrortps/client/core/serialization/xrce_protocol.h>

void read_format_data(Session* session, MicroBuffer* submessage, StreamId stream_id);
void read_format_sample(Session* session, MicroBuffer* submessage, StreamId stream_id);
void read_format_data_seq(Session* session, MicroBuffer* submessage, StreamId stream_id);
void read_format_sample_seq(Session* session, MicroBuffer* submessage, StreamId stream_id);
void read_format_packed_samples(Session* session, MicroBuffer* submessage, StreamId stream_id);

void read_data_submessage(Session* session, MicroBuffer* submessage, StreamId stream_id, uint8_t format_flags)
{
    switch(format_flags)
    {
        case FORMAT_DATA:
            read_format_data(session, submessage, stream_id);
            break;

        case FORMAT_SAMPLE:
            read_format_sample(session, submessage, stream_id);
            break;

        case FORMAT_DATA_SEQ:
            read_format_data_seq(session, submessage, stream_id);
            break;

        case FORMAT_SAMPLE_SEQ:
            read_format_sample_seq(session, submessage, stream_id);
            break;

        case FORMAT_PACKED_SAMPLES:
            read_format_packed_samples(session, submessage, stream_id);
            break;

        default:
            break;
    }
}
