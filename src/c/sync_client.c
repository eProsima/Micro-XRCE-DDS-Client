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

        /* Init micro buffers. */
        init_micro_buffer(&session->in_builtin_streams.none_stream.micro_buffer,
                          session->in_builtin_streams.none_stream.buf,
                          sizeof(session->in_builtin_streams.none_stream.buf));
        init_micro_buffer(&session->out_builtin_streams.none_stream.micro_buffer,
                          session->out_builtin_streams.none_stream.buf,
                          sizeof(session->out_builtin_streams.best_effort_stream.buf));
        init_micro_buffer(&session->in_builtin_streams.best_effort_stream.micro_buffer,
                          session->in_builtin_streams.best_effort_stream.buf,
                          sizeof(session->in_builtin_streams.none_stream.buf));
        init_micro_buffer(&session->out_builtin_streams.best_effort_stream.micro_buffer,
                          session->out_builtin_streams.best_effort_stream.buf,
                          sizeof(session->out_builtin_streams.best_effort_stream.buf));
        size_t num_msgs = sizeof(session->out_builtin_streams.reliable_stream.micro_buffers) /
                                 sizeof(session->out_builtin_streams.reliable_stream.micro_buffers[0]);
        for (int i = 0; i < num_msgs; i++)
        {
            init_micro_buffer(&session->in_builtin_streams.reliable_stream.micro_buffers[i],
                              session->in_builtin_streams.reliable_stream.buf[i],
                              sizeof(session->in_builtin_streams.reliable_stream.buf[i]));
            init_micro_buffer(&session->out_builtin_streams.reliable_stream.micro_buffers[i],
                              session->out_builtin_streams.reliable_stream.buf[i],
                              sizeof(session->out_builtin_streams.reliable_stream.buf[i]));
        }

        /* Set micro buffers endiannes. */
        /* TODO (Julian): endiannes should be selected automatically. */
        session->out_builtin_streams.none_stream.micro_buffer.endianness = LITTLE_ENDIANNESS;
        session->out_builtin_streams.best_effort_stream.micro_buffer.endianness = LITTLE_ENDIANNESS;
        for (int i = 0; i < num_msgs; i++)
        {
            session->out_builtin_streams.reliable_stream.micro_buffers[i].endianness = LITTLE_ENDIANNESS;
        }

        /* Init request id. */
        session->request_id = 0;

        /* Init builtin streams sequence numbers. */
        session->in_builtin_streams.best_effort_stream.seq_num = 0;
        session->in_builtin_streams.reliable_stream.seq_num = 0;
        session->out_builtin_streams.best_effort_stream.seq_num = 0;
        session->out_builtin_streams.reliable_stream.seq_num = 0;
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

    MicroBuffer* output_buffer = &session->out_builtin_streams.none_stream.micro_buffer;

    bool serialization_result = true;

    /* Serialize MessageHeader. */
    reset_micro_buffer(output_buffer);
    MessageHeader header;
    header.session_id = session->id;
    header.stream_id = 0x00;
    header.sequence_nr = 0;
    serialization_result &= serialize_MessageHeader(output_buffer, &header);
    if (127 < session->id)
    {
        serialization_result &= serialize_ClientKey(output_buffer, &session->key);
    }

    /* Serialize CREATE_CLIENT_Payload. */
    MicroState submessage_begin = get_micro_state(output_buffer);
    output_buffer->iterator += 4;
    serialization_result &= serialize_CREATE_CLIENT_Payload(output_buffer, &payload);
    MicroState submessage_end = get_micro_state(output_buffer);

    /* Serialize SubmessageHeader. */
    restore_micro_state(output_buffer, submessage_begin);
    SubmessageHeader sub_header;
    sub_header.id = SUBMESSAGE_ID_CREATE_CLIENT;
    /* TODO (Julian): introduce flags. */
    sub_header.flags = 0x07;
    sub_header.length = (submessage_end.position - submessage_begin.position) - 4;
    serialize_SubmessageHeader(output_buffer, &sub_header);
    restore_micro_state(output_buffer, submessage_end);

    if (serialization_result)
    {
        /* Send message and wait for status. */
        PRINTL_CREATE_CLIENT_SUBMESSAGE(&payload);
        int attempt_counter = 0;
        MicroBuffer* input_buffer = &session->in_builtin_streams.none_stream.micro_buffer;
        send_data(output_buffer->init, (output_buffer->iterator - output_buffer->init), session->transport_id);
        while ((attempt_counter < MICRORTPS_MAX_ATTEMPTS) && (result.implementation_status != MICRORTPS_STATUS_OK))
        {
            attempt_counter++;
                /* TODO (Julian): add timeout functionality. */
            reset_micro_buffer(input_buffer);
            int len = receive_data(input_buffer->init, (input_buffer->final - input_buffer->init), session->transport_id);
            if (len > 0)
            {
                MessageHeader header;
                deserialize_MessageHeader(input_buffer, &header);
                if (127 < header.session_id)
                {
                    ClientKey key;
                    deserialize_ClientKey(input_buffer, &key);
                }
                SubmessageHeader sub_header;
                deserialize_SubmessageHeader(input_buffer, &sub_header);
                input_buffer->endianness = (sub_header.flags & 0x01) ? LITTLE_ENDIANNESS : BIG_ENDIANNESS;
                if (sub_header.id == SUBMESSAGE_ID_STATUS)
                {
                    STATUS_Payload status_payload;
                    deserialize_STATUS_Payload(input_buffer, &status_payload);
                    PRINTL_STATUS_SUBMESSAGE(&status_payload);
                    PRINTL_SERIALIZATION(RECV, output_buffer->init, len);
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
        payload.object_representation._.participant.base.representation._.object_reference.data = ref;
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
        payload.object_representation._.topic.base.representation._.xml_string_represenatation.data = xml;
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
        payload.object_representation._.publisher.base.representation._.string_represenatation.data = xml;
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
        payload.object_representation._.subscriber.base.representation._.string_represenatation.data = xml;
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
        payload.object_representation._.data_writer.base.representation._.string_represenatation.data = xml;
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
        payload.object_representation._.data_reader.base.representation._.string_represenatation.data = xml;
        payload.object_representation._.data_reader.base.representation._.string_represenatation.size = strlen(xml);
        payload.object_representation._.data_reader.subscriber_id = subscriber_id;

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

    MicroBuffer* output_buffer = &session->out_builtin_streams.reliable_stream.micro_buffers[0];
    reset_micro_buffer(output_buffer);

    /* Serialize Header. */
    bool serialize_result = true;
    MessageHeader header;
    header.session_id = session->id;
    header.stream_id = 0x00;
    header.sequence_nr = 0;
    serialize_result &= serialize_MessageHeader(output_buffer, &header);
    if (127 < &session->key)
    {
        serialize_result = serialize_ClientKey(output_buffer, &session->key);
    }

    /* Serialize CREATE_Payload. */
    MicroState submessage_begin = get_micro_state(output_buffer);
    output_buffer->iterator += 4;
    serialize_result = serialize_CREATE_Payload(output_buffer, payload);
    MicroState submessage_end = get_micro_state(output_buffer);

    /* Serialize SubmessageHeader. */
    restore_micro_state(output_buffer, submessage_begin);
    SubmessageHeader sub_header;
    sub_header.id = SUBMESSAGE_ID_CREATE;
    sub_header.flags = 0;
    if (creation_mode.reuse) sub_header.flags |= FLAG_REUSE;
    if (creation_mode.replace) sub_header.flags |= FLAG_REPLACE;
    sub_header.flags |= output_buffer->endianness;
    sub_header.length = (submessage_end.position - submessage_begin.position) - 4;
    serialize_SubmessageHeader(output_buffer, &sub_header);
    restore_micro_state(output_buffer, submessage_end);

    if (serialize_result)
    {
        PRINTL_CREATE_RESOURCE_SUBMESSAGE(payload);
        int attempts_counter = 0;
        MicroBuffer* input_buffer = &session->in_builtin_streams.none_stream.micro_buffer;
        send_data(output_buffer->init, (output_buffer->iterator - output_buffer->init), session->transport_id);
        while ((attempts_counter < MICRORTPS_MAX_ATTEMPTS) && (result.implementation_status != MICRORTPS_STATUS_OK))
        {
            attempts_counter++;
            /* TODO (Julian): add timeout functionality. */
            reset_micro_buffer(input_buffer);
            int len = receive_data(input_buffer->init, (input_buffer->final - input_buffer->init), session->transport_id);
            if (len > 0)
            {
                MessageHeader header;
                deserialize_MessageHeader(input_buffer, &header);
                if (127 < header.session_id)
                {
                    ClientKey key;
                    deserialize_ClientKey(input_buffer, &key);
                }
                SubmessageHeader sub_header;
                deserialize_SubmessageHeader(input_buffer, &sub_header);
                input_buffer->endianness = (sub_header.flags & 0x01) ? LITTLE_ENDIANNESS : BIG_ENDIANNESS;
                if (sub_header.id == SUBMESSAGE_ID_STATUS)
                {
                    STATUS_Payload status_payload;
                    deserialize_STATUS_Payload(input_buffer, &status_payload);
                    PRINTL_STATUS_SUBMESSAGE(&status_payload);
                    PRINTL_SERIALIZATION(RECV, output_buffer->init, len);
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



