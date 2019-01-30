
#include <gtest/gtest.h>

extern "C"
{
#include <c/core/session/stream/seq_num.c>
#include <c/core/session/stream/output_reliable_stream.c>
}

#define BUFFER_SIZE           128
#define HISTORY               4
#define OFFSET                8
#define MAX_MESSAGE_SIZE      (BUFFER_SIZE / HISTORY - INTERNAL_RELIABLE_BUFFER_OFFSET)
#define SUBMESSAGE_SIZE       8
#define MAX_SUBMESSAGE_SIZE   MAX_MESSAGE_SIZE - OFFSET
#define FRAGMENT_OFFSET       4

bool operator == (const uxrOutputReliableStream& stream1, const uxrOutputReliableStream& stream2)
{
    return stream1.buffer == stream2.buffer
        && stream1.size == stream2.size
        && stream1.history == stream2.history
        && stream1.offset == stream2.offset
        && stream1.last_written == stream2.last_written
        && stream1.last_sent == stream2.last_sent
        && stream1.last_acknown == stream2.last_acknown
        && stream1.next_heartbeat_timestamp == stream2.next_heartbeat_timestamp
        && stream1.next_heartbeat_tries == stream2.next_heartbeat_tries
        && stream1.send_lost == stream2.send_lost
        && stream1.on_new_fragment == stream2.on_new_fragment;
}

bool operator != (const uxrOutputReliableStream& stream1, const uxrOutputReliableStream& stream2)
{
    return !(stream1 == stream2);
}

class OutputReliableStreamTest : public testing::Test
{
public:
    OutputReliableStreamTest()
    {
        uxr_init_output_reliable_stream(&stream, buffer, BUFFER_SIZE, HISTORY, OFFSET, on_new_fragment);
        EXPECT_EQ(buffer, stream.buffer);
        EXPECT_EQ(BUFFER_SIZE, stream.size);
        EXPECT_EQ(HISTORY, stream.history);
        EXPECT_EQ(OFFSET, stream.offset);
        EXPECT_EQ(0, stream.last_written);
        EXPECT_EQ(SEQ_NUM_MAX, stream.last_sent);
        EXPECT_EQ(SEQ_NUM_MAX, stream.last_acknown);
        EXPECT_EQ(INT64_MAX, stream.next_heartbeat_timestamp);
        EXPECT_EQ(0, stream.next_heartbeat_tries);
        EXPECT_EQ(false, stream.send_lost);
        EXPECT_EQ(on_new_fragment, stream.on_new_fragment);

        for(size_t i = 0; i < HISTORY; ++i)
        {
            uint8_t* slot = uxr_get_reliable_buffer(buffer, BUFFER_SIZE, HISTORY, i);
            EXPECT_EQ(OFFSET, uxr_get_reliable_buffer_length(slot));
        }
    }

    void copy(uxrOutputReliableStream* dest, uxrOutputReliableStream* source)
    {
        dest->buffer = source->buffer;
        dest->size = source->size;
        dest->history = source->history;
        dest->offset = source->offset;

        dest->last_written = source->last_written;
        dest->last_sent = source->last_sent;
        dest->last_acknown = source->last_acknown;

        dest->next_heartbeat_timestamp = source->next_heartbeat_timestamp;
        dest->next_heartbeat_tries = source->next_heartbeat_tries;
        dest->send_lost = source->send_lost;

        dest->on_new_fragment = source->on_new_fragment;
    }

    virtual ~OutputReliableStreamTest()
    {
    }

protected:
    uxrOutputReliableStream stream;
    uint8_t buffer[BUFFER_SIZE];

    static void on_new_fragment(ucdrBuffer* ub, uxrOutputReliableStream* stream)
    {
        (void) stream;
        uint8_t fragment_header[FRAGMENT_OFFSET];
        ucdr_serialize_array_uint8_t(ub, fragment_header, FRAGMENT_OFFSET);
    }
};


TEST_F(OutputReliableStreamTest, OutputBuffer)
{
    uint8_t* buffer_from_generic = uxr_get_reliable_buffer(buffer, BUFFER_SIZE, HISTORY, 2);
    uint8_t* buffer_from_output = uxr_get_output_buffer(&stream, 2);
    EXPECT_EQ(buffer_from_generic, buffer_from_output);
}

