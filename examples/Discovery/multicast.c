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

bool on_agent_found(const AgentAddress* address, int64_t timestamp, void* args)
{
    (void) timestamp; (void) args;
    printf("Found agent => ip: %s, port: %d\n", address->ip, address->port);
    return false;
}

int main(int args, char** argv)
{
    (void) args; (void) argv;

    AgentAddress choosen;
    if(mr_discovery_agents_multicast(10000, 1000, on_agent_found, NULL, &choosen))
    {
        //true -> The user returns true in the callback.
        //false -> timeout
        printf("Choosen agent => ip: %s, port: %d\n", choosen.ip, choosen.port);
    }

    return 0;
}
