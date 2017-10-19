#include "micrortps/client/message_input.h"
#include "micrortps/client/xrce_protocol_serialization.h"

#ifndef NDEBUG
#include <string.h>
#include <stdio.h>
#define ERROR_PRINT(fmt, args...) fprintf(stderr, fmt, ## args)
#else
#define DEBUG_PRINT(fmt, args...)
#endif


int parse_submessage(MicroBuffer* reader, AuxMemory* aux_memory, MessageInputCallback* callback)
{
    // Submessage message header.
    SubmessageHeader submessage_header;
    deserialize_SubmessageHeader(reader, &submessage_header, NULL);
    if(callback->on_submessage_header)
        callback->on_submessage_header(&submessage_header, callback->object);

    // We will need as much a quantity of aux memory equivalent to the payload length.
    reset_aux_memory(aux_memory, submessage_header.length);

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
            DataPayload data_payload;
            deserialize_BaseObjectReply(reader, &data_payload.reply, aux_memory);
            if(callback->on_data_submessage)
                callback->on_data_submessage(&data_payload, callback->object);
        }
        break;

        default:
            ERROR_PRINT("Parse message error: Unknown submessage id 0x%02X", submessage_header.id);
            return 0;
    }

    return 1;
}

int parse_message(uint8_t* message_data, uint32_t length, MessageInputCallback* callback)
{
    // All messages need at least 12 bytes (MessageHeader and SubmessageHeader)
    if(length < 12)
    {
        ERROR_PRINT("Parse message error: Message length is %u (less than 12).", length);
        return 0;
    }

    // Init the reader for the message.
    MicroBuffer reader;
    init_external_buffer(&reader, message_data, length);

    // Parse message header.
    // If the callback implementation fails, the message will not be parse
    MessageHeader message_header;
    deserialize_MessageHeader(&reader, &message_header, NULL);
    if(callback->on_message_header)
        if(!callback->on_message_header(&message_header, callback->object))
        {
            ERROR_PRINT("Parse message error: Message header not undestood.");
            return 0;
        }

    // Prepare the auxiliar memory for deserialization process.
    AuxMemory aux_memory;
    init_aux_memory(&aux_memory);

    // Parse all submessages of the message
    do
    {
        if(!parse_submessage(&reader, &aux_memory, callback))
        {
            free_aux_memory(&aux_memory);
            return 0;
        }
    }
    while(reader.iterator + align_to(&reader, 4) + 4 > reader.final); // while another submessage fix...

    free_aux_memory(&aux_memory);
    return 1;
}