#include <micrortps/client/client.h>
#include <stdio.h>

#define BUFFER_SIZE 1024

int main(int args, char** argv)
{
    printf("--- XRCE CLIENT ---\n");

    // Choosing transport
    locator_id_t transport_id;

    bool request_help = true;
    if(args > 2)
    {
        if(strcmp(argv[1], "serial") == 0)
        {
            transport_id = add_serial_locator("/dev/ttyACM0");
            request_help = false;
        }
        else if(strcmp(argv[1], "udp") == 0 && args == 4)
        {
            transport_id = add_udp_locator(atoi(argv[2]), atoi(argv[3]));
            request_help = false;
        }
    }

    if(request_help)
        printf("Help: program [serial | udp recv_port send_port]");


    ClientState* state = new_client_state(transport_id, BUFFER_SIZE);

    create_xrce_client(state);

    free_client_state(state);
    rm_locator(transport_id);

    return 0;
}