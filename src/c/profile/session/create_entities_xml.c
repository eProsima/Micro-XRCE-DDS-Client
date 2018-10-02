#include <microxrce/client/profile/session/create_entities_xml.h>
#include <microxrce/client/core/serialization/xrce_protocol.h>
#include <string.h>

static uint16_t create_entity_xml(uxrSession* session, uxrStreamId stream_id,
                                  uxrObjectId object_id, const char* xml, uint8_t mode,
                                  CREATE_Payload* payload);

//==================================================================
//                              PUBLIC
//==================================================================
uint16_t uxr_write_configure_participant_xml(uxrSession* session, uxrStreamId stream_id,
                                         uxrObjectId object_id, uint16_t domain, const char* xml, uint8_t mode)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_PARTICIPANT;
    payload.object_representation._.participant.domain_id = (int16_t)domain;

    return create_entity_xml(session, stream_id, object_id, xml, mode, &payload);
}

uint16_t uxr_write_configure_topic_xml(uxrSession* session, uxrStreamId stream_id,
                                   uxrObjectId object_id, uxrObjectId participant_id, const char* xml, uint8_t mode)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_TOPIC;
    object_id_to_raw(participant_id, payload.object_representation._.topic.participant_id.data);

    return create_entity_xml(session, stream_id, object_id, xml, mode, &payload);
}

uint16_t uxr_write_configure_publisher_xml(uxrSession* session, uxrStreamId stream_id,
                                       uxrObjectId object_id, uxrObjectId participant_id, const char* xml, uint8_t mode)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_PUBLISHER;
    object_id_to_raw(participant_id, payload.object_representation._.publisher.participant_id.data);

    return create_entity_xml(session, stream_id, object_id, xml, mode, &payload);
}

uint16_t uxr_write_configure_subscriber_xml(uxrSession* session, uxrStreamId stream_id,
                                        uxrObjectId object_id, uxrObjectId participant_id, const char* xml, uint8_t mode)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_SUBSCRIBER;
    object_id_to_raw(participant_id, payload.object_representation._.subscriber.participant_id.data);

    return create_entity_xml(session, stream_id, object_id, xml, mode, &payload);
}

uint16_t uxr_write_configure_datawriter_xml(uxrSession* session, uxrStreamId stream_id,
                                        uxrObjectId object_id, uxrObjectId publisher_id, const char* xml, uint8_t mode)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_DATAWRITER;
    object_id_to_raw(publisher_id, payload.object_representation._.data_writer.publisher_id.data);

    return create_entity_xml(session, stream_id, object_id, xml, mode, &payload);
}

uint16_t uxr_write_configure_datareader_xml(uxrSession* session, uxrStreamId stream_id,
                                        uxrObjectId object_id, uxrObjectId subscriber_id, const char* xml, uint8_t mode)
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

inline uint16_t create_entity_xml(uxrSession* session, uxrStreamId stream_id,
                                  uxrObjectId object_id, const char* xml, uint8_t mode,
                                  CREATE_Payload* payload)
{
    // Use participant access to access to the xml base of any object variant. //Future elegant change?
    payload->object_representation._.participant.base.representation.format = REPRESENTATION_AS_XML_STRING;
    payload->object_representation._.participant.base.representation._.xml_string_represenatation = (char*)xml;

    return common_create_entity(session, stream_id, object_id, (uint16_t)(strlen(xml) + 1), mode, payload);
}
