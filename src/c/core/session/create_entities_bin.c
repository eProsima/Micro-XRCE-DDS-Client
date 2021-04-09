#include <uxr/client/core/type/xrce_types.h>

#include "common_create_entities_internal.h"

#include <string.h>

//==================================================================
//                              PUBLIC
//==================================================================
uint16_t uxr_buffer_create_participant_bin(
        uxrSession* session,
        uxrStreamId stream_id,
        uxrObjectId object_id,
        uint16_t domain,
        uint8_t mode)
{
    CREATE_Payload payload;
    payload.object_representation.kind = DDS_XRCE_OBJK_PARTICIPANT;
    payload.object_representation._.participant.domain_id = (int16_t)domain;
    payload.object_representation._.participant.base.representation.format = DDS_XRCE_REPRESENTATION_IN_BINARY;

    OBJK_DomainParticipant_Binary participant;
    participant.optional_domain_reference = false;
    participant.optional_qos_profile_reference = false;

    ucdrBuffer ub;
    ucdr_init_buffer(&ub, payload.object_representation._.participant.base.representation._.binary_representation.data, UXR_BINARY_SEQUENCE_MAX);
    uxr_serialize_OBJK_DomainParticipant_Binary(&ub, &participant);
    payload.object_representation._.participant.base.representation._.binary_representation.size = ub.offset;

    return uxr_common_create_entity(session, stream_id, object_id, ub.offset, mode, &payload);
}

uint16_t uxr_buffer_create_topic_bin(
        uxrSession* session,
        uxrStreamId stream_id,
        uxrObjectId object_id,
        uxrObjectId participant_id,
        const char* topic_name,
        const char* type_name,
        uint8_t mode)
{
    CREATE_Payload payload;
    payload.object_representation.kind = DDS_XRCE_OBJK_TOPIC;
    uxr_object_id_to_raw(participant_id, payload.object_representation._.topic.participant_id.data);
    payload.object_representation._.topic.base.representation.format = DDS_XRCE_REPRESENTATION_IN_BINARY;

    OBJK_Topic_Binary topic;
    topic.topic_name = topic_name;
    topic.optional_type_name = true;
    topic.type_name = type_name;
    topic.optional_type_reference = false;

    ucdrBuffer ub;
    ucdr_init_buffer(&ub, payload.object_representation._.topic.base.representation._.binary_representation.data, UXR_BINARY_SEQUENCE_MAX);
    uxr_serialize_OBJK_Topic_Binary(&ub, &topic);
    payload.object_representation._.topic.base.representation._.binary_representation.size = ub.offset;

    return uxr_common_create_entity(session, stream_id, object_id, ub.offset, mode, &payload);
}

uint16_t uxr_buffer_create_publisher_bin(
        uxrSession* session,
        uxrStreamId stream_id,
        uxrObjectId object_id,
        uxrObjectId participant_id,
        uint8_t mode)
{
    CREATE_Payload payload;
    payload.object_representation.kind = DDS_XRCE_OBJK_PUBLISHER;
    uxr_object_id_to_raw(participant_id, payload.object_representation._.publisher.participant_id.data);
    payload.object_representation._.publisher.base.representation.format = DDS_XRCE_REPRESENTATION_IN_BINARY;

    OBJK_Publisher_Binary publisher;
    publisher.optional_publisher_name = false;
    publisher.optional_qos = false;

    ucdrBuffer ub;
    ucdr_init_buffer(&ub, payload.object_representation._.publisher.base.representation._.binary_representation.data, UXR_BINARY_SEQUENCE_MAX);
    uxr_serialize_OBJK_Publisher_Binary(&ub, &publisher);
    payload.object_representation._.publisher.base.representation._.binary_representation.size = ub.offset;

    return uxr_common_create_entity(session, stream_id, object_id, ub.offset, mode, &payload);
}

uint16_t uxr_buffer_create_subscriber_bin(
        uxrSession* session,
        uxrStreamId stream_id,
        uxrObjectId object_id,
        uxrObjectId participant_id,
        uint8_t mode)
{
    CREATE_Payload payload;
    payload.object_representation.kind = UXR_SUBSCRIBER_ID;
    uxr_object_id_to_raw(participant_id, payload.object_representation._.subscriber.participant_id.data);
    payload.object_representation._.subscriber.base.representation.format = DDS_XRCE_REPRESENTATION_IN_BINARY;

    OBJK_Subscriber_Binary subscriber;
    subscriber.optional_subscriber_name = false;
    subscriber.optional_qos = false;

    ucdrBuffer ub;
    ucdr_init_buffer(&ub, payload.object_representation._.subscriber.base.representation._.binary_representation.data, UXR_BINARY_SEQUENCE_MAX);
    uxr_serialize_OBJK_Subscriber_Binary(&ub, &subscriber);
    payload.object_representation._.subscriber.base.representation._.binary_representation.size = ub.offset;

    return uxr_common_create_entity(session, stream_id, object_id, ub.offset, mode, &payload);
}

