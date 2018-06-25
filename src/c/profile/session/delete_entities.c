#include <micrortps/client/profile/session/delete_entities.h>
#include <micrortps/client/core/serialization/xrce_protocol.h>
#include <micrortps/client/core/session/submessage.h>

uint16_t write_delete_entity(Session* session, StreamId stream_id, mrObjectId object_id)
{

    uint16_t request_id = generate_request_id(session);

    DELETE_Payload payload;
    payload.base.request_id.data[0] = (uint8_t) (request_id >> 8);
    payload.base.request_id.data[1] = (uint8_t) request_id;
    object_id_to_raw(object_id, payload.base.object_id.data);

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