TEST_F(OutputReliableStreamTest, OutputBufferSize)
{
    size_t size_from_generic = uxr_get_reliable_buffer_size(BUFFER_SIZE, HISTORY);
    size_t size_from_output = uxr_get_output_buffer_size(&stream);
    EXPECT_EQ(size_from_generic, size_from_output);
}

TEST_F(OutputReliableStreamTest, UpToDate)
{
    bool up_to_date = uxr_is_output_up_to_date(&stream);
    EXPECT_TRUE(up_to_date);
}

TEST_F(OutputReliableStreamTest, NotUpToDate)
{
    ucdrBuffer ub;
    (void) uxr_prepare_reliable_buffer_to_write(&stream, SUBMESSAGE_SIZE, FRAGMENT_OFFSET, &ub);
    uint8_t* message; size_t length; uxrSeqNum seq_num;
    (void) uxr_prepare_next_reliable_buffer_to_send(&stream, &message, &length, &seq_num);

    bool up_to_date = uxr_is_output_up_to_date(&stream);
    EXPECT_FALSE(up_to_date);
}

TEST_F(OutputReliableStreamTest, WriteOneMessageSameSlot)
{
    uint8_t* slot_0 = uxr_get_output_buffer(&stream, 0);

    ucdrBuffer ub;
    bool available_to_write = uxr_prepare_reliable_buffer_to_write(&stream, SUBMESSAGE_SIZE, FRAGMENT_OFFSET, &ub);
    ASSERT_TRUE(available_to_write);
    EXPECT_EQ(0, stream.last_written);
    EXPECT_EQ(OFFSET + SUBMESSAGE_SIZE, uxr_get_reliable_buffer_length(slot_0));
    EXPECT_EQ(slot_0, ub.init);
    EXPECT_EQ(slot_0 + OFFSET, ub.iterator);
    EXPECT_EQ(slot_0 + OFFSET + SUBMESSAGE_SIZE, ub.final);
}

TEST_F(OutputReliableStreamTest, WriteTwoMessagesSameSlot)
{
    uint8_t* slot_0 = uxr_get_output_buffer(&stream, 0);

    ucdrBuffer ub;
    bool available_to_write = uxr_prepare_reliable_buffer_to_write(&stream, SUBMESSAGE_SIZE, FRAGMENT_OFFSET, &ub);
    ASSERT_TRUE(available_to_write);
    EXPECT_EQ(0, stream.last_written);
    EXPECT_EQ(OFFSET + SUBMESSAGE_SIZE, uxr_get_reliable_buffer_length(slot_0));
    EXPECT_EQ(slot_0, ub.init);
    EXPECT_EQ(slot_0 + OFFSET, ub.iterator);
    EXPECT_EQ(slot_0 + OFFSET + SUBMESSAGE_SIZE, ub.final);

    available_to_write = uxr_prepare_reliable_buffer_to_write(&stream, SUBMESSAGE_SIZE, FRAGMENT_OFFSET, &ub);
    ASSERT_TRUE(available_to_write);
    EXPECT_EQ(0, stream.last_written);
    EXPECT_EQ(OFFSET + SUBMESSAGE_SIZE * 2, uxr_get_reliable_buffer_length(slot_0));
    EXPECT_EQ(slot_0, ub.init);
    EXPECT_EQ(slot_0 + OFFSET + SUBMESSAGE_SIZE, ub.iterator);
    EXPECT_EQ(slot_0 + OFFSET + SUBMESSAGE_SIZE * 2, ub.final);
}

