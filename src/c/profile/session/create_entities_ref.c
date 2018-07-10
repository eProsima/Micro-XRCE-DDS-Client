#include <micrortps/client/profile/session/create_entities_ref.h>
#include <micrortps/client/core/serialization/xrce_protocol.h>
#include <string.h>

static uint16_t create_entity_ref(Session* session, StreamId stream_id,
                                  mrObjectId object_id, char* ref, uint8_t flags,
                                  CREATE_Payload* payload);

//==================================================================
//                              PUBLIC
//==================================================================
uint16_t write_create_qos_profile_ref(Session* session, StreamId stream_id,
                                         mrObjectId object_id, char* ref, uint8_t flags)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_QOSPROFILE;

    return create_entity_ref(session, stream_id, object_id, ref, flags, &payload);
}

uint16_t write_create_type_ref(Session* session, StreamId stream_id,
                                  mrObjectId object_id, char* ref, uint8_t flags)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_TYPE;

    return create_entity_ref(session, stream_id, object_id, ref, flags, &payload);
}

uint16_t write_create_domain_ref(Session* session, StreamId stream_id,
                                    mrObjectId object_id, char* ref, uint8_t flags)
{
    (void) session; (void) stream_id; (void) object_id; (void) ref; (void) flags;
    //TODO
    return false;
}

uint16_t write_create_participant_ref(Session* session, StreamId stream_id,
                                         mrObjectId object_id, char* ref, uint8_t flags)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_PARTICIPANT;

    return create_entity_ref(session, stream_id, object_id, ref, flags, &payload);
}

uint16_t write_create_topic_ref(Session* session, StreamId stream_id,
                                   mrObjectId object_id, mrObjectId participant_id, char* ref, uint8_t flags)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_TOPIC;
    object_id_to_raw(participant_id, payload.object_representation._.topic.participant_id.data);

    return create_entity_ref(session, stream_id, object_id, ref, flags, &payload);
}

//==================================================================
//                             PRIVATE
//==================================================================

inline uint16_t create_entity_ref(Session* session, StreamId stream_id,
                                  mrObjectId object_id, char* ref, uint8_t flags,
                                  CREATE_Payload* payload)
{
    uint32_t ref_size = strlen(ref) + 1;

    // Use participant access to access to the ref base of any object variant. //Future elegant change?
    payload->object_representation._.participant.base.representation.format = REPRESENTATION_BY_REFERENCE;
    payload->object_representation._.participant.base.representation._.object_reference.size = ref_size;
    memcpy(payload->object_representation._.participant.base.representation._.object_reference.data, ref, ref_size);

    return common_create_entity(session, stream_id, object_id, ref_size, flags, payload);
}
