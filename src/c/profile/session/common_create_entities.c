#include <micrortps/client/profile/session/common_create_entities.h>
#include <micrortps/client/core/session/submessage.h>
#include <micrortps/client/core/serialization/xrce_protocol.h>

//==================================================================
//                              PUBLIC
//==================================================================
uint16_t mr_write_delete_entity(mrSession* session, mrStreamId stream_id, mrObjectId object_id)
{
    uint16_t request_id = MR_INVALID_REQUEST_ID;

    DELETE_Payload payload;

    // Change this when microcdr supports size_of function.
    uint16_t payload_length = 0; //DELETE_Payload_size(&payload);
    payload_length = (uint16_t)(payload_length + 4); // delete payload (request id + object_id), no padding.

    mcBuffer mb;
    if(prepare_stream_to_write(&session->streams, stream_id, (uint16_t)(payload_length + SUBHEADER_SIZE), &mb))
    {
        (void) write_submessage_header(&mb, SUBMESSAGE_ID_DELETE, payload_length, 0);

        request_id = init_base_object_request(&session->info, object_id, &payload.base);
        (void) serialize_DELETE_Payload(&mb, &payload);
    }

    return request_id;
}

uint16_t common_create_entity(mrSession* session, mrStreamId stream_id,
                                  mrObjectId object_id, uint16_t xml_ref_size, uint8_t mode,
                                  CREATE_Payload* payload)
{
    uint16_t request_id = MR_INVALID_REQUEST_ID;

    // Change this when microcdr supports size_of function. Currently, DOMAIN_ID is not supported.
    uint16_t payload_length = 0; //CREATE_Payload_size(&payload);
    payload_length = (uint16_t)(payload_length + 4); // base
    payload_length = (uint16_t)(payload_length + 1); // objk type
    payload_length = (uint16_t)(payload_length + 1); // base3 type => xml
    payload_length = (uint16_t)(payload_length + 2); // padding
    payload_length = (uint16_t)(payload_length + 4); // xml length
    payload_length = (uint16_t)(payload_length + xml_ref_size); // xml data (note: compiler executes strlen one time this function)
    payload_length = (uint16_t)(payload_length + ((object_id.type == OBJK_PARTICIPANT && payload_length % 2 != 0) ? 1 : 0)); // necessary padding
    payload_length = (uint16_t)(payload_length + 2); //object id ref

    mcBuffer mb;
    if(prepare_stream_to_write(&session->streams, stream_id, (uint16_t)(payload_length + SUBHEADER_SIZE), &mb))
    {
        request_id = init_base_object_request(&session->info, object_id, &payload->base);
        (void) write_submessage_header(&mb, SUBMESSAGE_ID_CREATE, payload_length, mode);
        (void) serialize_CREATE_Payload(&mb, payload);
    }

    return request_id;
}

