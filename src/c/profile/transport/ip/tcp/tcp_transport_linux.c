#include <uxr/client/profile/transport/ip/tcp/tcp_transport_linux.h>
#include "tcp_transport_internal.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#ifdef PLATFORM_NAME_LINUX
static void sigpipe_handler(int fd)
{
    (void)fd;
}
#endif

bool uxr_init_tcp_platform(
        struct uxrTCPPlatform* platform,
        uxrIpProtocol ip_protocol,
        const char* ip,
        uint16_t port)
{
    bool rv = false;

    /* Socket initialization. */
    switch (ip_protocol)
    {
        case UXR_IPv4:
            platform->poll_fd.fd = socket(AF_INET, SOCK_STREAM, 0);
            break;
        case UXR_IPv6:
            platform->poll_fd.fd = socket(AF_INET6, SOCK_STREAM, 0);
            break;
    }

    if (-1 != platform->poll_fd.fd)
    {
#ifdef PLATFORM_NAME_LINUX
        signal(SIGPIPE, sigpipe_handler);
#endif

        /* Remote IP setup. */
        switch (ip_protocol)
        {
            case UXR_IPv4:
            {
                struct sockaddr_in temp_addr;
                temp_addr.sin_family = AF_INET;
                temp_addr.sin_port = htons(port);
                temp_addr.sin_addr.s_addr = inet_addr(ip);
                platform->remote_addr = *((struct sockaddr_storage *) &temp_addr);
                break;
            }
            case UXR_IPv6:
            {
                struct sockaddr_in6 temp_addr;
                temp_addr.sin6_family = AF_INET6;
                temp_addr.sin6_port = htons(port);
                inet_pton(AF_INET6, ip, &(temp_addr.sin6_addr));
                platform->remote_addr = *((struct sockaddr_storage *) &temp_addr);
                break;
            }
        }

        /* Poll setup. */
        platform->poll_fd.events = POLLIN;

        /* Server connection. */
        int connected = connect(platform->poll_fd.fd,
                                (struct sockaddr*)&platform->remote_addr,
                                sizeof(platform->remote_addr));
        rv = (0 == connected);
    }
    return rv;
}

bool uxr_close_tcp_platform(
        struct uxrTCPPlatform* platform)
{
    return (-1 == platform->poll_fd.fd) ? true : (0 == close(platform->poll_fd.fd));
}

size_t uxr_write_tcp_data_platform(
        struct uxrTCPPlatform* platform,
        const uint8_t* buf,
        size_t len,
        uint8_t* errcode)
{
    size_t rv = 0;
    ssize_t bytes_sent = send(platform->poll_fd.fd, (void*)buf, len, 0);
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
    int poll_rv = poll(&platform->poll_fd, 1, timeout);
    if (0 < poll_rv)
    {
        ssize_t bytes_received = recv(platform->poll_fd.fd, (void*)buf, len, 0);
        if (-1 != bytes_received)
        {
            rv = (size_t)bytes_received;
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

void uxr_disconnect_tcp_platform(
        struct uxrTCPPlatform* platform)
{
    close(platform->poll_fd.fd);
    platform->poll_fd.fd = -1;
}
