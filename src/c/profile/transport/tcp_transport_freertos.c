#include <micrortps/client/profile/transport/tcp_transport_freertos.h>
#include <FreeRTOS_sockets.h>

int init_udp_transport(TCPTransport* transport, const char* ip, uint16_t port)
{
    int result = 0;

    // Socket initialization.
    transport->socket_fd = FreeRTOS_socket(FREERTOS_AF_INET,
                                                     FREERTOS_SOCK_STREAM,
                                                     FREERTOS_IPPROTO_TCP);
    if (FREERTOS_INVALID_SOCKET == transport->socket_fd)
    {
        result = -1;
    }

    if (0 < result)
    {
        // Remote IP setup.
        transport->remote_addr.sin_port = port;
        transport->remote_addr.sin_addr = FreeRTOS_inet_addr(ip);

        // Connect to server.
        BaseType_t connected = FreeRTOS_connect(transport->socket_fd,
                                                &transport->remote_addr,
                                                sizeof(transport->remote_addr));
        if (connected != 0)
        {
            result = -1;
        }
    }

    return result;
}

intmax_t send_tcp_data(TCPTransport* transport, const void* buf, size_t len)
{
    intmax_t result = 0;

    BaseType_t sent = FreeRTOS_send(transport->socket_fd, buf, len, 0);
    if (sent == -pdFREERTOS_ERRNO_EWOULDBLOCK ||
        sent == -pdFREERTOS_ERRNO_EINVAL ||
        sent == -pdFREERTOS_ERRNO_EINTR)
    {
        result = -1;
    }

    return result;
}

intmax_t recv_tcp_data(TCPTransport* transport, void** buf, size_t* len, int timeout)
{
    intmax_t result = 0;

    TickType_t ticks_timeout = timeout / portTICK_PERIOD_MS;
    FreeRTOS_setsockopt(transport->socket_fd,
                        0,
                        FREERTOS_SO_SNDTIMEO,
                        &ticks_timeout,
                        0);
    BaseType_t received = FreeRTOS_recv(transport->socket_fd,
                                        transport->buffer,
                                        sizeof(transport->buffer), 0);
    if (received == -pdFREERTOS_ERRNO_ENOMEN ||
        received == -pdFREERTOS_ERRNO_ENOTCONN ||
        received == -pdFREERTOS_ERRNO_EINTR ||
        received == -pdFREERTOS_ERRNO_EINVAL)
    {
        result = -1;
    }
    else
    {
        *len = (size_t)received;
        *buf = (void*)transport->buffer;
    }

    return result;
}
