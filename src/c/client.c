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

#include <micrortps/client/client.h>
#include <micrortps/client/reliable_control.h>
#include <micrortps/client/input_message.h>
#include <micrortps/client/output_message.h>
#include "log/message.h"

#include <string.h>
#include <time.h>

uint8_t new_udp_session(Session* const session,
                        SessionId id,
                        ClientKey key,
                        uint16_t remote_port,
                        const uint8_t* const server_ip,
                        micrortps_locator_t* const locator
                        OnTopic on_topic_callback,
                        void* on_topic_args)
{
    if (NULL == session || NULL == server_ip || NULL == locator)
    {
        printf("# new_udp_session() -> BAD PARAMETERS!\n");
        return MICRORTPS_ERR_LOCATOR;
    }

    uint8_t result = MICRORTPS_STATUS_OK;
    locator_id_t locator_id = add_udp_locator_client(remote_port, server_ip, locator);

    if (0 >= locator_id)
    {
        result = MICRORTPS_ERR_LOCATOR;
    }
    else
    {
        /* Select id and key. */
        session->id = id;
        session->key = key;
        session->transport_id = locator_id;

        /* Init request id. */
        session->request_id = 0;
        session->header_offset = (128 > id) ? HEADER_MAX_SIZE : HEADER_MIN_SIZE;

        /* Init streams. */
        // input best effort
        session->input_best_effort_stream.seq_num = 0;
        init_micro_buffer(&session->input_best_effort_stream.micro_buffer,
                          session->input_best_effort_stream.buf, MICRORTPS_MTU_SIZE);

        // output best effort
        session->output_best_effort_stream.seq_num = 0;
        init_micro_buffer_endian(&session->output_best_effort_stream.micro_buffer,
                          session->output_best_effort_stream.buf, MICRORTPS_MTU_SIZE, MACHINE_ENDIANNESS);
        session->output_best_effort_stream.micro_buffer.iterator += session->header_offset;

        // input reliable
        session->input_reliable_stream.seq_num = 0;
        for (int i = 0; i < MICRORTPS_MAX_MSG_NUM; i++)
        {
            init_micro_buffer(&session->input_reliable_stream.store[i].micro_buffer,
                              session->input_reliable_stream.store[i].buf, MICRORTPS_MTU_SIZE);
        }

        // output reliable
        session->output_reliable_stream.seq_num = 0;
        for (int i = 0; i < MICRORTPS_MAX_MSG_NUM; i++)
        {
            init_micro_buffer_endian(&session->output_reliable_stream.store[i].micro_buffer,
                              session->output_reliable_stream.store[i].buf, MICRORTPS_MTU_SIZE, MACHINE_ENDIANNESS);
            session->output_reliable_stream.store[i].micro_buffer.iterator += session->header_offset;
        }

        session->on_topic_callback = on_topic_callback;
        session->on_topic_args = on_topic_args;

        session->last_status_received = false;
    }

    return result;
}

void close_session(Session* session)
{
    remove_locator(session->transport_id);
}

