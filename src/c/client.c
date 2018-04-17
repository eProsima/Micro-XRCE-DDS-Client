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

bool new_udp_session(Session* const session,
                     SessionId id,
                     ClientKey key,
                     const uint8_t* const server_ip,
                     uint16_t remote_port,
                     OnTopic on_topic_callback,
                     void* on_topic_args)
{
    if (NULL == session || NULL == server_ip)
    {
        printf("# new_udp_session() -> BAD PARAMETERS!\n");
        return false;
    }

    bool result = false;
    locator_id_t locator_id = add_udp_locator_client(remote_port, server_ip, &session->locator);

    if (0 < locator_id)
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
        session->input_best_effort_stream.last_handled = UINT16_MAX;

        // output best effort
        session->output_best_effort_stream.last_sent = UINT16_MAX;
        init_micro_buffer_offset(&session->output_best_effort_stream.buffer.micro_buffer,
                                  session->output_best_effort_stream.buffer.data, MICRORTPS_MTU_SIZE,
                                  session->header_offset);

        // input reliable
        session->input_reliable_stream.last_handled = UINT16_MAX;
        session->input_reliable_stream.last_announced = UINT16_MAX;
        for (int i = 0; i < MICRORTPS_MAX_MSG_NUM; i++)
        {
            init_micro_buffer(&session->input_reliable_stream.buffers[i].micro_buffer,
                              session->input_reliable_stream.buffers[i].data, MICRORTPS_MTU_SIZE);
        }
        session->input_reliable_stream.last_acknack_timestamp = 0;

        // output reliable
        session->output_reliable_stream.last_sent = UINT16_MAX;
        session->output_reliable_stream.last_acknown = UINT16_MAX;
        for (int i = 0; i < MICRORTPS_MAX_MSG_NUM; i++)
        {
            init_micro_buffer_offset(&session->output_reliable_stream.buffers[i].micro_buffer,
                                      session->output_reliable_stream.buffers[i].data, MICRORTPS_MTU_SIZE,
                                      session->header_offset);
        }
        session->output_reliable_stream.last_heartbeat_timestamp = 0;

        session->on_topic_callback = on_topic_callback;
        session->on_topic_args = on_topic_args;

        session->last_status_received = false;
        session->last_status_request_id = false;
        result = true;
    }

    return result;
}

void free_udp_session(Session* session)
{
    remove_locator(session->transport_id);
}

bool init_session_sync(Session* session)
{
    /* Generate CREATE_CLIENT_Payload. */
    uint64_t nanoseconds = get_nano_time();

    CREATE_CLIENT_Payload payload;
    payload.base.request_id = get_raw_request_id(session->request_id);
    payload.base.object_id = OBJECTID_CLIENT;
    payload.client_representation.xrce_cookie = XRCE_COOKIE;
    payload.client_representation.xrce_version = XRCE_VERSION;
    payload.client_representation.xrce_vendor_id = (XrceVendorId){{0x01, 0x0F}};
    payload.client_representation.client_timestamp.seconds = (int32_t)(nanoseconds / 1000000000);
    payload.client_representation.client_timestamp.nanoseconds = nanoseconds % 1000000000;
    payload.client_representation.client_key = session->key;
    payload.client_representation.session_id = session->id;
    payload.client_representation.optional_properties = false;
    payload.client_representation.properties.size = 0;

    uint8_t buffer[MICRORTPS_MIN_BUFFER_SIZE];
    MicroBuffer output_buffer;
    init_micro_buffer(&output_buffer, buffer, sizeof(buffer));

    /* Serialize MessageHeader. */
    MessageHeader header;
    header.session_id = SESSIONID_NONE_WITH_CLIENT_KEY;
    header.stream_id = 0x00;
    header.sequence_nr = 0;
    serialize_MessageHeader(&output_buffer, &header);
    if (128 > header.session_id)
    {
        serialize_ClientKey(&output_buffer, &session->key);
    }

    /* Serialize CREATE_CLIENT_Payload. */
    MicroState submessage_begin = get_micro_state(&output_buffer);
    output_buffer.iterator += 4;
    serialize_CREATE_CLIENT_Payload(&output_buffer, &payload);
    MicroState submessage_end = get_micro_state(&output_buffer);

    /* Serialize SubmessageHeader. */
    restore_micro_state(&output_buffer, submessage_begin);
    SubmessageHeader sub_header;
    sub_header.id = SUBMESSAGE_ID_CREATE_CLIENT;
    /* TODO (Julian): introduce flags. */
    sub_header.flags = 0x07;
    sub_header.length = (uint16_t)(submessage_end.position - submessage_begin.position) - (uint16_t)4u;
    serialize_SubmessageHeader(&output_buffer, &sub_header);
    restore_micro_state(&output_buffer, submessage_end);

    /* Send message and wait for status. */
    PRINTL_CREATE_CLIENT_SUBMESSAGE(&payload);

    return send_until_status(session, session->request_id, MICRORTPS_BEST_EFFORT_MAX_ATTEMPTS, &output_buffer);
}

