#include <micrortps/client/client.h>
#include "client_private.h"

#include "log/message.h"

#include <time.h>
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

#include <stdlib.h>

#define MAX_TOPIC_LENGTH 256

// ----------------------------------------------------------------------------------
//                                  CLIENT STATE
// ----------------------------------------------------------------------------------
ClientState* new_serial_client_state(uint32_t buffer_size, const char* device)
{
    locator_id_t lid = add_serial_locator(device);
    if (0 >= lid)
    {
        return NULL;
    }
    return new_client_state(buffer_size, lid);
}

ClientState* new_udp_client_state(uint32_t buffer_size, uint16_t send_port, uint16_t recv_port, uint16_t remote_port, const char* server_ip)
{
    locator_id_t lid = add_udp_locator(send_port, recv_port, remote_port, server_ip);
    if (0 >= lid)
    {
        return NULL;
    }
    return new_client_state(buffer_size, lid);
}

ClientState* new_client_state(uint32_t buffer_size, locator_id_t transport_id)
{
    ClientState* state = malloc(sizeof(ClientState));

    state->transport_id = transport_id;

    state->buffer_size = buffer_size;
    state->buffer = malloc(buffer_size);

    state->output_sequence_number = 0;
    state->input_sequence_number = 0;

    OutputMessageCallback output_callback = {0};
    output_callback.args = state;
    output_callback.on_initialize_message = on_initialize_message;
    output_callback.on_out_of_bounds = on_out_of_bounds;

    InputMessageCallback input_callback = {0};
    input_callback.args = state;
    input_callback.on_message_header = on_message_header;
    input_callback.on_status_submessage = on_status_submessage;
    input_callback.on_data_submessage = on_data_submessage;
    input_callback.on_data_payload = on_data_payload;

    init_output_message(&state->output_message, output_callback, state->buffer, buffer_size);
    init_input_message(&state->input_message, input_callback, state->buffer, buffer_size);

    state->next_request_id = 0;
    state->next_object_id = 0;

    init_callback_data_storage(&state->callback_data_storage, 0xFF00);

    //Default message encoding
    state->output_message.writer.endianness = LITTLE_ENDIANNESS;

    return state;
}

void free_client_state(ClientState* state)
{
    rm_locator(state->transport_id);

    free_callback_data_storage(&state->callback_data_storage);
    free_input_message(&state->input_message);
    free(state->buffer);
    free(state);
}

uint16_t get_num_request_id(RequestId request_id)
{
    if(MACHINE_ENDIANNESS == LITTLE_ENDIANNESS)
        return request_id.data[1] + (request_id.data[0] << 8);
    else
        return request_id.data[0] + (request_id.data[1] << 8);
}

RequestId get_raw_request_id(uint16_t request_id)
{
    if(MACHINE_ENDIANNESS == LITTLE_ENDIANNESS)
        return (RequestId){{(uint8_t)(request_id >> 8), (uint8_t)request_id}};
    else
        return (RequestId){{(uint8_t)(request_id), (uint8_t)request_id >> 8}};
}

uint16_t get_num_object_id(ObjectId object_id)
{
    if(MACHINE_ENDIANNESS == LITTLE_ENDIANNESS)
        return object_id.data[1] + (object_id.data[0] << 8);
    else
        return object_id.data[0] + (object_id.data[1] << 8);
}

ObjectId get_raw_object_id(uint16_t object_id)
{
    if(MACHINE_ENDIANNESS == LITTLE_ENDIANNESS)
        return (ObjectId){{(uint8_t)(object_id >> 8), (uint8_t)object_id}};
    else
        return (ObjectId){{(uint8_t)(object_id), (uint8_t)object_id >> 8}};
}

XRCEInfo create_xrce_info(RequestId request_id, ObjectId object_id)
{
    return (XRCEInfo){get_num_request_id(request_id), get_num_object_id(object_id)};
}

// ----------------------------------------------------------------------------------
//                              XRCE API - CLIENT TO AGENT
// ----------------------------------------------------------------------------------

