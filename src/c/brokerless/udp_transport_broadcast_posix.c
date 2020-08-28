
#include <uxr/client/profile/transport/ip/ip.h>

#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#define BROADCAST_DEFAULT_IP   "255.255.255.255"
#define BROADCAST_DEFAULT_PORT 9000

int fd_recv;
int fd_send;
struct sockaddr_in send_addr, recv_addr;

bool init_udp_broadcast_transport_datagram()
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
    send_addr.sin_port = (in_port_t) htons(BROADCAST_DEFAULT_PORT);
    inet_aton(BROADCAST_DEFAULT_IP, &send_addr.sin_addr);

    memset(&recv_addr, 0, sizeof recv_addr);
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = (in_port_t) htons(BROADCAST_DEFAULT_PORT);
    recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(fd_recv, (struct sockaddr*) &recv_addr, sizeof recv_addr);

    return true;
}

bool close_udp_broadcast_transport_datagram()
{
    return ((0 == close(fd_recv)) && 0 == close(fd_send));
}

size_t udp_broadcast_send_datagram(
        const uint8_t* buf,
        size_t len)
{
    size_t rv = 0;
    
    // printf("Sending %d bytes brokerless\n",len);
   
    ssize_t bytes_sent = sendto(fd_send, buf, len, 0, (struct sockaddr*) &send_addr, sizeof(send_addr));
    if (0 > bytes_sent) {
        rv = 0;
    } else {
        rv = bytes_sent;
    }

    return rv;
}

size_t udp_broadcast_recv_datagram(
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
    int fromlen;
    fromlen = sizeof(from);

    ssize_t readed_bytes =  recvfrom(fd_recv, (void*)buf, len, 0, &from, &fromlen);

    // if(readed_bytes != -1)
    //     printf("Received %d/%d bytes brokerless from %s:%d Timeout %d\n", readed_bytes, len, inet_ntoa(from.sin_addr), ntohs(from.sin_port), timeout);
    
    return (readed_bytes > 0) ? readed_bytes : 0;
}

