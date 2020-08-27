
#include <uxr/client/profile/transport/ip/ip.h>

#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#define MULTICAST_DEFAULT_IP   "127.255.255.255"
#define MULTICAST_DEFAULT_PORT 9090

int fd;
struct sockaddr_in send_addr, recv_addr;

bool init_udp_broadcast_transport_datagram()
{
    int trueflag = 1, count = 0;
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        printf("socket");

    if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &trueflag, sizeof trueflag) < 0)
        printf("setsockopt");

    memset(&send_addr, 0, sizeof send_addr);
    send_addr.sin_family = AF_INET;
    send_addr.sin_port = (in_port_t) htons(MULTICAST_DEFAULT_PORT);
    inet_aton(MULTICAST_DEFAULT_IP, &send_addr.sin_addr);

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &trueflag, sizeof trueflag) < 0)
        printf("setsockopt");

    memset(&recv_addr, 0, sizeof recv_addr);
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = (in_port_t) htons(MULTICAST_DEFAULT_PORT);
    recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(fd, (struct sockaddr*) &recv_addr, sizeof recv_addr) < 0)
        printf("bind");

    return true;
}

bool close_udp_broadcast_transport_datagram()
{
    return (0 == close(fd));
}

size_t udp_broadcast_send_datagram(
        const uint8_t* buf,
        size_t len)
{
    size_t rv = 0;
    
    // printf("Sending %d bytes brokerless\n\t",len);
   
    ssize_t bytes_sent = sendto(fd, buf, len, 0, (struct sockaddr*) &send_addr, sizeof send_addr);
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

    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    ssize_t readed_bytes = recv(fd, (void*)buf, len, 0);

    // printf("Received %d/%d bytes brokerless. Timeout %d\n", readed_bytes, len, timeout);
    
    return (readed_bytes > 0) ? readed_bytes : 0;
}

