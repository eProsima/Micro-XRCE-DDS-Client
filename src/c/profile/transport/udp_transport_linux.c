#include <micrortps/client/profile/transport/udp_transport_linux.h>

int init_udp_transport(UDPTransport* transport, const char* ip, uint16_t port)
{
    int result = 0;

    // Socket initialization.
    transport->socket_fd = socket(PF_INET, SOCK_DGRAM, 0);
    if (-1 == transport->socket_fd)
    {
        result = -errno;
    }

    if (0 < result)
    {
        // Remote IP setup.
        struct sockaddr_in temp_addr;
        temp_addr.sin_family = AF_INET;
        temp_addr.sin_port = port;
        temp_addr.sin_addr.s_addr = inet_addr(ip);
        memset(temp_addr.sin_zero, '\0', sizeof(temp_addr.sin_zero));
        transport->remote_addr = *((struct sockaddr *) &temp_addr);

        // Poll setup.
        transport->poll_fd.fd = transport->socket_fd;
        transport->poll_fd.events = POLLIN;

        // Remote address filter.
        int connected = connect(transport->socket_fd,
                                &transport->remote_addr,
                                sizeof(transport->remote_addr));
        if (-1 == connected)
        {
            result = -errno;
        }
    }

    return result;
}

intmax_t send_udp_data(UDPTransport* transport, const void* buf, size_t len)
{
    int sent = send(transport->socket_fd, buf, len, 0);
    return (sent <= 0) ? (intmax_t)sent : (intmax_t)-errno;
}

intmax_t recv_udp_data(UDPTransport* transport, void** buf, size_t* len, int timeout)
{
    intmax_t result = 0;

    int poll_rv = poll(&transport->poll_fd, 1, timeout);
    if (0 < poll_rv)
    {
        int received = recv(transport->socket_fd,
                            (void*)transport->buffer,
                            sizeof(transport->buffer), 0);
        if (0 <= received)
        {
            *len = (size_t)received;
            *buf = (void*)transport->buffer;
        }
        else
        {
            result = (intmax_t)-errno;
        }
    }
    else if (0 == poll_rv)
    {
        result = -1;
    }
    else
    {
        result = (intmax_t)-errno;
    }

    return result;
}
