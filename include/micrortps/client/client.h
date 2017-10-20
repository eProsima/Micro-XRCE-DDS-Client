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

// State funcions
ClientState* new_serial_client_state(uint32_t buffer_size, const char* device);
ClientState* new_udp_client_state(uint32_t buffer_size, uint16_t recv_port, uint16_t send_port);
void free_client_state(ClientState* state);

// XRCE Client API funcions
void create_xrce_client(ClientState* state);
void create_participant(ClientState* state);

// Comunication functions
void send_to_agent(ClientState* state);
void received_from_agent(ClientState* state);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_CLIENT_H_