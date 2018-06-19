#include <micrortps/client/profile/transport/uart_transport_linux.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

intmax_t read_uart_data(void* transport, uint8_t* buf, size_t len);

int init_uart_transport(UARTTransport* transport, const char* device, const uint8_t addr)
{
    int result = 0;

    // Open device.
    transport->addr = addr;
    transport->fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (transport->fd != -1)
    {
        // Init SerialIO
        init_serial_io(&transport->serial_io, addr);

        // Send init flag.
        uint8_t flag = MICRORTPS_FRAMING_FLAG;
        result = write(transport->fd, &flag, 1);
    }
    else
    {
        result = -1;
    }
    return result;
}

intmax_t send_uart_msg(UARTTransport* transport, const uint8_t* buf, size_t len)
{
    intmax_t result = write_serial_msg(&transport->serial_io, buf, len);
    if (0 < result)
    {
        intmax_t sent_size = write(transport->fd, transport->serial_io.output.buffer, (size_t)result);
        if (sent_size != result)
        {
            result = (0 < sent_size) ? -EMSGSIZE : -errno;
        }
    }
    return result;
}

intmax_t recv_uart_msg(UARTTransport* transport, uint8_t** buf, size_t* len, int timeout)
{
    (void)timeout;
    intmax_t result = read_serial_msg(&transport->serial_io,
                                      read_uart_data,
                                      (void*)transport,
                                      transport->buffer,
                                      sizeof(transport->buffer));
    if (0 < result)
    {
        *len = result;
        *buf = transport->buffer;
    }
    else
    {
        *len = 0;
        *buf = NULL;
    }
    return result;
}

intmax_t read_uart_data(void* transport, uint8_t* buf, size_t len)
{
    UARTTransport* uart_transport = (UARTTransport*)transport;
    intmax_t result = read(uart_transport->fd, buf, len);
    return (0 < result) ? result : -errno;
}


