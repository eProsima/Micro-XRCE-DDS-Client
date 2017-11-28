#include "output_message.h"
#include "xrce_protocol_serialization.h"

#ifndef NDEBUG
#include <string.h>
#include <stdio.h>
#endif

#include <stdlib.h>

// PRIVATE DEFINITIONS
void begin_message(OutputMessage* message);

void begin_submessage(OutputMessage* message, MicroState* submessage_beginning,
                      MicroState* header_beginning, MicroState* payload_beginning);

bool end_submessage(OutputMessage* message, MicroState submessage_beginning,
                    MicroState header_beginning, MicroState payload_beginning,
                    SubmessageId id, uint8_t flags);


// ----------------------------------------------------------------------------------
//                                 PUBLIC API
// ----------------------------------------------------------------------------------
void init_output_message(OutputMessage* message, OutputMessageCallback callback,
                          uint8_t* out_buffer, uint32_t out_buffer_size)
{
    init_external_buffer(&message->writer, out_buffer, out_buffer_size);

    message->callback = callback;
}

uint32_t get_message_length(OutputMessage* message)
{
    return message->writer.iterator - message->writer.init;
}

bool add_create_client_submessage(OutputMessage* message, const CreateClientPayload* payload)
{
    MicroState submessage_beginning, header_beginning, payload_beginning;

    begin_submessage(message, &submessage_beginning, &header_beginning, &payload_beginning);
    serialize_CreateClientPayload(&message->writer, payload);

    return end_submessage(message, submessage_beginning, header_beginning, payload_beginning,
        SUBMESSAGE_ID_CREATE_CLIENT, message->writer.endianness);
}

bool add_create_resource_submessage(OutputMessage* message, const CreateResourcePayload* payload, CreationMode creation_mode)
{
    MicroState submessage_beginning, header_beginning, payload_beginning;

    begin_submessage(message, &submessage_beginning, &header_beginning, &payload_beginning);
    serialize_CreateResourcePayload(&message->writer, payload);

    return end_submessage(message, submessage_beginning, header_beginning, payload_beginning,
        SUBMESSAGE_ID_CREATE, creation_mode | message->writer.endianness);
}

bool add_delete_resource_submessage(OutputMessage* message, const DeleteResourcePayload* payload)
{
    MicroState submessage_beginning, header_beginning, payload_beginning;

    begin_submessage(message, &submessage_beginning, &header_beginning, &payload_beginning);
    serialize_DeleteResourcePayload(&message->writer, payload);

    return end_submessage(message, submessage_beginning, header_beginning, payload_beginning,
        SUBMESSAGE_ID_DELETE, message->writer.endianness);
}

bool add_get_info_submessage(OutputMessage* message, const GetInfoPayload* payload)
{
    MicroState submessage_beginning, header_beginning, payload_beginning;

    begin_submessage(message, &submessage_beginning, &header_beginning, &payload_beginning);
    serialize_GetInfoPayload(&message->writer, payload);

    return end_submessage(message, submessage_beginning, header_beginning, payload_beginning,
        SUBMESSAGE_ID_GET_INFO, message->writer.endianness);
}

bool add_read_data_submessage(OutputMessage* message, const ReadDataPayload* payload)
{
    MicroState submessage_beginning, header_beginning, payload_beginning;

    begin_submessage(message, &submessage_beginning, &header_beginning, &payload_beginning);
    serialize_ReadDataPayload(&message->writer, payload);

    return end_submessage(message, submessage_beginning, header_beginning, payload_beginning,
        SUBMESSAGE_ID_READ_DATA, message->writer.endianness);
}

bool add_write_data_submessage(OutputMessage* message, const WriteDataPayload* payload)
{
    MicroState submessage_beginning, header_beginning, payload_beginning;

    begin_submessage(message, &submessage_beginning, &header_beginning, &payload_beginning);
    serialize_WriteDataPayload(&message->writer, payload);

    return end_submessage(message, submessage_beginning, header_beginning, payload_beginning,
        SUBMESSAGE_ID_WRITE_DATA, message->writer.endianness);
}

// ----------------------------------------------------------------------------------
//                                  PRIVATE UTILS
// ----------------------------------------------------------------------------------
void begin_message(OutputMessage* message)
{
    MessageHeader message_header;
    ClientKey key;
    message->callback.on_initialize_message(&message_header, &key, message->callback.args);

    serialize_MessageHeader(&message->writer, &message_header);
    if(message_header.session_id >= 128)
        serialize_ClientKey(&message->writer, &key);
}

void begin_submessage(OutputMessage* message, MicroState* submessage_beginning,
                      MicroState* header_beginning, MicroState* payload_beginning)
{
    if(message->writer.iterator == message->writer.init)
        begin_message(message);

    *submessage_beginning = get_micro_state(&message->writer);

    align_to(&message->writer, 4);
    *header_beginning = get_micro_state(&message->writer);

    message->writer.iterator += 4; //Space for SubMessageHeader.
    *payload_beginning = get_micro_state(&message->writer);
}

bool end_submessage(OutputMessage* message, MicroState submessage_beginning,
                    MicroState header_beginning, MicroState payload_beginning,
                    SubmessageId id, uint8_t flags)
{
    if(message->writer.iterator > message->writer.final)
    {
        restore_micro_state(&message->writer, submessage_beginning);
        return false;
    }

    MicroState current_state = get_micro_state(&message->writer);

    restore_micro_state(&message->writer, header_beginning);

    SubmessageHeader header;
    header.id = id;
    header.flags = flags;
    header.length = current_state.position - payload_beginning.position;

    if(message->callback.on_submessage_header)
        message->callback.on_submessage_header(&header, NULL);

    serialize_SubmessageHeader(&message->writer, &header);

    restore_micro_state(&message->writer, current_state);
    return true;
}