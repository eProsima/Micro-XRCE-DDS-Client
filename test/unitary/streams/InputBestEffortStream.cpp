#include <gtest/gtest.h>
#include <micrortps/client/core/session/input_best_effort_stream.h>
#include <microcdr/microcdr.h>

#define STREAM_BUFFER_SIZE 512
#define OFFSET 4

class InputBestEffortStreamTest : public testing::Test
{
public:
    InputBestEffortStreamTest()
    {
        init_input_best_effort_stream(&stream);
    }

    void reset()
    {
    }

    virtual ~InputBestEffortStreamTest()
    {
    }

protected:
    mrInputBestEffortStream stream;
};

TEST_F(InputBestEffortStreamTest, ReadMessagePrevious)
{
    mrSeqNum next_seq_num = seq_num_sub(stream.last_handled, 1);
    bool ready_to_read = receive_best_effort_message(&stream, next_seq_num);
    ASSERT_TRUE(ready_to_read);
    ASSERT_EQ(stream.last_handled, next_seq_num);
}

TEST_F(InputBestEffortStreamTest, ReadMessageSame)
{
    mrSeqNum next_seq_num = stream.last_handled;
    bool ready_to_read = receive_best_effort_message(&stream, next_seq_num);
    ASSERT_TRUE(ready_to_read);
    ASSERT_EQ(stream.last_handled, next_seq_num);
}

TEST_F(InputBestEffortStreamTest, ReadMessageNext)
{
    mrSeqNum next_seq_num = seq_num_add(stream.last_handled, 1);
    bool ready_to_read = receive_best_effort_message(&stream, next_seq_num);
    ASSERT_TRUE(ready_to_read);
    ASSERT_EQ(stream.last_handled, next_seq_num);
}

TEST_F(InputBestEffortStreamTest, ReadMessageSpacedNext)
{
    mrSeqNum next_seq_num = seq_num_add(stream.last_handled, 10);
    bool ready_to_read = receive_best_effort_message(&stream, next_seq_num);
    ASSERT_TRUE(ready_to_read);
    ASSERT_EQ(stream.last_handled, next_seq_num);
}

TEST_F(InputBestEffortStreamTest, Reset)
{
    reset_input_best_effort_stream(&stream);
    ASSERT_EQ(stream.last_handled, UINT16_MAX);
}
