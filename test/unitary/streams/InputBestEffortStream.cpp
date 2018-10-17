#include <gtest/gtest.h>
#include <core/session/stream/input_best_effort_stream_internal.h>
#include <core/session/stream/seq_num_internal.h>
#include <ucdr/microcdr.h>

#define STREAM_BUFFER_SIZE 512
#define OFFSET 4

class InputBestEffortStreamTest : public testing::Test
{
public:
    InputBestEffortStreamTest()
    {
        uxr_init_input_best_effort_stream(&stream);
    }

    void reset()
    {
    }

    virtual ~InputBestEffortStreamTest()
    {
    }

protected:
    uxrInputBestEffortStream stream;
};

TEST_F(InputBestEffortStreamTest, ReadMessagePrevious)
{
    uxrSeqNum last_handled = stream.last_handled;
    bool ready_to_read = uxr_receive_best_effort_message(&stream, uxr_seq_num_sub(stream.last_handled, 1));
    ASSERT_FALSE(ready_to_read);
    ASSERT_EQ(stream.last_handled, last_handled);
}

TEST_F(InputBestEffortStreamTest, ReadMessageSame)
{
    uxrSeqNum last_handled = stream.last_handled;
    bool ready_to_read = uxr_receive_best_effort_message(&stream, stream.last_handled);
    ASSERT_FALSE(ready_to_read);
    ASSERT_EQ(stream.last_handled, last_handled);
}

TEST_F(InputBestEffortStreamTest, ReadMessageNext)
{
    uxrSeqNum next_seq_num = uxr_seq_num_add(stream.last_handled, 1);
    bool ready_to_read = uxr_receive_best_effort_message(&stream, next_seq_num);
    ASSERT_TRUE(ready_to_read);
    ASSERT_EQ(stream.last_handled, next_seq_num);
}

TEST_F(InputBestEffortStreamTest, ReadMessageSpacedNext)
{
    uxrSeqNum next_seq_num = uxr_seq_num_add(stream.last_handled, 10);
    bool ready_to_read = uxr_receive_best_effort_message(&stream, next_seq_num);
    ASSERT_TRUE(ready_to_read);
    ASSERT_EQ(stream.last_handled, next_seq_num);
}

TEST_F(InputBestEffortStreamTest, Reset)
{
    uxr_reset_input_best_effort_stream(&stream);
    ASSERT_EQ(stream.last_handled, UINT16_MAX);
}
