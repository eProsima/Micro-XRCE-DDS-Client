
#include <gtest/gtest.h>

extern "C"
{
#include "../src/c/core/session/stream/seq_num.c"
#include "../src/c/core/session/stream/input_best_effort_stream.c"
}

TEST(InputBestEffortStreamTest, Init)
{
    uxrInputBestEffortStream stream;
    uxr_init_input_best_effort_stream(&stream);
    EXPECT_EQ(SEQ_NUM_MAX, stream.last_handled);
}

TEST(InputBestEffortStreamTest, Reset)
{
    uxrInputBestEffortStream stream;
    uxr_init_input_best_effort_stream(&stream);
    uxr_reset_input_best_effort_stream(&stream);
    EXPECT_EQ(SEQ_NUM_MAX, stream.last_handled);
}

TEST(InputBestEffortStreamTest, ReceiveExpected)
{
    uxrInputBestEffortStream stream;
    uxr_init_input_best_effort_stream(&stream);
    uxrSeqNum expected_seq_num = uxr_seq_num_add(stream.last_handled, 1);
    bool available_to_read = uxr_receive_best_effort_message(&stream, expected_seq_num);
    EXPECT_TRUE(available_to_read);
    EXPECT_EQ(expected_seq_num, stream.last_handled);
}

TEST(InputBestEffortStreamTest, ReceiveNoExpectedLost)
{
    uxrInputBestEffortStream stream;
    uxr_init_input_best_effort_stream(&stream);
    uxrSeqNum expected_seq_num = uxr_seq_num_add(stream.last_handled, 2);
    bool available_to_read = uxr_receive_best_effort_message(&stream, expected_seq_num);
    EXPECT_TRUE(available_to_read);
    EXPECT_EQ(expected_seq_num, stream.last_handled);
}

TEST(InputBestEffortStreamTest, ReceiveNoExpected)
{
    uxrInputBestEffortStream stream;
    uxr_init_input_best_effort_stream(&stream);
    uxrSeqNum expected_seq_num = stream.last_handled;
    bool available_to_read = uxr_receive_best_effort_message(&stream, expected_seq_num);
    EXPECT_FALSE(available_to_read);
    EXPECT_EQ(expected_seq_num, stream.last_handled);
}
