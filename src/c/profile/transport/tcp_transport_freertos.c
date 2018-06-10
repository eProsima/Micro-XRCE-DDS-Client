#include <micrortps/client/profile/transport/tcp_transport.h>
#include <FreeRTOS_sockets.h>

#define TCP_TRANSPORT_MTU 512

intmax_t send_data(TCPTransport* transport, const void* buf, size_t len);
intmax_t recv_data(TCPTransport* transport, void** buf,  size_t* len, int timeout);

struct TCPProperties
{
    uint8_t buffer[TCP_TRANSPORT_MTU];
    Socket_t socket_fd;
    struct freertos_sockaddr remote_addr;
};

int init_udp_transport(TCPTransport* transport, const char* ip, uint16_t port)
{
    int result = 0;

    transport->send_data = send_data;
    transport->recv_data = recv_data;

    // Socket initialization.
    transport->properties->socket_fd = FreeRTOS_socket(FREERTOS_AF_INET,
                                                     FREERTOS_SOCK_STREAM,
                                                     FREERTOS_IPPROTO_TCP);
    if (FREERTOS_INVALID_SOCKET == transport->properties->socket_fd)
    {
        result = -1;
    }

    if (0 < result)
    {
        // Remote IP setup.
        transport->properties->remote_addr.sin_port = port;
        transport->properties->remote_addr.sin_addr = FreeRTOS_inet_addr(ip);

        // Connect to server.
        BaseType_t connected = FreeRTOS_connect(transport->properties->socket_fd,
                                                &transport->properties->remote_addr,
                                                sizeof(transport->properties->remote_addr));
        if (connected != 0)
        {
            result = -1;
        }
    }

    return result;
}

intmax_t send_data(TCPTransport* transport, const void* buf, size_t len)
{
    intmax_t result = 0;

    BaseType_t sent = FreeRTOS_send(transport->properties->xSocket, buf, len, 0);
    if (sent == -pdFREERTOS_ERRNO_EWOULDBLOCK ||
        sent == -pdFREERTOS_ERRNO_EINVAL ||
        sent == -pdFREERTOS_ERRNO_EINTR)
    {
        result = -1;
    }

    return result;
}

intmax_t recv_data(TCPTransport* transport, void** buf, size_t* len, int timeout)
{
    intmax_t result = 0;

    TickType_t ticks_timeout = timeout / portTICK_PERIOD_MS;
    FreeRTOS_setsockopt(transport->properties->socket_fd,
                        0,
                        FREERTOS_SO_SNDTIMEO,
                        &ticks_timeout,
                        0);
    BaseType_t received = FreeRTOS_recv(transport->properties->xSocket,
                                        transport->properties->buffer,
                                        sizeof(transport->properties->buffer), 0);
    if (received == -pdFREERTOS_ERRNO_ENOMEN ||
        received == -pdFREERTOS_ERRNO_ENOTCONN ||
        received == -pdFREERTOS_ERRNO_EINTR ||
        received == -pdFREERTOS_ERRNO_EINVAL)
    {
        *len = 0;
        *buf = NULL;
        result = -1;
    }
    else
    {
        *len = (size_t)received;
        *buf = (void*)transport->properties->buffer;
    }

    return result;
}
