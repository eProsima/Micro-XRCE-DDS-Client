#include <micrortps/client/profile/session/create_entities_xml.h>
#include <micrortps/client/core/serialization/xrce_protocol.h>
#include <string.h>

static uint16_t create_entity_xml(mrSession* session, mrStreamId stream_id,
                                  mrObjectId object_id, const char* xml, uint8_t mode,
                                  CREATE_Payload* payload);

//==================================================================
//                              PUBLIC
//==================================================================
uint16_t mr_write_configure_participant_xml(mrSession* session, mrStreamId stream_id,
                                         mrObjectId object_id, uint16_t domain, const char* xml, uint8_t mode)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_PARTICIPANT;
    payload.object_representation._.participant.domain_id = domain;

    return create_entity_xml(session, stream_id, object_id, xml, mode, &payload);
}

uint16_t mr_write_configure_topic_xml(mrSession* session, mrStreamId stream_id,
                                   mrObjectId object_id, mrObjectId participant_id, const char* xml, uint8_t mode)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_TOPIC;
    object_id_to_raw(participant_id, payload.object_representation._.topic.participant_id.data);

    return create_entity_xml(session, stream_id, object_id, xml, mode, &payload);
}

uint16_t mr_write_configure_publisher_xml(mrSession* session, mrStreamId stream_id,
                                       mrObjectId object_id, mrObjectId participant_id, const char* xml, uint8_t mode)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_PUBLISHER;
    object_id_to_raw(participant_id, payload.object_representation._.publisher.participant_id.data);

    return create_entity_xml(session, stream_id, object_id, xml, mode, &payload);
}

uint16_t mr_write_configure_subscriber_xml(mrSession* session, mrStreamId stream_id,
                                        mrObjectId object_id, mrObjectId participant_id, const char* xml, uint8_t mode)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_SUBSCRIBER;
    object_id_to_raw(participant_id, payload.object_representation._.subscriber.participant_id.data);

    return create_entity_xml(session, stream_id, object_id, xml, mode, &payload);
}

uint16_t mr_write_configure_datawriter_xml(mrSession* session, mrStreamId stream_id,
                                        mrObjectId object_id, mrObjectId publisher_id, const char* xml, uint8_t mode)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_DATAWRITER;
    object_id_to_raw(publisher_id, payload.object_representation._.data_writer.publisher_id.data);

    return create_entity_xml(session, stream_id, object_id, xml, mode, &payload);
}

uint16_t mr_write_configure_datareader_xml(mrSession* session, mrStreamId stream_id,
                                        mrObjectId object_id, mrObjectId subscriber_id, const char* xml, uint8_t mode)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_DATAREADER;
    object_id_to_raw(subscriber_id, payload.object_representation._.data_reader.subscriber_id.data);

    return create_entity_xml(session, stream_id, object_id, xml, mode, &payload);
}

//==================================================================
//                             PRIVATE
//==================================================================

inline uint16_t create_entity_xml(mrSession* session, mrStreamId stream_id,
                                  mrObjectId object_id, const char* xml, uint8_t mode,
                                  CREATE_Payload* payload)
{
    uint16_t returned_value = MR_INVALID_REQUEST_ID;
    uint32_t xml_size = strlen(xml) + 1;
    if(xml_size <= UINT16_MAX)
    {
        // Use participant access to access to the xml base of any object variant. //Future elegant change?
        payload->object_representation._.participant.base.representation.format = REPRESENTATION_AS_XML_STRING;
        payload->object_representation._.participant.base.representation._.xml_string_represenatation.size = xml_size;
        memcpy(payload->object_representation._.participant.base.representation._.xml_string_represenatation.data, xml, xml_size);

        returned_value = common_create_entity(session, stream_id, object_id, (uint16_t)xml_size, mode, payload);
    }

    return returned_value;
}
