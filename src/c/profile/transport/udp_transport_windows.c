#include <micrortps/client/profile/transport/udp_transport_windows.h>

int init_udp_transport(UDPTransport* transport, const char* ip, uint16_t port)
{
    int result = 0;

    // Socket initialization.
    transport->socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (INVALID_SOCKET == transport->socket_fd)
    {
        result = -WSAGetLastError();
    }

    if (0 < result)
    {
        // Remote IP setup.
        struct sockaddr_in temp_addr;
        temp_addr.sin_family = AF_INET;
        temp_addr.sin_port = port;
        temp_addr.sin_addr.s_addr = inet_addr(ip);
        memset(temp_addr.sin_zero, '\0', sizeof(temp_addr.sin_zero));
        transport->remote_addr = *((struct sockaddr *)&temp_addr);

        // Poll setup.
        transport->poll_fd.fd = transport->socket_fd;
        transport->poll_fd.events = POLLIN;

        // Remote address filter.
        int connected = connect(transport->socket_fd,
                                &transport->remote_addr,
                                sizeof(transport->remote_addr));
        if (-1 == connected)
        {
            result = -WSAGetLastError();
        }
    }

    return result;
}

intmax_t send_udp_data(UDPTransport* transport, const void* buf, size_t len)
{
    int sent = send(transport->socket_fd, buf, len, 0);
    return (SOCKET_ERROR != sent) ? (intmax_t)sent : (intmax_t)-WSAGetLastError();
}

intmax_t recv_udp_data(UDPTransport* transport, void** buf, size_t* len, int timeout)
{
    intmax_t result = 0;

    int poll_rv = WSAPoll(&transport->poll_fd, 1, timeout);
    if (0 < poll_rv)
    {
        int received = recv(transport->socket_fd,
                            (void*)transport->buffer,
                            sizeof(transport->buffer), 0);
        if (SOCKET_ERROR != received)
        {
            *len = (size_t)received;
            *buf = (void*)transport->buffer;
        }
        else
        {
            result = (intmax_t)-WSAGetLastError();
        }
    }
    else if (0 == poll_rv)
    {
        result = -1;
    }
    else
    {
        result = (intmax_t)-WSAGetLastError();
    }

    return result;
}
