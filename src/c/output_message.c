// Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <micrortps/client/output_message.h>
#include <micrortps/client/xrce_protocol_serialization.h>

#ifndef NDEBUG
#include <string.h>
#include <stdio.h>
#endif

#include <stdlib.h>

// PRIVATE DEFINITIONS
bool begin_message(OutputMessage *message);

bool begin_submessage(OutputMessage *message, MicroState *submessage_beginning,
                      MicroState *header_beginning, MicroState *payload_beginning);

bool end_submessage(OutputMessage *message,
                    MicroState header_beginning, MicroState payload_beginning,
                    SubmessageId id, uint8_t flags);

bool try_add_create_client_submessage(OutputMessage *message, const CreateClientPayload *payload);
bool try_add_create_resource_submessage(OutputMessage *message, const CreateResourcePayload *payload, CreationMode creation_mode);
bool try_add_delete_resource_submessage(OutputMessage *message, const DeleteResourcePayload *payload);
bool try_add_get_info_submessage(OutputMessage *message, const GetInfoPayload *payload);
bool try_add_read_data_submessage(OutputMessage *message, const ReadDataPayload *payload);
bool try_add_write_data_submessage(OutputMessage *message, const WriteDataPayload *payload);

// ----------------------------------------------------------------------------------
//                                 PUBLIC API
// ----------------------------------------------------------------------------------
void init_output_message(OutputMessage *message, OutputMessageCallback callback,
                         uint8_t *out_buffer, uint32_t out_buffer_size)
{
    init_external_buffer(&message->writer, out_buffer, out_buffer_size);

    message->callback = callback;
}

uint32_t get_message_length(OutputMessage *message)
{
    return message->writer.iterator - message->writer.init;
}

bool add_create_client_submessage(OutputMessage *message, const CreateClientPayload *payload)
{
    bool added = try_add_create_client_submessage(message, payload);
    if (!added)
    {
        message->callback.on_out_of_bounds(message->callback.args);
        added = try_add_create_client_submessage(message, payload);
    }
    return added;
}

bool try_add_create_client_submessage(OutputMessage *message, const CreateClientPayload *payload)
{
    MicroState submessage_beginning, header_beginning, payload_beginning;

    bool ret = begin_submessage(message, &submessage_beginning, &header_beginning, &payload_beginning) &&
               serialize_CreateClientPayload(&message->writer, payload) &&
               end_submessage(message, header_beginning, payload_beginning,
                              SUBMESSAGE_ID_CREATE_CLIENT, message->writer.endianness);
    if (!ret)
    {
        restore_micro_state(&message->writer, submessage_beginning);
    }
    return ret;
}

bool add_create_resource_submessage(OutputMessage *message, const CreateResourcePayload *payload, CreationMode creation_mode)
{
    bool added = try_add_create_resource_submessage(message, payload, creation_mode);
    if (!added)
    {
        message->callback.on_out_of_bounds(message->callback.args);
        added = try_add_create_resource_submessage(message, payload, creation_mode);
    }
    return added;
}

bool try_add_create_resource_submessage(OutputMessage *message, const CreateResourcePayload *payload, CreationMode creation_mode)
{
    MicroState submessage_beginning, header_beginning, payload_beginning;

    bool ret = begin_submessage(message, &submessage_beginning, &header_beginning, &payload_beginning) &&
               serialize_CreateResourcePayload(&message->writer, payload) &&
               end_submessage(message, header_beginning, payload_beginning,
                              SUBMESSAGE_ID_CREATE, creation_mode | message->writer.endianness);
    if (!ret)
    {
        restore_micro_state(&message->writer, submessage_beginning);
    }
    return ret;
}

bool add_delete_resource_submessage(OutputMessage *message, const DeleteResourcePayload *payload)
{
    bool added = try_add_delete_resource_submessage(message, payload);
    if (!added)
    {
        message->callback.on_out_of_bounds(message->callback.args);
        added = try_add_delete_resource_submessage(message, payload);
    }
    return added;
}

bool try_add_delete_resource_submessage(OutputMessage *message, const DeleteResourcePayload *payload)
{
    MicroState submessage_beginning, header_beginning, payload_beginning;

    bool ret = begin_submessage(message, &submessage_beginning, &header_beginning, &payload_beginning) &&
               serialize_DeleteResourcePayload(&message->writer, payload) &&
               end_submessage(message, header_beginning, payload_beginning,
                              SUBMESSAGE_ID_DELETE, message->writer.endianness);
    if (!ret)
    {
        restore_micro_state(&message->writer, submessage_beginning);
    }
    return ret;
}

