#include <uxr/client/profile/transport/serial/serial_transport_posix.h>
#include <uxr/client/profile/transport/serial/serial_transport_platform.h>

#include <unistd.h>
#include <errno.h>

bool uxr_init_serial_platform(
        void* args,
        int fd,
        uint8_t remote_addr,
        uint8_t local_addr)
{
    (void) remote_addr;
    (void) local_addr;

    struct uxrSerialPlatform* platform = (struct uxrSerialPlatform*) args;

    /* Poll setup. */
    platform->poll_fd.fd = fd;
    platform->poll_fd.events = POLLIN;

    return true;
}

bool uxr_close_serial_platform(
        void* args)
{
    struct uxrSerialPlatform* platform = (struct uxrSerialPlatform*) args;
    return (-1 == platform->poll_fd.fd) ? true : (0 == close(platform->poll_fd.fd));
}

size_t uxr_write_serial_data_platform(
        void* args,
        const uint8_t* buf,
        size_t len,
        uint8_t* errcode)
{
    size_t rv = 0;
    struct uxrSerialPlatform* platform = (struct uxrSerialPlatform*) args;

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

size_t uxr_read_serial_data_platform(
        void* args,
        uint8_t* buf,
        size_t len,
        int timeout,
        uint8_t* errcode)
{
    size_t rv = 0;
    struct uxrSerialPlatform* platform = (struct uxrSerialPlatform*) args;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(platform->poll_fd.fd, &fds);
    struct timeval t_out = { 0, timeout*1000};
    int poll_rv = select(platform->poll_fd.fd+1, &fds, NULL, NULL, &t_out);

    //int poll_rv = poll(&platform->poll_fd, 1, timeout);
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
