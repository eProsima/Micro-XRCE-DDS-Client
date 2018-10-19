#include <gtest/gtest.h>
#include <core/session/stream/output_best_effort_stream_internal.h>
#include <core/session/stream/seq_num_internal.h>
#include <ucdr/microcdr.h>

#define STREAM_BUFFER_SIZE 512
#define OFFSET size_t(4)

class OutputBestEffortStreamTest : public testing::Test
{
public:
    OutputBestEffortStreamTest()
    : submessage(STREAM_BUFFER_SIZE / 8, 'A')
    {
        uxr_init_output_best_effort_stream(&stream, buffer, STREAM_BUFFER_SIZE, OFFSET);
    }

    void write_submessage(bool expected)
    {
        size_t previous_writer = stream.writer;

        ucdrBuffer mb;
        bool available_to_write = uxr_prepare_best_effort_buffer_to_write(&stream, submessage.size(), &mb);
        if(!expected)
        {
            ASSERT_FALSE(available_to_write);
            ASSERT_EQ(previous_writer, stream.writer);
            return;
        }

        bool serialized = ucdr_serialize_array_char(&mb, submessage.c_str(), static_cast<uint16_t>(submessage.size()));

        ASSERT_TRUE(available_to_write);
        ASSERT_TRUE(serialized);
        ASSERT_EQ(previous_writer + submessage.size(), stream.writer);
        for(size_t i = 0; i < submessage.size(); i++)
        {
            ASSERT_EQ(submessage[i], buffer[previous_writer + i]);
        }
    }

    void send_message()
    {
        uxrSeqNum previous_seq_num = stream.last_send;
        size_t previous_writer = stream.writer;

        uint8_t* output_buffer; size_t length; uxrSeqNum seq_num;
        bool available_to_send = uxr_prepare_best_effort_buffer_to_send(&stream, &output_buffer, &length, &seq_num);

        ASSERT_EQ(previous_seq_num, uxr_seq_num_sub(seq_num, 1));
        ASSERT_EQ(previous_writer, length);
        ASSERT_EQ(stream.writer, stream.offset);
        ASSERT_TRUE(available_to_send);
    }

    void write_submessages_all(bool inside)
    {
        while(stream.writer + submessage.size() < stream.size)
        {
            write_submessage(true);
        }

        if(!inside)
        {
            write_submessage(false);
        }
    }

    void reset()
    {
        uxr_reset_output_best_effort_stream(&stream);
        ASSERT_EQ(stream.writer, OFFSET);
        ASSERT_EQ(stream.last_send, UINT16_MAX);
    }

    virtual ~OutputBestEffortStreamTest()
    {
    }

protected:
    const std::string submessage;
    uint8_t buffer[STREAM_BUFFER_SIZE];
    uxrOutputBestEffortStream stream;
};

TEST_F(OutputBestEffortStreamTest, WriteOneSubmessage)
{
    write_submessage(true);
}

TEST_F(OutputBestEffortStreamTest, WriteSubmessageFull)
{
    write_submessages_all(true);
}

TEST_F(OutputBestEffortStreamTest, WriteSubmessageError)
{
    write_submessages_all(false);
}

TEST_F(OutputBestEffortStreamTest, SendOneSubmessage)
{
    write_submessage(true);
    send_message();
}

TEST_F(OutputBestEffortStreamTest, SendSubmessageFull)
{
    write_submessages_all(true);
    send_message();
}

TEST_F(OutputBestEffortStreamTest, SendSubmessageError)
{
    write_submessages_all(false);
    send_message();
}

TEST_F(OutputBestEffortStreamTest, Reset)
{
    write_submessages_all(true);
    send_message();
    reset();
}
