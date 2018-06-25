#include <micrortps/client/profile/session/create_entities_xml.h>
#include <micrortps/client/core/serialization/xrce_protocol.h>
#include <micrortps/client/core/session/submessage.h>
#include <string.h>

const uint8_t ENTITY_REPLACE = (uint8_t)FLAG_REUSE;
const uint8_t ENTITY_REUSE = (uint8_t)FLAG_REPLACE;

uint16_t common_create_entity_xml(Session* session, StreamId stream_id,
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

    return common_create_entity_xml(session, stream_id, object_id, xml, flags, &payload);
}

uint16_t write_configure_type_xml(Session* session, StreamId stream_id,
                                  mrObjectId object_id, char* xml, uint8_t flags)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_TYPE;

    return common_create_entity_xml(session, stream_id, object_id, xml, flags, &payload);
}

uint16_t write_configure_application_xml(Session* session, StreamId stream_id,
                                         mrObjectId object_id, char* xml, uint8_t flags)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_APPLICATION;

    return common_create_entity_xml(session, stream_id, object_id, xml, flags, &payload);
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

    return common_create_entity_xml(session, stream_id, object_id, xml, flags, &payload);
}

uint16_t write_configure_topic_xml(Session* session, StreamId stream_id,
                                   mrObjectId object_id, mrObjectId participant_id, char* xml, uint8_t flags)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_TOPIC;
    object_id_to_raw(participant_id, payload.object_representation._.topic.participant_id.data);

    return common_create_entity_xml(session, stream_id, object_id, xml, flags, &payload);
}

uint16_t write_configure_publisher_xml(Session* session, StreamId stream_id,
                                       mrObjectId object_id, mrObjectId participant_id, char* xml, uint8_t flags)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_PUBLISHER;
    object_id_to_raw(participant_id, payload.object_representation._.publisher.participant_id.data);

    return common_create_entity_xml(session, stream_id, object_id, xml, flags, &payload);
}

uint16_t write_configure_subscriber_xml(Session* session, StreamId stream_id,
                                        mrObjectId object_id, mrObjectId participant_id, char* xml, uint8_t flags)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_SUBSCRIBER;
    object_id_to_raw(participant_id, payload.object_representation._.subscriber.participant_id.data);

    return common_create_entity_xml(session, stream_id, object_id, xml, flags, &payload);
}

uint16_t write_configure_datawriter_xml(Session* session, StreamId stream_id,
                                        mrObjectId object_id, mrObjectId publisher_id, char* xml, uint8_t flags)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_DATAWRITER;
    object_id_to_raw(publisher_id, payload.object_representation._.data_writer.publisher_id.data);

    return common_create_entity_xml(session, stream_id, object_id, xml, flags, &payload);
}

uint16_t write_configure_datareader_xml(Session* session, StreamId stream_id,
                                        mrObjectId object_id, mrObjectId subscriber_id, char* xml, uint8_t flags)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_DATAREADER;
    object_id_to_raw(subscriber_id, payload.object_representation._.data_reader.subscriber_id.data);

    return common_create_entity_xml(session, stream_id, object_id, xml, flags, &payload);
}

//==================================================================
//                             PRIVATE
//==================================================================

uint16_t common_create_entity_xml(Session* session, StreamId stream_id,
                                  mrObjectId object_id, char* xml, uint8_t flags,
                                  CREATE_Payload* payload)
{
    uint16_t request_id = generate_request_id(session);

    payload->base.request_id.data[0] = (uint8_t) (request_id >> 8);
    payload->base.request_id.data[1] = (uint8_t) request_id;
    object_id_to_raw(object_id, payload->base.object_id.data);

    // Use participant access to access to the xml base of any object variant. //Future elegant change?
    payload->object_representation._.participant.base.representation.format = REPRESENTATION_AS_XML_STRING;
    payload->object_representation._.participant.base.representation._.xml_string_represenatation.data = xml;
    payload->object_representation._.participant.base.representation._.xml_string_represenatation.size = strlen(xml) + 1;

    // Change this when microcdr supports size_of functionobject id. Currently, DOMAIN_ID is not supported.
    size_t payload_length = 0; //CREATE_Payload_size(&payload);
    payload_length += 4; // base
    payload_length += 1; // objk type
    payload_length += 1; // base3 type => xml
    payload_length += 2; // padding
    payload_length += 4; // xml length
    payload_length += strlen(xml) + 1; // xml data (note: compiler executes strlen one time this function)
    payload_length += (object_id.type == OBJK_PARTICIPANT && payload_length % 2 != 0) ? 1 : 0; // necessary padding
    payload_length += (object_id.type == APPLICATION_ID || object_id.type == TYPE_ID ||
                       object_id.type  == DOMAIN_ID || object_id.type == QOS_PROFILE_ID) ? 0 : 2; //object id ref

    MicroBuffer mb;
    bool available = prepare_stream_to_write(session, stream_id, payload_length + SUBHEADER_SIZE, &mb);
    if(available)
    {
        (void) write_submessage_header(&mb, SUBMESSAGE_ID_CREATE, payload_length, flags);
        (void) serialize_CREATE_Payload(&mb, payload);
    }
    else
    {
        request_id = 0;
    }

    return request_id;
}
