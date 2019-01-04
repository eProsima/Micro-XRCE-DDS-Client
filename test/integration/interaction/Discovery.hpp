#ifndef IN_TEST_DISCOVERY_HPP
#define IN_TEST_DISCOVERY_HPP

#include "BigHelloWorld.h"
#include "Gateway.hpp"
#include "Client.hpp"

#include <uxr/client/client.h>
#include <ucdr/microcdr.h>

#include <gtest/gtest.h>
#include <iostream>
#include <thread>

class Discovery
{
public:
    Discovery(int transport, const std::vector<uint16_t>& agent_ports)
    : agent_ports_(agent_ports)
    , transport_(transport)
    {
    }

    void unicast(const std::vector<uint16_t>& discovery_ports)
    {
        std::vector<uxrAgentAddress> agent_list;
        for(uint16_t it : discovery_ports)
        {
            agent_list.emplace_back(uxrAgentAddress{"127.0.0.1", it});
        }

        ASSERT_FALSE(uxr_discovery_agents_unicast(1, 1000, on_agent_found, this, &chosen_, agent_list.data(), agent_list.size()));
        ASSERT_TRUE(agent_ports_.empty());
    }

    void multicast()
    {
        ASSERT_FALSE(uxr_discovery_agents_multicast(1, 1000, on_agent_found, this, &chosen_));
        ASSERT_TRUE(agent_ports_.empty());
    }

private:
    static bool on_agent_found(const uxrAgentAddress* address, int64_t timestamp, void* args)
    {
        static_cast<Discovery*>(args)->on_agent_found_member(address, timestamp);
        return false;
    }

    void on_agent_found_member(const uxrAgentAddress* address, int64_t timestamp)
    {
        (void) timestamp;

        std::cout << "Agent found on port: " << address->port << std::endl;


        std::vector<uint16_t>::iterator it = std::find(agent_ports_.begin(), agent_ports_.end(), address->port);

        bool found = it != agent_ports_.end();
        if(found)
        {
            Client client(0.0f, 1);
            std::cout << "Client connecting to " << address->port << std::endl;
            client.init_transport(transport_, address->ip, address->port);
            client.close_transport(transport_);

            agent_ports_.erase(it);
        }

        //ASSERT_TRUE(found); //in multicast, it is possible to read agents out of the tests that will not be found.
    }

    std::vector<uint16_t> agent_ports_;
    uxrAgentAddress chosen_;
    int transport_;
};

#endif //IN_TEST_DISCOVERY_HPP
