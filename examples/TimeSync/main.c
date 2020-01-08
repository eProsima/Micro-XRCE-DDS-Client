// Copyright 2017-present Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
#include <stdlib.h>
#include <inttypes.h>
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

int main(int args, char** argv)
{
    // CLI
    if(3 > args || 0 == atoi(argv[2]))
    {
        printf("usage: program [-h | --help] | ip port\n");
        return 0;
    }

    char* ip = argv[1];
    uint16_t port = (uint16_t)atoi(argv[2]);

    // Transport
    uxrUDPTransport transport;
    uxrUDPPlatform udp_platform;
    if(!uxr_init_udp_transport(&transport, &udp_platform, ip, port))
    {
        printf("Error at create transport.\n");
        return 1;
    }

    // Session
    uxrSession session;
    uxr_init_session(&session, &transport.comm, 0xCCCCDDDD);
    if(!uxr_create_session(&session))
    {
        printf("Error at create session.\n");
        return 1;
    }

    // Synchronize with the Agent
    bool synchronized = false;
    do
    {
        synchronized = uxr_sync_session(&session, 1000);
#ifdef WIN32
        printf("synchronized with time offset %-4I64d us\n", session.time_offset / 1000);
        Sleep(1);
#else
        printf("synchronized with time offset %-4" PRId64 "us\n", session.time_offset / 1000);
        sleep(1);
#endif

    } while (synchronized);
    
    // Delete resources
    uxr_delete_session(&session);
    uxr_close_udp_transport(&transport);

    return 0;
}
