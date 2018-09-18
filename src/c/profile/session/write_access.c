#include <micrortps/client/profile/session/write_access.h>
#include <micrortps/client/core/serialization/xrce_protocol.h>
#include <micrortps/client/core/session/submessage.h>


//==================================================================
//                             PUBLIC
//==================================================================
mrTopicId mr_init_topic_micro_buffer(mrSession* session, mrStreamId stream_id, struct MicroBuffer* mb, uint32_t topic_size)
{
    (void) session; (void) stream_id; (void) mb; (void) topic_size;
    //TODO
}

bool mr_write_data(mrSession* session, mrObjectId datawriter_id, mrTopicId topic_id)
{
    (void) session; (void) datawriter_id; (void) topic_id;
    //TODO
}

