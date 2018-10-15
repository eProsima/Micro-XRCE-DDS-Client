#include <uxr/client/profile/transport/serial/serial_transport_linux.h>
#include <uxr/client/profile/transport/serial/serial_transport.h>

#include <unistd.h>
#include <errno.h>

/*******************************************************************************
 * Static members.
 *******************************************************************************/
static int serial_errno = 0;

bool uxr_init_serial_platform(struct uxrSerialPlatform* platform, int fd, uint8_t remote_addr, uint8_t local_addr)
{
    /* Poll setup. */
    platform->poll_fd.fd = fd;
    platform->poll_fd.events = POLLIN;

    return true;
}

bool uxr_close_serial_platform(struct uxrSerialPlatform* platform)
{
    return (0 == close(platform->poll_fd.fd));
}

uint16_t uxr_write_serial_data_platform(uxrSerialPlatform* platform, uint8_t* buf, uint16_t len)
{
    uint16_t rv = 0;

    ssize_t bytes_written = write(platform->poll_fd.fd, (void*)buf, (size_t)len);
    if ((0 < bytes_written) && (bytes_written == len))
    {
        rv = (uint16_t)bytes_written;
    }
    return rv;
}

uint16_t uxr_read_serial_data_platform(uxrSerialPlatform* platform, uint8_t* buf, uint16_t len, int timeout)
{
    uint16_t rv = 0;

    int poll_rv = poll(&platform->poll_fd, 1, timeout);
    if (0 < poll_rv)
    {
        ssize_t bytes_read = read(platform->poll_fd.fd, buf, len);
        if (0 < bytes_read)
        {
            rv = (uint16_t)bytes_read;
        }
    }

    return rv;
}

