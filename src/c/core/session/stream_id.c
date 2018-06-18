#include <micrortps/client/core/session/stream_id.h>
//#include "../xrce_protocol_serialization.h"

#define BEST_EFFORT_STREAM_THRESHOLD 1
#define RELIABLE_STREAM_THRESHOLD  128

//==================================================================
//                             PUBLIC
//==================================================================
StreamId create_stream_id(uint8_t index, StreamType type, StreamDirection direction)
{
    StreamId stream_id;
    stream_id.direction = direction;
    stream_id.index = index;
    stream_id.type = type;

    switch(type)
    {
        case NONE_STREAM:
            stream_id.raw = index;
            break;
        case BEST_EFFORT_STREAM:
            stream_id.raw = index + BEST_EFFORT_STREAM_THRESHOLD;
            break;
        case RELIABLE_STREAM:
            stream_id.raw = index + RELIABLE_STREAM_THRESHOLD;
            break;
    }

    return stream_id;
}

StreamId create_stream_id_from_raw(uint8_t stream_id_raw, StreamDirection direction)
{
    StreamId stream_id;
    stream_id.raw = stream_id_raw;
    stream_id.direction = direction;

    if(BEST_EFFORT_STREAM_THRESHOLD > stream_id_raw)
    {
        stream_id.index = stream_id_raw;
        stream_id.type = NONE_STREAM;
    }
    else if(RELIABLE_STREAM_THRESHOLD > stream_id_raw)
    {
        stream_id.index = stream_id_raw - BEST_EFFORT_STREAM_THRESHOLD;
        stream_id.type = BEST_EFFORT_STREAM;
    }
    else
    {
        stream_id.index = stream_id_raw - RELIABLE_STREAM_THRESHOLD;
        stream_id.type = RELIABLE_STREAM;
    }

    return stream_id;
}

