#include <micrortps/client/profile/transport/uart_transport_linux.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/*******************************************************************************
 * Static members.
 *******************************************************************************/
static int uart_errno = 0;

/*******************************************************************************
 * Private function declarations.
 *******************************************************************************/
static uint16_t read_uart_data(void* instance, uint8_t* buf, size_t len, int timeout);
static bool send_uart_msg(void* instance, const uint8_t* buf, size_t len);
static bool recv_uart_msg(void* instance, uint8_t** buf, size_t* len, int timeout);

/*******************************************************************************
 * Private function definitions.
 *******************************************************************************/
static uint16_t read_uart_data(void* instance, uint8_t* buf, size_t len, int timeout)
{
    uint16_t rv = 0;
    UARTTransport* transport = (UARTTransport*)instance;

    int poll_rv = poll(&transport->poll_fd, 1, timeout);
    if (0 < poll_rv)
    {
        ssize_t bytes_read = read(transport->poll_fd.fd, buf, len);
        if (0 < bytes_read)
        {
            rv = (uint16_t)bytes_read;
        }
    }

    return rv;
}

static bool send_uart_msg(void* instance, const uint8_t* buf, size_t len)
{
    bool rv = false;
    UARTTransport* transport = (UARTTransport*)instance;

    uint16_t bytes_written = write_serial_msg(&transport->serial_io, buf, len, transport->local_addr);
    if (0 < bytes_written)
    {
        ssize_t bytes_sent = write(transport->poll_fd.fd, transport->serial_io.output.buffer, (size_t)bytes_written);
        if (0 < bytes_sent && (uint16_t)bytes_sent == bytes_written)
        {
            rv = true;
        }
    }
    uart_errno = rv ? 0 : -1;

    return rv;
}

static bool recv_uart_msg(void* instance, uint8_t** buf, size_t* len, int timeout)
{
    bool rv = true;
    UARTTransport* transport = (UARTTransport*)instance;
    uint8_t addr;

    uint8_t bytes_read = read_serial_msg(&transport->serial_io, read_uart_data, instance,
                                         transport->buffer, sizeof(transport->buffer), &addr, timeout);
    if (0 < bytes_read && addr == transport->remote_addr)
    {
        *len = bytes_read;
        *buf = transport->buffer;
    }
    else
    {
        uart_errno = -1;
        rv = false;
    }

    return rv;
}

static int get_uart_error()
{
    return uart_errno;
}

/*******************************************************************************
 * Public function definitions.
 *******************************************************************************/
int init_uart_transport(UARTTransport* transport, const char* device, uint8_t remote_addr, uint8_t local_addr)
{
    int rv = -1;

    /* Open device */
    transport->remote_addr = remote_addr;
    transport->local_addr = local_addr;
    transport->poll_fd.fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (transport->poll_fd.fd != -1)
    {
        /* Init SerialIO. */
        init_serial_io(&transport->serial_io);

        /* Send init flag. */
        uint8_t flag = MICRORTPS_FRAMING_END_FLAG;
        ssize_t bytes_written = write(transport->poll_fd.fd, &flag, 1);
        if (0 < bytes_written && 1 == bytes_written)
        {
            /* Poll setup. */
            transport->poll_fd.events = POLLIN;

            /* Interface setup. */
            transport->comm.instance = (void*)transport;
            transport->comm.send_msg = send_uart_msg;
            transport->comm.recv_msg = recv_uart_msg;
            transport->comm.comm_error = get_uart_error;
            rv = 0;
        }
    }

    return rv ;
}

int init_uart_transport_fd(UARTTransport* transport, int fd, uint8_t remote_addr, uint8_t local_addr)
{
    int rv = -1;

    /* Open device */
    transport->remote_addr = remote_addr;
    transport->local_addr = local_addr;
    transport->poll_fd.fd = fd;

    /* Init SerialIO. */
    init_serial_io(&transport->serial_io);

    /* Send init flag. */
    uint8_t flag = MICRORTPS_FRAMING_END_FLAG;
    ssize_t bytes_written = write(transport->poll_fd.fd, &flag, 1);
    if (0 < bytes_written && 1 == bytes_written)
    {
        /* Poll setup. */
        transport->poll_fd.events = POLLIN;

        /* Interface setup. */
        transport->comm.instance = (void*)transport;
        transport->comm.send_msg = send_uart_msg;
        transport->comm.recv_msg = recv_uart_msg;
        transport->comm.comm_error = get_uart_error;
        rv = 0;
    }

    return rv;
}
