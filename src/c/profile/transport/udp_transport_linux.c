#include <uxr/client/profile/transport/udp_transport_linux.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

/*******************************************************************************
 * Private function declarations.
 *******************************************************************************/
static bool send_udp_msg(void* instance, const uint8_t* buf, size_t len);
static bool recv_udp_msg(void* instance, uint8_t** buf, size_t* len, int timeout);
static int get_udp_error(void);

/*******************************************************************************
 * Private function definitions.
 *******************************************************************************/
bool send_udp_msg(void* instance, const uint8_t* buf, size_t len)
{
    bool rv = true;
    uxrUDPTransport* transport = (uxrUDPTransport*)instance;

    ssize_t bytes_sent = send(transport->poll_fd.fd, (void*)buf, len, 0);
    if (0 > bytes_sent)
    {
        rv = false;
    }

    return rv;
}

bool recv_udp_msg(void* instance, uint8_t** buf, size_t* len, int timeout)
{
    bool rv = false;
    uxrUDPTransport* transport = (uxrUDPTransport*)instance;

    int poll_rv = poll(&transport->poll_fd, 1, timeout);
    if (0 < poll_rv)
    {
        ssize_t bytes_received = recv(transport->poll_fd.fd, (void*)transport->buffer, sizeof(transport->buffer), 0);
        if (0 < bytes_received)
        {
            *len = (size_t)bytes_received;
            *buf = transport->buffer;
            rv = true;
        }
    }
    else
    {
        if (0 == poll_rv)
        {
            errno = ETIME;
        }
    }

    return rv;
}

int get_udp_error(void)
{
    return errno;
}

/*******************************************************************************
 * Public function definitions.
 *******************************************************************************/
bool uxr_init_udp_transport(uxrUDPTransport* transport, const char* ip, uint16_t port)
{
    bool rv = false;

    /* Socket initialization */
    int fd = socket(PF_INET, SOCK_DGRAM, 0);
    if (-1 != fd)
    {
        /* Remote IP setup. */
        struct sockaddr_in remote_addr;
        if(0 != inet_aton(ip, &remote_addr.sin_addr))
        {
            remote_addr.sin_family = AF_INET;
            remote_addr.sin_port = htons(port);

            /* Poll setup. */
            transport->poll_fd.fd = fd;
            transport->poll_fd.events = POLLIN;

            /* Remote address filter. */
            int connected = connect(fd, (struct sockaddr*)&remote_addr, sizeof(remote_addr));
            if (0 == connected)
            {
                /* Interface setup. */
                transport->comm.instance = (void*)transport;
                transport->comm.send_msg = send_udp_msg;
                transport->comm.recv_msg = recv_udp_msg;
                transport->comm.comm_error = get_udp_error;
                transport->comm.mtu = UXR_CONFIG_UDP_TRANSPORT_MTU;
                rv = true;
            }
        }
    }

    return rv;
}

bool uxr_close_udp_transport(uxrUDPTransport* transport)
{
    return (0 == close(transport->poll_fd.fd));
}