bool close_session_sync(Session* session)
{
    uint8_t buffer[MICRORTPS_MIN_BUFFER_SIZE];
    MicroBuffer output_buffer;
    init_micro_buffer(&output_buffer, buffer, sizeof(buffer));

    MessageHeader header;
    header.session_id = SESSIONID_NONE_WITH_CLIENT_KEY;
    header.stream_id = 0x00;
    header.sequence_nr = 0;
    serialize_MessageHeader(&output_buffer, &header);
    if (128 > header.session_id)
    {
        serialize_ClientKey(&output_buffer, &session->key);
    }

    uint16_t payload_size = 4; //size_of_Delete_Payload(payload)

    align_to(&output_buffer, 4);
    SubmessageHeader sub_header = (SubmessageHeader){ SUBMESSAGE_ID_DELETE, output_buffer.endianness, payload_size };
    serialize_SubmessageHeader(&output_buffer, &sub_header);

    DELETE_Payload payload;
    payload.base.request_id = get_raw_request_id(session->request_id);
    payload.base.object_id = OBJECTID_CLIENT;
    serialize_DELETE_Payload(&output_buffer, &payload);
    PRINTL_DELETE_RESOURCE_SUBMESSAGE(&payload);

    return send_until_status(session, session->request_id, MICRORTPS_BEST_EFFORT_MAX_ATTEMPTS, &output_buffer);
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
    payload.object_representation._.participant.base.representation._.object_reference.size = (uint32_t)len;

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
    payload.object_representation._.topic.base.representation._.xml_string_represenatation.size = (uint32_t)strlen(xml);
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
    if ((object_id.data[1] & 0x0F) != OBJK_PUBLISHER || (participant_id.data[1] & 0x0F) != OBJK_PARTICIPANT)
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
    payload.object_representation._.publisher.base.representation._.string_represenatation.size = (uint32_t)strlen(xml);
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
    if ((object_id.data[1] & 0x0F) != OBJK_SUBSCRIBER || (participant_id.data[1] & 0x0F) != OBJK_PARTICIPANT)
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
    payload.object_representation._.subscriber.base.representation._.string_represenatation.size = (uint32_t)strlen(xml);
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
    if ((object_id.data[1] & 0x0F) != OBJK_DATAWRITER || (publisher_id.data[1] & 0x0F) != OBJK_PUBLISHER)
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
    payload.object_representation._.data_writer.base.representation._.string_represenatation.size = (uint32_t)strlen(xml);
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
    if ((object_id.data[1] & 0x0F) != OBJK_DATAREADER || (subscriber_id.data[1] & 0x0F) != OBJK_SUBSCRIBER)
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
    payload.object_representation._.data_reader.base.representation._.string_represenatation.size = (uint32_t)strlen(xml);
    payload.object_representation._.data_reader.subscriber_id = subscriber_id;

    return create_reliable_object_sync(session, &session->output_reliable_stream, &payload, reuse, replace);
}

