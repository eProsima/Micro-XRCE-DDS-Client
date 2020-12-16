#include "CustomComm.hpp"

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

CustomComm::CustomComm()
{
    buffer = new CircularBuffer(20);
    uxr_set_custom_transport_callbacks(&master_, true, open, close, write, read);
    uxr_init_custom_transport(&master_, this);

    uxr_set_custom_transport_callbacks(&slave_, true, open, close, write, read);
    uxr_init_custom_transport(&slave_, this);
}

CustomComm::~CustomComm()
{
}

bool CustomComm::open(uxrCustomTransport* transport)
{   
    (void) transport;
    return true;
}   

bool CustomComm::close(uxrCustomTransport* transport)
{
    (void) transport;
    return true;
}

size_t CustomComm::write(uxrCustomTransport* transport, const uint8_t* buf, size_t len, uint8_t* error)
{
    (void) error;
    CustomComm* custom_comm = static_cast<CustomComm*>(transport->args);

    size_t written = 0;
    while (written < len){
        if (0 == custom_comm->buffer->write(buf[written])){
            written++;
        }else{
            break;
        }
    }
    return written;
}

size_t CustomComm::read(uxrCustomTransport* transport, uint8_t* buf, size_t len, int timeout, uint8_t* error)
{
    (void) error;
    (void) timeout;
    CustomComm* custom_comm = static_cast<CustomComm*>(transport->args);

    size_t readed = 0;
    while (readed < len){
        if (0 == custom_comm->buffer->read(&buf[readed])){
            readed++;
        }else{
            break;
        }
    }

    return readed;
}

TEST_F(CustomComm, SingleWriteReadTest)
{
    uint8_t output_msg[3] = {11, 11, 89};
    uint8_t* input_msg;
    size_t input_msg_len;

    /* Send message. */
    ASSERT_TRUE(master_.comm.send_msg(&master_, output_msg, sizeof(output_msg)));

    /* Receive message. */
    ASSERT_TRUE(slave_.comm.recv_msg(&slave_, &input_msg, &input_msg_len, 10));
    ASSERT_EQ(memcmp(output_msg, input_msg, sizeof(output_msg)), 0);
    ASSERT_FALSE(slave_.comm.recv_msg(&slave_, &input_msg, &input_msg_len, 10));
}

TEST_F(CustomComm, MultiWriteReadTest)
{
    uint16_t msgs_size = 1;
    uint8_t output_msg[3] = {11, 11, 89};
    uint8_t* input_msg;
    size_t input_msg_len;

    /* Send messages. */
    for (int i = 0; i < msgs_size; ++i)
    {
        ASSERT_TRUE(master_.comm.send_msg(&master_, output_msg, sizeof(output_msg)));
    }

    /* Receive messages. */
    for (int i = 0; i < msgs_size; ++i)
    {
        ASSERT_TRUE(slave_.comm.recv_msg(&slave_, &input_msg, &input_msg_len, 10));
        ASSERT_EQ(memcmp(output_msg, input_msg, sizeof(output_msg)), 0);
    }
    ASSERT_FALSE(slave_.comm.recv_msg(&slave_, &input_msg, &input_msg_len, 10));
}

TEST_F(CustomComm, SingleOctetTest)
{
    uint8_t output_msg = 0;
    uint8_t* input_msg;
    size_t input_msg_len;

    /* Send messages. */
    ASSERT_TRUE(master_.comm.send_msg(&master_, &output_msg, 1));

    /* Receive message. */
    ASSERT_TRUE(slave_.comm.recv_msg(&slave_, &input_msg, &input_msg_len, 10));
    ASSERT_EQ(input_msg_len, size_t(1));
    ASSERT_EQ(*input_msg, 0);
}

TEST_F(CustomComm, NoPayloadTest)
{
    uint8_t output_msg = 0;

    /* Send messages. */
    ASSERT_FALSE(master_.comm.send_msg(&master_, &output_msg, 0));
}


