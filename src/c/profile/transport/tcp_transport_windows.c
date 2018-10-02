#include <uxr/client/profile/transport/tcp_transport_windows.h>

/*******************************************************************************
 * Private function declarations.
 *******************************************************************************/
static bool send_tcp_msg(void* instance, const uint8_t* buf, size_t len);
static bool recv_tcp_msg(void* instance, uint8_t** buf, size_t* len, int timeout);
static int get_tcp_error(void);
static uint16_t read_tcp_data(uxrTCPTransport* transport);
static inline void disconnect_tcp(uxrTCPTransport* transport);

/*******************************************************************************
 * Private function definitions.
 *******************************************************************************/
bool send_tcp_msg(void* instance, const uint8_t* buf, size_t len)
{
    bool rv = true;
    uxrTCPTransport* transport = (uxrTCPTransport*)instance;
    uint16_t bytes_sent = 0;
    int send_rv = 0;
    uint8_t msg_size_buf[2];

    /* Send message size. */
    msg_size_buf[0] = (uint8_t)(0x00FF & len);
    msg_size_buf[1] = (uint8_t)((0xFF00 & len) >> 8);
    do
    {
        send_rv = send(transport->poll_fd.fd, (char*)msg_size_buf, 2, 0);
        if (SOCKET_ERROR != send_rv)
        {
            bytes_sent += (uint16_t)send_rv;
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
            send_rv = send(transport->socket_fd, (const char*)(buf + bytes_sent), (int)(len - bytes_sent), 0);
            if (SOCKET_ERROR != send_rv)
            {
                bytes_sent += (uint16_t)send_rv;
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

static bool recv_tcp_msg(void* instance, uint8_t** buf, size_t* len, int timeout)
{
    bool rv = false;
    uxrTCPTransport* transport = (uxrTCPTransport*)instance;

    int poll_rv = WSAPoll(&transport->poll_fd, 1, timeout);
    if (0 < poll_rv)
    {
        uint16_t bytes_read = read_tcp_data(transport);
        if (0 < bytes_read)
        {
            *len = (size_t)bytes_read;
            *buf = transport->input_buffer.buffer;
            rv = true;
        }
    }
    else
    {
        if (0 == poll_rv)
        {
            WSASetLastError(WAIT_TIMEOUT);
        }
    }

    return rv;
}

static int get_tcp_error(void)
{
    return WSAGetLastError();
}

uint16_t read_tcp_data(uxrTCPTransport *transport)
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
                int bytes_received = recv(transport->poll_fd.fd, (char*)size_buf, 2, 0);
                if (SOCKET_ERROR != bytes_received)
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
                        WSASetLastError(WSAENOTCONN);
                    }
                    disconnect_tcp(transport);
                    exit_flag = true;
                }
                exit_flag = (0 == WSAPoll(&transport->poll_fd, 1, 0));
                break;
            }
            case MR_TCP_SIZE_INCOMPLETE:
            {
                uint8_t size_msb;
                int bytes_received = recv(transport->poll_fd.fd, (char*)&size_msb, 1, 0);
                if (SOCKET_ERROR != bytes_received)
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
                        WSASetLastError(WSAENOTCONN);
                    }
                    disconnect_tcp(transport);
                    exit_flag = true;
                }
                exit_flag = (0 == WSAPoll(&transport->poll_fd, 1, 0));
                break;
            }
            case MR_TCP_SIZE_READ:
            {
                int bytes_received = recv(transport->poll_fd.fd,
                                              (char*)transport->input_buffer.buffer,
                                              transport->input_buffer.msg_size, 0);
                if (SOCKET_ERROR != bytes_received)
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
                        WSASetLastError(WSAENOTCONN);
                    }
                    disconnect_tcp(transport);
                    exit_flag = true;
                }
                break;
            }
            case MR_TCP_MESSAGE_INCOMPLETE:
            {
                int bytes_received = recv(transport->poll_fd.fd,
                                              (char*)transport->input_buffer.buffer + transport->input_buffer.position,
                                              (int)(transport->input_buffer.msg_size - transport->input_buffer.position),
                                              0);
                if (SOCKET_ERROR != bytes_received)
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
                        WSASetLastError(WSAENOTCONN);
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

void disconnect_tcp(uxrTCPTransport *transport)
{
    closesocket(transport->poll_fd.fd);
    transport->poll_fd.fd = INVALID_SOCKET;
}

/*******************************************************************************
 * Public function definitions.
 *******************************************************************************/
bool uxr_init_tcp_transport(uxrTCPTransport* transport, const char* ip, uint16_t port)
{
    bool rv = false;

    /* WSA initialization. */
    WSADATA wsa_data;
    if (0 != WSAStartup(MAKEWORD(2, 2), &wsa_data))
    {
        return false;
    }

    /* Socket initialization. */
    transport->socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET != transport->socket_fd)
    {
        /* Remote IP setup. */
        struct sockaddr_in temp_addr;
        temp_addr.sin_family = AF_INET;
        temp_addr.sin_port = htons(port);
        temp_addr.sin_addr.s_addr = inet_addr(ip);
        memset(temp_addr.sin_zero, '\0', sizeof(temp_addr.sin_zero));
        transport->remote_addr = *((struct sockaddr *)&temp_addr);

        /* Poll setup. */
        transport->poll_fd.fd = transport->socket_fd;
        transport->poll_fd.events = POLLIN;

        /* Server connection. */
        int connected = connect(transport->socket_fd,
                                &transport->remote_addr,
                                sizeof(transport->remote_addr));
        if (SOCKET_ERROR != connected)
        {
            /* Interface setup. */
            transport->comm.instance = (void*)transport;
            transport->comm.send_msg = send_tcp_msg;
            transport->comm.recv_msg = recv_tcp_msg;
            transport->comm.comm_error = get_tcp_error;
            transport->comm.mtu = UXR_CONFIG_TCP_TRANSPORT_MTU;
            transport->input_buffer.state = MR_TCP_BUFFER_EMPTY;
            rv = true;
        }
    }

    return rv;
}

bool uxr_close_tcp_transport(uxrTCPTransport* transport)
{
    (void)transport;
    return (0 == WSACleanup());
}
