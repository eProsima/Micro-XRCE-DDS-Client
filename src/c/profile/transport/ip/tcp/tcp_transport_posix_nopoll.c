#include <uxr/client/profile/transport/ip/tcp/tcp_transport_posix_nopoll.h>
#include <uxr/client/util/time.h>
#include "tcp_transport_internal.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#ifdef UCLIENT_PLATFORM_LINUX
static void sigpipe_handler(
        int fd)
{
    (void)fd;
}

#endif /* ifdef UCLIENT_PLATFORM_LINUX */

bool uxr_init_tcp_platform(
        struct uxrTCPPlatform* platform,
        uxrIpProtocol ip_protocol,
        const char* ip,
        const char* port)
{
    bool rv = false;
    int errsv = errno;

    switch (ip_protocol)
    {
        case UXR_IPv4:
            platform->fd = socket(AF_INET, SOCK_STREAM, 0);
            break;
        case UXR_IPv6:
            platform->fd = socket(AF_INET6, SOCK_STREAM, 0);
            break;
    }

    if (-1 != platform->fd)
    {
#ifdef UCLIENT_PLATFORM_LINUX
        signal(SIGPIPE, sigpipe_handler);
#endif /* ifdef UCLIENT_PLATFORM_LINUX */
        struct addrinfo hints;
        struct addrinfo* result;
        struct addrinfo* ptr;

        memset(&hints, 0, sizeof(hints));
        switch (ip_protocol)
        {
            case UXR_IPv4:
                hints.ai_family = AF_INET;
                break;
            case UXR_IPv6:
                hints.ai_family = AF_INET6;
                break;
        }
        hints.ai_socktype = SOCK_STREAM;

        if (0 == getaddrinfo(ip, port, &hints, &result))
        {
            for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
            {
                int connect_rv;
                do
                {
                    errno = errsv;
                    connect_rv = connect(platform->fd, ptr->ai_addr, ptr->ai_addrlen);
                } while (-1 == connect_rv && EINTR == errno);
                if (0 == connect_rv)
                {
                    rv = true;
                    break;
                }
            }
        }
        freeaddrinfo(result);
    }
    return rv;
}

bool uxr_close_tcp_platform(
        struct uxrTCPPlatform* platform)
{
    return (-1 == platform->fd) ? true : (0 == close(platform->fd));
}

size_t uxr_write_tcp_data_platform(
        struct uxrTCPPlatform* platform,
        const uint8_t* buf,
        size_t len,
        uint8_t* errcode)
{
    size_t rv = 0;
    int errsv = errno;
    ssize_t bytes_sent;
    do
    {
        errno = errsv;
        bytes_sent = send(platform->fd, (void*)buf, len, 0);
    } while (-1 == bytes_sent && EINTR == errno);
    if (-1 != bytes_sent)
    {
        rv = (size_t)bytes_sent;
        *errcode = 0;
    }
    else
    {
        *errcode = 1;
    }
    return rv;
}

size_t uxr_read_tcp_data_platform(
        struct uxrTCPPlatform* platform,
        uint8_t* buf,
        size_t len,
        int timeout,
        uint8_t* errcode)
{
    size_t rv = 0;
    int errsv = errno;

    int64_t start_timestamp = uxr_millis();
    int remaining_time = timeout;

    ssize_t bytes_received;
    do
    {
        errno = errsv;

        remaining_time = (remaining_time <= 0) ? 1 : remaining_time;

        struct timeval tv;
        tv.tv_sec = remaining_time / 1000;
        tv.tv_usec = (remaining_time % 1000) * 1000;

        if (0 != setsockopt(platform->fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)))
        {
            *errcode = 1;
            return 0;
        }

        bytes_received = recv(platform->fd, (void*)buf, len, 0);

        remaining_time = timeout - (int)(uxr_millis() - start_timestamp);
    } while (-1 == bytes_received && EINTR == errno);
    if (-1 != bytes_received)
    {
        rv = (size_t)bytes_received;
        *errcode = 0;
    }
    else
    {
        *errcode = 1;
    }

    return rv;
}

void uxr_disconnect_tcp_platform(
        struct uxrTCPPlatform* platform)
{
    close(platform->fd);
    platform->fd = -1;
}
