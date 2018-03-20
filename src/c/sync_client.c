#include <micrortps/client/sync_client.h>
#include <client_private.h>
#include <log/message.h>

#include <string.h>
#include <time.h>

uint8_t new_udp_session_sync(SyncSession* session,
                             SessionId id,
                             ClientKey key,
                             uint16_t send_port,
                             uint16_t recv_port,
                             uint16_t remote_port,
                             const char* server_ip)
{
    uint8_t result = MICRORTPS_STATUS_OK;

    locator_id_t locator_id = add_udp_locator(send_port, recv_port, remote_port, server_ip);

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
        session->header_offset = (0x80 <= id) ? HEADER_MAX_SIZE : HEADER_MIN_SIZE;

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

    }

    return result;
}

ResultStatus init_session_syn(SyncSession* session)
{
    ResultStatus result = {STATUS_ERR_RESOURCES, MICRORTPS_ERR_MAX_ATTEMPTS};

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
    payload.client_representation.client_key = (ClientKey){{0xAA, 0xBB, 0xCC, 0xDD}};
    payload.client_representation.session_id = 0xEE;
    payload.client_representation.optional_properties = false;
    payload.client_representation.properties.size = 0;

    uint8_t buffer[MICRORTPS_MIN_BUFFER_SIZE];
    MicroBuffer output_buffer;
    init_micro_buffer_endian(&output_buffer, buffer, sizeof(buffer), MACHINE_ENDIANNESS);

    bool serialization_result = true;

    /* Serialize MessageHeader. */
    MessageHeader header;
    header.session_id = session->id;
    header.stream_id = 0x00;
    header.sequence_nr = 0;
    serialization_result &= serialize_MessageHeader(&output_buffer, &header);
    if (127 < session->id)
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

    if (serialization_result)
    {
        /* Send message and wait for status. */
        PRINTL_CREATE_CLIENT_SUBMESSAGE(&payload);
        PRINTL_SERIALIZATION(SEND, output_buffer.init, output_buffer.iterator - output_buffer.init);
        int attempt_counter = 0;

        MicroBuffer input_buffer;
        init_micro_buffer(&input_buffer, buffer, sizeof(buffer));

        send_data(output_buffer.init, (output_buffer.iterator - output_buffer.init), session->transport_id);
        while ((attempt_counter < MICRORTPS_MAX_ATTEMPTS) && (result.implementation_status != MICRORTPS_STATUS_OK))
        {
            attempt_counter++;
                /* TODO (Julian): add timeout functionality. */
            reset_micro_buffer(&input_buffer);
            int len = receive_data(input_buffer.init, (input_buffer.final - input_buffer.init), session->transport_id);
            if (len > 0)
            {
                MessageHeader header;
                deserialize_MessageHeader(&input_buffer, &header);
                if (127 < header.session_id)
                {
                    ClientKey key;
                    deserialize_ClientKey(&input_buffer, &key);
                }
                SubmessageHeader sub_header;
                deserialize_SubmessageHeader(&input_buffer, &sub_header);
                input_buffer.endianness = (sub_header.flags & 0x01) ? LITTLE_ENDIANNESS : BIG_ENDIANNESS;
                if (sub_header.id == SUBMESSAGE_ID_STATUS)
                {
                    STATUS_Payload status_payload;
                    deserialize_STATUS_Payload(&input_buffer, &status_payload);
                    PRINTL_STATUS_SUBMESSAGE(&status_payload);
                    PRINTL_SERIALIZATION(RECV, input_buffer.init, len);
                    if (memcmp(&status_payload.base.related_request.request_id,
                               &payload.base.request_id,
                               sizeof(RequestId)) == 0)
                    {
                        result.status = status_payload.base.result.status;
                        result.implementation_status = MICRORTPS_STATUS_OK;
                    }
                }
            }
        }
    }
    else
    {
        result.implementation_status = MICRORTPS_ERR_SERIALIZATION;
    }

    return result;
}

