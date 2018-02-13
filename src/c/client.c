/*
 * Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <micrortps/client/client.h>
#include "client_private.h"
#include "log/message.h"
#include <time.h>
#include <stdlib.h>

#define MAX_TOPIC_LENGTH 256

/* ----------------------------------------------------------------------------------------------
                                    CLIENT SESSION
 ---------------------------------------------------------------------------------------------- */
uint8_t new_udp_session(Session* session,
                     uint8_t* buf,
                     uint32_t buf_size,
                     uint16_t send_port,
                     uint16_t recv_port,
                     uint16_t remote_port,
                     const char* server_ip)
{
    uint8_t result = 0xFF;
    locator_id_t locator_id = add_udp_locator(send_port, recv_port, remote_port, server_ip);

    if (0>= locator_id)
    {
        result = SESSION_LOCATOR_ERR;
    }
    else
    {
        session->transport_id = locator_id;
        session->buffer = buf;
        session->buffer_size = buf_size;

        session->output_sequence_number = 0;
        session->input_sequence_number = 0;

        OutputMessageCallback output_callback = {0};
        output_callback.args = session;
        output_callback.on_initialize_message = on_initialize_message;
        output_callback.on_out_of_bounds = on_out_of_bounds;

        InputMessageCallback input_callback = {0};
        input_callback.args = session;
        input_callback.on_message_header = on_message_header;
        input_callback.on_status_submessage = on_status_submessage;
        input_callback.on_data_submessage = on_data_submessage;
        input_callback.on_data_payload = on_data_payload;

        init_output_message(&session->output_message, output_callback,
                            session->buffer, session->buffer_size);
        init_input_message(&session->input_message, input_callback,
                           session->buffer, session->buffer_size);

        session->next_request_id = 0;
        session->next_object_id = 0;

        session->output_message.writer.endianness = LITTLE_ENDIANNESS;

        result = SESSION_CREATED;
    }

    return result;
}