bool init_session_syn(Session* session)
{
    /* Generate CREATE_CLIENT_Payload. */
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
    CREATE_CLIENT_Payload payload;
    payload.base.request_id = get_raw_request_id(session->request_id);
    payload.base.object_id = OBJECTID_CLIENT;
    payload.client_representation.xrce_cookie = XRCE_COOKIE;
    payload.client_representation.xrce_version = XRCE_VERSION;
    payload.client_representation.xrce_vendor_id = (XrceVendorId){{0x01, 0x0F}};
    payload.client_representation.client_timestamp.seconds = ts.tv_sec;
    payload.client_representation.client_timestamp.nanoseconds = ts.tv_nsec;
    payload.client_representation.client_key = session->key;
    payload.client_representation.session_id = session->id;
    payload.client_representation.optional_properties = false;
    payload.client_representation.properties.size = 0;

    uint8_t buffer[MICRORTPS_MIN_BUFFER_SIZE];
    MicroBuffer output_buffer;
    init_micro_buffer_endian(&output_buffer, buffer, sizeof(buffer), MACHINE_ENDIANNESS);

    bool serialization_result = true;

    /* Serialize MessageHeader. */
    MessageHeader header;
    header.session_id = SESSIONID_NONE_WITH_CLIENT_KEY;
    header.stream_id = 0x00;
    header.sequence_nr = 0;
    serialization_result &= serialize_MessageHeader(&output_buffer, &header);
    if (128 > header.session_id)
    {
        serialization_result &= serialize_ClientKey(&output_buffer, &session->key);
    }

    /* Serialize CREATE_CLIENT_Payload. */
    MicroState submessage_begin = get_micro_state(&output_buffer);
    output_buffer.iterator += 4;
    serialization_result &= serialize_CREATE_CLIENT_Payload(&output_buffer, &payload);
    MicroState submessage_end = get_micro_state(&output_buffer);

    /* Serialize SubmessageHeader. */
    restore_micro_state(&output_buffer, submessage_begin);
    SubmessageHeader sub_header;
    sub_header.id = SUBMESSAGE_ID_CREATE_CLIENT;
    /* TODO (Julian): introduce flags. */
    sub_header.flags = 0x07;
    sub_header.length = (submessage_end.position - submessage_begin.position) - 4;
    serialize_SubmessageHeader(&output_buffer, &sub_header);
    restore_micro_state(&output_buffer, submessage_end);

    /* Send message and wait for status. */
    PRINTL_CREATE_CLIENT_SUBMESSAGE(&payload);
    PRINTL_SERIALIZATION(SEND, output_buffer.init, output_buffer.iterator - output_buffer.init);

    send_data(output_buffer.init, (output_buffer.iterator - output_buffer.init), session->transport_id);

    return run_until_status(session);
}

bool create_participant_sync_by_ref(Session* session,
                               const ObjectId object_id,
                               const char* ref,
                               bool reuse,
                               bool replace)
{
    /* Check participant id. */
    if ((object_id.data[1] & 0x0F) != OBJK_PARTICIPANT)
    {
        session->last_status.status = STATUS_ERR_UNKNOWN_REFERENCE;
        session->last_status_received = true;
        return false;
    }

    session->request_id++;

    size_t len = strlen(ref);
    if (len > REFERENCE_MAX_LEN)
    {
        len = REFERENCE_MAX_LEN;
    }

    CREATE_Payload payload;
    payload.base.request_id = get_raw_request_id(session->request_id);
    payload.base.object_id = object_id;
    payload.object_representation.kind = OBJK_PARTICIPANT;
    payload.object_representation._.participant.base.representation.format = REPRESENTATION_BY_REFERENCE;
    payload.object_representation._.participant.base.representation._.object_reference.data = (char*)ref; //MISRA non-compliant?
    payload.object_representation._.participant.base.representation._.object_reference.size = len;

    return create_reliable_object_sync(session, &session->output_reliable_stream, &payload, reuse, replace);
}

bool create_topic_sync_by_xml(Session* session,
                         const ObjectId object_id,
                         const char* xml,
                         const ObjectId participant_id,
                         bool reuse,
                         bool replace)
{
    /* Check participant and topic ids. */
    if ((object_id.data[1] & 0x0F) != OBJK_TOPIC || (participant_id.data[1] & 0x01) != OBJK_PARTICIPANT)
    {
        session->last_status.status = STATUS_ERR_UNKNOWN_REFERENCE;
        session->last_status_received = true;
        return false;
    }

    session->request_id++;

    CREATE_Payload payload;
    payload.base.request_id = get_raw_request_id(session->request_id);
    payload.base.object_id = object_id;
    payload.object_representation.kind = OBJK_TOPIC;
    payload.object_representation._.topic.base.representation.format = REPRESENTATION_AS_XML_STRING;
    payload.object_representation._.topic.base.representation._.xml_string_represenatation.data = (char*)xml;
    payload.object_representation._.topic.base.representation._.xml_string_represenatation.size = strlen(xml);
    payload.object_representation._.topic.participant_id = participant_id;

    return create_reliable_object_sync(session, &session->output_reliable_stream, &payload, reuse, replace);
}

