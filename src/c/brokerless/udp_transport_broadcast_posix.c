
#include "brokerless_internal.h"

#include <uxr/client/profile/transport/ip/ip.h>

#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#define BROADCAST_DEFAULT_IP   "255.255.255.255"

int fd_recv;
int fd_send;
struct sockaddr_in send_addr, recv_addr;

bool brokerless_init_transport()
{
    int trueflag = 1;
    fd_recv = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    fd_send = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    setsockopt(fd_recv, SOL_SOCKET, SO_BROADCAST, &trueflag, sizeof trueflag);
    setsockopt(fd_send, SOL_SOCKET, SO_BROADCAST, &trueflag, sizeof trueflag);

    setsockopt(fd_recv, SOL_SOCKET, SO_REUSEADDR, &trueflag, sizeof trueflag);
    setsockopt(fd_send, SOL_SOCKET, SO_REUSEADDR, &trueflag, sizeof trueflag);

    memset(&send_addr, 0, sizeof send_addr);
    send_addr.sin_family = AF_INET;
    send_addr.sin_port = (in_port_t) htons(UCLIENT_BROKERLESS_PORT);
    inet_aton(BROADCAST_DEFAULT_IP, &send_addr.sin_addr);

    memset(&recv_addr, 0, sizeof recv_addr);
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = (in_port_t) htons(UCLIENT_BROKERLESS_PORT);
    recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(fd_recv, (struct sockaddr*) &recv_addr, sizeof recv_addr);

    return true;
}

bool brokerless_close_transport()
{
    return ((0 == close(fd_recv)) && 0 == close(fd_send));
}

size_t brokerless_broadcast_send(
        const uint8_t* buf,
        size_t len)
{
    size_t rv = 0;
       
    ssize_t bytes_sent = sendto(fd_send, buf, len, 0, (struct sockaddr*) &send_addr, sizeof(send_addr));
    if (0 > bytes_sent) {
        rv = 0;
    } else {
        rv = (size_t)bytes_sent;
    }

    return rv;
}

size_t brokerless_broadcast_recv(
        uint8_t* buf,
        size_t len,
        int timeout)
{   

    timeout = (timeout <= 0) ? 1 : timeout;

    struct timeval tv;
    tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

    setsockopt(fd_recv, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    struct sockaddr_in from;
    uint fromlen;
    fromlen = sizeof(from);

    ssize_t readed_bytes =  recvfrom(fd_recv, (void*)buf, len, 0, (struct sockaddr * restrict)&from, (socklen_t * restrict)&fromlen);

    return (readed_bytes > 0) ? (size_t)readed_bytes : 0;
}

