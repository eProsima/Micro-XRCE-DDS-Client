#include <micrortps/client/profile/transport/udp_transport.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

#define UDP_TRANSPORT_LINUX_MTU 512

intmax_t send_data(UDPTransport* transport, const char* buf, size_t len);
intmax_t recv_data(UDPTransport* transport, char** buf, size_t* len);

struct UDPProperties
{
    char buffer[UDP_TRANSPORT_LINUX_MTU];
    int socketfd;
    struct sockaddr remote_addr;
};

int init_udp_transport(UDPTransport* transport, const char* ip, uint16_t port)
{
    transport->send_data = send_data;
    transport->recv_data = recv_data;

    // Socket initialization.
    transport->properties->socketfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (-1 == transport->properties->socketfd)
    {
        // TODO (julian): define a common error code for multi-platform.
        return errno;
    }

    // Remote IP setup.
    struct sockaddr_in temp_addr;
    temp_addr.sin_family = AF_INET;
    temp_addr.sin_port = port;
    temp_addr.sin_addr.s_addr = inet_addr(ip);
    memset(temp_addr.sin_zero, '\0', sizeof(temp_addr.sin_zero));
    transport->properties->remote_addr = *((struct sockaddr *) &temp_addr);

    return 0;
}

intmax_t send_data(UDPTransport* transport, const char* buf, size_t len)
{
    return (intmax_t)sendto(transport->properties->socketfd, buf, len, 0,
            &transport->properties->remote_addr, sizeof(transport->properties->remote_addr));
}

intmax_t recv_data(UDPTransport* transport, char** buf, size_t* len)
{
    socklen_t address_len = sizeof(transport->properties->remote_addr);
    intmax_t result = (intmax_t)recvfrom(transport->properties->socketfd,
                                         transport->properties->buffer,
                                         sizeof(transport->properties->buffer), 0,
                                         &transport->properties->remote_addr,
                                         &address_len);
    if (0 < result)
    {
        *len = (size_t)result;
        *buf = transport->properties->buffer;
    }
    else
    {
        *len = 0;
    }

    return result;
}
