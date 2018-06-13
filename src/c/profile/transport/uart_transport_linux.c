#include <micrortps/client/profile/transport/uart_transport_linux.h>
#include <micrortps/client/communication/serial_protocol.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

int init_uart_transport(UARTTransport* transport, const char* device, const uint8_t addr)
{
    transport->addr = addr;
    transport->fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    return (transport->fd != -1) ? 0 : -errno;
}

intmax_t send_uart_data(UARTTransport* transport, const uint8_t* buf, size_t len)
{
    // Write header.
    uint8_t header[3] = {MICRORTPS_FRAMING_FLAG, transport->addr, (uint8_t)len};
    intmax_t result = write(transport->fd, (void*)header, sizeof(header)); 
    result = (0 <= result) ? result : -errno;

    // Write payload with stuffing and CRC.
    if (0 <= result)
    {
        size_t market = 0;
        size_t counter = 0;
        uint16_t crc = 0;
        for (unsigned int i = 0; i < len && 0 <= result; ++i)
        {
            if (*(buf + i) == MICRORTPS_FRAMING_FLAG || *(buf + i) == MICRORTPS_FRAMING_ESP)
            {
                // Write previous data.
                result = write(transport->fd, (void*)(buf + market), counter);
                result = (0 <= result) ? result : -errno;

                // Reset market and counter.
                market = market + counter + 1;
                counter = 0;

                // Write special octet.
                if (0 <= result)
                {
                    uint8_t temp[2] = {MICRORTPS_FRAMING_ESP, *(buf + i) ^ MICRORTPS_FRAMING_XOR};
                    result = write(transport->fd, (void*)&temp, 2);
                    result = (0 <= result) ? result : -errno;
                    update_crc(&crc, *(buf + i));
                }
            }
            else
            {
                update_crc(&crc, *(buf + i));
                counter++;
            }
        }

        if (0 <= result)
        {
            result = write(transport->fd, (void*)(buf + market), counter);
            result = (0 <= result) ? result : -errno;
        }

        // Write CRC.
        if (0 <= result)
        {
            result = write(transport->fd, (void*)&crc, 2);
            result = (0 <= result) ? result : -errno;
        }
    }

    return result;
}

intmax_t recv_uart_data(UARTTransport* transport, uint8_t** buf, size_t* len, int timeout)
{
    return 0;
}