bool create_publisher_sync_by_xml(Session* session,
                             const ObjectId object_id,
                             const char* xml,
                             const ObjectId participant_id,
                             bool reuse,
                             bool replace)
{
    /* Check participant and publisher ids. */
    if ((object_id.data[1] & 0x0F) != OBJK_PUBLISHER && (participant_id.data[1] & 0x0F) != OBJK_PARTICIPANT)
    {
        session->last_status.status = STATUS_ERR_UNKNOWN_REFERENCE;
        session->last_status_received = true;
        return false;
    }

    session->request_id++;

    CREATE_Payload payload;
    payload.base.request_id = get_raw_request_id(session->request_id);
    payload.base.object_id = object_id;
    payload.object_representation.kind = OBJK_PUBLISHER;
    payload.object_representation._.publisher.base.representation.format = REPRESENTATION_AS_XML_STRING;
    payload.object_representation._.publisher.base.representation._.string_represenatation.data = (char*)xml;
    payload.object_representation._.publisher.base.representation._.string_represenatation.size = strlen(xml);
    payload.object_representation._.publisher.participant_id = participant_id;

    return create_reliable_object_sync(session, &session->output_reliable_stream, &payload, reuse, replace);
}

bool create_subscriber_sync_by_xml(Session* session,
                              const ObjectId object_id,
                              const char* xml,
                              const ObjectId participant_id,
                              bool reuse,
                              bool replace)
{
    /* Check participant and publisher ids. */
    if ((object_id.data[1] & 0x0F) != OBJK_SUBSCRIBER && (participant_id.data[1] & 0x0F) != OBJK_PARTICIPANT)
    {
        session->last_status.status = STATUS_ERR_UNKNOWN_REFERENCE;
        session->last_status_received = true;
        return false;
    }

    session->request_id++;

    CREATE_Payload payload;
    payload.base.request_id = get_raw_request_id(session->request_id);
    payload.base.object_id = object_id;
    payload.object_representation.kind = OBJK_SUBSCRIBER;
    payload.object_representation._.subscriber.base.representation.format = REPRESENTATION_AS_XML_STRING;
    payload.object_representation._.subscriber.base.representation._.string_represenatation.data = (char*)xml;
    payload.object_representation._.subscriber.base.representation._.string_represenatation.size = strlen(xml);
    payload.object_representation._.subscriber.participant_id = participant_id;

    return create_reliable_object_sync(session, &session->output_reliable_stream, &payload, reuse, replace);
}

bool create_datawriter_sync_by_xml(Session* session,
                              const ObjectId object_id,
                              const char* xml,
                              const ObjectId publisher_id,
                              bool reuse,
                              bool replace)
{
    /* Check publisher and datawriter ids. */
    if ((object_id.data[1] & 0x0F) != OBJK_DATAWRITER && (publisher_id.data[1] & 0x0F) != OBJK_PUBLISHER)
    {
        session->last_status.status = STATUS_ERR_UNKNOWN_REFERENCE;
        session->last_status_received = true;
        return false;
    }

    session->request_id++;

    CREATE_Payload payload;
    payload.base.request_id = get_raw_request_id(session->request_id);
    payload.base.object_id = object_id;
    payload.object_representation.kind = OBJK_DATAWRITER;
    payload.object_representation._.data_writer.base.representation.format = REPRESENTATION_AS_XML_STRING;
    payload.object_representation._.data_writer.base.representation._.string_represenatation.data = (char*)xml;
    payload.object_representation._.data_writer.base.representation._.string_represenatation.size = strlen(xml);
    payload.object_representation._.data_writer.publisher_id = publisher_id;

    return create_reliable_object_sync(session, &session->output_reliable_stream, &payload, reuse, replace);
}

bool create_datareader_sync_by_xml(Session* session,
                              const ObjectId object_id,
                              const char* xml,
                              const ObjectId subscriber_id,
                              bool reuse,
                              bool replace)
{
    /* Check participant and publisher ids. */
    if ((object_id.data[1] & 0x0F) != OBJK_DATAREADER && (subscriber_id.data[1] & 0x0F) != OBJK_SUBSCRIBER)
    {
        session->last_status.status = STATUS_ERR_UNKNOWN_REFERENCE;
        session->last_status_received = true;
        return false;
    }

    session->request_id++;

    CREATE_Payload payload;
    payload.base.request_id = get_raw_request_id(session->request_id);
    payload.base.object_id = object_id;
    payload.object_representation.kind = OBJK_DATAREADER;
    payload.object_representation._.data_reader.base.representation.format = REPRESENTATION_AS_XML_STRING;
    payload.object_representation._.data_reader.base.representation._.string_represenatation.data = (char*)xml;
    payload.object_representation._.data_reader.base.representation._.string_represenatation.size = strlen(xml);
    payload.object_representation._.data_reader.subscriber_id = subscriber_id;

    return create_reliable_object_sync(session, &session->output_reliable_stream, &payload, reuse, replace);
}

