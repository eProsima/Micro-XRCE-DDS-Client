#include <micrortps/client/profile/session/common_create_entities.h>
#include <micrortps/client/core/serialization/xrce_protocol.h>
#include <micrortps/client/core/session/submessage.h>

const uint8_t ENTITY_REPLACE = (uint8_t)FLAG_REPLACE;
const uint8_t ENTITY_REUSE = (uint8_t)FLAG_REUSE;

uint16_t create_base_object_request(Session* session, mrObjectId object_id, BaseObjectRequest* base);

//==================================================================
//                              PUBLIC
//==================================================================
uint16_t write_delete_entity(Session* session, StreamId stream_id, mrObjectId object_id)
{
    DELETE_Payload payload;
    uint16_t request_id = create_base_object_request(session, object_id, &payload.base);

    // Change this when microcdr supports size_of functionobject id.
    int payload_length = 0; //DELETE_Payload_size(&payload);
    payload_length += 4; // delete payload (request id * object_id), no padding.

    MicroBuffer mb;
    bool available = prepare_stream_to_write(session, stream_id, payload_length + SUBHEADER_SIZE, &mb);
    if(available)
    {
        (void) write_submessage_header(&mb, SUBMESSAGE_ID_DELETE, payload_length, 0);
        (void) serialize_DELETE_Payload(&mb, &payload);
    }
    else
    {
        request_id = 0;
    }

    return request_id;
}

uint16_t common_create_entity(Session* session, StreamId stream_id,
                                  mrObjectId object_id, size_t xml_ref_size, uint8_t flags,
                                  CREATE_Payload* payload)
{
    uint16_t request_id = create_base_object_request(session, object_id, &payload->base);

    // Change this when microcdr supports size_of function. Currently, DOMAIN_ID is not supported.
    size_t payload_length = 0; //CREATE_Payload_size(&payload);
    payload_length += 4; // base
    payload_length += 1; // objk type
    payload_length += 1; // base3 type => xml
    payload_length += 2; // padding
    payload_length += 4; // xml length
    payload_length += xml_ref_size; // xml data (note: compiler executes strlen one time this function)
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

//==================================================================
//                              PRIVATE
//==================================================================
uint16_t create_base_object_request(Session* session, mrObjectId object_id, BaseObjectRequest* base)
{
    uint16_t request_id = generate_request_id(session);

    base->request_id.data[0] = (uint8_t) (request_id >> 8);
    base->request_id.data[1] = (uint8_t) request_id;
    object_id_to_raw(object_id, base->object_id.data);

    return request_id;
}
