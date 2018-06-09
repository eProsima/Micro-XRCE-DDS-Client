#include <micrortps/client/profile/session/udp_session.h>
#include <errno.h>

/*
 * Session declarations.
 */
intmax_t send_data(uint8_t* buf, size_t len);
intmax_t recv_data(uint8_t** buf, size_t* len);

/*
 * UDPSession definitions.
 */
int init_udp_session(UDPSession* udp_session, const char* ip, uint16_t port)
{
    // Session functions association.
    udp_session->core->send_data = send_data;
    udp_session->core->revc_data = recv_data;

    // Socket initialization.
    udp_session->socketfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (-1 == udp_session->sockefd)
    {
        return errno;
    }

    // Set up remote IP.
    struct sockaddr_in temp_addr;
    temp_addr.sin_family = AF_INET;
    temp_addr.sin_port = port;
    temp_addr.sin_addr.s_addr = inet_addr(ip);
    memset(temp_addr.sin_zero, '\0', sizeof(temp_addr.sin_zero));
    udp_session->remote_addr = *((struct sockaddr *) &temp_addr);

    return 0;
}

/*
 * Session definitions.
 */

