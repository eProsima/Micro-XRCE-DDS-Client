#include <micrortps/client/core/session/output_reliable_stream.h>
#include <micrortps/client/core/session/submessage.h>
#include <micrortps/client/core/serialization/xrce_protocol.h>
#include <microcdr/microcdr.h>

// Remove when Microcdr supports size_of functions
#define HEARTBEAT_PAYLOAD_SIZE 4

#define MIN_HEARTBEAT_TIME_INTERVAL_NS 1000000

void process_acknack(OutputReliableStream* stream, uint16_t bitmap, uint16_t first_unacked_seq_num);

//==================================================================
//                             PUBLIC
//==================================================================
void init_output_reliable_stream(OutputReliableStream* stream, uint8_t* buffer, size_t size, size_t history, uint8_t offset)
{
    stream->buffer = buffer;
    stream->writer = offset;
    stream->size = size;
    stream->history = history;
    stream->offset = offset;

    stream->last_written = UINT16_MAX;
    stream->last_sent = UINT16_MAX;
    stream->last_acknown = UINT16_MAX;

    stream->nack_bitmap = 0;
    stream->next_heartbeat_time_stamp = 0;
    stream->next_heartbeat_tries = 0;
}

bool prepare_next_reliable_buffer_to_send(OutputReliableStream* stream, uint8_t** buffer, size_t* length, uint16_t* seq_num)
{
    bool data_to_send = (0 >= seq_num_cmp(stream->last_sent, stream->last_written));
    if(data_to_send)
    {
        stream->last_sent = seq_num_add(stream->last_sent, 1);

        *seq_num = stream->last_sent;
        *buffer = stream->buffer + (stream->size / stream->history) * (stream->last_sent % stream->history);
        *length = stream->writer; //bad: there is only a stream->writer

        stream->writer = stream->offset;
    }

    return data_to_send;
}

bool prepare_reliable_stream_to_write(OutputReliableStream* stream, int size, MicroBuffer* mb)
{
    size_t max_message_size = stream->size / stream->history;
    bool available_to_write = stream->writer + size <= max_message_size;
    //TODO: if the writer + size dont fix, get a new history place.
    if(available_to_write)
    {
        size_t current_buffer_pos = (stream->last_sent % stream->history) * max_message_size;
        init_micro_buffer_offset(mb, stream->buffer + current_buffer_pos, stream->writer + size, stream->writer);

        stream->last_written = seq_num_add(stream->last_written, 1);
    }

    return available_to_write;
}

bool output_reliable_stream_must_notify(OutputReliableStream* stream, uint32_t current_timestamp)
{
    //stream->next_heartbeat_time_stamp += MIN_HEARTBEAT_TIME_INTERVAL_NS * ++stream->next_heartbeat_tries;
    //TODO
    (void) stream; (void) current_timestamp;
    return false;
}

bool output_reliable_stream_must_send(OutputReliableStream* stream, uint8_t** buffer, size_t *length)
{
    //TODO
    (void) stream; (void) buffer; (void) length;
    return false;
}

void write_heartbeat(OutputReliableStream* stream, MicroBuffer* mb)
{
    HEARTBEAT_Payload payload;
    payload.first_unacked_seq_nr = seq_num_add(stream->last_acknown, 1);
    payload.last_unacked_seq_nr = stream->last_sent;

    (void) write_submessage_header(mb, SUBMESSAGE_ID_HEARTBEAT, HEARTBEAT_PAYLOAD_SIZE, 0);
    (void) serialize_HEARTBEAT_Payload(mb, &payload);
}

void read_acknack(OutputReliableStream* stream, MicroBuffer* payload)
{
    ACKNACK_Payload acknack;
    deserialize_ACKNACK_Payload(payload, &acknack);

    uint16_t nack_bitmap = ((uint16_t)acknack.nack_bitmap[0] << 8) + (uint16_t)acknack.nack_bitmap[1];

    process_acknack(stream, acknack.first_unacked_seq_num, nack_bitmap);
}

//==================================================================
//                             PUBLIC
//==================================================================
void process_acknack(OutputReliableStream* stream, uint16_t bitmap, uint16_t first_unacked_seq_num)
{
    //TODO
    (void) stream; (void) bitmap; (void) first_unacked_seq_num;
}

