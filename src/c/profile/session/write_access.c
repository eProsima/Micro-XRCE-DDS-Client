#include <micrortps/client/profile/session/write_access.h>
#include <micrortps/client/core/serialization/xrce_protocol.h>
#include <micrortps/client/core/session/submessage.h>

#define WRITE_DATA_PAYLOAD_SIZE 8

//==================================================================
//                             PUBLIC
//==================================================================
bool mr_prepare_output_stream(mrSession* session, mrStreamId stream_id, mrObjectId datawriter_id,
                              struct MicroBuffer* mb_topic, uint32_t topic_size)
{
    MicroBuffer mb;
    size_t submessage_size = SUBHEADER_SIZE + WRITE_DATA_PAYLOAD_SIZE + topic_size;
    if(prepare_stream_to_write(&session->streams, stream_id, submessage_size, &mb))
    {
        uint16_t payload_size = (uint16_t)(WRITE_DATA_PAYLOAD_SIZE + topic_size);
        (void) write_submessage_header(&mb, SUBMESSAGE_ID_WRITE_DATA, payload_size, FORMAT_DATA);

        WRITE_DATA_Payload_Data payload;
        init_base_object_request(&session->info, datawriter_id, &payload.base);
        (void) serialize_WRITE_DATA_Payload_Data(&mb, &payload);
        (void) serialize_uint32_t(&mb, topic_size); //REMOVE: when topics have not a previous size in the agent.

        init_micro_buffer(mb_topic, mb.iterator, topic_size);
    }
    else
    {
        mb_topic->error = true;
    }

    return !mb_topic->error;
}

