#include "micrortps/client/message_output.h"
#include "micrortps/client/xrce_protocol_serialization.h"

#ifndef NDEBUG
#include <string.h>
#include <stdio.h>
#endif

// PRIVATE DEFINITIONS
void begin_submessage(MessageOutput* message, MicroState* submessage_beginning,
                      MicroState* header_beginning, MicroState* payload_beginning);

bool end_submessage(MessageOutput* message, MicroState submessage_beginning,
                    MicroState header_beginning, MicroState payload_beginning,
                    SubmessageId id, uint8_t flags);


void begin_submessage(MessageOutput* message, MicroState* submessage_beginning,
                      MicroState* header_beginning, MicroState* payload_beginning)
{
    if(message->writer.iterator == message->writer.init)
    {
        MessageHeader message_header;
        message->callback.on_initialize_message(&message_header, message->callback.object);

        serialize_MessageHeader(&message->writer, &message_header);
    }

    *submessage_beginning = get_micro_state(&message->writer);

    align_to(&message->writer, 4);

    *header_beginning = get_micro_state(&message->writer);

    message->writer.iterator += 4; //Space for MessageHeader.

    *payload_beginning = get_micro_state(&message->writer);
}

bool end_submessage(MessageOutput* message, MicroState submessage_beginning,
                    MicroState header_beginning, MicroState payload_beginning,
                    SubmessageId id, uint8_t flags)
{
    if(message->writer.iterator == message->writer.final)
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

    serialize_SubmessageHeader(&message->writer, &header);

    restore_micro_state(&message->writer, current_state);
    return true;
}

void init_message_output(MessageOutput* message, MessageOutputCallback callback,
                          uint8_t* out_buffer, uint32_t out_buffer_size)
{
    init_external_buffer(&message->writer, out_buffer, out_buffer_size);

    message->callback = callback;
}

bool add_create_client_submessage(MessageOutput* message, const CreateClientPayload* payload)
{
    MicroState submessage_beginning, header_beginning, payload_beginning;

    begin_submessage(message, &submessage_beginning, &header_beginning, &payload_beginning);
    serialize_CreateClientPayload(&message->writer, payload);

    return end_submessage(message, submessage_beginning, header_beginning, payload_beginning,
        SUBMESSAGE_ID_CREATE_CLIENT, message->writer.endianness);
}

bool add_create_resource_submessage(MessageOutput* message, const CreateResourcePayload* payload, uint8_t creation_mode)
{
    MicroState submessage_beginning, header_beginning, payload_beginning;

    begin_submessage(message, &submessage_beginning, &header_beginning, &payload_beginning);
    serialize_CreateResourcePayload(&message->writer, payload);

    return end_submessage(message, submessage_beginning, header_beginning, payload_beginning,
        SUBMESSAGE_ID_CREATE, creation_mode | message->writer.endianness);
}

bool add_delete_resource_submessage(MessageOutput* message, const DeleteResourcePayload* payload)
{
    MicroState submessage_beginning, header_beginning, payload_beginning;

    begin_submessage(message, &submessage_beginning, &header_beginning, &payload_beginning);
    serialize_DeleteResourcePayload(&message->writer, payload);

    return end_submessage(message, submessage_beginning, header_beginning, payload_beginning,
        SUBMESSAGE_ID_DELETE, message->writer.endianness);
}

bool add_get_info_submessage(MessageOutput* message, const GetInfoPayload* payload)
{
    MicroState submessage_beginning, header_beginning, payload_beginning;

    begin_submessage(message, &submessage_beginning, &header_beginning, &payload_beginning);
    serialize_GetInfoPayload(&message->writer, payload);

    return end_submessage(message, submessage_beginning, header_beginning, payload_beginning,
        SUBMESSAGE_ID_GET_INFO, message->writer.endianness);
}

bool add_read_data_submessage(MessageOutput* message, const ReadDataPayload* payload)
{
    MicroState submessage_beginning, header_beginning, payload_beginning;

    begin_submessage(message, &submessage_beginning, &header_beginning, &payload_beginning);
    serialize_ReadDataPayload(&message->writer, payload);

    return end_submessage(message, submessage_beginning, header_beginning, payload_beginning,
        SUBMESSAGE_ID_READ_DATA, message->writer.endianness);
}

bool add_write_data_submessage(MessageOutput* message, const WriteDataPayload* payload)
{
    MicroState submessage_beginning, header_beginning, payload_beginning;

    begin_submessage(message, &submessage_beginning, &header_beginning, &payload_beginning);
    serialize_WriteDataPayload(&message->writer, payload);

    return end_submessage(message, submessage_beginning, header_beginning, payload_beginning,
        SUBMESSAGE_ID_WRITE_DATA, message->writer.endianness);
}

