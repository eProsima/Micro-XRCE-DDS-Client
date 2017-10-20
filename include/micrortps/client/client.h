#ifndef _MICRORTPS_CLIENT_CLIENT_H_
#define _MICRORTPS_CLIENT_CLIENT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

typedef enum CreateModeFlags
{
    REUSE_MODE =   0x01 << 0,
    REPLACE_MODE = 0x01 << 1
} CreateModeFlags;

typedef struct ClientState ClientState;

ClientState* new_client_state();
void free_client_state(ClientState* state);

void create_xrce_client(ClientState* state);
void create_participant(ClientState* state);

void received_data(ClientState* state);

/*void create_xrce_participant(ClientState* state);
void create_xrce_topic(ClientState* state);
void create_xrce_publisher(ClientState* state);
void create_xrce_subscriber(ClientState* state);
void create_xrce_data_writer(ClientState* state);
void create_xrce_data_reader(ClientState* state);*/

//void force_send();
//void received();

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_CLIENT_H_