TEST_F(OutputReliableStreamTest, WriteThreeMessagessTwoSlots)
{
    uint8_t* slot_0 = uxr_get_output_buffer(&stream, 0);
    uint8_t* slot_1 = uxr_get_output_buffer(&stream, 1);

    ucdrBuffer ub;
    bool available_to_write = uxr_prepare_reliable_buffer_to_write(&stream, SUBMESSAGE_SIZE, FRAGMENT_OFFSET, &ub);
    ASSERT_TRUE(available_to_write);
    EXPECT_EQ(0, stream.last_written);
    EXPECT_EQ(OFFSET + SUBMESSAGE_SIZE, uxr_get_reliable_buffer_length(slot_0));
    EXPECT_EQ(slot_0, ub.init);
    EXPECT_EQ(slot_0 + OFFSET, ub.iterator);
    EXPECT_EQ(slot_0 + OFFSET + SUBMESSAGE_SIZE, ub.final);

    available_to_write = uxr_prepare_reliable_buffer_to_write(&stream, SUBMESSAGE_SIZE, FRAGMENT_OFFSET, &ub);
    ASSERT_TRUE(available_to_write);
    EXPECT_EQ(0, stream.last_written);
    EXPECT_EQ(OFFSET + SUBMESSAGE_SIZE * 2, uxr_get_reliable_buffer_length(slot_0));
    EXPECT_EQ(slot_0, ub.init);
    EXPECT_EQ(slot_0 + OFFSET + SUBMESSAGE_SIZE, ub.iterator);
    EXPECT_EQ(slot_0 + OFFSET + SUBMESSAGE_SIZE * 2, ub.final);

    available_to_write = uxr_prepare_reliable_buffer_to_write(&stream, SUBMESSAGE_SIZE, FRAGMENT_OFFSET, &ub);
    ASSERT_TRUE(available_to_write);
    EXPECT_EQ(1, stream.last_written);
    EXPECT_EQ(OFFSET + SUBMESSAGE_SIZE * 2, uxr_get_reliable_buffer_length(slot_0));
    EXPECT_EQ(OFFSET + SUBMESSAGE_SIZE, uxr_get_reliable_buffer_length(slot_1));
    EXPECT_EQ(slot_1, ub.init);
    EXPECT_EQ(slot_1 + OFFSET, ub.iterator);
    EXPECT_EQ(slot_1 + OFFSET + SUBMESSAGE_SIZE, ub.final);
}

TEST_F(OutputReliableStreamTest, WriteFragmentMessage)
{
    size_t size = uxr_get_output_buffer_size(&stream);
    const size_t message_length = BUFFER_SIZE / HISTORY + SUBMESSAGE_SIZE;
    uint8_t* slot_0 = uxr_get_output_buffer(&stream, 0);
    uint8_t* slot_1 = uxr_get_output_buffer(&stream, 1);
    uint8_t* slot_2 = uxr_get_output_buffer(&stream, 2);

    uxrOutputReliableStream backup;
    copy(&backup, &stream);

    ucdrBuffer ub;
    bool available_to_write = uxr_prepare_reliable_buffer_to_write(&stream, message_length, FRAGMENT_OFFSET, &ub);
    ASSERT_TRUE(available_to_write);
    EXPECT_EQ(2, stream.last_written);
    EXPECT_EQ(size, uxr_get_reliable_buffer_length(slot_0));
    EXPECT_EQ(size, uxr_get_reliable_buffer_length(slot_1));
    EXPECT_EQ(OFFSET + FRAGMENT_OFFSET + SUBMESSAGE_SIZE, uxr_get_reliable_buffer_length(slot_2));
    EXPECT_EQ(slot_0, ub.init);
    EXPECT_EQ(slot_0 + OFFSET + FRAGMENT_OFFSET, ub.iterator);
    EXPECT_EQ(slot_0 + size, ub.final);
    EXPECT_EQ(on_full_output_buffer, ub.on_full_buffer);

    uint8_t message_to_write[message_length];
    EXPECT_TRUE(ucdr_serialize_array_uint8_t(&ub, message_to_write, message_length));
    EXPECT_EQ(slot_2, ub.init);
    EXPECT_EQ(slot_2 + OFFSET + FRAGMENT_OFFSET + SUBMESSAGE_SIZE, ub.iterator);
    EXPECT_EQ(slot_2 + OFFSET + FRAGMENT_OFFSET + SUBMESSAGE_SIZE, ub.final);
}

