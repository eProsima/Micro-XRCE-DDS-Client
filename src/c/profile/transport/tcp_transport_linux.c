#include <microxrce/client/profile/transport/tcp_transport_linux.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

/*******************************************************************************
 * Private function declarations.
 *******************************************************************************/
static bool send_tcp_msg(void* instance, const uint8_t* buf, size_t len);
static bool recv_tcp_msg(void* instance, uint8_t** buf, size_t* len, int timeout);
static int get_tcp_error(void);
static uint16_t read_tcp_data(mrTCPTransport* transport);
static inline void disconnect_tcp(mrTCPTransport* transport);

/*******************************************************************************
 * Private function definitions.
 *******************************************************************************/
bool send_tcp_msg(void* instance, const uint8_t* buf, size_t len)
{
    bool rv = true;
    mrTCPTransport* transport = (mrTCPTransport*)instance;
    uint16_t bytes_sent = 0;
    ssize_t send_rv = 0;
    uint8_t msg_size_buf[2];

    /* Send message size. */
    msg_size_buf[0] = (uint8_t)(0x00FF & len);
    msg_size_buf[1] = (uint8_t)((0xFF00 & len) >> 8);
    do
    {
        send_rv = send(transport->poll_fd.fd, msg_size_buf, 2, 0);
        if (0 <= send_rv)
        {
            bytes_sent = (uint16_t)(bytes_sent + send_rv);
        }
        else
        {
            disconnect_tcp(transport);
            rv = false;
        }
    }
    while (rv && bytes_sent != 2);

    /* Send message payload. */
    if (rv)
    {
        bytes_sent = 0;
        do
        {
            send_rv = send(transport->poll_fd.fd, buf + bytes_sent, len - bytes_sent, 0);
            if (0 <= send_rv)
            {
                bytes_sent = (uint16_t)(bytes_sent + send_rv);
            }
            else
            {
                disconnect_tcp(transport);
                rv = false;
            }
        }
        while (rv && bytes_sent != (uint16_t)len);
    }

    return rv;
}

bool recv_tcp_msg(void* instance, uint8_t** buf, size_t* len, int timeout)
{
    bool rv = false;
    mrTCPTransport* transport = (mrTCPTransport*)instance;

    int poll_rv = poll(&transport->poll_fd, 1, timeout);
    if (0 < poll_rv)
    {
        uint16_t bytes_read = read_tcp_data(transport);
        if (0 < bytes_read)
        {
            *len = (size_t)bytes_read;
            *buf = transport->input_buffer.buffer;
            rv = true;
        }
        else
        {
            rv = false;
        }
    }
    else
    {
        if (0 == poll_rv)
        {
            rv = false;
        }
    }

    return rv;
}

int get_tcp_error(void)
{
    return errno;
}