XRCEInfo create_client(ClientState* state, OnStatusReceived on_status, void* on_status_args)
{
    state->on_status_received = on_status;
    state->on_status_received_args = on_status_args;

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    XRCEInfo info = {++state->next_request_id, get_num_object_id(OBJECTID_CLIENT)};

    CreateClientPayload payload;
    payload.request.base.request_id = get_raw_request_id(info.request_id);
    payload.request.object_id = OBJECTID_CLIENT;
    payload.representation.xrce_cookie = XRCE_COOKIE;
    payload.representation.xrce_version = XRCE_VERSION;
    payload.representation.xrce_vendor_id = (XrceVendorId){{0x01, 0x0F}};
    payload.representation.client_timestamp.seconds = ts.tv_sec;
    payload.representation.client_timestamp.nanoseconds = ts.tv_nsec;
    payload.representation.client_key = (ClientKey){{0xAA, 0xBB, 0xCC, 0xDD}};
    payload.representation.session_id = 0xEE;

    state->session_id = SESSIONID_NONE_WITH_CLIENT_KEY;
    state->stream_id = STREAMID_NONE;
    state->key = payload.representation.client_key;

    if (add_create_client_submessage(&state->output_message, &payload))
    {
        PRINTL_CREATE_CLIENT_SUBMESSAGE(&payload);
    }

    return info;
}

XRCEInfo create_participant(ClientState* state)
{
    XRCEInfo info = {++state->next_request_id, ++state->next_object_id};

    CreateResourcePayload payload;
    payload.request.base.request_id = get_raw_request_id(info.request_id);
    payload.request.object_id = get_raw_object_id(info.object_id);
    payload.representation.kind = OBJK_PARTICIPANT;
    payload.representation._.participant.base2.format = REPRESENTATION_BY_REFERENCE;
    payload.representation._.participant.base2._.object_name.size = 0;

    if (add_create_resource_submessage(&state->output_message, &payload, 0))
    {
        PRINTL_CREATE_RESOURCE_SUBMESSAGE(&payload);
    }

    return info;
}

XRCEInfo create_topic(ClientState* state, uint16_t participant_id, String xml)
{
    XRCEInfo info = {++state->next_request_id, ++state->next_object_id};

    CreateResourcePayload payload;
    payload.request.base.request_id = get_raw_request_id(info.request_id);
    payload.request.object_id = get_raw_object_id(info.object_id);
    payload.representation.kind = OBJK_TOPIC;
    payload.representation._.topic.base3.format = REPRESENTATION_AS_XML_STRING;
    payload.representation._.topic.base3._.xml.size = xml.length;
    payload.representation._.topic.base3._.xml.data = xml.data;
    payload.representation._.topic.participant_id = get_raw_object_id(participant_id);

    if (add_create_resource_submessage(&state->output_message, &payload, 0))
    {
        PRINTL_CREATE_RESOURCE_SUBMESSAGE(&payload);
    }

    return info;
}

XRCEInfo create_publisher(ClientState* state, uint16_t participant_id)
{
    XRCEInfo info = {++state->next_request_id, ++state->next_object_id};

    CreateResourcePayload payload;
    payload.request.base.request_id = get_raw_request_id(info.request_id);
    payload.request.object_id = get_raw_object_id(info.object_id);
    payload.representation.kind = OBJK_PUBLISHER;
    payload.representation._.publisher.base3.format = REPRESENTATION_BY_REFERENCE;
    payload.representation._.publisher.base3._.object_name.size = 0;
    payload.representation._.publisher.participant_id = get_raw_object_id(participant_id);

    if (add_create_resource_submessage(&state->output_message, &payload, 0))
    {
        PRINTL_CREATE_RESOURCE_SUBMESSAGE(&payload);
    }

    return info;
}

XRCEInfo create_subscriber(ClientState* state, uint16_t participant_id)
{
    XRCEInfo info = {++state->next_request_id, ++state->next_object_id};

    CreateResourcePayload payload;
    payload.request.base.request_id = get_raw_request_id(info.request_id);
    payload.request.object_id = get_raw_object_id(info.object_id);
    payload.representation.kind = OBJK_SUBSCRIBER;
    payload.representation._.subscriber.base3.format = REPRESENTATION_BY_REFERENCE;
    payload.representation._.subscriber.base3._.object_name.size = 0;
    payload.representation._.subscriber.participant_id = get_raw_object_id(participant_id);

    if (add_create_resource_submessage(&state->output_message, &payload, 0))
    {
        PRINTL_CREATE_RESOURCE_SUBMESSAGE(&payload);
    }

    return info;
}

XRCEInfo create_data_writer(ClientState* state, uint16_t participant_id, uint16_t publisher_id, String xml)
{
    XRCEInfo info = {++state->next_request_id, ++state->next_object_id};

    CreateResourcePayload payload;
    payload.request.base.request_id = get_raw_request_id(info.request_id);
    payload.request.object_id = get_raw_object_id(info.object_id);
    payload.representation.kind = OBJK_DATAWRITER;
    payload.representation._.data_writer.base3.format = REPRESENTATION_AS_XML_STRING;
    payload.representation._.data_writer.base3._.xml.size = xml.length;
    payload.representation._.data_writer.base3._.xml.data = xml.data;
    payload.representation._.data_writer.participant_id = get_raw_object_id(participant_id);
    payload.representation._.data_writer.publisher_id = get_raw_object_id(publisher_id);

    if (add_create_resource_submessage(&state->output_message, &payload, 0))
    {
        PRINTL_CREATE_RESOURCE_SUBMESSAGE(&payload);
    }

    return info;
}