bool delete_object_sync(Session* session, ObjectId object_id)
{
    session->request_id++;

    DELETE_Payload payload;
    payload.base.request_id = get_raw_request_id(session->request_id);
    payload.base.object_id = object_id;

    OutputReliableStream* reliable = &session->output_reliable_stream;
    uint16_t payload_size = 4; //size_of_Delete_Payload(payload)

    MicroBuffer* buffer = prepare_reliable_stream(reliable, SUBMESSAGE_ID_DELETE, payload_size);
    if(NULL != buffer)
    {
        serialize_DELETE_Payload(buffer, &payload);

        PRINTL_DELETE_RESOURCE_SUBMESSAGE(&payload);
    }

    return run_until_status(session, session->request_id, MICRORTPS_MAX_ATTEMPTS);
}

bool read_data_sync(Session* session, ObjectId data_reader_id, StreamId stream_id)
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

    uint16_t payload_size = 7; //size_of_READ_DATA_Payload(payload)

    bool result = false;
    if(128 > stream_id)
    {
        OutputBestEffortStream* best_effort = &session->output_best_effort_stream;
        MicroBuffer* buffer = prepare_best_effort_stream(best_effort, SUBMESSAGE_ID_READ_DATA, payload_size);
        if(NULL != buffer)
        {
            serialize_READ_DATA_Payload(buffer, &payload);
            PRINTL_READ_DATA_SUBMESSAGE(&payload);

            //TODO(Luis): Fix this. Temporal approach.
            MicroBuffer temp;
            init_micro_buffer_offset(&temp, buffer->init, (uint32_t)(buffer->final - buffer->init), (uint32_t)(buffer->iterator - buffer->init));

            send_best_effort_message(session, best_effort);
            result = send_until_status(session, session->request_id, MICRORTPS_BEST_EFFORT_MAX_ATTEMPTS, &temp);
        }
    }
    else
    {
        OutputReliableStream* reliable = &session->output_reliable_stream;
        MicroBuffer* buffer = prepare_reliable_stream(reliable, SUBMESSAGE_ID_READ_DATA, payload_size);
        if(NULL != buffer)
        {
            serialize_READ_DATA_Payload(buffer, &payload);
            PRINTL_READ_DATA_SUBMESSAGE(&payload);

            result = run_until_status(session, session->request_id, MICRORTPS_MAX_ATTEMPTS);
        }
    }

    return result;
}

bool create_reliable_object_sync(Session* session, OutputReliableStream* output_stream, const CREATE_Payload* payload, bool reuse, bool replace)
{
    MicroBuffer* output_buffer = &output_stream->buffers[seq_num_add(output_stream->last_sent, 1) % MICRORTPS_MAX_MSG_NUM].micro_buffer;

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
    sub_header.length = (uint16_t)(submessage_end.position - submessage_begin.position) - (uint16_t)4u;
    serialize_SubmessageHeader(output_buffer, &sub_header);
    restore_micro_state(output_buffer, submessage_end);

    PRINTL_CREATE_RESOURCE_SUBMESSAGE(payload);

    return run_until_status(session, session->request_id, MICRORTPS_MAX_ATTEMPTS);
}

bool reliable_stream_is_available(OutputReliableStream* output_stream)
{
    return seq_num_sub(output_stream->last_sent, output_stream->last_acknown) != MICRORTPS_MAX_MSG_NUM;
}

MicroBuffer* prepare_best_effort_stream(OutputBestEffortStream* output_stream, uint8_t submessage_id, uint16_t payload_size)
{
    MicroBuffer* output_buffer = &output_stream->buffer.micro_buffer;

    if(SUBHEADER_SIZE + payload_size > output_buffer->final - output_buffer->iterator)
    {
        return NULL;
    }

    align_to(output_buffer, 4);

    SubmessageHeader sub_header = (SubmessageHeader){ submessage_id, output_buffer->endianness, payload_size };
    serialize_SubmessageHeader(output_buffer, &sub_header);

    return output_buffer;
}

