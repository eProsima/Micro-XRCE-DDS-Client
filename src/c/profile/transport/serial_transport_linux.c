#include <microxrce/client/profile/transport/serial_transport_linux.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/*******************************************************************************
 * Static members.
 *******************************************************************************/
static int serial_errno = 0;

/*******************************************************************************
 * Private function declarations.
 *******************************************************************************/
static uint16_t read_serial_data(void* instance, uint8_t* buf, size_t len, int timeout);
static bool send_serial_msg(void* instance, const uint8_t* buf, size_t len);
static bool recv_serial_msg(void* instance, uint8_t** buf, size_t* len, int timeout);

/*******************************************************************************
 * Private function definitions.
 *******************************************************************************/
static uint16_t read_serial_data(void* instance, uint8_t* buf, size_t len, int timeout)
{
    uint16_t rv = 0;
    mrSerialTransport* transport = (mrSerialTransport*)instance;

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

static bool send_serial_msg(void* instance, const uint8_t* buf, size_t len)
{
    bool rv = false;
    mrSerialTransport* transport = (mrSerialTransport*)instance;

    uint16_t bytes_written = write_serial_msg(&transport->serial_io,
                                              buf,
                                              len,
                                              transport->local_addr,
                                              transport->remote_addr);
    if (0 < bytes_written)
    {
        ssize_t bytes_sent = write(transport->poll_fd.fd, transport->serial_io.output.buffer, (size_t)bytes_written);
        if (0 < bytes_sent && (uint16_t)bytes_sent == bytes_written)
        {
            rv = true;
        }
    }
    serial_errno = rv ? 0 : -1;

    return rv;
}

static bool recv_serial_msg(void* instance, uint8_t** buf, size_t* len, int timeout)
{
    bool rv = true;
    mrSerialTransport* transport = (mrSerialTransport*)instance;
    uint8_t src_addr;
    uint8_t rmt_addr;
    uint16_t bytes_read = read_serial_msg(&transport->serial_io,
                                          read_serial_data,
                                          instance,
                                          transport->buffer,
                                          sizeof(transport->buffer),
                                          &src_addr,
                                          &rmt_addr,
                                          timeout);
    if (0 < bytes_read && src_addr == transport->remote_addr)
    {
        *len = bytes_read;
        *buf = transport->buffer;
    }
    else
    {
        serial_errno = -1;
        rv = false;
    }

    return rv;
}

static int get_serial_error(void)
{
    return serial_errno;
}

/*******************************************************************************
 * Public function definitions.
 *******************************************************************************/
bool mr_init_serial_transport(mrSerialTransport* transport, const char* device, uint8_t remote_addr, uint8_t local_addr)
{
    bool rv = false;

    /* Open device */
    int fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd != -1)
    {
        rv = mr_init_serial_transport_fd(transport, fd, remote_addr, local_addr);
    }

    return rv;
}

bool mr_init_serial_transport_fd(mrSerialTransport* transport, int fd, uint8_t remote_addr, uint8_t local_addr)
{
    bool rv = false;

    transport->remote_addr = remote_addr;
    transport->local_addr = local_addr;
    transport->poll_fd.fd = fd;

    /* Init SerialIO. */
    init_serial_io(&transport->serial_io);

    /* Send init flag. */
    uint8_t flag = UXR_FRAMING_END_FLAG;
    ssize_t bytes_written = write(transport->poll_fd.fd, &flag, 1);
    if (0 < bytes_written && 1 == bytes_written)
    {
        /* Poll setup. */
        transport->poll_fd.events = POLLIN;

        /* Interface setup. */
        transport->comm.instance = (void*)transport;
        transport->comm.send_msg = send_serial_msg;
        transport->comm.recv_msg = recv_serial_msg;
        transport->comm.comm_error = get_serial_error;
        transport->comm.mtu = UXR_CONFIG_SERIAL_TRANSPORT_MTU;
        rv = true;
    }

    return rv;
}

bool mr_close_serial_transport(mrSerialTransport* transport)
{
    return (0 == close(transport->poll_fd.fd));
}
