#include "input_message.h"
#include "xrce_protocol_serialization.h"

#include <stdlib.h>

#ifndef NDEBUG

#include <string.h>
#include <stdio.h>
#define ERROR_PRINT(fmt, args...) fprintf(stderr, fmt, ## args)

#else

#define DEBUG_PRINT(fmt, args...) ;

#endif

void init_input_message(InputMessage* message, InputMessageCallback callback, uint8_t* in_buffer, uint32_t in_buffer_size)
{
    init_aux_memory(&message->aux_memory);
    init_external_buffer(&message->reader, in_buffer, 0);

    message->callback = callback;
    message->buffer = in_buffer;
    message->buffer_size = in_buffer_size;
}

void free_input_message(InputMessage* message)
{
    free_aux_memory(&message->aux_memory);
}

void parse_data_payload(InputMessage* message, DataFormat format, const BaseObjectReply* reply)
{
    MicroBuffer* reader = &message->reader;
    AuxMemory* aux_memory = &message->aux_memory;
    InputMessageCallback* callback = &message->callback;

    switch(format)
    {
        case FORMAT_DATA:
        {
            SampleData data;
            deserialize_SampleData(reader, &data, aux_memory);
            if(callback->on_data_payload)
                callback->on_data_payload(reply, &data, callback->object, reader->endianness);
        }
        break;
        case FORMAT_SAMPLE:
        {
            Sample sample;
            deserialize_Sample(reader, &sample, aux_memory);
            if(callback->on_sample_payload)
                callback->on_sample_payload(reply, &sample, callback->object, reader->endianness);
        }
        break;
        case FORMAT_DATA_SEQ:
        {
            SampleDataSequence data_sequence;
            deserialize_SampleDataSequence(reader, &data_sequence, aux_memory);
            if(callback->on_data_sequence_payload)
                callback->on_data_sequence_payload(reply, &data_sequence, callback->object, reader->endianness);
        }
        break;
        case FORMAT_SAMPLE_SEQ:
        {
            SampleSequence sample_sequence;
            deserialize_SampleSequence(reader, &sample_sequence, aux_memory);
            if(callback->on_sample_sequence_payload)
                callback->on_sample_sequence_payload(reply, &sample_sequence, callback->object, reader->endianness);
        }
        break;
        case FORMAT_PACKED_SAMPLES:
        {
            PackedSamples packed_samples;
            deserialize_PackedSamples(reader, &packed_samples, aux_memory);
            if(callback->on_packed_samples_payload)
                callback->on_packed_samples_payload(reply, &packed_samples, callback->object, reader->endianness);
        }
        break;
    }
}

int parse_submessage(InputMessage* message)
{
    MicroBuffer* reader = &message->reader;
    AuxMemory* aux_memory = &message->aux_memory;
    InputMessageCallback* callback = &message->callback;

    // Submessage message header.
    align_to(reader, 4);
    SubmessageHeader submessage_header;
    deserialize_SubmessageHeader(reader, &submessage_header, NULL);
    if(callback->on_submessage_header)
        callback->on_submessage_header(&submessage_header, callback->object);

    // Configure the reader for the specific message endianness
    reader->endianness = submessage_header.flags & FLAG_ENDIANNESS;

    // We will need as much a quantity of aux memory equivalent to the payload length.
    increase_aux_memory(&message->aux_memory, submessage_header.length);

    // Submessage is not complete.
    if(reader->iterator + submessage_header.length > reader->final)
    {
        ERROR_PRINT("Parse message error: Subessage not complete.\n");
        return 0;
    }

    // Parse payload and call the corresponding callback.
    switch(submessage_header.id)
    {
        case SUBMESSAGE_ID_STATUS:
        {
            StatusPayload payload;
            deserialize_StatusPayload(reader, &payload, aux_memory);
            if(callback->on_status_submessage)
                callback->on_status_submessage(&payload, callback->object);
        }
        break;

        case SUBMESSAGE_ID_INFO:
        {
            InfoPayload payload;
            deserialize_InfoPayload(reader, &payload, aux_memory);
            if(callback->on_info_submessage)
                callback->on_info_submessage(&payload, callback->object);
        }
        break;

        case SUBMESSAGE_ID_DATA:
        {
            BaseObjectReply data_reply;
            deserialize_BaseObjectReply(reader, &data_reply, aux_memory);
            if(callback->on_data_submessage)
            {
                DataFormat format = callback->on_data_submessage(&data_reply, callback->object);
                parse_data_payload(message, format, &data_reply);
            }
            else
            {
                // skip submessage.
                reader->iterator += submessage_header.length;
                reader->last_data_size = 0;
            }
        }
        break;

        default:
            ERROR_PRINT("Parse message error: Unknown submessage id 0x%02X\n", submessage_header.id);
            return 0;
    }

    return 1;
}

int parse_message(InputMessage* message, uint32_t length)
{
    InputMessageCallback* callback = &message->callback;

    // Init the reader
    MicroBuffer* reader = &message->reader;
    if(length <= message->buffer_size)
        init_external_buffer(reader, message->buffer, length);
    else
    {
        ERROR_PRINT("Parse message error: Message length greater than input buffer.\n");
        return 0;
    }

    // Parse message header.
    MessageHeader message_header;
    ClientKey key;
    deserialize_MessageHeader(reader, &message_header, NULL);
    if(message_header.session_id >= 128)
        deserialize_ClientKey(reader, &key, NULL);

    // If the callback implementation fails, the message will not be parse
    if(callback->on_message_header &&
        !callback->on_message_header(&message_header, &key, callback->object))
    {
        ERROR_PRINT("Parse message error: Message header not undestood.\n");
        return 0;
    }

    // Parse all submessages of the message
    do
    {
        if(!parse_submessage(message))
            return 0;
    }
    while(reader->iterator < reader->final); // while another submessage fix...

    return 1;
}