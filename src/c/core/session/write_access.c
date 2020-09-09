#include <uxr/client/core/session/write_access.h>
#include <uxr/client/core/type/xrce_types.h>

#include "session_internal.h"
#include "session_info_internal.h"
#include "submessage_internal.h"
#include "../../brokerless/brokerless_internal.h"

#define WRITE_DATA_PAYLOAD_SIZE 4
#define SAMPLE_IDENTITY_SIZE    24

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

#ifdef UCLIENT_PROFILE_BROKERLESS
        SampleIdentity sample_id;
        sample_id.sequence_number.low = rv;
        add_brokerless_message_with_sample_id(&ub, len, requester_id, sample_id);
#endif
        ucdr_serialize_array_uint8_t(&ub, buffer, len);
    }

    return rv;
}

uint16_t uxr_buffer_reply(
    uxrSession* session,
    uxrStreamId stream_id,
    uxrObjectId replier_id,
    SampleIdentity* sample_id,
    uint8_t* buffer,
    size_t len)
{
    uint16_t rv = UXR_INVALID_REQUEST_ID;
    ucdrBuffer ub;
    size_t payload_size = WRITE_DATA_PAYLOAD_SIZE + SAMPLE_IDENTITY_SIZE + len;

    ub.error = !uxr_prepare_stream_to_write_submessage(session, stream_id, payload_size, &ub, SUBMESSAGE_ID_WRITE_DATA, FORMAT_DATA);
    if (!ub.error)
    {
        WRITE_DATA_Payload_Data payload;
        rv = uxr_init_base_object_request(&session->info, replier_id, &payload.base);
        uxr_serialize_WRITE_DATA_Payload_Data(&ub, &payload);
        uxr_serialize_SampleIdentity(&ub, sample_id);

#ifdef UCLIENT_PROFILE_BROKERLESS
        add_brokerless_message_with_sample_id(&ub, len, replier_id, *sample_id);
#endif

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

        OnFullBuffer on_full_buffer = ub->on_full_buffer;
        void* args = ub->args;
        ucdr_init_buffer(ub, ub->iterator, (size_t)(ub->final - ub->iterator));
        ucdr_set_on_full_buffer_callback(ub, on_full_buffer, args);

#ifdef UCLIENT_PROFILE_BROKERLESS
        add_brokerless_message(ub, topic_size, datawriter_id);
#endif
    }

    return !ub->error;
}

