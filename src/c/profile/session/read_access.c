#include <micrortps/client/profile/session/read_access.h>
#include <micrortps/client/core/serialization/xrce_protocol.h>
#include <micrortps/client/core/session/submessage.h>

const uint8_t MR_FORMAT_DATA = FORMAT_DATA;
const uint8_t MR_FORMAT_SAMPLE = FORMAT_SAMPLE;
const uint8_t MR_FORMAT_DATA_SEQ = FORMAT_DATA_SEQ;
const uint8_t MR_FORMAT_SAMPLE_SEQ = FORMAT_SAMPLE_SEQ;
const uint8_t MR_FORMAT_PACKED_SAMPLES = FORMAT_PACKED_SAMPLES;

void read_format_data(Session* session, MicroBuffer* payload,
                      StreamId stream_id, mrObjectId object_id, uint16_t request_id);
void read_format_sample(Session* session, MicroBuffer* payload,
                        StreamId stream_id, mrObjectId object_id, uint16_t request_id);
void read_format_data_seq(Session* session, MicroBuffer* payload,
                          StreamId stream_id, mrObjectId object_id, uint16_t request_id);
void read_format_sample_seq(Session* session, MicroBuffer* payload,
                            StreamId stream_id, mrObjectId object_id, uint16_t request_id);
void read_format_packed_samples(Session* session, MicroBuffer* payload,
                                StreamId stream_id, mrObjectId object_id, uint16_t request_id);

//==================================================================
//                             PUBLIC
//==================================================================
uint16_t write_read_data(Session* session, StreamId stream_id, mrObjectId datareader_id, uint8_t format, DeliveryControl* control)
{
    uint16_t request_id = INVALID_REQUEST_ID;

    READ_DATA_Payload payload;
    payload.read_specification.data_format = format;
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
    int payload_length = 0; //READ_DATA_Payload_size(&payload);
    payload_length += 4; // (request id + object_id), no padding.
    payload_length += 3; // format and two optionals.
    payload_length += (control != NULL) ? 1 : 0; // padding
    payload_length += (control != NULL) ? 8 : 0; // delivery control

    MicroBuffer mb;
    bool available = prepare_stream_to_write(session, stream_id, payload_length + SUBHEADER_SIZE, &mb);
    if(available)
    {
        request_id = init_base_object_request(session, datareader_id, &payload.base);
        (void) write_submessage_header(&mb, SUBMESSAGE_ID_READ_DATA, payload_length, 0);
        (void) serialize_READ_DATA_Payload(&mb, &payload);
    }

    return request_id;
}

void read_submessage_data(Session* session, MicroBuffer* submessage, StreamId stream_id, uint8_t format_flags)
{
    if(session->on_topic != NULL)
    {
        BaseObjectRequest base;
        deserialize_BaseObjectRequest(submessage, &base);
        mrObjectId object_id = create_object_id_from_raw(base.object_id.data);
        uint16_t request_id = (((uint16_t) base.request_id.data[0]) << 8) + base.request_id.data[1];

        switch(format_flags)
        {
            case FORMAT_DATA:
                read_format_data(session, submessage, stream_id, object_id, request_id);
                break;

            case FORMAT_SAMPLE:
                read_format_sample(session, submessage, stream_id, object_id, request_id);
                break;

            case FORMAT_DATA_SEQ:
                read_format_data_seq(session, submessage, stream_id, object_id, request_id);
                break;

            case FORMAT_SAMPLE_SEQ:
                read_format_sample_seq(session, submessage, stream_id, object_id, request_id);
                break;

            case FORMAT_PACKED_SAMPLES:
                read_format_packed_samples(session, submessage, stream_id, object_id, request_id);
                break;

            default:
                break;
        }
    }
}

//==================================================================
//                            PRIVATE
//==================================================================
void read_format_data(Session* session, MicroBuffer* payload,
                      StreamId stream_id, mrObjectId object_id, uint16_t request_id)
{
    uint32_t offset;
    deserialize_uint32_t(payload, &offset); //Remove this when data serialization will be according to the XRCE spec.
    session->on_topic(session, object_id, request_id, payload, stream_id, session->on_topic_args);
}

void read_format_sample(Session* session, MicroBuffer* payload,
                        StreamId stream_id, mrObjectId object_id, uint16_t request_id)
{
    (void) session; (void) payload; (void) stream_id; (void) object_id; (void) request_id;
    //TODO
}

void read_format_data_seq(Session* session, MicroBuffer* payload,
                          StreamId stream_id, mrObjectId object_id, uint16_t request_id)
{
    (void) session; (void) payload; (void) stream_id; (void) object_id; (void) request_id;
    //TODO
}

void read_format_sample_seq(Session* session, MicroBuffer* payload,
                            StreamId stream_id, mrObjectId object_id, uint16_t request_id)
{
    (void) session; (void) payload; (void) stream_id; (void) object_id; (void) request_id;
    //TODO
}

void read_format_packed_samples(Session* session, MicroBuffer* payload,
                                StreamId stream_id, mrObjectId object_id, uint16_t request_id)
{
    (void) session; (void) payload; (void) stream_id; (void) object_id; (void) request_id;
    //TODO
}