uint16_t read_tcp_data(mrTCPTransport* transport)
{
    uint16_t rv = 0;
    bool exit_flag = false;

    /* State Machine. */
    while(!exit_flag)
    {
        switch (transport->input_buffer.state)
        {
            case MR_TCP_BUFFER_EMPTY:
            {
                transport->input_buffer.position = 0;
                uint8_t size_buf[2];
                ssize_t bytes_received = recv(transport->poll_fd.fd, size_buf, 2, 0);
                if (0 < bytes_received)
                {
                    transport->input_buffer.msg_size = 0;
                    if (2 == bytes_received)
                    {
                        transport->input_buffer.msg_size = (uint16_t)(((uint16_t)size_buf[1] << 8) | size_buf[0]);
                        if (transport->input_buffer.msg_size != 0)
                        {
                            transport->input_buffer.state = MR_TCP_SIZE_READ;
                        }
                        else
                        {
                            transport->input_buffer.state = MR_TCP_BUFFER_EMPTY;
                        }
                    }
                    else
                    {
                        transport->input_buffer.msg_size = (uint16_t)size_buf[0];
                        transport->input_buffer.state = MR_TCP_SIZE_INCOMPLETE;
                    }
                }
                else
                {
                    if (0 == bytes_received)
                    {
                        errno = ENOTCONN;
                    }
                    disconnect_tcp(transport);
                    exit_flag = true;
                }
                exit_flag = (0 == poll(&transport->poll_fd, 1, 0));
                break;
            }
            case MR_TCP_SIZE_INCOMPLETE:
            {
                uint8_t size_msb;
                ssize_t bytes_received = recv(transport->poll_fd.fd, &size_msb, 1, 0);
                if (0 < bytes_received)
                {
                    transport->input_buffer.msg_size = (uint16_t)(size_msb << 8) | transport->input_buffer.msg_size;
                    if (transport->input_buffer.msg_size != 0)
                    {
                        transport->input_buffer.state = MR_TCP_SIZE_READ;
                    }
                    else
                    {
                        transport->input_buffer.state = MR_TCP_BUFFER_EMPTY;
                    }
                }
                else
                {
                    if (0 == bytes_received)
                    {
                        errno = ENOTCONN;
                    }
                    disconnect_tcp(transport);
                    exit_flag = true;
                }
                exit_flag = (0 == poll(&transport->poll_fd, 1, 0));
                break;
            }
            case MR_TCP_SIZE_READ:
            {
                ssize_t bytes_received = recv(transport->poll_fd.fd,
                                              transport->input_buffer.buffer,
                                              transport->input_buffer.msg_size, 0);
                if (0 < bytes_received)
                {
                    if ((uint16_t)bytes_received == transport->input_buffer.msg_size)
                    {
                        transport->input_buffer.state = MR_TCP_MESSAGE_AVAILABLE;
                    }
                    else
                    {
                        transport->input_buffer.position = (uint16_t)bytes_received;
                        transport->input_buffer.state = MR_TCP_MESSAGE_INCOMPLETE;
                        exit_flag = true;
                    }
                }
                else
                {
                    if (0 == bytes_received)
                    {
                        errno = ENOTCONN;
                    }
                    disconnect_tcp(transport);
                    exit_flag = true;
                }
                break;
            }
            case MR_TCP_MESSAGE_INCOMPLETE:
            {
                ssize_t bytes_received = recv(transport->poll_fd.fd,
                                              transport->input_buffer.buffer + transport->input_buffer.position,
                                              (size_t)(transport->input_buffer.msg_size - transport->input_buffer.position),
                                              0);
                if (0 < bytes_received)
                {
                    transport->input_buffer.position = (uint16_t)(transport->input_buffer.position +  bytes_received);
                    if (transport->input_buffer.position == transport->input_buffer.msg_size)
                    {
                        transport->input_buffer.state = MR_TCP_MESSAGE_AVAILABLE;
                    }
                    else
                    {
                        exit_flag = true;
                    }
                }
                else
                {
                    if (0 == bytes_received)
                    {
                        errno = ENOTCONN;
                    }
                    disconnect_tcp(transport);
                    exit_flag = true;
                }
                break;
            }
            case MR_TCP_MESSAGE_AVAILABLE:
            {
                rv = transport->input_buffer.msg_size;
                transport->input_buffer.state = MR_TCP_BUFFER_EMPTY;
                exit_flag = true;
                break;
            }
            default:
                rv = 0;
                exit_flag = true;
                break;
        }
    }

    return rv;
}

void disconnect_tcp(mrTCPTransport* transport)
{
    close(transport->poll_fd.fd);
    transport->poll_fd.fd = -1;
}

/*******************************************************************************
 * Public function definitions.
 *******************************************************************************/
bool mr_init_tcp_transport(mrTCPTransport* transport, const char* ip, uint16_t port)
{
    bool rv = false;

    /* Socket initialization. */
    transport->poll_fd.fd = socket(PF_INET, SOCK_STREAM, 0);
    if (-1 != transport->poll_fd.fd)
    {
        /* Remote IP setup. */
        struct sockaddr_in temp_addr;
        temp_addr.sin_family = AF_INET;
        temp_addr.sin_port = htons(port);
        temp_addr.sin_addr.s_addr = inet_addr(ip);
        transport->remote_addr = *((struct sockaddr *) &temp_addr);

        /* Poll setup. */
        transport->poll_fd.events = POLLIN;

        /* Server connection. */
        int connected = connect(transport->poll_fd.fd,
                                &transport->remote_addr,
                                sizeof(transport->remote_addr));
        if(0 == connected)
        {
            /* Interface setup. */
            transport->comm.instance = (void*)transport;
            transport->comm.send_msg = send_tcp_msg;
            transport->comm.recv_msg = recv_tcp_msg;
            transport->comm.comm_error = get_tcp_error;
            transport->comm.mtu = MR_CONFIG_TCP_TRANSPORT_MTU;
            rv = true;
        }
    }

    return rv;
}

bool mr_close_tcp_transport(mrTCPTransport* transport)
{
    return (0 == close(transport->poll_fd.fd));
}
