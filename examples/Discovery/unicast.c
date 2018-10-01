// Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <micrortps/client/client.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_AGENTS 10

bool on_agent_found(const AgentAddress* address, int64_t timestamp, void* args)
{
    (void) timestamp; (void) args;
    printf("Found agent => ip: %s, port: %d\n", address->ip, address->port);
    return false;
}

int main(int args, char** argv)
{
    if(args >= 2 && (0 == strcmp("-h", argv[1]) || 0 == strcmp("--help", argv[1])
                || 0 != args % 2 || MAX_AGENTS * 2 < args + 2))
    {
        printf("usage: program [ -h | --help | [<ip> <port> ...] ]\n");
        return 0;
    }

    size_t agent_list_size = 0;
    AgentAddress agent_list[MAX_AGENTS];
    for(int i = 2; i < args; i += 2)
    {
        strcpy(agent_list[agent_list_size].ip, argv[i]);
        agent_list[i].port = atoi(argv[i + 1]);

        ++agent_list_size;
    }

    AgentAddress choosen;
    if(mr_discovery_agents_unicast(10000, 1000, agent_list, agent_list_size, on_agent_found, NULL, &choosen))
    {
        //true -> The user returns true in the callback.
        //false -> timeout or the user returns false in all callbacks
        printf("Choosen agent => ip: %s, port: %d\n", choosen.ip, choosen.port);
    }

    return 0;
}
