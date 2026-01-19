#include <uxr/client/profile/transport/ip/udp/udp_transport_posix.h>
#include <uxr/client/util/time.h>
#include "udp_transport_internal.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>

bool uxr_init_udp_platform(
        uxrUDPPlatform* platform,
        uxrIpProtocol ip_protocol,
        const char* ip,
        const char* port)
{
    bool rv = false;
    int errsv = errno;

    switch (ip_protocol)
    {
        case UXR_IPv4:
            platform->poll_fd.fd = socket(AF_INET, SOCK_DGRAM, 0);
            break;
        case UXR_IPv6:
            platform->poll_fd.fd = socket(AF_INET6, SOCK_DGRAM, 0);
            break;
    }

    if (-1 != platform->poll_fd.fd)
    {
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
        hints.ai_socktype = SOCK_DGRAM;

        if (0 == getaddrinfo(ip, port, &hints, &result))
        {
            for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
            {
                int connect_rv;
                do
                {
                    errno = errsv;
                    connect_rv = connect(platform->poll_fd.fd, ptr->ai_addr, ptr->ai_addrlen);
                } while (-1 == connect_rv && EINTR == errno);
                if (0 == connect_rv)
                {
                    platform->poll_fd.events = POLLIN;
                    rv = true;
                    break;
                }
            }
            freeaddrinfo(result);
        }
    }
    return rv;
}

bool uxr_close_udp_platform(
        uxrUDPPlatform* platform)
{
    return (-1 == platform->poll_fd.fd) ? true : (0 == close(platform->poll_fd.fd));
}

size_t uxr_write_udp_data_platform(
        uxrUDPPlatform* platform,
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
        bytes_sent = send(platform->poll_fd.fd, (void*)buf, len, 0);
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

size_t uxr_read_udp_data_platform(
        uxrUDPPlatform* platform,
        uint8_t* buf,
        size_t len,
        int timeout,
        uint8_t* errcode)
{
    size_t rv = 0;
    int errsv = errno;
    int64_t start_timestamp = uxr_millis();
    int remaining_time = timeout;
    int poll_rv;
    do
    {
        errno = errsv;
        remaining_time = (remaining_time <= 0) ? 0 : remaining_time;
        poll_rv = poll(&platform->poll_fd, 1, remaining_time);
        remaining_time = timeout - (int)(uxr_millis() - start_timestamp);
    } while (-1 == poll_rv && EINTR == errno);
    if (0 < poll_rv)
    {
        ssize_t bytes_received;
        do
        {
            errno = errsv;
            bytes_received = recv(platform->poll_fd.fd, (void*)buf, len, 0);
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
    }
    else
    {
        *errcode = (0 == poll_rv) ? 0 : 1;
    }
    return rv;
}
