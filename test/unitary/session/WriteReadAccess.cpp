extern "C"
{
#include <c/core/serialization/xrce_types.c>
#include <c/core/serialization/xrce_header.c>
#include <c/core/serialization/xrce_subheader.c>

#include <c/core/session/stream/seq_num.c>
#include <c/core/session/stream/stream_id.c>
#include <c/core/session/stream/stream_storage.c>
#include <c/core/session/stream/input_best_effort_stream.c>
#include <c/core/session/stream/output_best_effort_stream.c>
#include <c/core/session/stream/input_reliable_stream.c>
#include <c/core/session/stream/output_reliable_stream.c>

#include <c/core/session/object_id.c>
#include <c/core/session/submessage.c>
#include <c/core/session/session_info.c>
#include <c/core/session/read_access.c>
#include <c/core/session/write_access.c>
#include <c/core/session/session.c>

#include <c/util/time.c>
}

#include <gtest/gtest.h>

#define MTU     64
#define HISTORY 4
#define TOPIC_FITTED_SIZE   (MTU - (MIN_HEADER_SIZE + SUBHEADER_SIZE + WRITE_DATA_PAYLOAD_SIZE))
#define REQUEST_FITTED_SIZE TOPIC_FITTED_SIZE
#define REPLY_FITTED_SIZE   (MTU - (MIN_HEADER_SIZE + SUBHEADER_SIZE + WRITE_DATA_PAYLOAD_SIZE) - sizeof(SampleIdentity))

class WriteReadAccessTest : public testing::Test
{
public:
    WriteReadAccessTest()
        : session_{}
        , data_writer_id_{uxr_object_id(0, UXR_DATAWRITER_ID)}
        , data_reader_id_{uxr_object_id(0, UXR_DATAREADER_ID)}
        , requester_id_{uxr_object_id(0, UXR_REQUESTER_ID)}
        , replier_id_{uxr_object_id(0, UXR_REPLIER_ID)}
    {
        uxr_init_session(&session_, NULL, 0xAAAABBBB);
        uxr_create_output_best_effort_stream(&session_, output_best_effort_buffer, MTU);
    }

protected:
    uxrSession session_;
    uxrObjectId data_writer_id_;
    uxrObjectId data_reader_id_;
    uxrObjectId requester_id_;
    uxrObjectId replier_id_;

private:
    uint8_t output_best_effort_buffer[MTU];
};

TEST_F(WriteReadAccessTest, PrepareStreamToWriteSubmessage)
{
    ucdrBuffer ub;
    uxrStreamId stream_id;
    uint16_t data_length;

    // stream_id:   no valid
    // data_length: fitted
    // extected:    false
    stream_id = uxr_stream_id(1, UXR_BEST_EFFORT_STREAM, UXR_OUTPUT_STREAM);
    data_length = uint16_t(TOPIC_FITTED_SIZE);
    ASSERT_FALSE(uxr_prepare_output_stream(&session_, stream_id, data_writer_id_, &ub, data_length));

    // stream_id:   valid
    // data_length: no fitted
    // extected:    false
    stream_id = uxr_stream_id(0, UXR_BEST_EFFORT_STREAM, UXR_OUTPUT_STREAM);
    data_length = uint16_t(1 + TOPIC_FITTED_SIZE);
    ASSERT_FALSE(uxr_prepare_output_stream(&session_, stream_id, data_writer_id_, &ub, data_length));

    // stream_id:   valid
    // data_length: fitted
    // extected:    true
    stream_id = uxr_stream_id(0, UXR_BEST_EFFORT_STREAM, UXR_OUTPUT_STREAM);
    data_length = uint16_t(TOPIC_FITTED_SIZE);
    ASSERT_TRUE(uxr_prepare_output_stream(&session_, stream_id, data_writer_id_, &ub, data_length));
}

TEST_F(WriteReadAccessTest, BufferRequest)
{
    uint8_t buffer[MTU];
    uxrStreamId stream_id;
    uint16_t data_length;

    // stream_id:   no valid
    // data_length: fitted
    // extected:    false
    stream_id = uxr_stream_id(1, UXR_BEST_EFFORT_STREAM, UXR_OUTPUT_STREAM);
    data_length = uint16_t(REQUEST_FITTED_SIZE);
    ASSERT_FALSE(uxr_buffer_request(&session_, stream_id, requester_id_, buffer, data_length));

    // stream_id:   valid
    // data_length: no fitted
    // extected:    false
    stream_id = uxr_stream_id(0, UXR_BEST_EFFORT_STREAM, UXR_OUTPUT_STREAM);
    data_length = uint16_t(1 + REQUEST_FITTED_SIZE);
    ASSERT_FALSE(uxr_buffer_request(&session_, stream_id, requester_id_, buffer, data_length));

    // stream_id:   valid
    // data_length: fitted
    // extected:    true
    stream_id = uxr_stream_id(0, UXR_BEST_EFFORT_STREAM, UXR_OUTPUT_STREAM);
    data_length = uint16_t(REQUEST_FITTED_SIZE);
    ASSERT_TRUE(uxr_buffer_request(&session_, stream_id, requester_id_, buffer, data_length));

}

TEST_F(WriteReadAccessTest, BufferReply)
{
    uint8_t buffer[MTU];
    SampleIdentity sample_id{};
    uxrStreamId stream_id;
    uint16_t data_length;

    // stream_id:   no valid
    // data_length: fitted
    // extected:    false
    stream_id = uxr_stream_id(1, UXR_BEST_EFFORT_STREAM, UXR_OUTPUT_STREAM);
    data_length = uint16_t(REPLY_FITTED_SIZE);
    ASSERT_FALSE(uxr_buffer_reply(&session_, stream_id, requester_id_, &sample_id, buffer, data_length));

    // stream_id:   valid
    // data_length: no fitted
    // extected:    false
    stream_id = uxr_stream_id(0, UXR_BEST_EFFORT_STREAM, UXR_OUTPUT_STREAM);
    data_length = uint16_t(1 + REPLY_FITTED_SIZE);
    ASSERT_FALSE(uxr_buffer_reply(&session_, stream_id, requester_id_, &sample_id, buffer, data_length));


    // stream_id:   valid
    // data_length: fitted
    // extected:    true
    stream_id = uxr_stream_id(0, UXR_BEST_EFFORT_STREAM, UXR_OUTPUT_STREAM);
    data_length = uint16_t(REPLY_FITTED_SIZE);
    ASSERT_TRUE(uxr_buffer_reply(&session_, stream_id, replier_id_, &sample_id, buffer, data_length));

}