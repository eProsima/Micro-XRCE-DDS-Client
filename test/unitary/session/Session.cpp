extern "C"
{
#include "../src/c/core/serialization/xrce_protocol.c"
#include "../src/c/core/serialization/xrce_header.c"
#include "../src/c/core/serialization/xrce_subheader.c"

#include "../src/c/core/session/stream/seq_num.c"
#include "../src/c/core/session/stream/stream_id.c"
#include "../src/c/core/session/stream/stream_storage.c"
#include "../src/c/core/session/stream/input_best_effort_stream.c"
#include "../src/c/core/session/stream/output_best_effort_stream.c"
#include "../src/c/core/session/stream/input_reliable_stream.c"
#include "../src/c/core/session/stream/output_reliable_stream.c"

#include "../src/c/core/session/object_id.c"
#include "../src/c/core/session/submessage.c"
#include "../src/c/core/session/session_info.c"

#undef UXR_MESSAGE_LOG
#undef UXR_SERIALIZATION_LOG
#include "../src/c/core/session/session.c"
}

#include <gtest/gtest.h>
#include <string>

#define MTU                   64
#define HISTORY               4
#define OFFSET                8


class SessionTest : public testing::Test
{
public:
    static SessionTest* current;
    SessionTest()
    {
        //Necessary for the mocks
        current = this;

        comm.instance = NULL;
        comm.mtu = 32;
        comm.send_msg = send_msg;
        comm.recv_msg = recv_msg;
        comm.comm_error = comm_error;

        uxr_init_session(&session, &comm, 0xAAAABBBB);

        EXPECT_EQ(&comm, session.comm);
        EXPECT_EQ(NULL, session.request_list);
        EXPECT_EQ(NULL, session.status_list);
        EXPECT_EQ(NULL, session.request_status_list_size);

        EXPECT_EQ(NULL, session.on_status);
        EXPECT_EQ(NULL, session.on_status_args);
        EXPECT_EQ(NULL, session.on_topic);
        EXPECT_EQ(NULL, session.on_topic_args);


        uxrStreamId id = uxr_create_input_best_effort_stream(&session);
        EXPECT_EQ(0, id.index);
        EXPECT_EQ(BEST_EFFORT_STREAM_THRESHOLD, id.raw);
        EXPECT_EQ(UXR_BEST_EFFORT_STREAM, id.type);
        EXPECT_EQ(UXR_INPUT_STREAM, id.direction);

        id = uxr_create_output_best_effort_stream(&session, output_best_effort_buffer, MTU);
        EXPECT_EQ(0, id.index);
        EXPECT_EQ(BEST_EFFORT_STREAM_THRESHOLD, id.raw);
        EXPECT_EQ(UXR_BEST_EFFORT_STREAM, id.type);
        EXPECT_EQ(UXR_OUTPUT_STREAM, id.direction);

        id = uxr_create_input_reliable_stream(&session, input_reliable_buffer, MTU * HISTORY, HISTORY);
        EXPECT_EQ(0, id.index);
        EXPECT_EQ(RELIABLE_STREAM_THRESHOLD, id.raw);
        EXPECT_EQ(UXR_RELIABLE_STREAM, id.type);
        EXPECT_EQ(UXR_INPUT_STREAM, id.direction);

        id = uxr_create_output_reliable_stream(&session, output_reliable_buffer, MTU * HISTORY, HISTORY);
        EXPECT_EQ(0, id.index);
        EXPECT_EQ(RELIABLE_STREAM_THRESHOLD, id.raw);
        EXPECT_EQ(UXR_RELIABLE_STREAM, id.type);
        EXPECT_EQ(UXR_OUTPUT_STREAM, id.direction);
    }

public:
    uxrCommunication comm;
    uxrSession session;
    uint8_t output_best_effort_buffer[MTU];
    uint8_t output_reliable_buffer[MTU * HISTORY];
    uint8_t input_reliable_buffer[MTU * HISTORY];

    static bool send_msg(void* instance, const uint8_t* buf, size_t len)
    {
        (void) instance; (void) buf; (void) len;
        return true;
    }

