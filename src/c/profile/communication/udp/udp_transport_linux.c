#include <micrortps/client/profile/communication/udp_transport.h>
#include <errno.h>

/**
 * Communication declarations.
 */
intmax_t send_data(Communication* communication, uint8_t* buf, size_t len);
intmax_t recv_data(Communication* communication, uint8_t** buf, size_t* len, uint32_t poll_ms);
bool is_reliable(Communication* communication);
size_t mtu(Communication* communication);

/**
 * TransportLayer declarations.
 */
int open_transport(TransportLayer* transport_layer, const char* ip, uint16_t port);
int close_transport(TransportLayer* transport_layer);

/**
 * UDPTransport definitions.
 */
int init_udp_transport(UDPTransport* udp_transport, const char* ip, uint16_t port)
{
    // Communication functions association.
    udp_transport->transport_layer->communication->send_data = send_data;
    udp_transport->transport_layer->communication->recv_data = recv_data;
    udp_transport->transport_layer->communication->is_reliable = is_reliable;

    // TransportLayer function association.
    udp_transport->transport_layer->open_transport = open_transport;
    udp_transport->transport_layer->close_transport = close_transport;

    // Socket initialization.
    udp_transport->socketfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (-1 == udp_transport->socketfd)
    {
        // TODO (julian): define a common error code for multi-platform.
        return errno;
    }

    // Set up remote IP.
    struct sockaddr_in temp_addr;
    temp_addr.sin_family = AF_INET;
    temp_addr.sin_port = port;
    temp_addr.sin_addr.s_addr = inet_addr(ip);
    memset(temp_addr.sin_zero, '\0', sizeof(temp_addr.sin_zero));
    udp_transport->remote_addr = *((struct sockaddr *) &temp_addr);

    return 0;
}

/**
 *
 * Communication definitions.
 */
intmax_t send_data(Communication* communication, uint8_t* buf, size_t len)
{
    // TODO (julian): is it MISRA? I don't think so.
    UDPTransport* udp_transport = (UDPTransport*)communication;
    return (intmax_t)sendto(udp_transport->socketfd, (char*)buf, len, 0, &udp_transport->remote_addr, sizeof(udp_transport->remote_addr));
}

intmax_t recv_data(Communication* communication, uint8_t** buf, size_t* len, uint32_t poll_ms)
{
    return 0;
}

bool is_reliable(Communication* communication) { return false; }

size_t mtu(Communication* communication) { return UDP_TRANSPORT_LINUX_MTU; }

/**
 * TransportLayer definitions.
 */
int open_transport(TransportLayer* transport_layer, const char* ip, uint16_t port)
{
    return 0;
}

int close_transport(TransportLayer* transport_layer)
{
    return 0;
}
