#include <SerialComm.hpp>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

SerialComm::SerialComm() :
    fds{0},
    master{0},
    slave{0}
{
    /*
     * Init pipes.
     */
    pipe(fds);
    fcntl(fds[0], F_SETFL, O_NONBLOCK);
    fcntl(fds[1], F_SETFL, O_NONBLOCK);
    fcntl(fds[1], F_SETPIPE_SZ, 4096);

    /*
     * Init master's transport.
     */
    master.fd = fds[1];
    init_serial_io(&master.serial_io, 0x00);
    uint8_t flag = MICRORTPS_FRAMING_FLAG;
    write(master.fd, &flag, 1);

    /*
     * Init slave's transport.
     */
    slave.fd = fds[0];
    init_serial_io(&slave.serial_io, 0x01);
}

TEST_F(SerialComm, WorstStuffingTest)
{
    uint8_t output_msg[MICRORTPS_SERIAL_MTU];
    uint8_t* input_msg;
    size_t input_msg_len;

    memset(output_msg, MICRORTPS_FRAMING_FLAG, sizeof(output_msg));
    ASSERT_GT(send_uart_msg(&master, output_msg, sizeof(output_msg)), 0);

    ASSERT_GT(recv_uart_msg(&slave, &input_msg, &input_msg_len, 0), 0);
    ASSERT_EQ(memcmp(output_msg, input_msg, sizeof(output_msg)), 0);
}

TEST_F(SerialComm, MessageOverflowTest)
{
    uint8_t output_msg[MICRORTPS_SERIAL_MTU + 1] = {0};
    ASSERT_LT(send_uart_msg(&master, output_msg, sizeof(output_msg)), 0);
}

TEST_F(SerialComm, FatigueTest)
{
    unsigned int msgs_size = 2048;
    unsigned int sent_counter = 0;
    unsigned int recv_counter = 0;
    uint8_t receiver_ratio = 8;
    uint8_t output_msg[MICRORTPS_SERIAL_MTU];
    uint8_t* input_msg;
    size_t input_msg_len;

    for (size_t i = 0; i < sizeof(output_msg); ++i)
    {
        output_msg[i] = i % sizeof(char);
    }

    for (unsigned int i = 0; i < msgs_size; ++i)
    {
        if (0 < send_uart_msg(&master, output_msg, sizeof(output_msg)))
        {
            ++sent_counter;
        }

        if (i % receiver_ratio == 0)
        {
            if (0 < recv_uart_msg(&slave, &input_msg, &input_msg_len, 0))
            {
                ++recv_counter;
            }
        }
    }

    while (0 < recv_uart_msg(&slave, &input_msg, &input_msg_len, 0))
    {
        ++recv_counter;
    }

    ASSERT_EQ(recv_counter, sent_counter);
}
