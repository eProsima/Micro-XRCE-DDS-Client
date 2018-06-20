#include <micrortps/client/profile/transport/udp_transport_linux.h>

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

    ssize_t bytes_sent = send(transport->socket_fd, (void*)buf, len, 0);
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

    int poll_rv = poll(&transport->poll_fd, 1, timeout);
    if (0 < poll_rv)
    {
        ssize_t bytes_received = recv(transport->socket_fd, (void*)transport->buffer, sizeof(transport->buffer), 0);
        if (0 <= bytes_received)
        {
            *len = (size_t)bytes_received;
            *buf = transport->buffer;
        }
    }
    else if (0 == poll_rv)
    {
        rv = false;
        errno = ETIME;
    }
    else
    {
        rv = false;
    }

    return rv;
}

static int get_udp_error()
{
    return errno;
}

/*******************************************************************************
 * Public function definitions.
 *******************************************************************************/
int init_udp_transport(UDPTransport* transport, const char* ip, uint16_t port)
{
    int rv = 0;

    /* Socket initialization */
    transport->socket_fd = socket(PF_INET, SOCK_DGRAM, 0);
    if (-1 == transport->socket_fd)
    {
        rv = errno;
    }

    if (0 < rv)
    {
        /* Remote IP setup. */
        struct sockaddr_in temp_addr;
        temp_addr.sin_family = AF_INET;
        temp_addr.sin_port = port;
        temp_addr.sin_addr.s_addr = inet_addr(ip);
        memset(temp_addr.sin_zero, '\0', sizeof(temp_addr.sin_zero));
        transport->remote_addr = *((struct sockaddr *) &temp_addr);

        /* Poll setup. */
        transport->poll_fd.fd = transport->socket_fd;
        transport->poll_fd.events = POLLIN;

        /* Remote address filter. */
        int connected = connect(transport->socket_fd,
                                &transport->remote_addr,
                                sizeof(transport->remote_addr));
        if (-1 == connected)
        {
            rv = errno;
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

    return rv;
}