ResultStatus create_participant_by_ref(SyncSession* session,
                                       const ObjectId object_id,
                                       const char* ref,
                                       bool reuse,
                                       bool replace)
{
    ResultStatus result = {STATUS_OK, MICRORTPS_STATUS_OK};

    /* Check participant id. */
    if ((object_id.data[1] & 0x0F) != OBJK_PARTICIPANT)
    {
        result.status = STATUS_ERR_UNKNOWN_REFERENCE;
    }
    else
    {
        CREATE_Payload payload;
        session->request_id++;
        payload.base.request_id = get_raw_request_id(session->request_id);
        payload.base.object_id = object_id;
        payload.object_representation.kind = OBJK_PARTICIPANT;
        payload.object_representation._.participant.base.representation.format = REPRESENTATION_BY_REFERENCE;
        size_t len = (strlen(ref) > REFERENCE_MAX_LEN) ? REFERENCE_MAX_LEN : strlen(ref);
        payload.object_representation._.participant.base.representation._.object_reference.data = (char*)ref; //MISRA non-compliant?
        payload.object_representation._.participant.base.representation._.object_reference.size = len;

        result = create_object_sync(session, &payload, 100, reuse, replace);
    }

    return result;
}

ResultStatus create_topic_by_xml(SyncSession* session,
                                 const ObjectId object_id,
                                 const char* xml,
                                 const ObjectId participant_id,
                                 bool reuse,
                                 bool replace)
{
    ResultStatus result = {STATUS_OK, MICRORTPS_STATUS_OK};

    /* Check participant and topic ids. */
    if ((object_id.data[1] & 0x0F) != OBJK_TOPIC)
    {
        result.status = STATUS_ERR_UNKNOWN_REFERENCE;
    }
    else if ((participant_id.data[1] & 0x01) != OBJK_PARTICIPANT)
    {
        result.status = STATUS_ERR_UNKNOWN_REFERENCE;
    }
    else
    {
        CREATE_Payload payload;
        session->request_id++;
        payload.base.request_id = get_raw_request_id(session->request_id);
        payload.base.object_id = object_id;
        payload.object_representation.kind = OBJK_TOPIC;
        payload.object_representation._.topic.base.representation.format = REPRESENTATION_AS_XML_STRING;
        payload.object_representation._.topic.base.representation._.xml_string_represenatation.data = (char*)xml;
        payload.object_representation._.topic.base.representation._.xml_string_represenatation.size = strlen(xml);
        payload.object_representation._.topic.participant_id = participant_id;

        result = create_object_sync(session, &payload, MICRORTPS_OBJK_TIMEOUT, reuse, replace);
    }

    return result;
}

ResultStatus create_publisher_by_xml(SyncSession* session,
                                     const ObjectId object_id,
                                     const char* xml,
                                     const ObjectId participant_id,
                                     bool reuse,
                                     bool replace)
{
    ResultStatus result = {STATUS_OK, MICRORTPS_STATUS_OK};

    /* Check participant and publisher ids. */
    if ((object_id.data[1] & 0x0F) != OBJK_PUBLISHER)
    {
        result.status = STATUS_ERR_UNKNOWN_REFERENCE;
    }
    else if ((participant_id.data[1] & 0x0F) != OBJK_PARTICIPANT)
    {
        result.status = STATUS_ERR_UNKNOWN_REFERENCE;
    }
    else
    {
        CREATE_Payload payload;
        session->request_id++;
        payload.base.request_id = get_raw_request_id(session->request_id);
        payload.base.object_id = object_id;
        payload.object_representation.kind = OBJK_PUBLISHER;
        payload.object_representation._.publisher.base.representation.format = REPRESENTATION_AS_XML_STRING;
        payload.object_representation._.publisher.base.representation._.string_represenatation.data = (char*)xml;
        payload.object_representation._.publisher.base.representation._.string_represenatation.size = strlen(xml);
        payload.object_representation._.publisher.participant_id = participant_id;

        result = create_object_sync(session, &payload, MICRORTPS_OBJK_TIMEOUT, reuse, replace);
    }

    return result;
}

