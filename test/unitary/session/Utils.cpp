#include <gtest/gtest.h>

extern "C"
{
#include <c/util/time_internal.h>
}

TEST(UtilsTest, ConvertToNanos)
{
    uint64_t nanos = uxr_convert_to_nanos(16, 42);
    EXPECT_EQ(16000000042, nanos);
}