bool delete_object_sync(Session* session, ObjectId object_id)
{
    session->request_id++;

    DELETE_Payload payload;
    payload.base.request_id = get_raw_request_id(session->request_id);
    payload.base.object_id = object_id;

    ReliableStream* reliable = &session->output_reliable_stream;
    uint32_t payload_size = 4; //size_of_Delete_Payload(payload)
    MicroBuffer* buffer = prepare_reliable_stream(reliable, SUBMESSAGE_ID_DELETE, payload_size);

    deserialize_DELETE_Payload(buffer, &payload);

    PRINTL_DELETE_RESOURCE_SUBMESSAGE(&payload);

    return run_until_status(session);
}

bool read_data_sync(Session* session, ObjectId data_reader_id)
{
    if ((data_reader_id.data[1] & 0x0F) != OBJK_DATAREADER)
    {
        session->last_status.status = STATUS_ERR_UNKNOWN_REFERENCE;
        session->last_status_received = true;
        return false;
    }

    session->request_id++;

    READ_DATA_Payload payload;
    payload.base.request_id = get_raw_request_id(session->request_id);
    payload.base.object_id = data_reader_id;
    payload.read_specification.optional_content_filter_expression = false;
    payload.read_specification.optional_delivery_control = false;
    payload.read_specification.data_format = FORMAT_DATA;

    ReliableStream* reliable = &session->output_reliable_stream;
    uint32_t payload_size = 7; //size_of_READ_DATA_Payload(payload)
    MicroBuffer* buffer = prepare_reliable_stream(reliable, SUBMESSAGE_ID_READ_DATA, payload_size);

    deserialize_READ_DATA_Payload(buffer, &payload);

    PRINTL_READ_DATA_SUBMESSAGE(&payload);

    return run_until_status(session);
}

bool create_reliable_object_sync(Session* session, ReliableStream* output_stream, const CREATE_Payload* payload, bool reuse, bool replace)
{
    MicroBuffer* output_buffer = &output_stream->store[output_stream->seq_num].micro_buffer;

    /* Serialize CREATE_Payload. */
    MicroState submessage_begin = get_micro_state(output_buffer);
    output_buffer->iterator += 4;
    serialize_CREATE_Payload(output_buffer, payload);
    MicroState submessage_end = get_micro_state(output_buffer);

    /* Serialize SubmessageHeader. */
    restore_micro_state(output_buffer, submessage_begin);
    SubmessageHeader sub_header;
    sub_header.id = SUBMESSAGE_ID_CREATE;
    sub_header.flags = 0;
    sub_header.flags |= output_buffer->endianness;
    sub_header.flags |= FLAG_REUSE * reuse;
    sub_header.flags |= FLAG_REPLACE * replace;
    sub_header.length = (submessage_end.position - submessage_begin.position) - 4;
    serialize_SubmessageHeader(output_buffer, &sub_header);
    restore_micro_state(output_buffer, submessage_end);

    PRINTL_CREATE_RESOURCE_SUBMESSAGE(payload);

    return run_until_status(session);
}

MicroBuffer* prepare_reliable_stream(ReliableStream* output_stream, uint8_t submessage_id, uint16_t payload_size)
{
    MicroBuffer* output_buffer = &output_stream->store[output_stream->seq_num].micro_buffer;
    if(SUBHEADER_SIZE + payload_size > output_buffer->final - output_buffer->iterator)
    {
        return NULL;
    }

    align_to(output_buffer, 4);

    SubmessageHeader sub_header = (SubmessageHeader){ submessage_id, output_buffer->endianness, payload_size };
    serialize_SubmessageHeader(output_buffer, &sub_header);

    return output_buffer;
}


MicroBuffer* prepare_best_effort_stream_for_topic(BestEffortStream* output_stream, ObjectId data_writer_id, uint16_t topic_size)
{
    MicroBuffer* output_buffer = &output_stream->micro_buffer;
    if(SUBHEADER_SIZE + PAYLOAD_DATA_SIZE + topic_size > output_buffer->final - output_buffer->iterator)
    {
        return NULL;
    }

    align_to(output_buffer, 4);

    SubmessageHeader sub_header = (SubmessageHeader){ SUBMESSAGE_ID_DATA, output_buffer->endianness, PAYLOAD_DATA_SIZE + topic_size };
    serialize_SubmessageHeader(output_buffer, &sub_header);

    WRITE_DATA_Payload_Data payload;
    payload.base.request_id = (RequestId){{0, 0}};
    payload.base.object_id = data_writer_id;
    serialize_BaseObjectRequest(output_buffer, &payload.base);
    serialize_uint32_t(output_buffer, topic_size); //Remove in next version. Emulates Data payload data.

    PRINTL_WRITE_DATA_DATA_SUBMESSAGE(&payload);

    return output_buffer;
}