ResultStatus create_subscriber_by_xml(SyncSession* session,
                                      const ObjectId object_id,
                                      const char* xml,
                                      const ObjectId participant_id,
                                      bool reuse,
                                      bool replace)
{
    ResultStatus result = {STATUS_OK, MICRORTPS_STATUS_OK};

    /* Check participant and publisher ids. */
    if ((object_id.data[1] & 0x0F) != OBJK_SUBSCRIBER)
    {
        result.status = STATUS_ERR_UNKNOWN_REFERENCE;
    }
    else if ((participant_id.data[1] & 0x0F) != OBJK_PARTICIPANT)
    {
        result.status = STATUS_ERR_UNKNOWN_REFERENCE;
    }
    else
    {
        CREATE_Payload payload;
        session->request_id++;
        payload.base.request_id = get_raw_request_id(session->request_id);
        payload.base.object_id = object_id;
        payload.object_representation.kind = OBJK_SUBSCRIBER;
        payload.object_representation._.subscriber.base.representation.format = REPRESENTATION_AS_XML_STRING;
        payload.object_representation._.subscriber.base.representation._.string_represenatation.data = (char*)xml;
        payload.object_representation._.subscriber.base.representation._.string_represenatation.size = strlen(xml);
        payload.object_representation._.subscriber.participant_id = participant_id;

        result = create_object_sync(session, &payload, MICRORTPS_OBJK_TIMEOUT, reuse, replace);
    }

    return result;
}

ResultStatus create_datawriter_by_xml(SyncSession* session,
                                      const ObjectId object_id,
                                      const char* xml,
                                      const ObjectId publisher_id,
                                      bool reuse,
                                      bool replace)
{
    ResultStatus result = {STATUS_OK, MICRORTPS_STATUS_OK};

    /* Check publisher and datawriter ids. */
    if ((object_id.data[1] & 0x0F) != OBJK_DATAWRITER)
    {
        result.status = STATUS_ERR_UNKNOWN_REFERENCE;
    }
    else if ((publisher_id.data[1] & 0x0F) != OBJK_PUBLISHER)
    {
        result.status = STATUS_ERR_UNKNOWN_REFERENCE;
    }
    else
    {
        CREATE_Payload payload;
        session->request_id++;
        payload.base.request_id = get_raw_request_id(session->request_id);
        payload.base.object_id = object_id;
        payload.object_representation.kind = OBJK_DATAWRITER;
        payload.object_representation._.data_writer.base.representation.format = REPRESENTATION_AS_XML_STRING;
        payload.object_representation._.data_writer.base.representation._.string_represenatation.data = (char*)xml;
        payload.object_representation._.data_writer.base.representation._.string_represenatation.size = strlen(xml);
        payload.object_representation._.data_writer.publisher_id = publisher_id;

        result = create_object_sync(session, &payload, MICRORTPS_OBJK_TIMEOUT, reuse, replace);
    }

    return result;
}

ResultStatus create_datareader_by_xml(SyncSession* session,
                                      const ObjectId object_id,
                                      const char* xml,
                                      const ObjectId subscriber_id,
                                      bool reuse,
                                      bool replace)
{
    ResultStatus result = {STATUS_OK, MICRORTPS_STATUS_OK};

    /* Check participant and publisher ids. */
    if ((object_id.data[1] & 0x0F) != OBJK_DATAREADER)
    {
        result.status = STATUS_ERR_UNKNOWN_REFERENCE;
    }
    else if ((subscriber_id.data[1] & 0x0F) != OBJK_SUBSCRIBER)
    {
        result.status = STATUS_ERR_UNKNOWN_REFERENCE;
    }
    else
    {
        CREATE_Payload payload;
        session->request_id++;
        payload.base.request_id = get_raw_request_id(session->request_id);
        payload.base.object_id = object_id;
        payload.object_representation.kind = OBJK_DATAREADER;
        payload.object_representation._.data_reader.base.representation.format = REPRESENTATION_AS_XML_STRING;
        payload.object_representation._.data_reader.base.representation._.string_represenatation.data = (char*)xml;
        payload.object_representation._.data_reader.base.representation._.string_represenatation.size = strlen(xml);
        payload.object_representation._.data_reader.subscriber_id = subscriber_id;

        //while(run_communication(session))
        //result = session->last_status;

        result = create_object_sync(session, &payload, MICRORTPS_OBJK_TIMEOUT, reuse, replace);
    }

    return result;
}

