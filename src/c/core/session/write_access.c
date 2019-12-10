#include <uxr/client/core/session/write_access.h>

#include "session_internal.h"
#include "session_info_internal.h"
#include "submessage_internal.h"
#include "../serialization/xrce_protocol_internal.h"

#define WRITE_DATA_PAYLOAD_SIZE 4

//==================================================================
//                             PUBLIC
//==================================================================
uint16_t uxr_buffer_request(
    uxrSession* session,
    uxrStreamId stream_id,
    uxrObjectId requester_id,
    uint8_t* buffer,
    size_t len)
{
    uint16_t rv = UXR_INVALID_REQUEST_ID;
    ucdrBuffer ub;
    size_t payload_size = WRITE_DATA_PAYLOAD_SIZE + len;

    ub.error = !uxr_prepare_stream_to_write_submessage(session, stream_id, payload_size, &ub, SUBMESSAGE_ID_WRITE_DATA, FORMAT_DATA);
    if (!ub.error)
    {
        WRITE_DATA_Payload_Data payload;
        rv = uxr_init_base_object_request(&session->info, requester_id, &payload.base);
        uxr_serialize_WRITE_DATA_Payload_Data(&ub, &payload);
        ucdr_serialize_array_uint8_t(&ub, buffer, len);
    }

    return rv;
}

bool uxr_prepare_output_stream(uxrSession* session, uxrStreamId stream_id, uxrObjectId datawriter_id,
                               ucdrBuffer* ub, uint32_t topic_size)
{
    size_t payload_size = WRITE_DATA_PAYLOAD_SIZE + topic_size;
    ub->error = !uxr_prepare_stream_to_write_submessage(session, stream_id, payload_size, ub, SUBMESSAGE_ID_WRITE_DATA, FORMAT_DATA);
    if(!ub->error)
    {
        WRITE_DATA_Payload_Data payload;
        uxr_init_base_object_request(&session->info, datawriter_id, &payload.base);
        (void) uxr_serialize_WRITE_DATA_Payload_Data(ub, &payload);

        ub->last_data_size = 8; //reset alignment (as if we were created a new ucdrBuffer)
    }

    return !ub->error;
}