    static bool recv_msg(void* instance, uint8_t** buf, size_t* len, int timeout)
    {
        (void) instance; (void) timeout;
        static std::array<uint8_t, MTU> input_buffer;

        if(std::string("CreateOk") == ::testing::UnitTest::GetInstance()->current_test_info()->name())
        {
            std::vector<uint8_t> message = {0x81, 0x00, 0x00, 0x00, 0x04, 0x01, 0x19, 0x00,
                                            0x00, 0x01, 0xFF, 0xFE, 0x00, 0x00, 0x58, 0x52,
                                            0x43, 0x45, 0x01, 0x00, 0x01, 0x0F , 0x0, 0x00,
                                            0x77, 0x6B, 0x48, 0x5C, 0x43, 0x14, 0x1C, 0x34,
                                            0x00};
            *len = message.size();
            *buf = input_buffer.data();
            std::copy_n(message.begin(), *len, input_buffer.begin());
            return true;
        }
        else if(std::string("CreateNoOk") == ::testing::UnitTest::GetInstance()->current_test_info()->name())
        {
            (void) buf; (void) len;
            return false;
        }
        if(std::string("DeleteOk") == ::testing::UnitTest::GetInstance()->current_test_info()->name())
        {
            std::vector<uint8_t> message = {0x81, 0x00, 0x00, 0x00, 0x05, 0x01, 0x06, 0x00,
                                            0x00, 0x02, 0xFF, 0xFE, 0x00, 0x00};
            *len = message.size();
            *buf = input_buffer.data();
            std::copy_n(message.begin(), *len, input_buffer.begin());
            return true;
        }
        else if(std::string("DeleteNoOk") == ::testing::UnitTest::GetInstance()->current_test_info()->name())
        {
            (void) buf; (void) len;
            return false;
        }

        return false;
    }

    static uint8_t comm_error(void)
    {
        return 0;
    }

    static void on_status_func (struct uxrSession* session, uxrObjectId object_id, uint16_t request_id,
                             uint8_t status, void* args)
    {
        (void) session; (void) object_id; (void) request_id; (void) status; (void) args;
    }

    static void on_topic_func (struct uxrSession* session, uxrObjectId object_id, uint16_t request_id,
                             uxrStreamId stream_id, struct ucdrBuffer* ub, void* args)
    {
        (void) session; (void) object_id; (void) request_id; (void) stream_id; (void) ub; (void) args;
    }
};

SessionTest* SessionTest::current = nullptr;

TEST_F(SessionTest, SetStatusCallback)
{
    int user_data;
    uxr_set_status_callback(&session, on_status_func, &user_data);
    EXPECT_EQ(session.on_status, on_status_func);
    EXPECT_EQ(session.on_status_args, &user_data);
}

TEST_F(SessionTest, SetTopicCallback)
{
    int user_data;
    uxr_set_topic_callback(&session, on_topic_func, &user_data);
    EXPECT_EQ(session.on_topic, on_topic_func);
    EXPECT_EQ(session.on_topic_args, &user_data);
}

TEST_F(SessionTest, CreateOk)
{
    bool created = uxr_create_session(&session);
    ASSERT_TRUE(created);
}

TEST_F(SessionTest, CreateNoOk)
{
    bool created = uxr_create_session(&session);
    ASSERT_FALSE(created);
}

TEST_F(SessionTest, DeleteOk)
{
    bool deleted = uxr_delete_session(&session);
    ASSERT_TRUE(deleted);
}

TEST_F(SessionTest, DeleteNoOk)
{
    bool deleted = uxr_delete_session(&session);
    ASSERT_FALSE(deleted);
}

TEST_F(SessionTest, RunTime)
{
    //TODO
}

TEST_F(SessionTest, RunUntilTimeout)
{
    //TODO
}

TEST_F(SessionTest, RunConfirmDelivery)
{
    //TODO
}

TEST_F(SessionTest, RunConfirmAllStatus)
{
    //TODO
}

TEST_F(SessionTest, RunConfirmOneStatus)
{
    //TODO
}

TEST_F(SessionTest, Flash)
{
    //TODO
}

// ****************************************************************************Y
//                                  MOCKS
// ****************************************************************************Y

int64_t uxr_millis(void)
{
    return 0;
}
