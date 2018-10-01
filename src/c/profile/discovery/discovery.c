#include <micrortps/client/profile/discovery/discovery.h>

bool mr_discovery_agents_multicast(int time, int period, mrOnAgentFound on_agent_func, void* args, AgentAddress* choosen)
{
    (void) time; (void) period; (void) on_agent_func; (void) args; (void) choosen;
    //TODO
    return false;
}

bool mr_discovery_agents_unicast(int time, int period, const AgentAddress* agent_list, size_t agent_list_size,
                                 mrOnAgentFound on_agent_func, void* args, AgentAddress* choosen)
{
    (void) time; (void) period; (void) agent_list; (void) agent_list_size, (void) on_agent_func; (void) args; (void) choosen;
    //TODO
    return false;
}

