#include <micrortps/client/profile/transport/udp_transport_freertos.h>
#include <string.h>

int init_udp_transport(UDPTransport* transport, const char* ip, uint16_t port)
{
    int result = 0;

    // Socket initialization.
    transport->socket_fd = FreeRTOS_socket(FREERTOS_AF_INET,
                                                       FREERTOS_SOCK_DGRAM,
                                                       FREERTOS_IPPROTO_UDP);
    if (FREERTOS_INVALID_SOCKET == transport->socket_fd)
    {
        result = -1;
    }

    if (0 < result)
    {
        // Remote IP setup.
        transport->remote_addr.sin_port = port;
        transport->remote_addr.sin_addr = FreeRTOS_inet_addr(ip);
    }

    return result;
}

intmax_t send_udp_data(UDPTransport* transport, const void* buf, size_t len)
{
    intmax_t result = 0;

    // Get buffer from the TCP stack.
    uint8_t* out_buffer = FreeRTOS_GetUDPPayloadBuffer(len + 15, portMAX_DELAY);
    if (NULL == out_buffer)
    {
        result = -1;
    }

    // Copy output buffer from source buffer.
    memcpy(out_buffer, buf, len);

    // Send with zero copy behaviour.
    int32_t sent = FreeRTOS_sendto(transport->socket_fd,
                                   out_buffer,
                                   len,
                                   FREERTOS_ZERO_COPY,
                                   &transport->remote_addr,
                                   sizeof(transport->remote_addr));

    // Check error.
    if (0 == sent)
    {
        result = -1;
    }

    return result;
}

intmax_t recv_udp_data(UDPTransport* transport, void** buf, size_t* len, int timeout)
{
    (void) timeout;
    intmax_t result = 0;

    TickType_t ticks_timeout = timeout / portTICK_PERIOD_MS;
    FreeRTOS_setsockopt(transport->socket_fd,
                        0,
                        FREERTOS_SO_RVCTIMEO,
                        &ticks_timeout,
                        0);
    struct freertps_sockaddr remote_addr;
    uint32_t remote_len;
    int32_t received = FreeRTOS_recvfrom(transport->socket_fd,
                                         (void*)transport->buffer,
                                         0,
                                         FREERTOS_ZERO_COPY,
                                         &remote_addr,
                                         &remote_len);

    if (received != -pdFREERTOS_ERRNO_EWOULDBLOCK &&
        received != -pdFREERTOS_ERRNO_EINVAL &&
        received != -pdFREERTOS_ERRNO_EINTR)
    {
        // Filter remote address.
        if (remote_addr.sin_addr == transport->remote_addr.sin_addr)
        {
            *len = (size_t)received;
            *buf = (void*)transport->buffer;
        }
        else
        {
            *len = 0;
            *buf = NULL;
        }
    }
    else
    {
        *len = 0;
        *buf = NULL;
        result = -1;
    }

    return result;
}
