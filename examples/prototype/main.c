#include <micrortps/client/client.h>
#include <stdio.h>

int main(int args, char** argv)
{
    printf("Hello World!\n");

    ClientState* state = new_client_state();

    create_xrce_client(state);

    return 0;
}