XRCEInfo create_data_reader(ClientState* state, uint16_t participant_id, uint16_t subscriber_id, String xml)
{
    XRCEInfo info = {++state->next_request_id, ++state->next_object_id};

    CreateResourcePayload payload;
    payload.request.base.request_id = get_raw_request_id(info.request_id);
    payload.request.object_id = get_raw_object_id(info.object_id);
    payload.representation.kind = OBJK_DATAREADER;
    payload.representation._.data_reader.base3.format = REPRESENTATION_AS_XML_STRING;
    payload.representation._.data_reader.base3._.xml.size = xml.length;
    payload.representation._.data_reader.base3._.xml.data = xml.data;
    payload.representation._.data_reader.participant_id = get_raw_object_id(participant_id);
    payload.representation._.data_reader.subscriber_id = get_raw_object_id(subscriber_id);

    if (add_create_resource_submessage(&state->output_message, &payload, 0))
    {
        PRINTL_CREATE_RESOURCE_SUBMESSAGE(&payload);
    }

    return info;
}

XRCEInfo delete_resource(ClientState* state, uint16_t resource_id)
{
    XRCEInfo info = {++state->next_request_id, resource_id};

    DeleteResourcePayload payload;
    payload.request.base.request_id = get_raw_request_id(info.request_id);
    payload.request.object_id = get_raw_object_id(info.object_id);

    if (add_delete_resource_submessage(&state->output_message, &payload))
    {
        PRINTL_DELETE_RESOURCE_SUBMESSAGE(&payload);
    }

    return info;
}

XRCEInfo write_data(ClientState* state, uint16_t data_writer_id, SerializeTopic serialize_topic, void* topic)
{
    //Knows previously the topic serialized length. Is it feasible?
    char buffer[MAX_TOPIC_LENGTH];

    MicroBuffer writer;
    init_external_buffer(&writer, (uint8_t*)buffer, MAX_TOPIC_LENGTH);
    writer.endianness = state->output_message.writer.endianness;

    if(serialize_topic(&writer, &(AbstractTopic){topic}))
    {
        XRCEInfo info = {++state->next_request_id, data_writer_id};

        WriteDataPayload payload;
        payload.request.base.request_id = get_raw_request_id(info.request_id);
        payload.request.object_id = get_raw_object_id(info.object_id);
        payload.data_to_write.format = FORMAT_DATA;
        payload.data_to_write._.data.size = writer.iterator - writer.init;
        payload.data_to_write._.data.data = writer.init;

        if (add_write_data_submessage(&state->output_message, &payload))
        {
            PRINTL_WRITE_DATA_SUBMESSAGE(&payload);
        }
        return info;
    }

    return (XRCEInfo){0x0000, 0xFFFF};
}

XRCEInfo read_data(ClientState* state, uint16_t data_reader_id, OnMessageReceived on_message, void *callback_args)
{
    int16_t callback_request_id = register_callback_data(&state->callback_data_storage, FORMAT_DATA, on_message, callback_args);

    XRCEInfo info = {callback_request_id, data_reader_id};

    ReadDataPayload payload;
    payload.request.base.request_id = get_raw_request_id(info.request_id);
    payload.request.object_id = get_raw_object_id(info.object_id);
    payload.read_specification.optional_content_filter_expression = false;
    payload.read_specification.optional_delivery_config = FORMAT_DATA;

    if (add_read_data_submessage(&state->output_message, &payload))
    {
        PRINTL_READ_DATA_SUBMESSAGE(&payload);
    }
    return info;
}

// ----------------------------------------------------------------------------------
//                              CALLBACKS - AGENT TO CLIENT
// ----------------------------------------------------------------------------------

void on_initialize_message(MessageHeader* header, ClientKey* key, void* vstate)
{
    ClientState* state = (ClientState*) vstate;

    header->session_id = state->session_id;
    header->stream_id = state->stream_id;
    header->sequence_nr = state->output_sequence_number;
    *key = state->key;

    state->output_sequence_number++;
}

void on_out_of_bounds(void* vstate)
{
    send_to_agent((ClientState*) vstate);
}

