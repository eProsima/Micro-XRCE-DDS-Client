#include <uxr/client/profile/session/write_access.h>

#include "../../core/session/session_internal.h"
#include "../../core/session/session_info_internal.h"
#include "../../core/session/submessage_internal.h"
#include "../../core/serialization/xrce_protocol_internal.h"

#define WRITE_DATA_PAYLOAD_SIZE 8

//==================================================================
//                             PUBLIC
//==================================================================
bool uxr_prepare_output_stream(uxrSession* session, uxrStreamId stream_id, uxrObjectId datawriter_id,
                              struct ucdrBuffer* ub_topic, uint32_t topic_size)
{
    ucdrBuffer ub;
    size_t payload_size = WRITE_DATA_PAYLOAD_SIZE + topic_size;
    if(uxr_prepare_stream_to_write_submessage(session, stream_id, payload_size,
                                              &ub, SUBMESSAGE_ID_WRITE_DATA, FORMAT_DATA))
    {
        WRITE_DATA_Payload_Data payload;
        uxr_init_base_object_request(&session->info, datawriter_id, &payload.base);
        (void) uxr_serialize_WRITE_DATA_Payload_Data(&ub, &payload);
        (void) ucdr_serialize_uint32_t(&ub, topic_size); //REMOVE: when topics have not a previous size in the agent.

        ucdr_init_buffer(ub_topic, ub.iterator, ucdr_buffer_remaining(&ub));
        ucdr_set_on_full_buffer_callback(ub_topic, ub.on_full_buffer, ub.args);
    }
    else
    {
        ub_topic->error = true;
    }

    return !ub_topic->error;
}

