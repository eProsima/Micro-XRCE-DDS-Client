#include <micrortps/client/sync_client.h>
#include <client_private.h>
#include <log/message.h>

#include <string.h>

ResultStatus create_participant_by_ref(ClientState* state,
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
        state->next_request_id++;
        payload.base.request_id = get_raw_request_id(state->next_request_id);
        payload.base.object_id = object_id;
        payload.object_representation.kind = OBJK_PARTICIPANT;
        payload.object_representation._.participant.base.representation.format = REPRESENTATION_BY_REFERENCE;
        size_t len = (strlen(ref) > REFERENCE_MAX_LEN) ? REFERENCE_MAX_LEN : strlen(ref);
        payload.object_representation._.participant.base.representation._.object_reference.data = ref;
        payload.object_representation._.participant.base.representation._.object_reference.size = len;

        result = create_object_sync(state, &payload, 100, reuse, replace);
    }

    return result;
}

ResultStatus create_topic_by_xml(ClientState* state,
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
        state->next_request_id++;
        payload.base.request_id = get_raw_request_id(state->next_request_id);
        payload.base.object_id = object_id;
        payload.object_representation.kind = OBJK_TOPIC;
        payload.object_representation._.topic.base.representation.format = REPRESENTATION_AS_XML_STRING;
        payload.object_representation._.topic.base.representation._.xml_string_represenatation.data = xml;
        payload.object_representation._.topic.base.representation._.xml_string_represenatation.size = strlen(xml);

        result = create_object_sync(state, &payload, MICRORTPS_OBJK_TIMEOUT, reuse, replace);
    }

    return result;
}

ResultStatus create_publisher_by_xml(ClientState* state,
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
    else if ((participant_id.data[1] && 0x0F) != OBJK_PARTICIPANT)
    {
        result.status = STATUS_ERR_UNKNOWN_REFERENCE;
    }
    else
    {
        CREATE_Payload payload;
        state->next_request_id++;
        payload.base.request_id = get_raw_request_id(state->next_request_id);
        payload.base.object_id = object_id;
        payload.object_representation.kind = OBJK_PUBLISHER;
        payload.object_representation._.publisher.base.representation.format = REPRESENTATION_AS_XML_STRING;
        payload.object_representation._.publisher.base.representation._.string_represenatation.data = xml;
        payload.object_representation._.publisher.base.representation._.string_represenatation.size = strlen(xml);

        result = create_object_sync(state, &payload, MICRORTPS_OBJK_TIMEOUT, reuse, replace);
    }

    return result;
}

ResultStatus create_subscriber_by_xml(ClientState* state,
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
    else if ((participant_id.data[1] && 0x0F) != OBJK_PARTICIPANT)
    {
        result.status = STATUS_ERR_UNKNOWN_REFERENCE;
    }
    else
    {
        CREATE_Payload payload;
        state->next_request_id++;
        payload.base.request_id = get_raw_request_id(state->next_request_id);
        payload.base.object_id = object_id;
        payload.object_representation.kind = OBJK_SUBSCRIBER;
        payload.object_representation._.subscriber.base.representation.format = REPRESENTATION_AS_XML_STRING;
        payload.object_representation._.subscriber.base.representation._.string_represenatation.data = xml;
        payload.object_representation._.subscriber.base.representation._.string_represenatation.size = strlen(xml);

        result = create_object_sync(state, &payload, MICRORTPS_OBJK_TIMEOUT, reuse, replace);
    }

    return result;
}

ResultStatus create_datawriter_by_xml(ClientState* state,
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
    else if ((publisher_id.data[1] && 0x0F) != OBJK_PUBLISHER)
    {
        result.status = STATUS_ERR_UNKNOWN_REFERENCE;
    }
    else
    {
        CREATE_Payload payload;
        state->next_request_id++;
        payload.base.request_id = get_raw_request_id(state->next_request_id);
        payload.base.object_id = object_id;
        payload.object_representation.kind = OBJK_DATAWRITER;
        payload.object_representation._.subscriber.base.representation.format = REPRESENTATION_AS_XML_STRING;
        payload.object_representation._.subscriber.base.representation._.string_represenatation.data = xml;
        payload.object_representation._.subscriber.base.representation._.string_represenatation.size = strlen(xml);

        result = create_object_sync(state, &payload, MICRORTPS_OBJK_TIMEOUT, reuse, replace);
    }

    return result;
}

ResultStatus create_datareader_by_xml(ClientState* state,
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
    else if ((subscriber_id.data[1] && 0x0F) != OBJK_SUBSCRIBER)
    {
        result.status = STATUS_ERR_UNKNOWN_REFERENCE;
    }
    else
    {
        CREATE_Payload payload;
        state->next_request_id++;
        payload.base.request_id = get_raw_request_id(state->next_request_id);
        payload.base.object_id = object_id;
        payload.object_representation.kind = OBJK_DATAREADER;
        payload.object_representation._.subscriber.base.representation.format = REPRESENTATION_AS_XML_STRING;
        payload.object_representation._.subscriber.base.representation._.string_represenatation.data = xml;
        payload.object_representation._.subscriber.base.representation._.string_represenatation.size = strlen(xml);

        result = create_object_sync(state, &payload, MICRORTPS_OBJK_TIMEOUT, reuse, replace);
    }

    return result;
}

ResultStatus create_object_sync(ClientState* state,
                                const CREATE_Payload* payload,
                                int timeout,
                                bool reuse,
                                bool replace)
{
    ResultStatus result = {STATUS_ERR_RESOURCES, MICRORTPS_ERR_MAX_TRIES};
    CreationMode creation_mode = {reuse, replace};

    reset_buffer(&state->output_message.writer);
    bool cond = false;
    cond = add_create_resource_submessage(&state->output_message, payload, creation_mode);

    if (cond)
    {
        PRINTL_CREATE_RESOURCE_SUBMESSAGE(payload);
        int try_counter = 0;
        /* TODO (Julian): remove extra message size when transport delete header at UDP. */
        uint8_t status_buf[STATUS_MSG_SIZE + 7] = {0};
        MicroBuffer micro_buffer;
        init_external_buffer(&micro_buffer, status_buf, STATUS_MSG_SIZE + 7);
        while ((try_counter < MICRORTPS_OBJK_MAX_TRIES) && (result.implementation_status != MICRORTPS_STATUS_OK))
        {
            try_counter++;
            send_to_agent(state);
            /* TODO (Julian): add timeout functionality. */
            reset_buffer(&micro_buffer);
            int len = receive_data(status_buf, STATUS_MSG_SIZE + 7, state->transport_id);
            if (len > 0)
            {
                MessageHeader header;
                deserialize_MessageHeader(&micro_buffer, &header, NULL);
                if (header.session_id < 128)
                {
                    ClientKey key;
                    deserialize_ClientKey(&micro_buffer, &key, NULL);
                }
                SubmessageHeader sub_header;
                deserialize_SubmessageHeader(&micro_buffer, &sub_header, NULL);
                micro_buffer.endianness = (sub_header.flags & 0x01) ? LITTLE_ENDIANNESS : BIG_ENDIANNESS;
                if (sub_header.id == SUBMESSAGE_ID_STATUS)
                {
                    STATUS_Payload status_payload;
                    deserialize_STATUS_Payload(&micro_buffer, &status_payload, NULL);
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



