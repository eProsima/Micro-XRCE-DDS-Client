#include <gtest/gtest.h>
#include <core/session/stream/output_reliable_stream_internal.h>
#include <ucdr/microcdr.h>

#define STREAM_HISTORY     4
#define STREAM_BUFFER_SIZE 512 * STREAM_HISTORY
#define OFFSET 4

class OutputReliableStreamTest : public testing::Test
{
public:
    OutputReliableStreamTest()
        : submessage(STREAM_BUFFER_SIZE / (8 * STREAM_HISTORY), 'A')
    {
        uxr_init_output_reliable_stream(&stream, buffer, STREAM_BUFFER_SIZE, STREAM_HISTORY, OFFSET);
    }

    void write_submessage(bool expected)
    {
        size_t previous_writer = uxr_get_output_buffer_length(buffer + 20);

        ucdrBuffer mb;
        bool available_to_write = uxr_prepare_reliable_buffer_to_write(&stream, submessage.size(), &mb);
        bool serialized = ucdr_serialize_array_char(&mb, submessage.c_str(), static_cast<uint16_t>(submessage.size()));

        if(expected)
        {
            ASSERT_TRUE(available_to_write);
            ASSERT_TRUE(serialized);
            ASSERT_EQ(previous_writer + submessage.size(), stream.writer);
            for(size_t i = 0; i < submessage.size(); i++)
            {
                ASSERT_EQ(submessage[i], buffer[previous_writer + i]);
            }
        }
        else
        {
            ASSERT_FALSE(available_to_write);
            ASSERT_FALSE(serialized);
            ASSERT_EQ(previous_writer, stream.writer);
        }
    }
    void send_message()
    {
        /*mrSeqNum previous_seq_num = stream.last_send;
        size_t previous_writer = stream.writer;

        uint8_t* output_buffer; size_t length; mrSeqNum seq_num;
        bool available_to_send = prepare_best_effort_buffer_to_send(&stream, &output_buffer, &length, &seq_num);

        ASSERT_EQ(previous_seq_num, seq_num_sub(seq_num, 1));
        ASSERT_EQ(previous_writer, length);
        ASSERT_EQ(stream.writer, stream.offset);
        ASSERT_TRUE(available_to_send); */
    }

    void write_submessages_all(bool inside)
    {
        /*while(stream.writer + submessage.size() < stream.size)
        {
            write_submessage(true);
        }

        if(!inside)
        {
            write_submessage(false);
        }*/
    }

    void reset()
    {
        /*reset_output_reliable_stream(&stream);
        ASSERT_EQ(stream.writer, OFFSET);
        ASSERT_EQ(stream.last_send, UINT16_MAX);*/
    }

    virtual ~OutputReliableStreamTest()
    {
    }

protected:
    const std::string submessage;
    uint8_t buffer[STREAM_BUFFER_SIZE];
    uxrOutputReliableStream stream;
};

TEST_F(OutputReliableStreamTest, WriteOneSubmessage)
{
    write_submessage(true);
}

TEST_F(OutputReliableStreamTest, WriteSubmessageFull)
{
    write_submessages_all(true);
}

TEST_F(OutputReliableStreamTest, WriteSubmessageError)
{
    write_submessages_all(false);
}

TEST_F(OutputReliableStreamTest, SendOneSubmessage)
{
    write_submessage(true);
    send_message();
}

TEST_F(OutputReliableStreamTest, SendSubmessageFull)
{
    write_submessages_all(true);
    send_message();
}

TEST_F(OutputReliableStreamTest, SendSubmessageError)
{
    write_submessages_all(false);
    send_message();
}

TEST_F(OutputReliableStreamTest, Reset)
{
    write_submessages_all(true);
    send_message();
    reset();
}