uint16_t uxr_buffer_create_datawriter_bin(
        uxrSession* session,
        uxrStreamId stream_id,
        uxrObjectId object_id,
        uxrObjectId publisher_id,
        const char* topic_name,
        bool reliable,
        bool keep_last,
        bool transient_local,
        uint8_t mode)
{
    CREATE_Payload payload;
    payload.object_representation.kind = UXR_DATAWRITER_ID;
    uxr_object_id_to_raw(publisher_id, payload.object_representation._.data_writer.publisher_id.data);
    payload.object_representation._.data_writer.base.representation.format = DDS_XRCE_REPRESENTATION_IN_BINARY;

    OBJK_DataWriter_Binary datawriter;
    datawriter.topic_name = topic_name;
    datawriter.optional_qos = true;
    datawriter.qos.optional_ownership_strength = false;
    datawriter.qos.base.optional_deadline_msec = false;
    datawriter.qos.base.optional_history_depth = false;
    datawriter.qos.base.optional_lifespan_msec = false;
    datawriter.qos.base.optional_user_data = false;
    
    datawriter.qos.base.qos_flags = 0;
    datawriter.qos.base.qos_flags |= (reliable) ? is_reliable : 0;
    datawriter.qos.base.qos_flags |= (keep_last) ? is_history_keep_last : 0;
    datawriter.qos.base.qos_flags |= (transient_local) ? is_durability_transient_local : 0;

    ucdrBuffer ub;
    ucdr_init_buffer(&ub, payload.object_representation._.data_writer.base.representation._.binary_representation.data, UXR_BINARY_SEQUENCE_MAX);
    uxr_serialize_OBJK_DataWriter_Binary(&ub, &datawriter);
    payload.object_representation._.data_writer.base.representation._.binary_representation.size = ub.offset;

    return uxr_common_create_entity(session, stream_id, object_id, ub.offset, mode, &payload);
}

uint16_t uxr_buffer_create_datareader_bin(
        uxrSession* session,
        uxrStreamId stream_id,
        uxrObjectId object_id,
        uxrObjectId subscriber_id,
        const char* topic_name,
        bool reliable,
        bool keep_last,
        bool transient_local,
        uint8_t mode)
{
    CREATE_Payload payload;
    payload.object_representation.kind = UXR_DATAREADER_ID;
    uxr_object_id_to_raw(subscriber_id, payload.object_representation._.data_reader.subscriber_id.data);
    payload.object_representation._.data_reader.base.representation.format = DDS_XRCE_REPRESENTATION_IN_BINARY;

    OBJK_DataReader_Binary datareader;
    datareader.topic_name = topic_name;
    datareader.optional_qos = true;
    datareader.qos.optional_contentbased_filter = false;
    datareader.qos.optional_timebasedfilter_msec = false;
    datareader.qos.base.optional_deadline_msec = false;
    datareader.qos.base.optional_history_depth = false;
    datareader.qos.base.optional_lifespan_msec = false;
    datareader.qos.base.optional_user_data = false;

    datareader.qos.base.qos_flags = 0;
    datareader.qos.base.qos_flags |= (reliable) ? is_reliable : 0;
    datareader.qos.base.qos_flags |= (keep_last) ? is_history_keep_last : 0;
    datareader.qos.base.qos_flags |= (transient_local) ? is_durability_transient_local : 0;

    ucdrBuffer ub;
    ucdr_init_buffer(&ub, payload.object_representation._.data_reader.base.representation._.binary_representation.data, UXR_BINARY_SEQUENCE_MAX);
    uxr_serialize_OBJK_DataReader_Binary(&ub, &datareader);
    payload.object_representation._.data_reader.base.representation._.binary_representation.size = ub.offset;

    return uxr_common_create_entity(session, stream_id, object_id, ub.offset, mode, &payload);
}

uint16_t uxr_buffer_create_requester_bin(
        uxrSession* session,
        uxrStreamId stream_id,
        uxrObjectId object_id,
        uxrObjectId participant_id,
        const char* xml,
        uint8_t mode)
{
    // TODO: Update
}

uint16_t uxr_buffer_create_replier_bin(
        uxrSession* session,
        uxrStreamId stream_id,
        uxrObjectId object_id,
        uxrObjectId participant_id,
        const char* xml,
        uint8_t mode)
{
    // TODO: Update
}

//==================================================================
//                             PRIVATE
//==================================================================

// inline uint16_t create_entity_xml(
//         uxrSession* session,
//         uxrStreamId stream_id,
//         uxrObjectId object_id,
//         const char* xml,
//         uint8_t mode,
//         CREATE_Payload* payload)
// {
//     // Use participant access to access to the xml base of any object variant. //Future elegant change?
//     payload->object_representation._.participant.base.representation.format = DDS_XRCE_REPRESENTATION_AS_XML_STRING;
//     payload->object_representation._.participant.base.representation._.xml_string_represenatation = (char*)xml;

//     return uxr_common_create_entity(session, stream_id, object_id, (uint16_t)(strlen(xml) + 1), mode, payload);
// }



// TODO:
// 1. Fill client API headers
// 2. Fill client binary creation implementation
// 3. Design QoS API (see RMW)
// 4. Desing a message for requester/replier
// 5. Implement Agent creation in FastDDS middleware
// 6. Test in general repo
// 7. Update RMW to bin creation
// 8. Remove XML/REF option from RMW
// 9. Update matching engine in client