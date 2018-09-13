#include <micrortps/client/profile/session/create_entities_ref.h>
#include <micrortps/client/core/serialization/xrce_protocol.h>
#include <string.h>

static uint16_t create_entity_ref(mrSession* session, mrStreamId stream_id,
                                  mrObjectId object_id, const char* ref, uint8_t mode,
                                  CREATE_Payload* payload);

//==================================================================
//                              PUBLIC
//==================================================================
uint16_t mr_write_create_participant_ref(mrSession* session, mrStreamId stream_id,
                                         mrObjectId object_id, int16_t domain_id, const char* ref, uint8_t mode)
{
    //assert with the object_id type

    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_PARTICIPANT;
    payload.object_representation._.participant.domain_id = domain_id;

    return create_entity_ref(session, stream_id, object_id, ref, mode, &payload);
}

uint16_t mr_write_create_topic_ref(mrSession* session, mrStreamId stream_id,
                                   mrObjectId object_id, mrObjectId participant_id,
                                   const char* ref, uint8_t mode)
{
    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_TOPIC;
    object_id_to_raw(participant_id, payload.object_representation._.topic.participant_id.data);

    return create_entity_ref(session, stream_id, object_id, ref, mode, &payload);
}

uint16_t mr_write_create_datawriter_ref(mrSession* session, mrStreamId stream_id,
                                        mrObjectId object_id, mrObjectId publisher_id,
                                        const char* ref, uint8_t mode)
{
    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_DATAWRITER;
    object_id_to_raw(publisher_id, payload.object_representation._.data_writer.publisher_id.data);

    return create_entity_ref(session, stream_id, object_id, ref, mode, &payload);
}

uint16_t mr_write_create_datareader_ref(mrSession* session, mrStreamId stream_id,
                                        mrObjectId object_id, mrObjectId subscriber_id,
                                        const char* ref, uint8_t mode)
{
    CREATE_Payload payload;
    payload.object_representation.kind = OBJK_DATAREADER;
    object_id_to_raw(subscriber_id, payload.object_representation._.data_reader.subscriber_id.data);

    return create_entity_ref(session, stream_id, object_id, ref, mode, &payload);
}

//==================================================================
//                             PRIVATE
//==================================================================

inline uint16_t create_entity_ref(mrSession* session, mrStreamId stream_id,
                                  mrObjectId object_id, const char* ref, uint8_t mode,
                                  CREATE_Payload* payload)
{
    uint16_t returned_value = MR_INVALID_REQUEST_ID;
    uint32_t ref_size = (uint32_t)strlen(ref) + 1;
    if(ref_size <= UINT16_MAX)
    {
        // Use participant access to access to the ref base of any object variant. //Future elegant change?
        payload->object_representation._.participant.base.representation.format = REPRESENTATION_BY_REFERENCE;
        payload->object_representation._.participant.base.representation._.object_reference.size = ref_size;
        memcpy(payload->object_representation._.participant.base.representation._.object_reference.data, ref, ref_size);

        returned_value = common_create_entity(session, stream_id, object_id, (uint16_t)ref_size, mode, payload);
    }

    return returned_value;
}
