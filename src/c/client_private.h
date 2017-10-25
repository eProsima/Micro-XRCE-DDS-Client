#ifndef _MICRORTPS_C_CLIENT_PRIVATE_H_
#define _MICRORTPS_C_CLIENT_PRIVATE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "input_message.h"
#include "output_message.h"
#include <transport/ddsxrce_transport.h>


typedef struct Topic
{
    String name;
    SerializeTopic serialize_topic;
    DeserializeTopic deserialize_topic;

} Topic;

typedef struct DataReader
{
    uint16_t id;
    Topic* topic;

} DataReader;

typedef struct ReadRequest
{
    uint16_t data_reader_request_id;
    DataReader* data_reader;
    DataFormat data_format;
    OnTopic on_topic;
    void* args;

} ReadRequest;

typedef struct ClientState
{
    locator_id_t transport_id;

    uint32_t buffer_size;
    uint8_t* input_buffer;
    uint8_t* output_buffer;

    uint8_t session_id;
    uint8_t stream_id;
    uint16_t output_sequence_number;
    uint16_t input_sequence_number;
    ClientKey key;

    OutputMessage output_message;
    InputMessage input_message;

    uint16_t next_request_id;
    uint16_t next_object_id;

    //TODO: Do it as a key-value map
    Topic topic_vector[10];
    uint32_t topic_vector_size;

    //TODO: Do it as a key-value map
    DataReader data_reader_vector[10];
    uint32_t data_reader_vector_size;

    //TODO: Do it as a key-value map
    ReadRequest read_request_vector[10];
    uint32_t read_request_vector_size;

} ClientState;


ClientState* new_client_state(uint32_t buffer_size, locator_id_t transport_id);

void on_initialize_message(MessageHeader* header, ClientKey* key, void* args);

bool on_message_header(const MessageHeader* header, const ClientKey* key, void* args);

void on_status_submessage(const StatusPayload* payload, void* args);
DataFormat on_data_submessage(const BaseObjectReply* data_reply, void* args);

void on_data_payload(const BaseObjectReply* reply, const SampleData* data, void* args, Endianness endianness);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_C_CLIENT_PRIVATE_H_