#include <gtest/gtest.h>
#include <micrortps/client/core/session/output_best_effort_stream.h>

class OutputBestEffortStreamTest : public testing::Test
{
public:
    OutputBestEffortStreamTest();
    virtual ~OutputBestEffortStreamTest();

protected:
};

OutputBestEffortStreamTest::OutputBestEffortStreamTest()
{

}

OutputBestEffortStreamTest::~OutputBestEffortStreamTest()
{

}

TEST_F(OutputBestEffortStreamTest, WriteSubmessage)
{
    std::cout << "ASD" << std::endl;
}

