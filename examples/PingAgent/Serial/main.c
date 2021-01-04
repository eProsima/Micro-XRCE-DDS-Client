// Copyright 2020 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
#include <uxr/client/util/ping.h>

#include <fcntl.h>  // O_RDWR, O_NOCTTY, O_NONBLOCK
#include <stdio.h>

int main(int argc, char** argv)
{
    // CLI
    if (2 != argc)
    {
        printf("usage: %s <dev>\n", argv[0]);
        return 1;
    }

    char* dev = argv[1];

    uxrSerialTransport transport;
    uxrSerialPlatform serial_platform;

    int fd = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);


    if (!uxr_init_serial_transport(&transport, &serial_platform, fd, 0, 1))
    {
        printf("Error during transport creation\n");
        return 1;
    }

    if (uxr_ping_agent_attempts(&transport.comm, 1000, 10))
    {
        printf("Success! Agent is up on device '%s'\n", dev);
    }
    else
    {
        printf("Sorry, no agent available at device '%s'\n", dev);
    }

    uxr_close_serial_transport(&transport);

    return 0;
}