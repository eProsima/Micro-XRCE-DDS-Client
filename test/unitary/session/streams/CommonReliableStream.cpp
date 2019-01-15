#include <gtest/gtest.h>

#include "../src/c/core/session/stream/common_reliable_stream_internal.h"

#define BUFFER_SIZE 512
#define HISTORY     8

TEST(CommonReliableStreamTest, GetBufferSize)
{
    size_t buffer_size = uxr_get_reliable_buffer_size(BUFFER_SIZE, HISTORY);
    EXPECT_EQ(BUFFER_SIZE / HISTORY - INTERNAL_RELIABLE_BUFFER_OFFSET, buffer_size);
}

TEST(CommonReliableStreamTest, GetBuffer)
{
    size_t history_pos = 3;
    uint8_t buffer[BUFFER_SIZE];
    uint8_t* slot = uxr_get_reliable_buffer(buffer, BUFFER_SIZE, HISTORY, history_pos);

    size_t slot_size = BUFFER_SIZE / HISTORY;
    EXPECT_EQ(history_pos * slot_size + INTERNAL_RELIABLE_BUFFER_OFFSET, slot - buffer);
}

TEST(CommonReliableStreamTest, SetGetBufferLength)
{
    size_t input_length = 0xFFFF;
    uint8_t buffer[BUFFER_SIZE];
    uint8_t* slot = buffer + INTERNAL_RELIABLE_BUFFER_OFFSET;
    uxr_set_reliable_buffer_length(slot, input_length);
    size_t output_length = uxr_get_reliable_buffer_length(slot);
    EXPECT_EQ(input_length, output_length);
}

