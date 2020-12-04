#include <uxr/client/core/session/write_access.h>
#include <uxr/client/core/type/xrce_types.h>

#include "session_internal.h"
#include "session_info_internal.h"
#include "submessage_internal.h"
#include "./stream/output_reliable_stream_internal.h"
#include "./stream/common_reliable_stream_internal.h"
#include "./stream/stream_storage_internal.h"

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

uint16_t uxr_prepare_output_stream_fragmented(uxrSession* session, uxrStreamId stream_id, uxrObjectId datawriter_id,
                               ucdrBuffer* ub, uint16_t topic_data_index, uint16_t topic_total_size)
{

    // 1. User told that is going to send TOTAL. INDEX is zero the first time
    // 2. If TOTAL fits in the current stream, prepare the whole stream and return TOTAL: user will serialize and on_full callback will handle the rest.
    //    This implies only one call to this function.
    // 3. If TOTAL does not fit in the current stream, prepare the whole stream and return the data that fits: user will serialize up to this data and then is
    //    in charge of running session and calling this with an updated INDEX

    uint16_t available_write_space = 0;
    uint16_t user_required_space = (topic_total_size - topic_data_index) + ((topic_data_index == 0) ? SUBHEADER_SIZE + WRITE_DATA_PAYLOAD_SIZE : 0);

    uxrOutputReliableStream * stream = uxr_get_output_reliable_stream(&session->streams, stream_id.index);
    uxrSeqNum seq_num = stream->last_written;
    uint8_t * buffer = uxr_get_reliable_buffer(&stream->base, seq_num);
    size_t buffer_size = uxr_get_reliable_buffer_size(&stream->base, seq_num);
    size_t buffer_capacity = uxr_get_reliable_buffer_capacity(&stream->base);

    // Always start in a fresh buffer to avoid handling first fragment size
    if(buffer_size > (size_t)stream->offset)
    {
        seq_num = uxr_seq_num_add(seq_num, 1);
        buffer = uxr_get_reliable_buffer(&stream->base, seq_num);
        buffer_size = uxr_get_reliable_buffer_size(&stream->base, seq_num);
    }

    size_t used_blocks = uxr_seq_num_sub(seq_num, stream->last_acknown);
    used_blocks = (used_blocks == 0) ? 
                    0 : 
                    (used_blocks > stream->base.history) ? 
                        stream->base.history :
                        used_blocks - 1;
    size_t remaining_blocks = stream->base.history - used_blocks;
    uint16_t available_block_size = (uint16_t)(buffer_capacity - (uint16_t)(stream->offset + SUBHEADER_SIZE));
    uint16_t writtable_size = (available_block_size * remaining_blocks);

    if (0 == remaining_blocks)
    {
        return 0;
    }

    if (writtable_size >= user_required_space)
    {
        uint16_t last_fragment_size;
        size_t necessary_blocks;

        if (0 == (user_required_space % available_block_size))
        {
            last_fragment_size = available_block_size;
            necessary_blocks = (uint16_t)(user_required_space / available_block_size);
        }
        else
        {
            last_fragment_size = user_required_space % available_block_size;
            necessary_blocks = (uint16_t)((user_required_space / available_block_size) + 1);
        }
    
        // Fill all streams with header and fragment subheaders and handle the last fragment FLAG and size

        ucdrBuffer temp_ub;
        for(uint16_t i = 0; i < necessary_blocks - 1; i++)
        {
            ucdr_init_buffer_origin_offset(
                &temp_ub,
                uxr_get_reliable_buffer(&stream->base, seq_num),
                buffer_capacity,
                0u,
                uxr_get_reliable_buffer_size(&stream->base, seq_num));
            uxr_buffer_submessage_header(&temp_ub, SUBMESSAGE_ID_FRAGMENT, available_block_size, 0);
            uxr_set_reliable_buffer_size(&stream->base, seq_num, buffer_capacity);
            seq_num = uxr_seq_num_add(seq_num, 1);
        }

        ucdr_init_buffer_origin_offset(
                &temp_ub,
                uxr_get_reliable_buffer(&stream->base, seq_num),
                buffer_capacity,
                0u,
                uxr_get_reliable_buffer_size(&stream->base, seq_num));
            uxr_buffer_submessage_header(&temp_ub, SUBMESSAGE_ID_FRAGMENT, last_fragment_size, FLAG_LAST_FRAGMENT);
            uxr_set_reliable_buffer_size(&stream->base, seq_num, stream->offset + (size_t)(SUBHEADER_SIZE) + last_fragment_size);

        available_write_space = user_required_space - ((topic_data_index == 0) ? SUBHEADER_SIZE + WRITE_DATA_PAYLOAD_SIZE : 0);
    }
    else
    {
        // Fill all streams with header and fragment subheaders

        ucdrBuffer temp_ub;
        for(uint16_t i = 0; i < remaining_blocks; i++)
        {
            ucdr_init_buffer_origin_offset(
                &temp_ub,
                uxr_get_reliable_buffer(&stream->base, seq_num),
                buffer_capacity,
                0u,
                uxr_get_reliable_buffer_size(&stream->base, seq_num));
            uxr_buffer_submessage_header(&temp_ub, SUBMESSAGE_ID_FRAGMENT, available_block_size, 0);
            uxr_set_reliable_buffer_size(&stream->base, seq_num, buffer_capacity);
            seq_num = uxr_seq_num_add(seq_num, 1);
        }

        available_write_space = writtable_size - ((topic_data_index == 0) ? SUBHEADER_SIZE + WRITE_DATA_PAYLOAD_SIZE : 0);
    }

    // Preparing the buffer for the user
    ucdr_init_buffer(
        ub,
        buffer + buffer_size + SUBHEADER_SIZE,
        (uint32_t)(buffer_capacity - buffer_size - SUBHEADER_SIZE));
    ucdr_set_on_full_buffer_callback(ub, on_full_output_buffer, stream);
    stream->last_written = seq_num;

    // If INDEX is zero fill the SUBMESSAGE_ID_WRITE_DATA
    if (topic_data_index == 0)
    {   
        size_t payload_size = WRITE_DATA_PAYLOAD_SIZE + topic_total_size;

        (void) uxr_buffer_submessage_header(ub, SUBMESSAGE_ID_WRITE_DATA, (uint16_t)payload_size, FORMAT_DATA);
    
        WRITE_DATA_Payload_Data payload;
        uxr_init_base_object_request(&session->info, datawriter_id, &payload.base);
        (void) uxr_serialize_WRITE_DATA_Payload_Data(ub, &payload);

        OnFullBuffer on_full_buffer = ub->on_full_buffer;
        void* args = ub->args;
        ucdr_init_buffer(ub, ub->iterator, (size_t)(ub->final - ub->iterator));
        ucdr_set_on_full_buffer_callback(ub, on_full_buffer, args);
    }

    return available_write_space;
}

