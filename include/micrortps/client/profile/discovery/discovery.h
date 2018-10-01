// Copyright 2018 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _MICRORTPS_CLIENT_PROFILE_DISCOVERY_DISCOVERY_H_
#define _MICRORTPS_CLIENT_PROFILE_DISCOVERY_DISCOVERY_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <micrortps/client/config.h>

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct AgentAddress
{
    char ip[16]; //xxx.xxx.xxx.xxx\0
    uint16_t port;
} AgentAddress;

typedef bool (*mrOnAgentFound) (const AgentAddress* address, int64_t timestamp, void* args);

MRDLLAPI bool mr_discovery_agents_multicast(int time, int period, mrOnAgentFound on_agent_func, void* args, AgentAddress* choosen);

MRDLLAPI bool mr_discovery_agents_unicast(int time, int period, const AgentAddress* agent_list, size_t agent_list_size,
                                          mrOnAgentFound on_agent_func, void* args, AgentAddress* choosen);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_PROFILE_DISCOVERY_DISCOVERY_H_
