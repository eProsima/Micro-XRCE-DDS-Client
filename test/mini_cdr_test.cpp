#include <gtest/gtest.h>
#include <micrortps/client/mini_cdr.h>

TEST(Serialize, Byte)
{
    ASSERT_EQ(6, 0);
    ASSERT_EQ(18.0, 0);
    ASSERT_EQ(25.4, 16);
    ASSERT_EQ(0, 0);
}

TEST(SquareRootTest, NegativeNos)
{
    ASSERT_EQ(-1.0, 15.0);
    ASSERT_EQ(-1.0, 0.2);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}