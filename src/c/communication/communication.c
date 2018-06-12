#include <micrortps/client/communication/communication.h>

#ifdef PROFILE_UDP_TRANSPORT
int init_udp_comm(Communication* communication, UDPTransport* transport)
{
    communication->udp_transport = transport;
    communication->transport_kind = UDP_TRANSPORT_KIND;
    return 0;
}
#endif //PROFILE_UDP_TRANSPORT

#ifdef PROFILE_TCP_TRANSPORT
int init_tcp_comm(Communication* communication, TCPTransport* transport)
{
    communication->tcp_transport = transport;
    communication->transport_kind = TCP_TRANSPORT_KIND;
    return 0;
}
#endif //PROFILE_TCP_TRANSPORT

#ifdef PROFILE_UART_TRANSPORT
int init_uart_comm(Communication* communication, UARTTransport* transport)
{
    communication->uart_transport = transport;
    communication->transport_kind = UART_TRANSPORT_KIND;
    return 0;
}
#endif //PROFILE_UART_TRANSPORT

intmax_t send_data(Communication* comm, const void* buf, size_t len)
{
    intmax_t result = 0;

    switch (comm->transport_kind) {
#ifdef PROFILE_UDP_TRANSPORT
        case UDP_TRANSPORT_KIND:
            result = send_udp_data(comm->udp_transport, buf, len);
            break;
#endif //PROFILE_UDP_TRANSPORT
#ifdef PROFILE_TCP_TRANSPORT
        case TCP_TRANSPORT_KIND:
            result = send_tcp_data(comm->tcp_transport, buf, len);
            break;
#endif //PROFILE_TCP_TRANSPORT
#ifdef PROFILE_UART_TRANSPORT
#endif //PROFILE_UART_TRANSPORT
        default:
            break;
    }

    return result;
}

intmax_t recv_data(Communication* comm, void** buf, size_t* len, int timeout)
{
    intmax_t result = 0;

    switch (comm->transport_kind) {
#ifdef PROFILE_UDP_TRANSPORT
        case UDP_TRANSPORT_KIND:
            result = recv_udp_data(comm->udp_transport, buf, len, timeout);
            break;
#endif //PROFILE_UDP_TRANSPORT
#ifdef PROFILE_TCP_TRANSPORT
        case TCP_TRANSPORT_KIND:
            result = recv_tcp_data(comm->tcp_transport, buf, len, timeout);
            break;
#endif //PROFILE_TCP_TRANSPORT
#ifdef PROFILE_UART_TRANSPORT
#endif //PROFILE_UART_TRANSPORT
        default:
            break;
    }

    return result;
}