ResultStatus create_object_sync(SyncSession* session,
                                const CREATE_Payload* payload,
                                int timeout,
                                bool reuse,
                                bool replace)
{
    ResultStatus result = {STATUS_ERR_RESOURCES, MICRORTPS_ERR_MAX_ATTEMPTS};
    CreationMode creation_mode = {reuse, replace};

    uint8_t buffer[MICRORTPS_MTU_SIZE];
    MicroBuffer output_buffer;
    init_micro_buffer(&output_buffer, buffer, sizeof(buffer));

    /* Serialize Header. */
    bool serialize_result = true;
    MessageHeader header;
    header.session_id = session->id;
    header.stream_id = 0x00;
    header.sequence_nr = 0;
    serialize_result &= serialize_MessageHeader(&output_buffer, &header);
    if (127 < session->id)
    {
        serialize_result = serialize_ClientKey(&output_buffer, &session->key);
    }

    /* Serialize CREATE_Payload. */
    MicroState submessage_begin = get_micro_state(&output_buffer);
    output_buffer.iterator += 4;
    serialize_result = serialize_CREATE_Payload(&output_buffer, payload);
    MicroState submessage_end = get_micro_state(&output_buffer);

    /* Serialize SubmessageHeader. */
    restore_micro_state(&output_buffer, submessage_begin);
    SubmessageHeader sub_header;
    sub_header.id = SUBMESSAGE_ID_CREATE;
    sub_header.flags = 0;
    if (creation_mode.reuse) sub_header.flags |= FLAG_REUSE;
    if (creation_mode.replace) sub_header.flags |= FLAG_REPLACE;
    sub_header.flags |= output_buffer.endianness;
    sub_header.length = (submessage_end.position - submessage_begin.position) - 4;
    serialize_SubmessageHeader(&output_buffer, &sub_header);
    restore_micro_state(&output_buffer, submessage_end);

    if (serialize_result)
    {
        PRINTL_CREATE_RESOURCE_SUBMESSAGE(payload);
        PRINTL_SERIALIZATION(SEND, output_buffer.init, output_buffer.iterator - output_buffer.init);
        int attempts_counter = 0;
        send_data(output_buffer.init, (output_buffer.iterator - output_buffer.init), session->transport_id);

        while ((attempts_counter < MICRORTPS_MAX_ATTEMPTS) && (result.implementation_status != MICRORTPS_STATUS_OK))
        {
            attempts_counter++;
            /* TODO (Julian): add timeout functionality. */
            MicroBuffer input_buffer;
            init_micro_buffer(&input_buffer, buffer, sizeof(buffer));
            int len = receive_data(input_buffer.init, (input_buffer.final - input_buffer.init), session->transport_id);
            if (len > 0)
            {
                MessageHeader header;
                deserialize_MessageHeader(&input_buffer, &header);
                if (0x80 <= header.session_id)
                {
                    ClientKey key;
                    deserialize_ClientKey(&input_buffer, &key);
                }
                SubmessageHeader sub_header;
                deserialize_SubmessageHeader(&input_buffer, &sub_header);
                input_buffer.endianness = (sub_header.flags & 0x01) ? LITTLE_ENDIANNESS : BIG_ENDIANNESS;
                if (sub_header.id == SUBMESSAGE_ID_STATUS)
                {
                    STATUS_Payload status_payload;
                    deserialize_STATUS_Payload(&input_buffer, &status_payload);
                    PRINTL_STATUS_SUBMESSAGE(&status_payload);
                    PRINTL_SERIALIZATION(RECV, input_buffer.init, len);
                    if (memcmp(&status_payload.base.related_request.request_id,
                               &payload->base.request_id,
                               sizeof(RequestId)) == 0)
                    {
                        result.status = status_payload.base.result.status;
                        result.implementation_status = MICRORTPS_STATUS_OK;
                    }
                }
            }
        }
    }
    else
    {
        result.status = STATUS_ERR_RESOURCES;
        result.implementation_status = MICRORTPS_ERR_SERIALIZATION;
    }

    return result;
}


MicroBuffer* prepare_best_effort_stream_for_topic(BestEffortStream* output_stream, uint16_t topic_size)
{
    MicroBuffer* output_buffer = &output_stream->micro_buffer;
    if(SUBMESSAGE_SIZE + PAYLOAD_DATA_SIZE + topic_size > output_buffer->final - output_buffer->iterator)
    {
        return NULL;
    }

    SubmessageHeader sub_header = (SubmessageHeader){ SUBMESSAGE_ID_DATA, 0, PAYLOAD_DATA_SIZE + topic_size };
    serialize_SubmessageHeader(output_buffer, &sub_header);

    serialize_uint32_t(output_buffer, topic_size); //Remove in next version. Emulates Data payload data.

    return output_buffer;
}