void close_session(Session* session)
{
    rm_locator(session->transport_id);
    free_callback_data_storage(&session->callback_data_storage);
    free_input_message(&session->input_message);
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

/* ----------------------------------------------------------------------------------------------
                                XRCE API - CLIENT TO AGENT
 ---------------------------------------------------------------------------------------------- */
uint8_t init_session(Session* session, XRCEInfo* info, OnStatusReceived on_status, void *on_status_args)
{
    uint8_t result = 0xFF;

    session->on_status_received = on_status;
    session->on_status_received_args = on_status_args;

    struct timespec ts;
    #ifdef WIN32
        SYSTEMTIME epoch_tm = {1970, 1, 4, 1, 0, 0, 0, 0};
        FILETIME epoch_ft;
        SystemTimeToFileTime(&epoch_tm, &epoch_ft);
        uint64_t epoch_time = (((uint64_t) epoch_ft.dwHighDateTime) << 32) + epoch_ft.dwLowDateTime;

        SYSTEMTIME tm;
        FILETIME ft;
        GetSystemTime(&tm);
        SystemTimeToFileTime(&tm, &ft);
        uint64_t current_time = (((uint64_t) ft.dwHighDateTime) << 32) + ft.dwLowDateTime;

        ts.tv_sec = (time_t) ((current_time - epoch_time) / 10000000);
        ts.tv_nsec = (time_t) ((current_time - epoch_time) % 10000000);
    #else
        clock_gettime(CLOCK_REALTIME, &ts);
    #endif

    ++session->next_request_id;
    if (info)
    {
        info->request_id = session->next_request_id;
        info->object_id = get_num_object_id(OBJECTID_CLIENT);
    }

    CREATE_CLIENT_Payload payload;
    payload.base.request_id = get_raw_request_id(state->next_request_id);
    payload.base.object_id = OBJECTID_CLIENT;
    payload.client_representation.xrce_cookie = XRCE_COOKIE;
    payload.client_representation.xrce_version = XRCE_VERSION;
    payload.client_representation.xrce_vendor_id = (XrceVendorId){{0x01, 0x0F}};
    payload.client_representation.client_timestamp.seconds = ts.tv_sec;
    payload.client_representation.client_timestamp.nanoseconds = ts.tv_nsec;
    payload.client_representation.client_key = (ClientKey){{0xAA, 0xBB, 0xCC, 0xDD}};
    payload.client_representation.session_id = 0xEE;
    payload.client_representation.optional_properties = false;
    payload.client_representation.properties.size = 0;

    state->session_id = SESSIONID_NONE_WITH_CLIENT_KEY;
    state->stream_id = STREAMID_NONE;
    state->key = payload.client_representation.client_key;

    if (add_create_client_submessage(&session->output_message, &payload))
    {
        PRINTL_CREATE_CLIENT_SUBMESSAGE(&payload);
        result = SESSION_INIT;
    }
    else
    {
        result = SESSION_SERIALIZATION_ERR;
    }

    return result;
}

XRCEInfo create_participant(Session* session)
{
    XRCEInfo info = {++session->next_request_id, ++session->next_object_id};

    CREATE_Payload payload;
    payload.base.request_id = get_raw_request_id(info.request_id);
    payload.base.object_id = get_raw_object_id(info.object_id);
    payload.object_representation.kind = OBJK_PARTICIPANT;
    payload.object_representation._.participant.base.representation.format = REPRESENTATION_BY_REFERENCE;
    payload.object_representation._.participant.base.representation._.object_reference.size = 0;

    if (add_create_resource_submessage(&session->output_message, &payload, (CreationMode){false, false}))
    {
        PRINTL_CREATE_RESOURCE_SUBMESSAGE(&payload);
    }

    return info;
}

XRCEInfo create_topic(Session* session, uint16_t participant_id, String xml)
{
    XRCEInfo info = {++session->next_request_id, ++session->next_object_id};

    CREATE_Payload payload;
    payload.base.request_id = get_raw_request_id(info.request_id);
    payload.base.object_id = get_raw_object_id(info.object_id);
    payload.object_representation.kind = OBJK_TOPIC;
    payload.object_representation._.topic.base.representation.format = REPRESENTATION_AS_XML_STRING;
    payload.object_representation._.topic.base.representation._.xml_string_represenatation.size = xml.length;
    payload.object_representation._.topic.base.representation._.xml_string_represenatation.data = xml.data;
    payload.object_representation._.topic.participant_id = get_raw_object_id(participant_id);

    if (add_create_resource_submessage(&session->output_message, &payload, (CreationMode){false, false}))
    {
        PRINTL_CREATE_RESOURCE_SUBMESSAGE(&payload);
    }

    return info;
}

XRCEInfo create_publisher(Session* session, uint16_t participant_id)
{
    XRCEInfo info = {++session->next_request_id, ++session->next_object_id};

    CREATE_Payload payload;
    payload.base.request_id = get_raw_request_id(info.request_id);
    payload.base.object_id = get_raw_object_id(info.object_id);
    payload.object_representation.kind = OBJK_PUBLISHER;
    payload.object_representation._.publisher.base.representation.format = REPRESENTATION_BY_REFERENCE;
    payload.object_representation._.publisher.base.representation._.object_name.size = 0;
    payload.object_representation._.publisher.participant_id = get_raw_object_id(participant_id);

    if (add_create_resource_submessage(&session->output_message, &payload, (CreationMode){false, false}))
    {
        PRINTL_CREATE_RESOURCE_SUBMESSAGE(&payload);
    }

    return info;
}

XRCEInfo create_subscriber(Session* session, uint16_t participant_id)
{
    XRCEInfo info = {++session->next_request_id, ++session->next_object_id};

    CREATE_Payload payload;
    payload.base.request_id = get_raw_request_id(info.request_id);
    payload.base.object_id = get_raw_object_id(info.object_id);
    payload.object_representation.kind = OBJK_SUBSCRIBER;
    payload.object_representation._.subscriber.base.representation.format = REPRESENTATION_BY_REFERENCE;
    payload.object_representation._.subscriber.base.representation._.object_name.size = 0;
    payload.object_representation._.subscriber.participant_id = get_raw_object_id(participant_id);

    if (add_create_resource_submessage(&session->output_message, &payload, (CreationMode){false, false}))
    {
        PRINTL_CREATE_RESOURCE_SUBMESSAGE(&payload);
    }

    return info;
}

XRCEInfo create_data_writer(Session* session, uint16_t participant_id, uint16_t publisher_id, String xml)
{
    XRCEInfo info = {++session->next_request_id, ++session->next_object_id};

    CREATE_Payload payload;
    payload.base.request_id = get_raw_request_id(info.request_id);
    payload.base.object_id = get_raw_object_id(info.object_id);
    payload.object_representation.kind = OBJK_DATAWRITER;
    payload.object_representation._.data_writer.base.format = REPRESENTATION_AS_XML_STRING;
    payload.object_representation._.data_writer.base._.xml_string_represenatation.size = xml.length;
    payload.object_representation._.data_writer.base._.xml_string_represenatation.data = xml.data;
    payload.object_representation._.data_writer.publisher_id = get_raw_object_id(publisher_id);

    if (add_create_resource_submessage(&session->output_message, &payload, (CreationMode){false, false}))
    {
        PRINTL_CREATE_RESOURCE_SUBMESSAGE(&payload);
    }

    return info;
}

XRCEInfo create_data_reader(Session* session, uint16_t participant_id, uint16_t subscriber_id, String xml)
{
    XRCEInfo info = {++session->next_request_id, ++session->next_object_id};

    CREATE_Payload payload;
    payload.base.request_id = get_raw_request_id(info.request_id);
    payload.base.object_id = get_raw_object_id(info.object_id);
    payload.object_representation.kind = OBJK_DATAREADER;
    payload.object_representation._.data_reader.base.format = REPRESENTATION_AS_XML_STRING;
    payload.object_representation._.data_reader.base._.xml_string_represenatation.size = xml.length;
    payload.object_representation._.data_reader.base._.xml_string_represenatation.data = xml.data;
    payload.object_representation._.data_reader.subscriber_id = get_raw_object_id(subscriber_id);

    if (add_create_resource_submessage(&session->output_message, &payload, (CreationMode){false, false}))
    {
        PRINTL_CREATE_RESOURCE_SUBMESSAGE(&payload);
    }

    return info;
}

XRCEInfo delete_resource(Session* session, uint16_t resource_id)
{
    XRCEInfo info = {++session->next_request_id, resource_id};

    DELETE_Payload payload;
    payload.base.request_id = get_raw_request_id(info.request_id);
    payload.base.object_id = get_raw_object_id(info.object_id);

    if (add_delete_resource_submessage(&session->output_message, &payload))
    {
        PRINTL_DELETE_RESOURCE_SUBMESSAGE(&payload);
    }

    return info;
}

XRCEInfo write_data(Session* session, uint16_t data_writer_id, SerializeTopic serialize_topic, void* topic)
{
    //Knows previously the topic serialized length. Is it feasible?
    char buffer[MAX_TOPIC_LENGTH];

    MicroBuffer writer;
    init_external_buffer(&writer, (uint8_t*)buffer, MAX_TOPIC_LENGTH);
    writer.endianness = session->output_message.writer.endianness;

    if(serialize_topic(&writer, &(AbstractTopic){topic}))
    {
        XRCEInfo info = {++session->next_request_id, data_writer_id};

        WRITE_DATA_Payload_Data payload;
        payload.base.request_id = get_raw_request_id(info.request_id);
        payload.base.object_id = get_raw_object_id(info.object_id);
        payload.data.data = writer.init;
        payload.data.size = writer.iterator - writer.init;

        if (add_write_data_submessage(&session->output_message, &payload))
        {
            PRINTL_WRITE_DATA_SUBMESSAGE(&payload);
        }
        return info;
    }

    return (XRCEInfo){0x0000, 0xFFFF};
}

XRCEInfo read_data(Session* session, uint16_t data_reader_id, OnMessageReceived on_message, void *callback_args)
{
    int16_t callback_request_id = register_callback_data(&session->callback_data_storage, FORMAT_DATA, on_message, callback_args);

    XRCEInfo info = {callback_request_id, data_reader_id};

    READ_DATA_Payload payload;
    payload.base.request_id = get_raw_request_id(info.request_id);
    payload.base.object_id = get_raw_object_id(info.object_id);
    payload.read_specification.data_format = FORMAT_DATA;
    payload.read_specification.optional_content_filter_expression = false;
    payload.read_specification.optional_delivery_control = false;

    if (add_read_data_submessage(&session->output_message, &payload))
    {
        PRINTL_READ_DATA_SUBMESSAGE(&payload);
    }
    return info;
}

/* ----------------------------------------------------------------------------------------------
                                CALLBACKS - AGENT TO CLIENT
 ---------------------------------------------------------------------------------------------- */
void on_initialize_message(MessageHeader* header, ClientKey* key, void* vsession)
{
    Session* session = (Session*) vsession;

    header->session_id = session->session_id;
    header->stream_id = session->stream_id;
    header->sequence_nr = session->output_sequence_number;
    *key = session->key;

    session->output_sequence_number++;
}

void on_out_of_bounds(void* vsession)
{
    send_to_agent((Session*) vsession);
}

bool on_message_header(const MessageHeader* header, const ClientKey* key, void* vsession)
{
    // TODO.
    (void) key;

    Session* session = (Session*) vsession;

    if(header->stream_id != STREAMID_NONE)
        if(header->sequence_nr != session->input_sequence_number)
        {
            PRINT_SEQUENCE_NUMBER(header->sequence_nr, session->input_sequence_number);
            return 0;
        }

    session->input_sequence_number++;
    return 1;
}

void on_status_submessage(const STATUS_Payload* payload, void* args)
{
    Session* session = (Session*) args;
    PRINTL_STATUS_SUBMESSAGE(payload);

    if(session->on_status_received)
    {
        XRCEInfo info = create_xrce_info(payload->base.related_request.request_id, payload->base.related_request.object_id);
        session->on_status_received(info,
                                  payload->base.result.status,
                                  payload->base.result.implementation_status,
                                  session->on_status_received_args);
    }
}

DataFormat on_data_submessage(const BaseObjectReply* reply, void* args)
{
    DataFormat result = FORMAT_EMPTY;
    Session* session = (Session*) args;
    XRCEInfo info = create_xrce_info(reply->related_request.request_id, reply->related_request.object_id);

    if(session->on_status_received)
    {
        session->on_status_received(info,
                                  reply->result.status,
                                  reply->result.implementation_status,
                                  session->on_status_received_args);
    }

    if(reply->result.status == STATUS_OK)
    {
        CallbackData* callback_data = get_callback_data(&session->callback_data_storage, info.request_id);
        if(callback_data)
        {
            result = callback_data->format;
        }
    }

    return result;
}

void on_data_payload(const BaseObjectReply* reply, const SampleData* data, void* args, Endianness endianness)
{
    Session* session = (Session*) args;
    PRINTL_DATA_SUBMESSAGE_SAMPLE_DATA(reply, data);

    XRCEInfo info = create_xrce_info(reply->related_request.request_id, reply->related_request.object_id);
    CallbackData* callback_data = get_callback_data(&session->callback_data_storage, info.request_id);
    if(callback_data)
    {
        MicroBuffer reader;
        init_external_buffer(&reader, data->data, data->size);
        reader.endianness = endianness;
        callback_data->on_message(info, &reader, callback_data->callback_args);
        remove_callback_data(&session->callback_data_storage, info.request_id);
    }
}

/* ----------------------------------------------------------------------------------------------
                                   COMMUNICATION
 ---------------------------------------------------------------------------------------------- */
bool send_to_agent(Session* session)
{
    uint32_t length = get_message_length(&session->output_message);
    if(length > 0)
    {
        int output_length = send_data(session->buffer, length, session->transport_id);
        if(output_length > 0)
        {
            PRINTL_SERIALIZATION(SEND, session->buffer, output_length);
            reset_buffer(&session->output_message.writer);

            return true;
        }
    }

    return false;
}

bool receive_from_agent(Session* session)
{
    int length = receive_data(session->buffer, session->buffer_size, session->transport_id);
    if(length > 0)
    {
        parse_message(&session->input_message, length);
        PRINTL_SERIALIZATION(RECV, session->buffer, length);

        return true;
    }

    return false;
}

/* ----------------------------------------------------------------------------------------------
                                CALLBACKS - DATA
 ---------------------------------------------------------------------------------------------- */
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