bool add_get_info_submessage(OutputMessage *message, const GetInfoPayload *payload)
{
    bool added = try_add_get_info_submessage(message, payload);
    if (!added)
    {
        message->callback.on_out_of_bounds(message->callback.args);
        added = try_add_get_info_submessage(message, payload);
    }
    return added;
}

bool try_add_get_info_submessage(OutputMessage *message, const GetInfoPayload *payload)
{
    MicroState submessage_beginning, header_beginning, payload_beginning;

    bool ret = begin_submessage(message, &submessage_beginning, &header_beginning, &payload_beginning) &&
               serialize_GetInfoPayload(&message->writer, payload) &&
               end_submessage(message, header_beginning, payload_beginning,
                              SUBMESSAGE_ID_GET_INFO, message->writer.endianness);

    if (!ret)
    {
        restore_micro_state(&message->writer, submessage_beginning);
    }
    return ret;
}

bool add_read_data_submessage(OutputMessage *message, const ReadDataPayload *payload)
{
    bool added = try_add_read_data_submessage(message, payload);
    if (!added)
    {
        message->callback.on_out_of_bounds(message->callback.args);
        added = try_add_read_data_submessage(message, payload);
    }
    return added;
}

bool try_add_read_data_submessage(OutputMessage *message, const ReadDataPayload *payload)
{
    MicroState submessage_beginning, header_beginning, payload_beginning;

    bool ret = begin_submessage(message, &submessage_beginning, &header_beginning, &payload_beginning) &&
               serialize_ReadDataPayload(&message->writer, payload) &&
               end_submessage(message, header_beginning, payload_beginning, SUBMESSAGE_ID_READ_DATA, message->writer.endianness);

    if (!ret)
    {
        restore_micro_state(&message->writer, submessage_beginning);
    }
    return ret;
}

bool add_write_data_submessage(OutputMessage *message, const WriteDataPayload *payload)
{
    bool added = try_add_write_data_submessage(message, payload);
    if (!added)
    {
        message->callback.on_out_of_bounds(message->callback.args);
        added = try_add_write_data_submessage(message, payload);
    }
    return added;
}

bool try_add_write_data_submessage(OutputMessage *message, const WriteDataPayload *payload)
{
    MicroState submessage_beginning, header_beginning, payload_beginning;

    bool ret = begin_submessage(message, &submessage_beginning, &header_beginning, &payload_beginning) &&
               serialize_WriteDataPayload(&message->writer, payload) &&
               end_submessage(message, header_beginning, payload_beginning,
                              SUBMESSAGE_ID_WRITE_DATA, message->writer.endianness);
    if (!ret)
    {
        restore_micro_state(&message->writer, submessage_beginning);
    }
    return ret;
}
// ----------------------------------------------------------------------------------
//                                  PRIVATE UTILS
// ----------------------------------------------------------------------------------
bool begin_message(OutputMessage *message)
{
    MessageHeader message_header;
    ClientKey key;
    message->callback.on_initialize_message(&message_header, &key, message->callback.args);

    bool ret = serialize_MessageHeader(&message->writer, &message_header);
    if (message_header.session_id >= 128)
        ret &= serialize_ClientKey(&message->writer, &key);
    return ret;
}

bool begin_submessage(OutputMessage *message, MicroState *submessage_beginning,
                      MicroState *header_beginning, MicroState *payload_beginning)
{
    bool ret = true;
    if (message->writer.iterator == message->writer.init)
        ret &= begin_message(message);

    *submessage_beginning = get_micro_state(&message->writer);

    align_to(&message->writer, 4);
    *header_beginning = get_micro_state(&message->writer);

    message->writer.iterator += 4; //Space for SubMessageHeader.
    *payload_beginning = get_micro_state(&message->writer);
    return ret;
}

bool end_submessage(OutputMessage *message,
                    MicroState header_beginning, MicroState payload_beginning,
                    SubmessageId id, uint8_t flags)
{
    MicroState current_state = get_micro_state(&message->writer);

    restore_micro_state(&message->writer, header_beginning);

    SubmessageHeader header;
    header.id = id;
    header.flags = flags;
    header.length = current_state.position - payload_beginning.position;

    if (message->callback.on_submessage_header)
        message->callback.on_submessage_header(&header, NULL);

    bool ret = serialize_SubmessageHeader(&message->writer, &header);

    restore_micro_state(&message->writer, current_state);
    return ret;
}