MicroBuffer* prepare_reliable_stream_for_topic(ReliableStream* output_stream, uint16_t topic_size)
{
    MicroBuffer* output_buffer = &output_stream->store[output_stream->seq_num].micro_buffer;
    if(SUBMESSAGE_SIZE + PAYLOAD_DATA_SIZE + topic_size > output_buffer->final - output_buffer->iterator)
    {
        return NULL;
    }

    SubmessageHeader sub_header = (SubmessageHeader){ SUBMESSAGE_ID_DATA, 0, PAYLOAD_DATA_SIZE + topic_size };
    serialize_SubmessageHeader(output_buffer, &sub_header);

    serialize_uint32_t(output_buffer, topic_size); //Remove in next version. Emulates Data payload data.

    return output_buffer;
}

void stamp_header(SyncSession* session, MicroBuffer* output_buffer, StreamId id, uint16_t seq_num)
{
    MicroBuffer header_buffer;
    init_micro_buffer(&header_buffer, output_buffer->init, HEADER_MAX_SIZE);

    MessageHeader header = (MessageHeader){session->id, id, seq_num};
    (void) serialize_MessageHeader(&header_buffer, &header);
    if (0x80 <= session->id)
    {
        (void) serialize_ClientKey(&header_buffer, &session->key);
    }
}

bool send_best_effort_message(SyncSession* session, BestEffortStream* output_stream)
{
    MicroBuffer* output_buffer = &output_stream->micro_buffer;

    stamp_header(session, output_buffer, STREAMID_BUILTIN_BEST_EFFORTS, output_stream->seq_num);

    int32_t bytes = send_data(output_buffer->init, (output_buffer->iterator - output_buffer->init), session->transport_id);
    output_stream->seq_num++;

    reset_micro_buffer(output_buffer);
    output_buffer->iterator += session->header_offset;

    return bytes > 0;
}

bool send_reliable_message(SyncSession* session, ReliableStream* output_stream)
{
    MicroBuffer* output_buffer = &output_stream->store[output_stream->seq_num].micro_buffer;

    stamp_header(session, output_buffer, STREAMID_BUILTIN_RELIABLE, output_stream->seq_num);

    int32_t bytes = send_data(output_buffer->init, (output_buffer->iterator - output_buffer->init), session->transport_id);
    output_stream->seq_num++;

    return bytes > 0;
}

bool send_heartbeat(SyncSession* session, ReliableStream* reference_stream)
{
    uint8_t buffer[MICRORTPS_MIN_BUFFER_SIZE];
    MicroBuffer output_buffer;
    init_micro_buffer_endian(&output_buffer, buffer, sizeof(buffer), MACHINE_ENDIANNESS);

    stamp_header(session, &output_buffer, STREAMID_BUILTIN_RELIABLE, 0);

    SubmessageHeader sub_header = (SubmessageHeader){ SUBMESSAGE_ID_DATA, 0, HEARTBEAT_MSG_SIZE };
    serialize_SubmessageHeader(&output_buffer, &sub_header);

    HEARTBEAT_Payload heartbeat;
    heartbeat.first_unacked_seq_nr = reference_stream->seq_num - 1;
    heartbeat.last_unacked_seq_nr = reference_stream->seq_num - 1;

    deserialize_HEARTBEAT_Payload(&output_buffer, &heartbeat);

    int32_t bytes = send_data(output_buffer.init, (output_buffer.iterator - output_buffer.init), session->transport_id);
    return bytes > 0;
}

bool receive_best_effort(BestEffortStream* input_stream, const uint16_t seq_num)
{
    if(seq_num < input_stream->seq_num)
    {
        return false;
    }

    input_stream->seq_num = seq_num + 1;

    return true;
}

