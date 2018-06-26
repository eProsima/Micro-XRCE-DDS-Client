#include <micrortps/client/profile/session/create_entities_xml.h>
#include <micrortps/client/core/serialization/xrce_protocol.h>
#include <string.h>

uint16_t create_entity_xml(Session* session, StreamId stream_id,
                                  mrObjectId object_id, char* xml, uint8_t flags,
                                  CREATE_Payload* payload);

//==================================================================
//                              PUBLIC
//==================================================================
uint16_t write_configure_qos_profile_xml(Session* session, StreamId stream_id,
                                         mrObjectId object_id, char* xml, uint8_t flags)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_QOSPROFILE;

    return create_entity_xml(session, stream_id, object_id, xml, flags, &payload);
}

uint16_t write_configure_type_xml(Session* session, StreamId stream_id,
                                  mrObjectId object_id, char* xml, uint8_t flags)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_TYPE;

    return create_entity_xml(session, stream_id, object_id, xml, flags, &payload);
}

uint16_t write_configure_application_xml(Session* session, StreamId stream_id,
                                         mrObjectId object_id, char* xml, uint8_t flags)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_APPLICATION;

    return create_entity_xml(session, stream_id, object_id, xml, flags, &payload);
}

uint16_t write_configure_domain_xml(Session* session, StreamId stream_id,
                                    mrObjectId object_id, char* xml, uint8_t flags)
{
    (void) session; (void) stream_id; (void) object_id; (void) xml; (void) flags;
    //TODO
    return false;
}

uint16_t write_configure_participant_xml(Session* session, StreamId stream_id,
                                         mrObjectId object_id, uint16_t domain, char* xml, uint8_t flags)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_PARTICIPANT;
    payload.object_representation._.participant.domain_id = domain;

    return create_entity_xml(session, stream_id, object_id, xml, flags, &payload);
}

uint16_t write_configure_topic_xml(Session* session, StreamId stream_id,
                                   mrObjectId object_id, mrObjectId participant_id, char* xml, uint8_t flags)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_TOPIC;
    object_id_to_raw(participant_id, payload.object_representation._.topic.participant_id.data);

    return create_entity_xml(session, stream_id, object_id, xml, flags, &payload);
}

uint16_t write_configure_publisher_xml(Session* session, StreamId stream_id,
                                       mrObjectId object_id, mrObjectId participant_id, char* xml, uint8_t flags)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_PUBLISHER;
    object_id_to_raw(participant_id, payload.object_representation._.publisher.participant_id.data);

    return create_entity_xml(session, stream_id, object_id, xml, flags, &payload);
}

uint16_t write_configure_subscriber_xml(Session* session, StreamId stream_id,
                                        mrObjectId object_id, mrObjectId participant_id, char* xml, uint8_t flags)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_SUBSCRIBER;
    object_id_to_raw(participant_id, payload.object_representation._.subscriber.participant_id.data);

    return create_entity_xml(session, stream_id, object_id, xml, flags, &payload);
}

uint16_t write_configure_datawriter_xml(Session* session, StreamId stream_id,
                                        mrObjectId object_id, mrObjectId publisher_id, char* xml, uint8_t flags)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_DATAWRITER;
    object_id_to_raw(publisher_id, payload.object_representation._.data_writer.publisher_id.data);

    return create_entity_xml(session, stream_id, object_id, xml, flags, &payload);
}

uint16_t write_configure_datareader_xml(Session* session, StreamId stream_id,
                                        mrObjectId object_id, mrObjectId subscriber_id, char* xml, uint8_t flags)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_DATAREADER;
    object_id_to_raw(subscriber_id, payload.object_representation._.data_reader.subscriber_id.data);

    return create_entity_xml(session, stream_id, object_id, xml, flags, &payload);
}

//==================================================================
//                             PRIVATE
//==================================================================

uint16_t create_entity_xml(Session* session, StreamId stream_id,
                                  mrObjectId object_id, char* xml, uint8_t flags,
                                  CREATE_Payload* payload)
{
    size_t xml_size = strlen(xml) + 1;

    // Use participant access to access to the xml base of any object variant. //Future elegant change?
    payload->object_representation._.participant.base.representation.format = REPRESENTATION_AS_XML_STRING;
    payload->object_representation._.participant.base.representation._.xml_string_represenatation.data = xml;
    payload->object_representation._.participant.base.representation._.xml_string_represenatation.size = xml_size;

    return common_create_entity(session, stream_id, object_id, xml_size, flags, payload);
}
