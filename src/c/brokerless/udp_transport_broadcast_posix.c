
#include <uxr/client/profile/transport/ip/ip.h>

#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#define MULTICAST_DEFAULT_IP   "239.255.0.2"
#define MULTICAST_DEFAULT_PORT 7400

int fd;
const TransportLocator multicast;

bool init_udp_broadcast_transport_datagram()
{
    fd = socket(AF_INET, SOCK_DGRAM, 0);

    int broadcastEnable=1;
    setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));

    uxr_ip_to_locator(MULTICAST_DEFAULT_IP, (uint16_t)MULTICAST_DEFAULT_PORT, UXR_IPv4, &multicast);

    return fd != -1;
}

bool close_udp_broadcast_transport_datagram()
{
    return (0 == close(fd));
}

bool udp_broadcast_send_datagram(
        const uint8_t* buf,
        size_t len)
{
    bool rv = true;
    
    printf("Sending %d bytes brokerless\n",len);
    struct sockaddr_in remote_addr;
    memcpy(&remote_addr.sin_addr, multicast._.medium_locator.address, sizeof(remote_addr.sin_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(multicast._.medium_locator.locator_port);

    ssize_t bytes_sent = sendto(fd, (const void*)buf, len, 0, (struct sockaddr*)&remote_addr, sizeof(remote_addr));
    if (0 > bytes_sent)
    {
        rv = false;
    }

    return rv;
}

bool udp_broadcast_recv_datagram(
        uint8_t* buf,
        size_t* len,
        int timeout)
{   

    size_t rv = 0;

    timeout = (timeout <= 0) ? 1 : timeout;

    struct timeval tv;
    tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    ssize_t bytes_received = recv(fd, (void*)buf, len, 0);

    printf("Received %d bytes brokerless. Timeout %d\n", bytes_received, timeout);
    
    return rv;
}