bool receive_reliable(ReliableStream* input_stream, MicroBuffer* submessages, const uint16_t seq_num)
{
    uint8_t index = seq_num % MICRORTPS_MAX_MSG_NUM;
    MicroBuffer* input_buffer = &input_stream->store[index].micro_buffer;

    if(input_buffer->iterator != input_buffer->init
        || seq_num < input_stream->seq_num
        || seq_num > input_stream->seq_num + MICRORTPS_MAX_MSG_NUM)
    {
        return false;
    }

    if(input_stream->seq_num != seq_num)
    {
        serialize_array_uint8_t(input_buffer, submessages->iterator, submessages->final - submessages->iterator);
    }
    else
    {
        for(int i = 0; i < MICRORTPS_MAX_MSG_NUM && seq_num == input_stream->seq_num; i++)
        {
            uint8_t aux_index = (seq_num + i) % MICRORTPS_MAX_MSG_NUM;
            MicroBuffer* aux_buffer = &input_stream->store[aux_index].micro_buffer;
            if(aux_buffer->iterator == aux_buffer->init)
            {
                input_stream->seq_num = seq_num + i;
            }
        }
    }

    return true;
}

void check_acknack(SyncSession* session, ReliableStream* output_stream, const uint16_t first_unacked_seq_num, uint8_t bitmap[2])
{
    for(int i = 0; i < MICRORTPS_MAX_MSG_NUM; i++)
    {
        uint8_t index = first_unacked_seq_num % MICRORTPS_MAX_MSG_NUM;
        MicroBuffer* output_buffer = &output_stream->store[index].micro_buffer;

        if(output_buffer->iterator != output_buffer->init)
        {
            bool lost = (8 > i) ? (bitmap[1] << i) : (bitmap[0] << (i - 8));
            if(lost)
            {
                send_data(output_buffer->init, (output_buffer->iterator - output_buffer->init), session->transport_id);
            }
            else
            {
                reset_micro_buffer(output_buffer);
                output_buffer->iterator += session->header_offset;
            }
        }
    }
}

void process_message(SyncSession* session, MicroBuffer* input_buffer)
{
    MessageHeader header;
    deserialize_MessageHeader(input_buffer, &header);
    if (0x80 <= header.session_id)
    {
        ClientKey key;
        deserialize_ClientKey(input_buffer, &key);
    }

    if(0 == header.session_id)
    {
        process_submessages(session, input_buffer);
    }
    else if(STREAMID_BUILTIN_BEST_EFFORTS == header.session_id)
    {
        BestEffortStream* input_stream = &session->input_best_effort_stream;
        bool ready_to_process = receive_best_effort(input_stream, header.sequence_nr);
        if(ready_to_process)
        {
            process_submessages(session, input_buffer);
        }
    }
    else if(STREAMID_BUILTIN_RELIABLE == header.session_id)
    {
        ReliableStream* input_stream = &session->input_reliable_stream;
        bool ready_to_read = receive_reliable(input_stream, input_buffer, header.sequence_nr);
        if(ready_to_read)
        {
            process_submessages(session, input_buffer);
            for(uint16_t i = header.sequence_nr + 1; i < input_stream->seq_num; i++)
            {
                uint8_t current_index = i % MICRORTPS_MAX_MSG_NUM;
                MicroBuffer* current_buffer = &input_stream->store[current_index].micro_buffer;
                process_submessages(session, current_buffer);
            }
        }
    }
}

void process_submessages(SyncSession* session,  MicroBuffer* input_buffer)
{
    while(input_buffer->final - input_buffer->iterator > SUBHEADER_SIZE)
    {
        SubmessageHeader sub_header;
        deserialize_SubmessageHeader(input_buffer, &sub_header);
        input_buffer->endianness = (sub_header.flags & 0x01) ? LITTLE_ENDIANNESS : BIG_ENDIANNESS;

        if(sub_header.length > input_buffer->final - input_buffer->iterator)
        {
            switch(sub_header.id)
            {
                case SUBMESSAGE_ID_STATUS:
                    process_status_submessage(input_buffer);
                    break;

                case SUBMESSAGE_ID_INFO:
                    process_info_submessage(input_buffer);
                    break;

                case SUBMESSAGE_ID_HEARTBEAT:
                    process_heartbeat_submessage(input_buffer);
                    break;

                case SUBMESSAGE_ID_ACKNACK:
                    process_acknack_submessage(input_buffer);
                    break;

                case SUBMESSAGE_ID_DATA:
                    process_data_submessage(input_buffer);
                    break;
            }
        }
    }
}