MicroBuffer* prepare_reliable_stream(OutputReliableStream* output_stream, uint8_t submessage_id, uint16_t payload_size)
{
    MicroBuffer* output_buffer = &output_stream->buffers[seq_num_add(output_stream->last_sent, 1) % MICRORTPS_MAX_MSG_NUM].micro_buffer;
    if(!reliable_stream_is_available(output_stream)
        || SUBHEADER_SIZE + payload_size > output_buffer->final - output_buffer->iterator)
    {
        return NULL;
    }

    align_to(output_buffer, 4);

    SubmessageHeader sub_header = (SubmessageHeader){ submessage_id, output_buffer->endianness, payload_size };
    serialize_SubmessageHeader(output_buffer, &sub_header);

    return output_buffer;
}


MicroBuffer* prepare_best_effort_stream_for_topic(OutputBestEffortStream* output_stream, ObjectId data_writer_id, uint16_t topic_size)
{
    MicroBuffer* output_buffer = &output_stream->buffer.micro_buffer;
    if(SUBHEADER_SIZE + PAYLOAD_DATA_SIZE + topic_size > output_buffer->final - output_buffer->iterator)
    {
        return NULL;
    }

    align_to(output_buffer, 4);

    SubmessageHeader sub_header = (SubmessageHeader){ SUBMESSAGE_ID_WRITE_DATA, output_buffer->endianness, PAYLOAD_DATA_SIZE + topic_size };
    serialize_SubmessageHeader(output_buffer, &sub_header);

    WRITE_DATA_Payload_Data payload;
    payload.base.request_id = (RequestId){{0, 0}};
    payload.base.object_id = data_writer_id;
    serialize_BaseObjectRequest(output_buffer, &payload.base);
    serialize_uint32_t(output_buffer, topic_size); //Remove in next version. Emulates Data payload data.

    PRINTL_WRITE_DATA_DATA_SUBMESSAGE(&payload);

    return output_buffer;
}

MicroBuffer* prepare_reliable_stream_for_topic(OutputReliableStream* output_stream, ObjectId data_writer_id, uint16_t topic_size)
{
    MicroBuffer* output_buffer = &output_stream->buffers[seq_num_add(output_stream->last_sent, 1) % MICRORTPS_MAX_MSG_NUM].micro_buffer;
    if(!reliable_stream_is_available(output_stream)
        || SUBHEADER_SIZE + PAYLOAD_DATA_SIZE + topic_size > output_buffer->final - output_buffer->iterator)
    {
        return NULL;
    }

    align_to(output_buffer, 4);

    SubmessageHeader sub_header = (SubmessageHeader){ SUBMESSAGE_ID_WRITE_DATA, output_buffer->endianness, PAYLOAD_DATA_SIZE + topic_size };
    serialize_SubmessageHeader(output_buffer, &sub_header);

    WRITE_DATA_Payload_Data payload;
    payload.base.request_id = (RequestId){{0, 0}};
    payload.base.object_id = data_writer_id;
    serialize_BaseObjectRequest(output_buffer, &payload.base);
    serialize_uint32_t(output_buffer, topic_size); //Remove in next version. Emulates Data payload data.

    PRINTL_WRITE_DATA_DATA_SUBMESSAGE(&payload);

    return output_buffer;
}


void stamp_header(Session* session, uint8_t* buffer, StreamId stream_id, uint16_t seq_num)
{
    MicroBuffer header_buffer;
    init_micro_buffer(&header_buffer, buffer, session->header_offset);

    MessageHeader header = (MessageHeader){session->id, stream_id, seq_num};
    (void) serialize_MessageHeader(&header_buffer, &header);
    if (128 > session->id)
    {
        (void) serialize_ClientKey(&header_buffer, &session->key);
    }
}