TEST_F(OutputReliableStreamTest, WriteMessagesUntilFullBuffer)
{
    ucdrBuffer ub;
    for(size_t i = 0; i < HISTORY; ++i)
    {
        bool available_to_write = uxr_prepare_reliable_buffer_to_write(&stream, MAX_SUBMESSAGE_SIZE, FRAGMENT_OFFSET, &ub);
        ASSERT_TRUE(available_to_write);
    }

    uxrOutputReliableStream backup;
    copy(&backup, &stream);
    bool available_to_write = uxr_prepare_reliable_buffer_to_write(&stream, MAX_SUBMESSAGE_SIZE, FRAGMENT_OFFSET, &ub);
    ASSERT_FALSE(available_to_write);
    EXPECT_EQ(backup, stream);

    for(size_t i = 0; i < HISTORY; ++i)
    {
        uint8_t* slot_i = uxr_get_output_buffer(&stream, i);
        EXPECT_EQ(MAX_MESSAGE_SIZE, uxr_get_reliable_buffer_length(slot_i));
    }
}

TEST_F(OutputReliableStreamTest, PrepareToSendOneMessage)
{
    ucdrBuffer ub;
    (void) uxr_prepare_reliable_buffer_to_write(&stream, SUBMESSAGE_SIZE, FRAGMENT_OFFSET, &ub);

    uint8_t* slot_0 = uxr_get_output_buffer(&stream, 0);
    uint8_t* message; size_t length; uxrSeqNum seq_num;
    bool data_to_send = uxr_prepare_next_reliable_buffer_to_send(&stream, &message, &length, &seq_num);
    ASSERT_TRUE(data_to_send);
    EXPECT_EQ(slot_0, message);
    EXPECT_EQ(uxr_get_reliable_buffer_length(slot_0), length);
    EXPECT_EQ(uxr_seq_num_add(SEQ_NUM_MAX, 1), seq_num);
}

TEST_F(OutputReliableStreamTest, PrepareToSendNoMessage)
{
    uint8_t* message; size_t length; uxrSeqNum seq_num;
    uxrOutputReliableStream backup;
    copy(&backup, &stream);
    bool data_to_send = uxr_prepare_next_reliable_buffer_to_send(&stream, &message, &length, &seq_num);
    ASSERT_FALSE(data_to_send);
    EXPECT_EQ(backup, stream);
}

TEST_F(OutputReliableStreamTest, PrepareToSendAllMessages)
{
    ucdrBuffer ub;
    for(size_t i = 0; i < HISTORY; ++i)
    {
        (void) uxr_prepare_reliable_buffer_to_write(&stream, MAX_SUBMESSAGE_SIZE, FRAGMENT_OFFSET, &ub);
    }

    for(size_t i = 0; i < HISTORY; ++i)
    {
        uint8_t* slot_i = uxr_get_output_buffer(&stream, i);
        uint8_t* message; size_t length; uxrSeqNum seq_num;
        bool data_to_send = uxr_prepare_next_reliable_buffer_to_send(&stream, &message, &length, &seq_num);
        ASSERT_TRUE(data_to_send);
        EXPECT_EQ(slot_i, message);
        EXPECT_EQ(uxr_get_reliable_buffer_length(slot_i), length);
        EXPECT_EQ(uxr_seq_num_add(SEQ_NUM_MAX, uint16_t(i + 1)), seq_num);
    }

    uxrOutputReliableStream backup;
    copy(&backup, &stream);
    uint8_t* message; size_t length; uxrSeqNum seq_num;
    bool data_to_send = uxr_prepare_next_reliable_buffer_to_send(&stream, &message, &length, &seq_num);
    ASSERT_FALSE(data_to_send);
    EXPECT_EQ(backup, stream);

    for(size_t i = 0; i < HISTORY; ++i)
    {
        uint8_t* slot_i = uxr_get_output_buffer(&stream, i);
        EXPECT_EQ(MAX_MESSAGE_SIZE, uxr_get_reliable_buffer_length(slot_i));
    }
}

TEST_F(OutputReliableStreamTest, HeartbeatWithUpToDate)
{
    uxrOutputReliableStream backup;
    copy(&backup, &stream);
    bool must_confirm = uxr_update_output_stream_heartbeat_timestamp(&stream, 0);
    ASSERT_FALSE(must_confirm);
    EXPECT_EQ(backup, stream);
}

