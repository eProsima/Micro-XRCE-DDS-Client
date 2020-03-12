#include <uxr/client/profile/transport/ip/udp/udp_transport_posix_nopoll.h>
#include "udp_transport_internal.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

bool uxr_init_udp_platform(
        uxrUDPPlatform* platform,
        uxrIpProtocol ip_protocol,
        const char* ip,
        const char* port)
{
    bool rv = false;

    switch (ip_protocol)
    {
        case UXR_IPv4:
            platform->fd = socket(AF_INET, SOCK_DGRAM, 0);
            break;
        case UXR_IPv6:
            platform->fd = socket(AF_INET6, SOCK_DGRAM, 0);
            break;
    }

    if (-1 != platform->fd)
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
                if (0 == connect(platform->fd, ptr->ai_addr, ptr->ai_addrlen))
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

bool uxr_close_udp_platform(
        uxrUDPPlatform* platform)
{
    return (-1 == platform->fd) ? true : (0 == close(platform->fd));
}

size_t uxr_write_udp_data_platform(
        uxrUDPPlatform* platform,
        const uint8_t* buf,
        size_t len,
        uint8_t* errcode)
{
    size_t rv = 0;
    ssize_t bytes_sent = send(platform->fd, (void*)buf, len, 0);
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

    struct timeval tv;
    tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

    setsockopt(platform->fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    ssize_t bytes_received = recv(platform->fd, (void*)buf, len, 0);
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
