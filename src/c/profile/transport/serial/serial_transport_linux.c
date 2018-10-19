#include <uxr/client/profile/transport/serial/serial_transport_linux.h>
#include <uxr/client/profile/transport/serial/serial_transport.h>

#include <unistd.h>
#include <errno.h>

bool uxr_init_serial_platform(struct uxrSerialPlatform* platform, int fd, uint8_t remote_addr, uint8_t local_addr)
{
    (void) remote_addr;
    (void) local_addr;

    /* Poll setup. */
    platform->poll_fd.fd = fd;
    platform->poll_fd.events = POLLIN;

    return true;
}

bool uxr_close_serial_platform(struct uxrSerialPlatform* platform)
{
    return (0 == close(platform->poll_fd.fd));
}

size_t uxr_write_serial_data_platform(uxrSerialPlatform* platform, uint8_t* buf, size_t len, uint8_t* errcode)
{
    size_t rv = 0;

    ssize_t bytes_written = write(platform->poll_fd.fd, (void*)buf, (size_t)len);
    if (-1 != bytes_written)
    {
        rv = (size_t)bytes_written;
        *errcode = 0;
    }
    else
    {
        *errcode = 1;
    }
    return rv;
}

size_t uxr_read_serial_data_platform(uxrSerialPlatform* platform, uint8_t* buf, size_t len, int timeout, uint8_t* errcode)
{
    size_t rv = 0;

    int poll_rv = poll(&platform->poll_fd, 1, timeout);
    if (0 < poll_rv)
    {
        ssize_t bytes_read = read(platform->poll_fd.fd, buf, len);
        if (-1 != bytes_read)
        {
            rv = (size_t)bytes_read;
            *errcode = 0;
        }
        else
        {
            *errcode = 1;
        }
    }
    else
    {
        *errcode = (0 == poll_rv) ? 0 : 1;
    }
    return rv;
}