TEST_F(OutputReliableStreamTest, HeartbeatFirstTry)
{
    ucdrBuffer ub;
    (void) uxr_prepare_reliable_buffer_to_write(&stream, SUBMESSAGE_SIZE, FRAGMENT_OFFSET, &ub);
    uint8_t* message; size_t length; uxrSeqNum seq_num;
    (void) uxr_prepare_next_reliable_buffer_to_send(&stream, &message, &length, &seq_num);

    bool must_confirm = uxr_update_output_stream_heartbeat_timestamp(&stream, 0);
    ASSERT_FALSE(must_confirm);
    EXPECT_EQ(MIN_HEARTBEAT_TIME_INTERVAL, stream.next_heartbeat_timestamp);
    EXPECT_EQ(1, stream.next_heartbeat_tries);
}

TEST_F(OutputReliableStreamTest, HeartbeatSuccessfulTry)
{
    ucdrBuffer ub;
    (void) uxr_prepare_reliable_buffer_to_write(&stream, SUBMESSAGE_SIZE, FRAGMENT_OFFSET, &ub);
    uint8_t* message; size_t length; uxrSeqNum seq_num;
    (void) uxr_prepare_next_reliable_buffer_to_send(&stream, &message, &length, &seq_num);
    (void) uxr_update_output_stream_heartbeat_timestamp(&stream, 0);

    bool must_confirm = uxr_update_output_stream_heartbeat_timestamp(&stream, MIN_HEARTBEAT_TIME_INTERVAL);
    ASSERT_TRUE(must_confirm);
    EXPECT_EQ(MIN_HEARTBEAT_TIME_INTERVAL * 3, stream.next_heartbeat_timestamp);
    EXPECT_EQ(2, stream.next_heartbeat_tries);
}

TEST_F(OutputReliableStreamTest, HeartbeatTwoSuccessfulTry)
{
    ucdrBuffer ub;
    (void) uxr_prepare_reliable_buffer_to_write(&stream, SUBMESSAGE_SIZE, FRAGMENT_OFFSET, &ub);
    uint8_t* message; size_t length; uxrSeqNum seq_num;
    (void) uxr_prepare_next_reliable_buffer_to_send(&stream, &message, &length, &seq_num);
    (void) uxr_update_output_stream_heartbeat_timestamp(&stream, 0);
    (void) uxr_update_output_stream_heartbeat_timestamp(&stream, MIN_HEARTBEAT_TIME_INTERVAL);

    bool must_confirm = uxr_update_output_stream_heartbeat_timestamp(&stream, MIN_HEARTBEAT_TIME_INTERVAL * 3);
    ASSERT_TRUE(must_confirm);
    EXPECT_EQ(MIN_HEARTBEAT_TIME_INTERVAL * 7, stream.next_heartbeat_timestamp);
    EXPECT_EQ(3, stream.next_heartbeat_tries);
}

TEST_F(OutputReliableStreamTest, HeartbeatUnsuccessfulSecondHeartbeat)
{
    ucdrBuffer ub;
    (void) uxr_prepare_reliable_buffer_to_write(&stream, SUBMESSAGE_SIZE, FRAGMENT_OFFSET, &ub);
    uint8_t* message; size_t length; uxrSeqNum seq_num;
    (void) uxr_prepare_next_reliable_buffer_to_send(&stream, &message, &length, &seq_num);
    (void) uxr_update_output_stream_heartbeat_timestamp(&stream, 0);
    (void) uxr_update_output_stream_heartbeat_timestamp(&stream, MIN_HEARTBEAT_TIME_INTERVAL * 3);

    bool must_confirm = uxr_update_output_stream_heartbeat_timestamp(&stream, MIN_HEARTBEAT_TIME_INTERVAL);
    ASSERT_FALSE(must_confirm);
    EXPECT_EQ(MIN_HEARTBEAT_TIME_INTERVAL * 3, stream.next_heartbeat_timestamp);
    EXPECT_EQ(2, stream.next_heartbeat_tries);
}

TEST_F(OutputReliableStreamTest, AcknackProcessNoLost)
{
    uint8_t* slot_0 = uxr_get_output_buffer(&stream, 0);
    ucdrBuffer ub;
    (void) uxr_prepare_reliable_buffer_to_write(&stream, SUBMESSAGE_SIZE, FRAGMENT_OFFSET, &ub);
    uint8_t* message; size_t length; uxrSeqNum seq_num;
    (void) uxr_prepare_next_reliable_buffer_to_send(&stream, &message, &length, &seq_num);
    (void) uxr_update_output_stream_heartbeat_timestamp(&stream, 0);

    uxr_process_acknack(&stream, 0, uxrSeqNum(1));
    EXPECT_FALSE(stream.send_lost);
    EXPECT_EQ(0, stream.last_acknown);
    EXPECT_EQ(0, stream.next_heartbeat_tries);
    EXPECT_EQ(OFFSET, uxr_get_reliable_buffer_length(slot_0));
}