MicroBuffer* prepare_reliable_stream_for_topic(ReliableStream* output_stream, ObjectId data_writer_id, uint16_t topic_size)
{
    MicroBuffer* output_buffer = &output_stream->store[output_stream->seq_num].micro_buffer;
    if(SUBHEADER_SIZE + PAYLOAD_DATA_SIZE + topic_size > output_buffer->final - output_buffer->iterator)
    {
        return NULL;
    }

    align_to(output_buffer, 4);

    SubmessageHeader sub_header = (SubmessageHeader){ SUBMESSAGE_ID_DATA, output_buffer->endianness, PAYLOAD_DATA_SIZE + topic_size };
    serialize_SubmessageHeader(output_buffer, &sub_header);

    WRITE_DATA_Payload_Data payload;
    payload.base.request_id = (RequestId){{0, 0}};
    payload.base.object_id = data_writer_id;
    serialize_BaseObjectRequest(output_buffer, &payload.base);
    serialize_uint32_t(output_buffer, topic_size); //Remove in next version. Emulates Data payload data.

    PRINTL_WRITE_DATA_DATA_SUBMESSAGE(&payload);

    return output_buffer;
}


void stamp_header(Session* session, MicroBuffer* output_buffer, StreamId id, uint16_t seq_num)
{
    MicroBuffer header_buffer;
    init_micro_buffer(&header_buffer, output_buffer->init, HEADER_MAX_SIZE);

    MessageHeader header = (MessageHeader){session->id, id, seq_num};
    (void) serialize_MessageHeader(&header_buffer, &header);
    if (128 > session->id)
    {
        (void) serialize_ClientKey(&header_buffer, &session->key);
    }
}

void run_communication(Session* session)
{
    BestEffortStream* output_best_effort_stream = &session->output_best_effort_stream;
    MicroBuffer* output_best_effort_buffer = &output_best_effort_stream->micro_buffer;
    if(output_best_effort_buffer->iterator - output_best_effort_buffer->init > HEADER_MAX_SIZE)
    {
        send_best_effort_message(session, output_best_effort_stream);
    }

    ReliableStream* output_reliable_stream = &session->output_reliable_stream;
    MicroBuffer* output_reliable_buffer = &output_reliable_stream->store[output_reliable_stream->seq_num].micro_buffer;
    if(output_reliable_buffer->iterator - output_reliable_buffer->init > HEADER_MAX_SIZE)
    {
        send_reliable_message(session, output_reliable_stream);
        send_heartbeat(session, output_reliable_stream);
    }

    /* TODO (Luis): add timeout funcionality */
    /* We suppose that the time timeout will be the minimun time to get data from the netword card. */
    uint8_t buffer[MICRORTPS_MTU_SIZE];
    uint32_t length = 0;
    while (0 < (length = receive_data(buffer, MICRORTPS_MTU_SIZE, session->transport_id)))
    {
        MicroBuffer input_buffer;
        init_micro_buffer(&input_buffer, buffer, length);

        if(input_buffer.final - input_buffer.init > session->header_offset)
        {
            PRINTL_SERIALIZATION(RECV, buffer, length);
            process_message(session, &input_buffer);
        }
    }

    ReliableStream* input_reliable_stream = &session->input_reliable_stream;
    if(input_reliable_stream->last_seq_num >= input_reliable_stream->seq_num)
    {
        send_acknack(session, input_reliable_stream);
    }
}


bool run_until_status(Session* session)
{
    //TODO add timeout
    uint32_t attempts_counter = 0;
    session->last_status_received = false;
    while (!session->last_status_received && attempts_counter < MICRORTPS_MAX_ATTEMPTS)
    {
        run_communication(session);
        attempts_counter++;
    }

    return session->last_status_received && session->last_status.status == STATUS_OK;
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

