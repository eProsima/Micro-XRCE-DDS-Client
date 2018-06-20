#include <micrortps/client/profile/transport/udp_transport_windows.h>

/*******************************************************************************
 * Private function declarations.
 *******************************************************************************/
static bool send_udp_msg(void* instance, const uint8_t* buf, size_t len);
static bool recv_udp_msg(void* instance, uint8_t** buf, size_t* len, int timeout);
static int get_udp_error();

/*******************************************************************************
 * Private function definitions.
 *******************************************************************************/
static bool send_udp_msg(void* instance, const uint8_t* buf, size_t len)
{
    bool rv = true;
    UDPTransport* transport = (UDPTransport*)instance;

    int bytes_sent = send(transport->socket_fd, (void*)buf, len, 0);
    if (0 > bytes_sent)
    {
        rv = false;
    }

    return rv;
}

static bool recv_udp_msg(void* instance, uint8_t** buf, size_t* len, int timeout)
{
    bool rv = true;
    UDPTransport* transport = (UDPTransport*)instance;

    int poll_rv = WSAPoll(&transport->poll_fd, 1, timeout);
    if (0 < poll_rv)
    {
        int bytes_received = recv(transport->socket_fd, (void*)transport->buffer, sizeof(transport->buffer), 0);
        if (SOCKET_ERROR != bytes_received)
        {
            *len = (size_t)bytes_received;
            *buf = transport->buffer;
        }
    }
    else if (0 == poll_rv)
    {
        rv = false;
        WSASetLastError(WAIT_TIMEOUT);
    }
    else
    {
        rv = false;
    }

    return rv;
}

static int get_udp_error()
{
    return WSAGetLastError();
}

/*******************************************************************************
 * Public function definitions.
 *******************************************************************************/
int init_udp_transport(UDPTransport* transport, const char* ip, uint16_t port)
{
    int result = 0;

    /* Socket initialization. */
    transport->socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (INVALID_SOCKET == transport->socket_fd)
    {
        result = WSAGetLastError();
    }

    if (0 < result)
    {
        /* Remote IP setup. */
        struct sockaddr_in temp_addr;
        temp_addr.sin_family = AF_INET;
        temp_addr.sin_port = port;
        temp_addr.sin_addr.s_addr = inet_addr(ip);
        memset(temp_addr.sin_zero, '\0', sizeof(temp_addr.sin_zero));
        transport->remote_addr = *((struct sockaddr *)&temp_addr);

        /* Poll setup. */
        transport->poll_fd.fd = transport->socket_fd;
        transport->poll_fd.events = POLLIN;

        /* Remote address filter. */
        int connected = connect(transport->socket_fd,
                                &transport->remote_addr,
                                sizeof(transport->remote_addr));
        if (-1 == connected)
        {
            result = WSAGetLastError();
        }
        else
        {
            /* Interface setup. */
            transport->comm.instance = (void*)transport;
            transport->comm.send_msg = send_udp_msg;
            transport->comm.recv_msg = recv_udp_msg;
            transport->comm.comm_error = get_udp_error;
        }
    }

    return result;
}
