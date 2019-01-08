#include <uxr/client/profile/session/read_access.h>

#include "../../core/session/session_internal.h"
#include "../../core/session/session_info_internal.h"
#include "../../core/session/submessage_internal.h"
#include "../../core/serialization/xrce_protocol_internal.h"

extern void read_submessage_format(uxrSession* session, ucdrBuffer* data, uint16_t length, uint8_t format,
                                   uxrStreamId stream_id, uxrObjectId object_id, uint16_t request_id);

static void read_format_data(uxrSession* session, ucdrBuffer* payload, uint16_t length,
                      uxrStreamId stream_id, uxrObjectId object_id, uint16_t request_id);
static void read_format_sample(uxrSession* session, ucdrBuffer* payload, uint16_t length,
                        uxrStreamId stream_id, uxrObjectId object_id, uint16_t request_id);
static void read_format_data_seq(uxrSession* session, ucdrBuffer* payload, uint16_t length,
                          uxrStreamId stream_id, uxrObjectId object_id, uint16_t request_id);
static void read_format_sample_seq(uxrSession* session, ucdrBuffer* payload, uint16_t length,
                            uxrStreamId stream_id, uxrObjectId object_id, uint16_t request_id);
static void read_format_packed_samples(uxrSession* session, ucdrBuffer* payload, uint16_t length,
                                uxrStreamId stream_id, uxrObjectId object_id, uint16_t request_id);
//==================================================================
//                             PUBLIC
//==================================================================
uint16_t uxr_buffer_request_data(uxrSession* session, uxrStreamId stream_id, uxrObjectId datareader_id,
                         uxrStreamId data_stream_id, uxrDeliveryControl* control)
{
    uint16_t request_id = UXR_INVALID_REQUEST_ID;

    READ_DATA_Payload payload;
    payload.read_specification.input_stream_id = data_stream_id.raw;
    payload.read_specification.data_format = FORMAT_DATA;
    payload.read_specification.optional_content_filter_expression = false; //not supported yet
    payload.read_specification.optional_delivery_control = (control != NULL);

    if(control != NULL)
    {
        payload.read_specification.delivery_control.max_bytes_per_seconds = control->max_bytes_per_second;
        payload.read_specification.delivery_control.max_elapsed_time = control->max_elapsed_time;
        payload.read_specification.delivery_control.max_samples = control->max_samples;
        payload.read_specification.delivery_control.min_pace_period = control->min_pace_period;
    }

    // Change this when microcdr supports size_of function.
    size_t payload_length = 0; //READ_DATA_Payload_size(&payload);
    payload_length += 4; // (request id + object_id), no padding.
    payload_length += 4; // stream, format, and two optionals.
    payload_length += (control != NULL) ? 8 : 0; // delivery control

    ucdrBuffer ub;
    if(uxr_prepare_stream_to_write_submessage(session, stream_id, payload_length, &ub, SUBMESSAGE_ID_READ_DATA, 0))
    {
        request_id = uxr_init_base_object_request(&session->info, datareader_id, &payload.base);
        (void) uxr_serialize_READ_DATA_Payload(&ub, &payload);
    }

    return request_id;
}

uint16_t uxr_buffer_cancel_data(uxrSession* session, uxrStreamId stream_id, uxrObjectId datareader_id)
{
    uxrDeliveryControl delivery_control = {0};
    return uxr_buffer_request_data(session, stream_id, datareader_id, (uxrStreamId){0}, &delivery_control);
}

//==================================================================
//                            PRIVATE
//==================================================================
void read_submessage_format(uxrSession* session, ucdrBuffer* data, uint16_t length, uint8_t format,
                      uxrStreamId stream_id, uxrObjectId object_id, uint16_t request_id)
{
    switch(format)
    {
        case FORMAT_DATA:
            read_format_data(session, data, length, stream_id, object_id, request_id);
            break;

        case FORMAT_SAMPLE:
            read_format_sample(session, data, length, stream_id, object_id, request_id);
            break;

        case FORMAT_DATA_SEQ:
            read_format_data_seq(session, data, length, stream_id, object_id, request_id);
            break;

        case FORMAT_SAMPLE_SEQ:
            read_format_sample_seq(session, data, length, stream_id, object_id, request_id);
            break;

        case FORMAT_PACKED_SAMPLES:
            read_format_packed_samples(session, data, length, stream_id, object_id, request_id);
            break;

        default:
            break;
    }
}

inline void read_format_data(uxrSession* session, ucdrBuffer* ub, uint16_t length,
                      uxrStreamId stream_id, uxrObjectId object_id, uint16_t request_id)
{
    (void) length;
    ub->last_data_size = 0; //reset alignment (as if we were created a new ucdrBuffer)

    session->on_topic(session, object_id, request_id, stream_id, ub, session->on_topic_args);
}

void read_format_sample(uxrSession* session, ucdrBuffer* payload, uint16_t length,
                        uxrStreamId stream_id, uxrObjectId object_id, uint16_t request_id)
{
    (void) session; (void) payload; (void) length; (void) stream_id; (void) object_id; (void) request_id;
    //TODO
}

void read_format_data_seq(uxrSession* session, ucdrBuffer* payload, uint16_t length,
                          uxrStreamId stream_id, uxrObjectId object_id, uint16_t request_id)
{
    (void) session; (void) payload; (void) length; (void) stream_id; (void) object_id; (void) request_id;
    //TODO
}

void read_format_sample_seq(uxrSession* session, ucdrBuffer* payload, uint16_t length,
                            uxrStreamId stream_id, uxrObjectId object_id, uint16_t request_id)
{
    (void) session; (void) payload; (void) length; (void) stream_id; (void) object_id; (void) request_id;
    //TODO
}

void read_format_packed_samples(uxrSession* session, ucdrBuffer* payload, uint16_t length,
                                uxrStreamId stream_id, uxrObjectId object_id, uint16_t request_id)
{
    (void) session; (void) payload; (void) length; (void) stream_id; (void) object_id; (void) request_id;
    //TODO
}
