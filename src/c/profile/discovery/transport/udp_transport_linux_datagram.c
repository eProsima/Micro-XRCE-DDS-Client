#include "udp_transport_linux_datagram_internal.h"

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

static bool send_to(uxrUDPTransportDatagram* transport, const uint8_t* buf, size_t len, const char* ip, uint16_t port);

bool uxr_init_udp_transport_datagram(uxrUDPTransportDatagram* transport)
{
    bool rv = false;

    int fd = socket(PF_INET, SOCK_DGRAM, 0);
    transport->poll_fd.fd = fd;
    transport->poll_fd.events = POLLIN;

    return rv;
}

bool uxr_udp_send_datagram_to(uxrUDPTransportDatagram* transport, const uint8_t* buf, size_t len, const char* ip, uint16_t port)
{
    bool rv = true;

    struct sockaddr_in remote_addr;
    if(0 != inet_aton(ip, &remote_addr.sin_addr))
    {
        remote_addr.sin_family = AF_INET;
        remote_addr.sin_port = htons(port);

        ssize_t bytes_sent = sendto(transport->poll_fd.fd, (void*)buf, len, 0,
                                    (struct sockaddr*)&remote_addr, sizeof(remote_addr));
        if (0 > bytes_sent)
        {
            rv = false;
        }
    }

    return rv;
}

bool uxr_udp_recv_datagram(uxrUDPTransportDatagram* transport, uint8_t** buf, size_t* len, int timeout)
{
    bool rv = true;

    int poll_rv = poll(&transport->poll_fd, 1, timeout);
    if (0 < poll_rv)
    {
        ssize_t bytes_received = recv(transport->poll_fd.fd, (void*)transport->buffer, sizeof(transport->buffer), 0);
        if (0 < bytes_received)
        {
            *len = (size_t)bytes_received;
            *buf = transport->buffer;
        }
        else
        {
            rv = false;
        }
    }
    else if (0 == poll_rv)
    {
        rv = false;
        errno = ETIME;
    }
    else
    {
        rv = false;
    }

    return rv;
}

void uxr_bytes_to_ip(const uint8_t* bytes, char* ip)
{
    struct in_addr addr;
    addr.s_addr = inet_addr((char*)bytes);
    char* internal_ip = inet_ntoa(addr);
    strcpy(ip, internal_ip);
}

