#include <gtest/gtest.h>

#define SUBMESSAGE_MAX 256
#define STREAM_BUFFER_SIZE 512
#define OFFSET 4

class OutputBestEffortStreamTest : public testing::Test
{
public:
    OutputBestEffortStreamTest()
    {
//        init_output_best_effort_stream(&stream, buffer, STREAM_BUFFER_SIZE, OFFSET);
    }

    virtual ~OutputBestEffortStreamTest()
    {
    }

protected:
    const std::string submessage = "This is a submessage";
    uint8_t buffer[STREAM_BUFFER_SIZE];
//    uxrOutputBestEffortStream stream;
};

TEST_F(OutputBestEffortStreamTest, WriteSubmessage)
{
    //TODO
}

/*TEST_F(OutputBestEffortStreamTest, WriteSubmessage)
{
    // Write
    ucdrBuffer mb;
    bool available_to_write = prepare_best_effort_buffer_to_write(&stream, submessage.size(), &mb);
    ucdr_serialize_array_char(&mb, submessage.c_str(), static_cast<uint16_t>(submessage.size()));

    // Send
    uxrSeqNum init_seq_num = stream.last_send;
    uint8_t* output_buffer; size_t length; uxrSeqNum seq_num;
    bool available_to_send = prepare_best_effort_buffer_to_send(&stream, &output_buffer, &length, &seq_num);

    // Check
    ASSERT_EQ(available_to_write, true);
    ASSERT_EQ(available_to_send, true);
    ASSERT_EQ(submessage.size(), length - OFFSET);
    ASSERT_EQ(init_seq_num, seq_num_sub(seq_num, 1));
    for(size_t i = 0; i < submessage.size(); i++)
    {
        ASSERT_EQ(submessage[i], output_buffer[OFFSET + i]);
    }
}*/