void process_submessage(SyncSession* session, MicroBuffer* input_buffer)
{
    switch(sub_header.id)
    {
        case SUBMESSAGE_ID_STATUS:
        {
            STATUS_Payload status;
            deserialize_STATUS_Payload(&input_buffer, &status);

            PRINTL_STATUS_SUBMESSAGE(&status);
            PRINTL_SERIALIZATION(RECV, buffer, length);

            session->last_status.status = status.base.result.status;
            session->last_status.implementation_status = status.base.result.implementation_status;

            break;
        }
        case SUBMESSAGE_ID_HEARTBEAT:
        {
            HEARTBEAT_Payload heartbeat;
            deserialize_HEARTBEAT_Payload(&input_buffer, &heartbeat);

            PRINTL_HEARTBEAT_SUBMESSAGE(&heartbeat);
            PRINTL_SERIALIZATION(RECV, buffer, length);
        }
        case SUBMESSAGE_ID_ACKNACK:
        {
            ACKNACK_Payload acknack;
            deserialize_ACKNACK_Payload(&input_buffer, &acknack);

            PRINTL_ACKNACK_SUBMESSAGE(&acknack);
            PRINTL_SERIALIZATION(RECV, buffer, length);

            ReliableStream* output_stream = &session->output_reliable_stream;
            check_acknack(session, output_stream, acknack.first_unacked_seq_num, acknack.nack_bitmap);

            break;
        }
        case SUBMESSAGE_ID_DATA:
        {
            DATA_Payload_Data payload;
            deserialize_DATA_Payload_Data(&input_buffer, &payload);

            PRINTL_DATA_DATA_SUBMESSAGE(&payload);
            PRINTL_SERIALIZATION(RECV, buffer, length);

            input_buffer.iterator = payload.data.data; //delete this when the topic had been deserialized out of data payload.

            uint16_t id = get_num_request_id(payload.base.request_id);

            if(STREAMID_BUILTIN_RELIABLE > header.stream_id)
            {
                BestEffortStream* input_stream = &session->input_best_effort_stream;
                bool ready_to_read = receive_best_effort(input_stream, &input_buffer, header.sequence_nr);
                if(ready_to_read)
                {
                    session->on_topic_callback(id, &input_buffer, session->on_topic_args);
                }
            }
            else
            {
                ReliableStream* input_stream = &session->input_reliable_stream;
                bool ready_to_read = receive_reliable(input_stream, &input_buffer, header.sequence_nr);
                if(ready_to_read)
                {
                    session->on_topic_callback(id, &input_buffer, session->on_topic_args);
                    for(uint16_t i = header.sequence_nr + 1; i < input_stream->seq_num; i++)
                    {
                        uint8_t current_index = i % MICRORTPS_MAX_MSG_NUM;
                        MicroBuffer* current_buffer = &input_stream->store[current_index].micro_buffer;
                        session->on_topic_callback(id, current_buffer, session->on_topic_args);
                    }
                }
            }

            break;
        }
    }
}

void run_communication(SyncSession* session)
{
    BestEffortStream* best_effort_stream = &session->output_best_effort_stream;
    MicroBuffer* best_effort_buffer = &best_effort_stream->micro_buffer;
    if(best_effort_buffer->iterator - best_effort_buffer->init > HEADER_MAX_SIZE)
    {
        send_best_effort_message(session, best_effort_stream);
    }

    ReliableStream* reliable_stream = &session->output_reliable_stream;
    MicroBuffer* reliable_buffer = &reliable_stream->store[reliable_stream->seq_num].micro_buffer;
    if(reliable_buffer->iterator - reliable_buffer->init > HEADER_MAX_SIZE)
    {
        send_reliable_message(session, reliable_stream);
        send_heartbeat(session, reliable_stream);
    }

    session->last_status.status = -1;
    session->last_status.implementation_status = STATUS_LAST_OP_NONE;

    uint8_t buffer[MICRORTPS_MTU_SIZE];

    /* TODO (Luis): add timeout funcionality */
    /* We suppose that the time timeout will be the minimun time to get data from the netword card. */
    uint32_t length = 0;
    while (0 < (length = receive_data(buffer, MICRORTPS_MTU_SIZE, session->transport_id)))
    {
        MicroBuffer input_buffer;
        init_micro_buffer(&input_buffer, buffer, length);

        if(input_buffer.final - input_buffer.init > session->header_offset)
        {
            process_message(session, &input_buffer);
        }
    }
}

