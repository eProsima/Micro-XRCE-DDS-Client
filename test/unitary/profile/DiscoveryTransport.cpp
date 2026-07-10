#include <gtest/gtest.h>

extern "C"
{
#include <c/profile/discovery/transport/udp_transport_datagram_internal.h>
}

TEST(DiscoveryTransportTest, ConvertsIpv4AddressBytes)
{
    const uint8_t address[] = {192, 168, 1, 42};
    char ip[16] = {};

    uxr_bytes_to_ip(address, ip);

    EXPECT_STREQ("192.168.1.42", ip);
}
