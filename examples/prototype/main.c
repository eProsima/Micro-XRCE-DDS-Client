#include <micrortps/client/client.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main(int args, char** argv)
{
    printf("--- XRCE CLIENT ---\n");

    ClientState* state = NULL;
    if(args > 2)
    {
        if(strcmp(argv[1], "serial") == 0)
        {
            state = new_serial_client_state(BUFFER_SIZE, "/dev/ttyACM0");
        }
        else if(strcmp(argv[1], "udp") == 0 && args == 4)
        {
            state = new_udp_client_state(BUFFER_SIZE, 2000, 2001);
        }
    }
    if(!state)
        printf("Help: program [serial | udp recv_port send_port]");

    create_client(state);
    send_to_agent(state);

    create_participant(state);
    send_to_agent(state);

    free_client_state(state);

    return 0;
}