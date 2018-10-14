#include <uxr/client/profile/transport/serial_transport_linux.h>
#include <uxr/client/util/time.h>

#include "../../core/communication/serial_protocol_internal.h"

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
static uint16_t write_serial_data(void* instance, uint8_t* buf, uint16_t len);
static uint16_t read_serial_data(void* instance, uint8_t* buf, uint16_t len, int timeout);
static bool send_serial_msg(void* instance, const uint8_t* buf, size_t len);
static bool recv_serial_msg(void* instance, uint8_t** buf, size_t* len, int timeout);

/*******************************************************************************
 * Private function definitions.
 *******************************************************************************/
static uint16_t write_serial_data(void* instance, uint8_t* buf, uint16_t len)
{
    uint16_t rv = 0;
    uxrSerialTransport* transport = (uxrSerialTransport*)instance;

    ssize_t bytes_written = write(transport->poll_fd.fd, (void*)buf, (size_t)len);
    if ((0 < bytes_written) && (bytes_written == len))
    {
        rv = (uint16_t)bytes_written;
    }
    return rv;
}

static uint16_t read_serial_data(void* instance, uint8_t* buf, uint16_t len, int timeout)
{
    uint16_t rv = 0;
    uxrSerialTransport* transport = (uxrSerialTransport*)instance;

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
    uxrSerialTransport* transport = (uxrSerialTransport*)instance;

    uint16_t bytes_written = uxr_write_serial_msg(&transport->serial_io.output,
                                                  write_serial_data,
                                                  instance,
                                                  buf,
                                                  (uint16_t)len,
                                                  transport->local_addr,
                                                  transport->remote_addr);
    if ((0 < bytes_written) && (bytes_written == len))
    {
        rv = true;
    }
    serial_errno = rv ? 0 : -1;

    return rv;
}

static bool recv_serial_msg(void* instance, uint8_t** buf, size_t* len, int timeout)
{
    bool rv = false;
    uxrSerialTransport* transport = (uxrSerialTransport*)instance;

    uint16_t bytes_read = 0;
    do
    {
        int64_t time_init = uxr_millis();
        bytes_read = uxr_read_serial_msg(&transport->serial_io.input,
                                         read_serial_data,
                                         instance,
                                         timeout);
        if (0 < bytes_read && transport->serial_io.input.src_addr == transport->remote_addr)
        {
            *len = bytes_read;
            *buf = transport->serial_io.input.buffer;
            rv = true;
        }
        else
        {
            serial_errno = -1;
        }
        timeout -= (int)(uxr_millis() - time_init);
    }
    while ((0 == bytes_read) && (0 < timeout));

    return rv;
}

static int get_serial_error(void)
{
    return serial_errno;
}

/*******************************************************************************
 * Public function definitions.
 *******************************************************************************/
bool uxr_init_serial_transport(uxrSerialTransport* transport, const char* device, uint8_t remote_addr, uint8_t local_addr)
{
    bool rv = false;

    /* Open device */
    int fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd != -1)
    {
        rv = uxr_init_serial_transport_fd(transport, fd, remote_addr, local_addr);
    }

    return rv;
}

bool uxr_init_serial_transport_fd(uxrSerialTransport* transport, int fd, uint8_t remote_addr, uint8_t local_addr)
{
    bool rv = false;

    transport->remote_addr = remote_addr;
    transport->local_addr = local_addr;
    transport->poll_fd.fd = fd;

    /* Init SerialIO. */
    uxr_init_serial_io(&transport->serial_io);

    /* Send init flag. */
    uint8_t flag = UXR_FRAMING_BEGIN_FLAG;
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

bool uxr_close_serial_transport(uxrSerialTransport* transport)
{
    return (0 == close(transport->poll_fd.fd));
}