TEST_F(CustomComm, WorstStuffingTest)
{
    uint8_t output_msg[UXR_CONFIG_SERIAL_TRANSPORT_MTU];
    uint8_t* input_msg;
    size_t input_msg_len;

    memset(output_msg, UXR_FRAMING_BEGIN_FLAG, sizeof(output_msg));
    ASSERT_TRUE(master_.comm.send_msg(&master_, output_msg, sizeof(output_msg)));

    ASSERT_TRUE(slave_.comm.recv_msg(&slave_, &input_msg, &input_msg_len, 100));
    ASSERT_EQ(memcmp(output_msg, input_msg, sizeof(output_msg)), 0);
}

TEST_F(CustomComm, FatigueTest)
{
    uint16_t msgs_size = uint16_t(~0);
    uint16_t sent_counter = 0;
    uint16_t recv_counter = 0;
    uint8_t receiver_ratio = 8;
    uint8_t output_msg[UXR_CONFIG_SERIAL_TRANSPORT_MTU];
    uint8_t* input_msg;
    size_t input_msg_len;

    for (size_t i = 0; i < sizeof(output_msg); ++i)
    {
        output_msg[i] = uint8_t(i % (std::numeric_limits<uint8_t>::max() + 1));
    }

    for (uint16_t i = 0; i < msgs_size; ++i)
    {
        size_t output_len = (i % sizeof(output_msg));
        if (master_.comm.send_msg(&master_, output_msg, output_len))
        {
            ++sent_counter;
        }

        if (i % receiver_ratio == 0)
        {
            if (slave_.comm.recv_msg(&slave_, &input_msg, &input_msg_len, 10))
            {
                ++recv_counter;
            }
        }
    }

    while (slave_.comm.recv_msg(&slave_, &input_msg, &input_msg_len, 10))
    {
        ++recv_counter;
    }

    ASSERT_EQ(recv_counter, sent_counter);
}

// TEST_F(SerialComm, SplitMessageTest)
// {
//     ASSERT_EQ(init(), 0);

//     uint8_t* input_msg;
//     size_t input_msg_len;
//     uint8_t output_msg[] = {0, 1, 1, 0, 0, 0, 0};
//     uint8_t flag = UXR_FRAMING_BEGIN_FLAG;

//     /* Send BEGIN flag. */
//     ASSERT_EQ(write(slave_.platform->poll_fd.fd, static_cast<void*>(&flag), 1), 1);

//     /* Send PAYLOAD. */
//     for (size_t i = 0; i < sizeof(output_msg); ++i)
//     {
//         ASSERT_FALSE(slave_.comm.recv_msg(&slave_, &input_msg, &input_msg_len, 10));
//         ASSERT_EQ(write(slave_.platform->poll_fd.fd, static_cast<void*>(&output_msg[i]), 1), 1);
//     }

//     ASSERT_TRUE(slave_.comm.recv_msg(&slave_, &input_msg, &input_msg_len, 10));
//     ASSERT_EQ(input_msg_len, size_t(1));
//     ASSERT_EQ(*input_msg, 0);
// }

// TEST_F(SerialComm, NoiseTest)
// {
//     ASSERT_EQ(init(), 0);

//     uint8_t* input_msg;
//     size_t input_msg_len;
//     uint8_t output_msg[3] = {11, 11, 89};
//     uint8_t tmp_buffer[100];

//     ASSERT_TRUE(master_.comm.send_msg(&master_, output_msg, sizeof(output_msg)));
//     ssize_t bytes_read = read(fd_, tmp_buffer, sizeof(tmp_buffer));
//     ASSERT_GT(bytes_read, 0);
//     ssize_t bytes_written = write(fd_, tmp_buffer, size_t(bytes_read));
//     ASSERT_EQ(bytes_read, bytes_written);
//     ASSERT_TRUE(slave_.comm.recv_msg(&slave_, &input_msg, &input_msg_len, 10));

//     /* Write/Read WITH noise message. */
//     ASSERT_TRUE(master_.comm.send_msg(&master_, output_msg, sizeof(output_msg)));
//     bytes_read = read(fd_, tmp_buffer, sizeof(tmp_buffer));
//     ASSERT_GT(bytes_read, 0);
//     tmp_buffer[4] = uint8_t(tmp_buffer[4] + 1);
//     bytes_written = write(fd_, tmp_buffer, size_t(bytes_read));
//     ASSERT_EQ(bytes_read, bytes_written);
//     ASSERT_FALSE(slave_.comm.recv_msg(&slave_, &input_msg, &input_msg_len, 10));
// }

