#include <micrortps/client/profile/transport/tcp_transport_linux.h>

int init_tcp_transport(TCPTransport* transport, const char* ip, uint16_t port)
{
    int result = 0;

    // Socket initialization.
    transport->socket_fd = socket(PF_INET, SOCK_STREAM, 0);
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
        memset(temp_addr.sin_zero,  '\0', sizeof(temp_addr.sin_zero));
        transport->remote_addr = *((struct sockaddr *) &temp_addr);

        // Poll setup.
        transport->poll_fd.fd = transport->socket_fd;
        transport->poll_fd.events = POLLIN;

        // Server connection.
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

intmax_t send_tcp_data(TCPTransport* transport, const void* buf, size_t len)
{
    intmax_t result = 0;

    int sent = send(transport->socket_fd, buf, len,  0);
    result = (sent <= 0) ? (intmax_t)sent : (intmax_t)-errno;

    return result;
}

intmax_t recv_tcp_data(TCPTransport* transport, void** buf, size_t* len, int timeout)
{
    intmax_t result = 0;

    int poll_rv = poll(&transport->poll_fd, 1, timeout);
    if (0 < poll_rv)
    {
        int received = recv(transport->socket_fd,
                            transport->buffer,
                            sizeof(transport->buffer), 0);
        if (0 <= received)
        {
            *len = (size_t)result;
            *buf = (void*)transport->buffer;
        }
        else
        {
            *len = 0;
            *buf = NULL;
            result = (intmax_t)-errno;
        }
    }
    else if (0 == poll_rv)
    {
        *len = 0;
        *buf = NULL;
    }
    else
    {
        *len = 0;
        *buf = NULL;
        result = (intmax_t)-errno;
    }

    return result;
}
