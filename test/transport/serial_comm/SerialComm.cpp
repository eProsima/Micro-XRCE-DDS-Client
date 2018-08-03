#include "SerialComm.hpp"

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

SerialComm::SerialComm() : fd_(-1)
{
}

SerialComm::~SerialComm()
{
    (void) unlink("/tmp/serial_fifo");
}

int SerialComm::init()
{
    int rv = 0;

    (void) unlink("/tmp/serial_fifo");
    if (0 < mkfifo("/tmp/serial_fifo", S_IRWXU | S_IRWXG | S_IRWXO))
    {
        rv = -1;
    }
    else
    {
        fd_ = open("/tmp/serial_fifo", O_RDWR | O_NONBLOCK);
        if (0 < fd_)
        {
            fcntl(fd_, F_SETFL, O_NONBLOCK);
            fcntl(fd_, F_SETPIPE_SZ, 4096);
            if (!mr_init_serial_transport_fd(&master_, fd_, 1, 0) ||
                !mr_init_serial_transport_fd(&slave_, fd_, 0, 1))
            {
                rv = -1;
            }
        }
        else
        {
            rv = -1;
        }
    }

    return rv;
}

TEST_F(SerialComm, WorstStuffingTest)
{
    ASSERT_EQ(init(), 0);
    uint8_t output_msg[MR_SERIAL_MTU];
    uint8_t* input_msg;
    size_t input_msg_len;

    memset(output_msg, MR_FRAMING_END_FLAG, sizeof(output_msg));
    ASSERT_TRUE(master_.comm.send_msg(&master_, output_msg, sizeof(output_msg)));

    ASSERT_TRUE(slave_.comm.recv_msg(&slave_, &input_msg, &input_msg_len, 0));
    ASSERT_EQ(memcmp(output_msg, input_msg, sizeof(output_msg)), 0);
}

TEST_F(SerialComm, MessageOverflowTest)
{
    ASSERT_EQ(init(), 0);
    uint8_t output_msg[MR_SERIAL_MTU + 1] = {0};
    ASSERT_FALSE(master_.comm.send_msg(&master_, output_msg, sizeof(output_msg)));
}

TEST_F(SerialComm, BufferOverflowTest)
{
    ASSERT_EQ(init(), 0);

    uint8_t* input_msg;
    size_t input_msg_len;
    uint8_t overflow_msg[MR_SERIAL_BUFFER_SIZE + 1] = {0};
    uint8_t output_msg[] = {0, 0, 1, 0, 0, 0, 0};
    uint8_t flag = MR_FRAMING_END_FLAG;

    /* Send BEGIN flag. */
    ASSERT_EQ(write(slave_.poll_fd.fd, static_cast<void*>(&flag), 1), 1);

    /* Send overflow PAYLOAD. */
    ASSERT_EQ(write(slave_.poll_fd.fd, static_cast<void*>(&overflow_msg), sizeof(overflow_msg)), sizeof(overflow_msg));
    ASSERT_FALSE(slave_.comm.recv_msg(&slave_, &input_msg, &input_msg_len, 0));

    /* Send BEGIN flag. */
    ASSERT_EQ(write(slave_.poll_fd.fd, static_cast<void*>(&flag), 1), 1);

    /* Send PAYLOAD. */
    ASSERT_EQ(write(slave_.poll_fd.fd, static_cast<void*>(&output_msg), sizeof(output_msg)), sizeof(output_msg));
    ASSERT_FALSE(slave_.comm.recv_msg(&slave_, &input_msg, &input_msg_len, 0));

    /* Send END flag. */
    ASSERT_EQ(write(slave_.poll_fd.fd, static_cast<void*>(&flag), 1), 1);

    ASSERT_TRUE(slave_.comm.recv_msg(&slave_, &input_msg, &input_msg_len, 0));
    ASSERT_EQ(*input_msg, 0);
    ASSERT_EQ(input_msg_len, 1);
}

TEST_F(SerialComm, FatigueTest)
{
    ASSERT_EQ(init(), 0);
    unsigned int msgs_size = 2048;
    unsigned int sent_counter = 0;
    unsigned int recv_counter = 0;
    uint8_t receiver_ratio = 8;
    uint8_t output_msg[MR_SERIAL_MTU];
    uint8_t* input_msg;
    size_t input_msg_len;

    for (size_t i = 0; i < sizeof(output_msg); ++i)
    {
        output_msg[i] = uint8_t(i % (std::numeric_limits<uint8_t>::max() + 1));
    }

    for (unsigned int i = 0; i < msgs_size; ++i)
    {
        if (master_.comm.send_msg(&master_, output_msg, sizeof(output_msg)))
        {
            ++sent_counter;
        }

        if (i % receiver_ratio == 0)
        {
            if (slave_.comm.recv_msg(&slave_, &input_msg, &input_msg_len, 0))
            {
                ++recv_counter;
            }
        }
    }

    while (slave_.comm.recv_msg(&slave_, &input_msg, &input_msg_len, 0))
    {
        ++recv_counter;
    }

    ASSERT_EQ(recv_counter, sent_counter);
}

TEST_F(SerialComm, SplitMessageTest)
{
    ASSERT_EQ(init(), 0);

    uint8_t* input_msg;
    size_t input_msg_len;
    uint8_t output_msg[] = {0, 0, 1, 0, 0, 0, 0};
    uint8_t flag = MR_FRAMING_END_FLAG;

    /* Send BEGIN flag. */
    ASSERT_EQ(write(slave_.poll_fd.fd, static_cast<void*>(&flag), 1), 1);

    /* Send PAYLOAD. */
    for (unsigned int i = 0; i < sizeof(output_msg); ++i)
    {
        ASSERT_EQ(write(slave_.poll_fd.fd, static_cast<void*>(&output_msg[i]), 1), 1);
        ASSERT_FALSE(slave_.comm.recv_msg(&slave_, &input_msg, &input_msg_len, 0));
    }

    /* Send END flag. */
    ASSERT_EQ(write(slave_.poll_fd.fd, static_cast<void*>(&flag), 1), 1);

    ASSERT_TRUE(slave_.comm.recv_msg(&slave_, &input_msg, &input_msg_len, 0));
    ASSERT_EQ(*input_msg, 0);
    ASSERT_EQ(input_msg_len, 1);
}