bool on_message_header(const MessageHeader* header, const ClientKey* key, void* vstate)
{
    // TODO.
    (void) key;

    ClientState* state = (ClientState*) vstate;

    if(header->stream_id != STREAMID_NONE)
        if(header->sequence_nr != state->input_sequence_number)
        {
            PRINT_SEQUENCE_NUMBER(header->sequence_nr, state->input_sequence_number);
            return 0;
        }

    state->input_sequence_number++;
    return 1;
}

void on_status_submessage(const StatusPayload* payload, void* args)
{
    ClientState* state = (ClientState*) args;
    PRINTL_STATUS_SUBMESSAGE(payload);

    if(state->on_status_received)
    {

        XRCEInfo info = create_xrce_info(payload->reply.base.result.request_id, payload->reply.object_id);
        state->on_status_received(info,
                payload->reply.base.result.status,
                payload->reply.base.result.implementation_status,
                state->on_status_received_args);
    }
}

DataFormat on_data_submessage(const BaseObjectReply* reply, void* args)
{
    ClientState* state = (ClientState*) args;

    XRCEInfo info = create_xrce_info(reply->base.result.request_id, reply->object_id);
    if(state->on_status_received)
    {
        state->on_status_received(info,
                reply->base.result.status,
                reply->base.result.implementation_status,
                state->on_status_received_args);
    }

    if(reply->base.result.implementation_status == STATUS_OK)
    {
        CallbackData* callback_data = get_callback_data(&state->callback_data_storage, info.request_id);
        if(callback_data)
            return callback_data->format;
    }

    return FORMAT_BAD;
}

void on_data_payload(const BaseObjectReply* reply, const SampleData* data, void* args, Endianness endianness)
{
    ClientState* state = (ClientState*) args;
    PRINTL_DATA_SUBMESSAGE_SAMPLE_DATA(reply, data);

    XRCEInfo info = create_xrce_info(reply->base.result.request_id, reply->object_id);
    CallbackData* callback_data = get_callback_data(&state->callback_data_storage, info.request_id);
    if(callback_data)
    {
        MicroBuffer reader;
        init_external_buffer(&reader, data->data, data->size);
        reader.endianness = endianness;

        callback_data->on_message(info, &reader, callback_data->callback_args);

        remove_callback_data(&state->callback_data_storage, info.request_id);
    }
}

// ----------------------------------------------------------------------------------
//                                 COMUNICATION
// ----------------------------------------------------------------------------------

bool send_to_agent(ClientState* state)
{
    uint32_t length = get_message_length(&state->output_message);
    if(length > 0)
    {
        int output_length = send_data(state->buffer, length, state->transport_id);
        if(output_length > 0)
        {
            PRINTL_SERIALIZATION(SEND, state->buffer, output_length);
            reset_buffer(&state->output_message.writer);

            return true;
        }
    }

    return false;
}

bool receive_from_agent(ClientState* state)
{
    int length = receive_data(state->buffer, state->buffer_size, state->transport_id);
    if(length > 0)
    {
        parse_message(&state->input_message, length);
        PRINTL_SERIALIZATION(RECV, state->buffer, length);

        return true;
    }

    return false;
}

// ----------------------------------------------------------------------------------------------
//    Data callback funcions
// ----------------------------------------------------------------------------------------------

void init_callback_data_storage(CallbackDataStorage* store, uint16_t initial_id)
{
    store->size = 1;
    store->initial_id = initial_id;
    store->existence = calloc(store->size, sizeof(bool));
    store->callbacks = malloc(sizeof(CallbackData) * store->size);
}

void free_callback_data_storage(CallbackDataStorage* store)
{
    free(store->callbacks);
    free(store->existence);
}

uint16_t register_callback_data(CallbackDataStorage* store, uint8_t format, OnMessageReceived on_message, void *callback_args)
{
    uint16_t id = store->size;
    for(uint32_t i = 0; i < store->size; i++)
        if(!store->existence[i])
            id = i;

    if(id == store->size)
    {
        uint32_t new_size = store->size * 2;
        store->existence = realloc(store->existence, sizeof(bool) *new_size);
        store->callbacks = realloc(store->callbacks, sizeof(CallbackData) *new_size);

        for(uint32_t i = store->size; i < new_size; i++)
            store->existence[i] = false;

        store->size = new_size;
    }

    store->existence[id] = true;
    store->callbacks[id] = (CallbackData){format, on_message, callback_args};

    return store->initial_id + id;
}

void remove_callback_data(CallbackDataStorage* store, uint16_t id)
{
    if(store->size + store->initial_id > id)
        store->existence[id - store->initial_id] = false;
}

CallbackData* get_callback_data(const CallbackDataStorage* store, uint16_t id)
{
    if(store->size + store->initial_id > id)
        return &store->callbacks[id - store->initial_id];
    return NULL;
}