void run_communication(Session* session)
{
    /* Send phase */
    OutputBestEffortStream* output_best_effort_stream = &session->output_best_effort_stream;
    MicroBuffer* output_best_effort_buffer = &output_best_effort_stream->buffer.micro_buffer;
    if(output_best_effort_buffer->iterator - output_best_effort_buffer->init > session->header_offset)
    {
        send_best_effort_message(session, output_best_effort_stream);
    }

    OutputReliableStream* output_reliable_stream = &session->output_reliable_stream;
    if(reliable_stream_is_available(output_reliable_stream))
    {
        MicroBuffer* output_reliable_buffer = &output_reliable_stream->buffers[seq_num_add(output_reliable_stream->last_sent, 1) % MICRORTPS_MAX_MSG_NUM].micro_buffer;
        if(output_reliable_buffer->iterator - output_reliable_buffer->init > session->header_offset)
        {
            send_reliable_message(session, output_reliable_stream);
        }
    }

    uint64_t current_ms = get_nano_time() / 1000000;
    if(output_reliable_stream->last_acknown != output_reliable_stream->last_sent)
    {
        if(output_reliable_stream->last_heartbeat_timestamp + MICRORTPS_HEARTBEAT_MIN_PERIOD_MS < current_ms)
        {
            send_heartbeat(session, output_reliable_stream);
            output_reliable_stream->last_heartbeat_timestamp = current_ms;
        }
    }

    /* Receive phase */
    uint8_t buffer[MICRORTPS_MTU_SIZE];
    uint32_t length = 0;
    while (0 < (length = receive_data_timed(buffer, MICRORTPS_MTU_SIZE, session->transport_id, MICRORTPS_TIMEOUT_MS)))
    {
        MicroBuffer input_buffer;
        init_micro_buffer(&input_buffer, buffer, length);

        if(input_buffer.final - input_buffer.init > session->header_offset)
        {
            PRINTL_SERIALIZATION(RECV, buffer, length);
            process_message(session, &input_buffer);
        }
    }
}

bool run_until_status(Session* session, uint16_t status_request_id, uint32_t attempts)
{
    uint32_t attempts_counter = 0;
    session->last_status_received = false;
    while (!session->last_status_received && attempts_counter < attempts)
    {
        run_communication(session);
        attempts_counter++;
        if(session->last_status_received)
        {
            session->last_status_received = session->last_status_request_id == status_request_id;
        }
    }

    return session->last_status_received && session->last_status.status == STATUS_OK;
}

bool send_until_status(Session* session, uint16_t status_request_id, uint32_t attempts, MicroBuffer* message)
{
    uint32_t attempts_counter = 0;
    bool result = false;
    while(!result && attempts_counter < attempts)
    {
        send_data(message->init, (message->iterator - message->init), session->transport_id);
        PRINTL_SERIALIZATION(SEND, message->init, (uint32_t)(message->iterator - message->init));
        result = run_until_status(session, status_request_id, 1);
        attempts_counter++;
    }

    return result;
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
    {
        return (RequestId){{(uint8_t)(request_id >> 8), (uint8_t)request_id}};
    }
    else
    {
        return (RequestId){{(uint8_t)request_id, (uint8_t)(request_id >> 8)}};
    }
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
    {
        return (ObjectId){{(uint8_t)(object_id >> 8), (uint8_t)object_id}};
    }
    else
    {
        return (ObjectId){{(uint8_t)object_id, (uint8_t)(object_id >> 8)}};
    }
}

uint64_t get_nano_time()
{
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

    return (current_time - epoch_time) * 100;
#else
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (ts.tv_sec * 1000000000) + ts.tv_nsec;
#endif
}

uint16_t seq_num_add(uint16_t seq_num, uint16_t increment)
{
    return (seq_num + increment) % (1 << 16);
}

uint16_t seq_num_sub(uint16_t seq_num, uint16_t decrement)
{
    uint16_t result;
    if(decrement > seq_num)
    {
        result = (seq_num + ((1 << 16) - decrement)) % (1 << 16);
    }
    else
    {
        result = seq_num - decrement;
    }
    return result;
}

int seq_num_cmp(uint16_t seq_num_1, uint16_t seq_num_2)
{
    int result;
    if(seq_num_1 == seq_num_2)
    {
        result = 0;
    }
    else if(seq_num_1 < seq_num_2 || seq_num_1 > (1 << 15))
    {
        result = -1;
    }
    else
    {
        result = 1;
    }
    return result;
}
