
#include <gtest/gtest.h>

extern "C"
{
#include "../src/c/core/session/stream/seq_num.c"
#include "../src/c/core/session/stream/input_reliable_stream.c"
}

#define BUFFER_SIZE 128
#define HISTORY     4
#define OFFSET      8

class InputReliableStreamTest : public testing::Test
{
public:
    InputReliableStreamTest()
    {
        uxr_init_input_reliable_stream(&stream, buffer, BUFFER_SIZE, HISTORY, on_get_fragmentation_info);
        EXPECT_EQ(SEQ_NUM_MAX, stream.last_handled);
        EXPECT_EQ(SEQ_NUM_MAX, stream.last_announced);
        EXPECT_EQ(buffer, stream.buffer);
        EXPECT_EQ(BUFFER_SIZE, stream.size);
        EXPECT_EQ(HISTORY, stream.history);
        //checks internal buffers
    }

    virtual ~InputReliableStreamTest()
    {
    }

protected:
    uxrInputReliableStream stream;
    uint8_t buffer[BUFFER_SIZE];

    static FragmentationInfo on_get_fragmentation_info(uint8_t* buffer)
    {
        (void) buffer;
        return NO_FRAGMENTED;;
    }
};

TEST_F(InputReliableStreamTest, ReceiveExpected)
{
}

