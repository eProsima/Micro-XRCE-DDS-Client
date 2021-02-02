#include <uxr/client/core/session/write_access.h>
#include <uxr/client/core/type/xrce_types.h>

#include "session_internal.h"
#include "session_info_internal.h"
#include "submessage_internal.h"
#include "./stream/output_reliable_stream_internal.h"
#include "./stream/common_reliable_stream_internal.h"
#include "./stream/stream_storage_internal.h"
#include "./stream/seq_num_internal.h"

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
    }

    return !ub->error;
}


// Continuous fragment mode

typedef struct {
    uxrSession* session;
    uxrOnBuffersFull flush_callback;
    uxrStreamId stream_id;
    size_t topic_size;
} continuous_args_t;

static continuous_args_t continuous_args;

bool on_full_output_buffer_fragmented(ucdrBuffer* ub, void* args)
{   
    continuous_args_t * local_args = (continuous_args_t *) args;

    uxrSession* session = local_args->session;
    uxrOutputReliableStream* stream = uxr_get_output_reliable_stream(&session->streams, local_args->stream_id.index);

    size_t remaining_blocks = get_available_free_slots(stream);

    if (0 == remaining_blocks)
    {
        if(!local_args->flush_callback(session) ||
           0 == (remaining_blocks = get_available_free_slots(stream)))
        {
            return true;
        }
    }

    size_t buffer_capacity = uxr_get_reliable_buffer_capacity(&stream->base);
    uint16_t available_block_size = (uint16_t)(buffer_capacity - (uint16_t)(stream->offset + SUBHEADER_SIZE));
    local_args->topic_size = local_args->topic_size - (size_t) available_block_size;

    ucdrBuffer temp_ub;
    ucdr_init_buffer_origin_offset(
            &temp_ub,
            uxr_get_reliable_buffer(&stream->base, stream->last_written),
            buffer_capacity,
            0u,
            uxr_get_reliable_buffer_size(&stream->base, stream->last_written));
    uxr_buffer_submessage_header(&temp_ub, SUBMESSAGE_ID_FRAGMENT, available_block_size, (local_args->topic_size < buffer_capacity) ? FLAG_LAST_FRAGMENT : 0);
    uxr_set_reliable_buffer_size(&stream->base, stream->last_written, buffer_capacity);
    stream->last_written = uxr_seq_num_add(stream->last_written, 1);
    
    // Preparing the buffer for the user
    ucdr_init_buffer(
        ub,
        temp_ub.iterator,
        (size_t)(temp_ub.final - temp_ub.iterator));
    ucdr_set_on_full_buffer_callback(ub, on_full_output_buffer_fragmented, args);

    return false;
}

bool uxr_prepare_output_stream_fragmented(
    uxrSession* session, 
    uxrStreamId stream_id, 
    uxrObjectId datawriter_id,
    ucdrBuffer* ub, 
    size_t topic_size,
    uxrOnBuffersFull flush_callback)
{
    size_t user_required_space = topic_size + SUBHEADER_SIZE + WRITE_DATA_PAYLOAD_SIZE;

    uxrOutputReliableStream* stream = uxr_get_output_reliable_stream(&session->streams, stream_id.index);

    if (stream == NULL)
    {
        return false;
    }

    size_t remaining_blocks = get_available_free_slots(stream);
    
    if (0 == remaining_blocks)
    {
        if(!flush_callback(session) ||
           0 == (remaining_blocks = get_available_free_slots(stream)))
        {
            return false;
        }
    }
    
    uxrSeqNum seq_num = stream->last_written;
    uint8_t* buffer = uxr_get_reliable_buffer(&stream->base, seq_num);
    size_t buffer_size = uxr_get_reliable_buffer_size(&stream->base, seq_num);
    size_t buffer_capacity = uxr_get_reliable_buffer_capacity(&stream->base);

    // Always start in a fresh buffer to avoid handling first fragment size
    if(buffer_size > (size_t)stream->offset)
    {
        seq_num = uxr_seq_num_add(seq_num, 1);
        buffer = uxr_get_reliable_buffer(&stream->base, seq_num);
        buffer_size = uxr_get_reliable_buffer_size(&stream->base, seq_num);
    }

    uint16_t available_block_size = (uint16_t)(buffer_capacity - (uint16_t)(stream->offset + SUBHEADER_SIZE));

    ucdrBuffer temp_ub;
    ucdr_init_buffer_origin_offset(
            &temp_ub,
            uxr_get_reliable_buffer(&stream->base, seq_num),
            buffer_capacity,
            0u,
            uxr_get_reliable_buffer_size(&stream->base, seq_num));
    uxr_buffer_submessage_header(&temp_ub, SUBMESSAGE_ID_FRAGMENT, available_block_size, (user_required_space < buffer_capacity) ? FLAG_LAST_FRAGMENT : 0);
    uxr_set_reliable_buffer_size(&stream->base, seq_num, buffer_capacity);
    seq_num = uxr_seq_num_add(seq_num, 1);

    // Preparing the buffer for the user
    ucdr_init_buffer(
        ub,
        buffer + buffer_size + SUBHEADER_SIZE,
        (uint32_t)(buffer_capacity - buffer_size - SUBHEADER_SIZE));

    stream->last_written = seq_num;

    // Fill the SUBMESSAGE_ID_WRITE_DATA
    size_t payload_size = WRITE_DATA_PAYLOAD_SIZE + topic_size;

    (void) uxr_buffer_submessage_header(ub, SUBMESSAGE_ID_WRITE_DATA, (uint16_t)payload_size, FORMAT_DATA);

    WRITE_DATA_Payload_Data payload;
    uxr_init_base_object_request(&session->info, datawriter_id, &payload.base);
    (void) uxr_serialize_WRITE_DATA_Payload_Data(ub, &payload);

    ucdr_init_buffer(ub, ub->iterator, (size_t)(ub->final - ub->iterator));

    continuous_args.session = session;
    continuous_args.stream_id = stream_id;
    continuous_args.topic_size = user_required_space;    
    continuous_args.flush_callback = flush_callback;    
    ucdr_set_on_full_buffer_callback(ub, on_full_output_buffer_fragmented, &continuous_args);

    return true;
}
