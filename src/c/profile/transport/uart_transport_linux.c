#include <micrortps/client/profile/transport/uart_transport_linux.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

/*******************************************************************************
 * Private function declarations.
 *******************************************************************************/
static int uart_errno = 0;

/*******************************************************************************
 * Function declarations.
 *******************************************************************************/
static uint16_t read_uart_data(void* transport, uint8_t* buf, size_t len);
static bool send_uart_msg(void* instance, const uint8_t* buf, size_t len);
static bool recv_uart_msg(void* instance, uint8_t** buf, size_t* len, int timeout);
static int get_uart_error();

/*******************************************************************************
 * Private function definitions.
 *******************************************************************************/
static uint16_t read_uart_data(void* transport, uint8_t* buf, size_t len)
{
    UARTTransport* uart_transport = (UARTTransport*)transport;
    ssize_t bytes_read = read(uart_transport->fd, buf, len);
    return (0 < bytes_read) ? bytes_read : 0;
}

static bool send_uart_msg(void* instance, const uint8_t* buf, size_t len)
{
    bool rv = false;
    UARTTransport* transport = (UARTTransport*)instance;

    uint16_t bytes_written = write_serial_msg(&transport->serial_io, buf, len);
    if (0 < bytes_written)
    {
        ssize_t bytes_sent = write(transport->fd, transport->serial_io.output.buffer, (size_t)bytes_written);
        if (bytes_sent == bytes_written)
        {
            rv = true;
        }
        else
        {
            uart_errno = -1;
        }
    }

    return rv;
}

static bool recv_uart_msg(void* instance, uint8_t** buf, size_t* len, int timeout)
{
    (void)timeout;
    bool rv = true;
    UARTTransport* transport = (UARTTransport*)instance;

    uint8_t bytes_read = read_serial_msg(&transport->serial_io, read_uart_data, instance,
                                         transport->buffer, sizeof(transport->buffer));
    if (0 < bytes_read)
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
int init_uart_transport(UARTTransport* transport, const char* device, uint8_t addr)
{
    int rv = 0;

    /* Open device */
    transport->addr = addr;
    transport->fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (transport->fd != -1)
    {
        /* Init SerialIO. */
        init_serial_io(&transport->serial_io, addr);

        /* Send init flag. */
        uint8_t flag = MICRORTPS_FRAMING_FLAG;
        ssize_t bytes_written = write(transport->fd, &flag, 1);
        if (0 < bytes_written)
        {
            rv = -1;
        }
        else
        {
            /* Interface setup. */
            transport->comm.instance = (void*)transport;
            transport->comm.send_msg = send_uart_msg;
            transport->comm.recv_msg = recv_uart_msg;
            transport->comm.comm_error = get_uart_error;
        }
    }
    else
    {
        rv = -1;
    }

    return rv ;
}

int init_uart_transport_fd(UARTTransport* transport, int fd, uint8_t addr)
{
    int rv = 0;

    /* Open device */
    transport->addr = addr;
    transport->fd = fd;

    /* Init SerialIO. */
    init_serial_io(&transport->serial_io, addr);

    /* Send init flag. */
    uint8_t flag = MICRORTPS_FRAMING_FLAG;
    ssize_t bytes_written = write(transport->fd, &flag, 1);
    if (0 < bytes_written)
    {
        rv = -1;
    }

    /* Interface setup. */
    transport->comm.instance = (void*)transport;
    transport->comm.send_msg = send_uart_msg;
    transport->comm.recv_msg = recv_uart_msg;
    transport->comm.comm_error = get_uart_error;

    return rv;
}
