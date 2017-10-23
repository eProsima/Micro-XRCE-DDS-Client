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
typedef struct MicroBuffer MicroBuffer;

typedef void (*SerializeTopic)(const MicroBuffer* buffer, const void* topic_structure);
typedef void (*DeserializeTopic)(MicroBuffer* buffer, uint8_t* topic);
typedef void (*OnTopic)(const void* topic, void* callback_object);

// ----------------------------------------------------------------------------------------------
//    State funcions
// ----------------------------------------------------------------------------------------------
ClientState* new_serial_client_state(uint32_t buffer_size, const char* device);
ClientState* new_udp_client_state(uint32_t buffer_size, uint16_t recv_port, uint16_t send_port);
void free_client_state(ClientState* state);

// ----------------------------------------------------------------------------------------------
//    XRCE Client API funcions
// ----------------------------------------------------------------------------------------------
void create_client(ClientState* state);
uint16_t create_participant(ClientState* state);
uint16_t create_publisher(ClientState* state, uint16_t participant_id);
uint16_t create_subscriber(ClientState* state, uint16_t participant_id);
uint16_t create_data_writer(ClientState* state, uint16_t participant_id, uint16_t publisher_id,
        char* topic, uint32_t topic_length, SerializeTopic serialization);
uint16_t create_data_reader(ClientState* state, uint16_t participant_id, uint16_t subscriber_id,
        char* topic, uint32_t topic_length, DeserializeTopic deserialization);

void delete_resource(ClientState* state, uint16_t resource_id);

void write_data(ClientState* state, uint16_t data_writer_id, void* topic);
void read_data(ClientState* state, uint16_t data_reader_id, OnTopic on_topic, void* callback_object,
        uint16_t max_messages);

// ----------------------------------------------------------------------------------------------
//    Comunication functions
// ----------------------------------------------------------------------------------------------
void send_to_agent(ClientState* state);
void received_from_agent(ClientState* state);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_CLIENT_H_