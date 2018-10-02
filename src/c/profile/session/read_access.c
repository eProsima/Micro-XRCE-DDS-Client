#include <microxrce/client/profile/session/read_access.h>
#include <microxrce/client/core/serialization/xrce_protocol.h>
#include <microxrce/client/core/session/submessage.h>

extern void read_submessage_format(mrSession* session, ucdrBuffer* data, uint16_t length, uint8_t format,
                                   mrStreamId stream_id, mrObjectId object_id, uint16_t request_id);

static void read_format_data(mrSession* session, ucdrBuffer* payload, uint16_t length,
                      mrStreamId stream_id, mrObjectId object_id, uint16_t request_id);
static void read_format_sample(mrSession* session, ucdrBuffer* payload, uint16_t length,
                        mrStreamId stream_id, mrObjectId object_id, uint16_t request_id);
static void read_format_data_seq(mrSession* session, ucdrBuffer* payload, uint16_t length,
                          mrStreamId stream_id, mrObjectId object_id, uint16_t request_id);
static void read_format_sample_seq(mrSession* session, ucdrBuffer* payload, uint16_t length,
                            mrStreamId stream_id, mrObjectId object_id, uint16_t request_id);
static void read_format_packed_samples(mrSession* session, ucdrBuffer* payload, uint16_t length,
                                mrStreamId stream_id, mrObjectId object_id, uint16_t request_id);
//==================================================================
//                             PUBLIC
//==================================================================
uint16_t mr_write_request_data(mrSession* session, mrStreamId stream_id, mrObjectId datareader_id,
                         mrStreamId data_stream_id, mrDeliveryControl* control)
{
    uint16_t request_id = MR_INVALID_REQUEST_ID;

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
    uint16_t payload_length = 0; //READ_DATA_Payload_size(&payload);
    payload_length = (uint16_t)(payload_length + 4); // (request id + object_id), no padding.
    payload_length = (uint16_t)(payload_length + 4); // stream, format, and two optionals.
    payload_length = (uint16_t)(payload_length + ((control != NULL) ? 8 : 0)); // delivery control

    ucdrBuffer mb;
    if(prepare_stream_to_write(&session->streams, stream_id, (uint16_t)(payload_length + SUBHEADER_SIZE), &mb))
    {
        (void) write_submessage_header(&mb, SUBMESSAGE_ID_READ_DATA, payload_length, 0);

        request_id = init_base_object_request(&session->info, datareader_id, &payload.base);
        (void) serialize_READ_DATA_Payload(&mb, &payload);
    }

    return request_id;
}

uint16_t mr_write_cancel_data(mrSession* session, mrStreamId stream_id, mrObjectId datareader_id)
{
    mrDeliveryControl delivery_control = {0};
    return mr_write_request_data(session, stream_id, datareader_id, (mrStreamId){0}, &delivery_control);
}

//==================================================================
//                            PRIVATE
//==================================================================
void read_submessage_format(mrSession* session, ucdrBuffer* data, uint16_t length, uint8_t format,
                      mrStreamId stream_id, mrObjectId object_id, uint16_t request_id)
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

inline void read_format_data(mrSession* session, ucdrBuffer* payload, uint16_t length,
                      mrStreamId stream_id, mrObjectId object_id, uint16_t request_id)
{
    uint32_t offset;
    ucdr_deserialize_uint32_t(payload, &offset); //Remove this when data serialization will be according to the XRCE spec.
    length = (uint16_t)(length - 4); //by the offset. Remove too with the future serialization according to XRCE

    ucdrBuffer mb_topic;
    ucdr_init_buffer(&mb_topic, payload->iterator, length);
    mb_topic.endianness = payload->endianness;
    session->on_topic(session, object_id, request_id, stream_id, &mb_topic, session->on_topic_args);
}

void read_format_sample(mrSession* session, ucdrBuffer* payload, uint16_t length,
                        mrStreamId stream_id, mrObjectId object_id, uint16_t request_id)
{
    (void) session; (void) payload; (void) length; (void) stream_id; (void) object_id; (void) request_id;
    //TODO
}

void read_format_data_seq(mrSession* session, ucdrBuffer* payload, uint16_t length,
                          mrStreamId stream_id, mrObjectId object_id, uint16_t request_id)
{
    (void) session; (void) payload; (void) length; (void) stream_id; (void) object_id; (void) request_id;
    //TODO
}

void read_format_sample_seq(mrSession* session, ucdrBuffer* payload, uint16_t length,
                            mrStreamId stream_id, mrObjectId object_id, uint16_t request_id)
{
    (void) session; (void) payload; (void) length; (void) stream_id; (void) object_id; (void) request_id;
    //TODO
}

void read_format_packed_samples(mrSession* session, ucdrBuffer* payload, uint16_t length,
                                mrStreamId stream_id, mrObjectId object_id, uint16_t request_id)
{
    (void) session; (void) payload; (void) length; (void) stream_id; (void) object_id; (void) request_id;
    //TODO
}
