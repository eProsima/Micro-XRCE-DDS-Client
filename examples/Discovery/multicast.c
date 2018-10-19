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

#include <uxr/client/client.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

bool on_agent_found(const uxrAgentAddress* address, int64_t timestamp, void* args)
{
    (void) timestamp; (void) args;
    printf("Found agent => ip: %s, port: %d\n", address->ip, address->port);
    return false;
}

int main(int args, char** argv)
{
    (void) args; (void) argv;

    uxrAgentAddress chosen;
    if(uxr_discovery_agents_multicast(10, 1000, on_agent_found, NULL, &chosen))
    {
        // True -> The user returns true in the callback.
        printf("Chosen agent => ip: %s, port: %d\n", chosen.ip, chosen.port);
    }

    return 0;
}