TEST_F(OutputReliableStreamTest, AcknackProcessLost)
{
    uint8_t* slot_0 = uxr_get_output_buffer(&stream, 0);
    ucdrBuffer ub;
    (void) uxr_prepare_reliable_buffer_to_write(&stream, SUBMESSAGE_SIZE, FRAGMENT_OFFSET, &ub);
    uint8_t* message; size_t length; uxrSeqNum seq_num;
    (void) uxr_prepare_next_reliable_buffer_to_send(&stream, &message, &length, &seq_num);
    (void) uxr_update_output_stream_heartbeat_timestamp(&stream, 0);
    size_t message_length = uxr_get_reliable_buffer_length(slot_0);

    uxr_process_acknack(&stream, 1, uxrSeqNum(0));
    EXPECT_TRUE(stream.send_lost);
    EXPECT_EQ(SEQ_NUM_MAX, stream.last_acknown);
    EXPECT_EQ(0, stream.next_heartbeat_tries);
    EXPECT_EQ(message_length, uxr_get_reliable_buffer_length(slot_0));
}

TEST_F(OutputReliableStreamTest, SendMessageLostNoLost)
{
    uint8_t* lost_message; size_t lost_length; uxrSeqNum lost_seq_num_it;
    bool must_send = uxr_next_reliable_nack_buffer_to_send(&stream, &lost_message, &lost_length, &lost_seq_num_it);
    ASSERT_FALSE(must_send);
}

TEST_F(OutputReliableStreamTest, SendMessageLost)
{
    uint8_t* slot_0 = uxr_get_output_buffer(&stream, 0);
    ucdrBuffer ub;
    (void) uxr_prepare_reliable_buffer_to_write(&stream, SUBMESSAGE_SIZE, FRAGMENT_OFFSET, &ub);
    uint8_t* message; size_t length; uxrSeqNum seq_num;
    (void) uxr_prepare_next_reliable_buffer_to_send(&stream, &message, &length, &seq_num);
    (void) uxr_update_output_stream_heartbeat_timestamp(&stream, 0);
    uxr_process_acknack(&stream, 1, uxrSeqNum(0));
    //at this point, the lost has been confirmed

    uint8_t* lost_message; size_t lost_length;
    uxrSeqNum seq_num_it = uxr_begin_output_nack_buffer_it(&stream);
    bool must_send = uxr_next_reliable_nack_buffer_to_send(&stream, &lost_message, &lost_length, &seq_num_it);
    ASSERT_TRUE(must_send);
    EXPECT_EQ(slot_0, lost_message);
    EXPECT_EQ(OFFSET + SUBMESSAGE_SIZE, lost_length);
    EXPECT_EQ(0, seq_num_it);

    must_send = uxr_next_reliable_nack_buffer_to_send(&stream, &lost_message, &lost_length, &seq_num_it);
    ASSERT_FALSE(must_send);
}

TEST_F(OutputReliableStreamTest, FragmentedSerialization)
{
    uint8_t* slot_1 = uxr_get_output_buffer(&stream, 1);

    ucdrBuffer ub;
    uint8_t message[MAX_MESSAGE_SIZE + 4];
    (void) uxr_prepare_reliable_buffer_to_write(&stream, MAX_SUBMESSAGE_SIZE + 4, FRAGMENT_OFFSET, &ub);
    bool serialize = ucdr_serialize_array_uint8_t(&ub, message, MAX_SUBMESSAGE_SIZE + 4);

    ASSERT_TRUE(serialize);
    EXPECT_EQ(slot_1, ub.init);
    EXPECT_EQ(slot_1 + uxr_get_reliable_buffer_length(slot_1), ub.iterator);
    EXPECT_EQ(slot_1 + uxr_get_reliable_buffer_length(slot_1), ub.final);
    EXPECT_EQ(on_full_output_buffer, ub.on_full_buffer);
}

