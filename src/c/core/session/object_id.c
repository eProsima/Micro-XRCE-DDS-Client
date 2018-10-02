#include <microxrce/client/core/session/object_id.h>
#include <microxrce/client/core/serialization/xrce_protocol.h>

const uint8_t MR_PARTICIPANT_ID = OBJK_PARTICIPANT;
const uint8_t MR_TOPIC_ID = OBJK_TOPIC;
const uint8_t MR_PUBLISHER_ID = OBJK_PUBLISHER;
const uint8_t MR_SUBSCRIBER_ID = OBJK_SUBSCRIBER;
const uint8_t MR_DATAWRITER_ID = OBJK_DATAWRITER;
const uint8_t MR_DATAREADER_ID = OBJK_DATAREADER;

//==================================================================
//                             PUBLIC
//==================================================================
mrObjectId mr_object_id(uint16_t id, uint8_t type)
{
    mrObjectId object_id;
    object_id.id = id;
    object_id.type = type;
    return object_id;
}

mrObjectId object_id_from_raw(const uint8_t* raw)
{
    mrObjectId object_id;
    object_id.id = (uint16_t)((((uint16_t)raw[0]) << 4) + (raw[1] >> 4));
    object_id.type = raw[1] & 0x0F;
    return object_id;
}

void object_id_to_raw(mrObjectId object_id, uint8_t* raw)
{
    raw[0] = (uint8_t) (object_id.id >> 4);
    raw[1] = (uint8_t)((((uint8_t) (object_id.id)) << 4) + object_id.type);
}
