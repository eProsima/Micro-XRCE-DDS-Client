#include "client.h"

#include "input_message.h"
#include "output_message.h"

#include "log/message.h"

#include <time.h>
#include <stdlib.h>
#include <microcdr/microcdr.h>
#include <transport/ddsxrce_transport.h>

#ifndef NDEBUG
#include <string.h>
#endif

#define MAX_TOPIC_LENGTH 256

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

int on_message_header(const MessageHeader* header, const ClientKey* key, void* args);

void on_status_submessage(const StatusPayload* payload, void* args);
DataFormat on_data_submessage(const BaseObjectReply* data_reply, void* args);

void on_data_payload(const BaseObjectReply* reply, const SampleData* data, void* args, Endianness endianness);

// ----------------------------------------------------------------------------------
//                                  CLIENT STATE
// ----------------------------------------------------------------------------------
ClientState* new_serial_client_state(uint32_t buffer_size, const char* device)
{
    return new_client_state(buffer_size, add_serial_locator(device));
}

ClientState* new_udp_client_state(uint32_t buffer_size, uint16_t recv_port, uint16_t send_port)
{
    return new_client_state(buffer_size, add_udp_locator(recv_port, send_port));
}

ClientState* new_client_state(uint32_t buffer_size, locator_id_t transport_id)
{
    ClientState* state = malloc(sizeof(ClientState));

    state->transport_id = transport_id;

    state->buffer_size = buffer_size;
    state->input_buffer = malloc(buffer_size);
    state->output_buffer = malloc(buffer_size);

    state->session_id = SESSIONID_NONE_WITH_CLIENT_KEY;
    state->stream_id = STREAMID_NONE;
    state->output_sequence_number = 0;
    state->input_sequence_number = 0;
    state->key = (ClientKey){0xFF, 0xFF, 0xFF, 0xFF};

    OutputMessageCallback output_callback = {0};
    output_callback.object = state;
    output_callback.on_initialize_message = on_initialize_message;

    InputMessageCallback input_callback = {0};
    input_callback.object = state;
    input_callback.on_message_header = on_message_header;
    input_callback.on_status_submessage = on_status_submessage;
    input_callback.on_data_submessage = on_data_submessage;
    input_callback.on_data_payload = on_data_payload;

    init_output_message(&state->output_message, output_callback, state->output_buffer, buffer_size);
    init_input_message(&state->input_message, input_callback, state->input_buffer, buffer_size);

    state->next_request_id = 0;
    state->next_object_id = 0;

    state->topic_vector_size = 0;
    state->read_request_vector_size = 0;

    //Default message encoding
    state->output_message.writer.endianness = LITTLE_ENDIANNESS;

    #ifndef NDEBUG
    memset(state->output_buffer, 0, state->buffer_size);
    memset(state->input_buffer, 0, state->buffer_size);
    #endif

    return state;
}

void free_client_state(ClientState* state)
{
    rm_locator(state->transport_id);
    free_input_message(&state->input_message);
    free(state->output_buffer);
    free(state->input_buffer);
    free(state);
}

// ----------------------------------------------------------------------------------
//                                    XRCE API
// ----------------------------------------------------------------------------------

void create_client(ClientState* state)
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);

    CreateClientPayload payload;
    payload.representation.xrce_cookie = XRCE_COOKIE;
    payload.representation.xrce_version = XRCE_VERSION;
    payload.representation.xrce_vendor_id = (XrceVendorId){0x01, 0x0F};
    payload.representation.client_timestamp.seconds = ts.tv_sec;
    payload.representation.client_timestamp.nanoseconds = ts.tv_nsec;
    payload.representation.client_key = (ClientKey){0xFF, 0xFF, 0xFF, 0xFF};
    payload.representation.session_id = 0x01;

    add_create_client_submessage(&state->output_message, &payload);
    PRINTL_CREATE_CLIENT_SUBMESSAGE(&payload);
}

uint16_t create_participant(ClientState* state)
{
    CreateResourcePayload payload;
    payload.request.base.request_id = ++state->next_request_id;
    payload.request.object_id = ++state->next_object_id;
    payload.representation.kind = OBJK_PARTICIPANT;
    payload.representation._.participant.base2.format = REPRESENTATION_BY_REFERENCE;
    payload.representation._.participant.base2._.object_name.size = 0;

    add_create_resource_submessage(&state->output_message, &payload, 0);
    PRINTL_CREATE_RESOURCE_SUBMESSAGE(&payload);

    return payload.request.object_id;
}

uint16_t create_publisher(ClientState* state, uint16_t participant_id)
{
    CreateResourcePayload payload;
    payload.request.base.request_id = ++state->next_request_id;
    payload.request.object_id = ++state->next_object_id;
    payload.representation.kind = OBJK_PUBLISHER;
    payload.representation._.publisher.base3.format = REPRESENTATION_BY_REFERENCE;
    payload.representation._.publisher.base3._.object_name.size = 0;
    payload.representation._.publisher.participant_id = participant_id;

    add_create_resource_submessage(&state->output_message, &payload, 0);
    PRINTL_CREATE_RESOURCE_SUBMESSAGE(&payload);

    return payload.request.object_id;
}

uint16_t create_subscriber(ClientState* state, uint16_t participant_id)
{
    CreateResourcePayload payload;
    payload.request.base.request_id = ++state->next_request_id;
    payload.request.object_id = ++state->next_object_id;
    payload.representation.kind = OBJK_SUBSCRIBER;
    payload.representation._.subscriber.base3.format = REPRESENTATION_BY_REFERENCE;
    payload.representation._.subscriber.base3._.object_name.size = 0;
    payload.representation._.subscriber.participant_id = participant_id;

    add_create_resource_submessage(&state->output_message, &payload, 0);
    PRINTL_CREATE_RESOURCE_SUBMESSAGE(&payload);

    return payload.request.object_id;
}

uint16_t create_topic(ClientState* state, uint16_t participant_id, String name,
        SerializeTopic serialization, DeserializeTopic deserialization)
{
    CreateResourcePayload payload;
    payload.request.base.request_id = ++state->next_request_id;
    payload.request.object_id = ++state->next_object_id;
    payload.representation.kind = OBJK_TOPIC;
    payload.representation._.topic.base3.format = REPRESENTATION_BY_REFERENCE;
    payload.representation._.topic.base3._.object_name.size = name.length;
    payload.representation._.topic.base3._.object_name.data = name.data;
    payload.representation._.topic.participant_id = participant_id;

    //TODO as key-value map
    state->topic_vector[state->topic_vector_size++] =
            (Topic){name, serialization, deserialization};

    add_create_resource_submessage(&state->output_message, &payload, 0);
    PRINTL_CREATE_RESOURCE_SUBMESSAGE(&payload);

    return payload.request.object_id;
}

uint16_t create_data_writer(ClientState* state, uint16_t participant_id, uint16_t publisher_id, String topic_name)
{
    CreateResourcePayload payload;
    payload.request.base.request_id = ++state->next_request_id;
    payload.request.object_id = ++state->next_object_id;
    payload.representation.kind = OBJK_DATAWRITER;
    payload.representation._.data_writer.base3.format = REPRESENTATION_BY_REFERENCE;
    payload.representation._.data_writer.base3._.object_name.size = topic_name.length;
    payload.representation._.data_writer.base3._.object_name.data = topic_name.data;
    payload.representation._.data_writer.participant_id = participant_id;
    payload.representation._.data_writer.publisher_id = publisher_id;

    add_create_resource_submessage(&state->output_message, &payload, 0);
    PRINTL_CREATE_RESOURCE_SUBMESSAGE(&payload);

    return payload.request.object_id;
}

uint16_t create_data_reader(ClientState* state, uint16_t participant_id, uint16_t subscriber_id, String topic_name)
{
    CreateResourcePayload payload;
    payload.request.base.request_id = ++state->next_request_id;
    payload.request.object_id = ++state->next_object_id;
    payload.representation.kind = OBJK_DATAREADER;
    payload.representation._.data_reader.base3.format = REPRESENTATION_BY_REFERENCE;
    payload.representation._.data_reader.base3._.object_name.size = topic_name.length;
    payload.representation._.data_reader.base3._.object_name.data = topic_name.data;
    payload.representation._.data_reader.participant_id = participant_id;
    payload.representation._.data_reader.subscriber_id = subscriber_id;

    //TODO as key-value map
    Topic* topic = NULL;
    for(uint32_t i = 0; i < state->topic_vector_size && !topic; i++)
        if(strcmp(state->topic_vector[i].name.data, topic_name.data) == 0)
            topic = &state->topic_vector[i];

    state->data_reader_vector[state->data_reader_vector_size++] =
            (DataReader){state->next_object_id, topic};

    add_create_resource_submessage(&state->output_message, &payload, 0);
    PRINTL_CREATE_RESOURCE_SUBMESSAGE(&payload);

    return payload.request.object_id;
}

void delete_resource(ClientState* state, uint16_t resource_id)
{
    DeleteResourcePayload payload;
    payload.request.base.request_id = ++state->next_request_id;
    payload.request.object_id = resource_id;

    add_delete_resource_submessage(&state->output_message, &payload);
    PRINTL_DELETE_RESOURCE_SUBMESSAGE(&payload);
}

void write_data(ClientState* state, uint16_t data_writer_id, void* topic)
{
    //TODO: change this, serialize directly in the message
    char buffer[MAX_TOPIC_LENGTH];
    SerializeTopic serialize_topic = NULL;
    for(uint32_t i = 0; i < state->topic_vector_size && !serialize_topic; i++)
        serialize_topic = state->topic_vector[i].serialize_topic;

    MicroBuffer writer;
    init_external_buffer(&writer, buffer, MAX_TOPIC_LENGTH);
    writer.endianness = state->output_message.writer.endianness;

    if(serialize_topic(&writer, &(AbstractTopic){topic}))
    {
        WriteDataPayload payload;
        payload.request.base.request_id = ++state->next_request_id;
        payload.request.object_id = data_writer_id;
        payload.data_to_write.format = FORMAT_DATA;
        payload.data_to_write._.data.size = writer.iterator - writer.init;
        payload.data_to_write._.data.data = writer.init;

        add_write_data_submessage(&state->output_message, &payload);
        PRINTL_WRITE_DATA_SUBMESSAGE(&payload);
    }
}

void read_data(ClientState* state, uint16_t data_reader_id, OnTopic on_topic, void* args,
        uint16_t max_samples)
{
    ReadDataPayload payload;
    payload.request.base.request_id = ++state->next_request_id;
    payload.request.object_id = data_reader_id;
    payload.read_specification.optional_content_filter_expression = false;
    payload.read_specification.optional_delivery_config = FORMAT_DATA;
    payload.read_specification.delivery_config.max_samples = max_samples;
    payload.read_specification.delivery_config.max_elapsed_time = 0;
    payload.read_specification.delivery_config.max_rate = 0;

    //TODO as key-value map
    DataReader* data_reader = NULL;
    for(uint32_t i = 0; i < state->data_reader_vector_size && !data_reader; i++)
        if(state->data_reader_vector[i].id == data_reader_id)
            data_reader = &state->data_reader_vector[i];

    state->read_request_vector[state->read_request_vector_size++] =
            (ReadRequest){payload.request.base.request_id, data_reader,
            payload.read_specification.optional_delivery_config, on_topic, args};

    add_read_data_submessage(&state->output_message, &payload);
    PRINTL_READ_DATA_SUBMESSAGE(&payload);
}

// ----------------------------------------------------------------------------------
//                                   CALLBACKS
// ----------------------------------------------------------------------------------

void on_initialize_message(MessageHeader* header, ClientKey* key, void* vstate)
{
    ClientState* state = (ClientState*) vstate;

    header->session_id = state->session_id;
    header->stream_id = state->stream_id;
    header->sequence_nr = state->output_sequence_number;
    *key = state->key;

    state->output_sequence_number++;
}

int on_message_header(const MessageHeader* header, const ClientKey* key, void* vstate)
{
    ClientState* state = (ClientState*) vstate;

    if(header->stream_id != STREAMID_NONE)
        if(header->sequence_nr != state->input_sequence_number)
            return 0;

    state->input_sequence_number++;

    return 1;
}

void on_status_submessage(const StatusPayload* payload, void* args)
{
    ClientState* state = (ClientState*) args;
    PRINTL_STATUS_SUBMESSAGE(payload);
}

DataFormat on_data_submessage(const BaseObjectReply* reply, void* args)
{
    ClientState* state = (ClientState*) args;

    //TODO as key-value map
    for(uint32_t i = 0; i < state->read_request_vector_size; i++)
        if(state->read_request_vector[i].data_reader_request_id == reply->base.result.request_id)
            return state->read_request_vector[i].data_format;
}

void on_data_payload(const BaseObjectReply* reply, const SampleData* data, void* args, Endianness endianness)
{
    ClientState* state = (ClientState*) args;

    PRINTL_DATA_SUBMESSAGE_SAMPLE_DATA(reply, data);

    OnTopic on_topic = NULL;
    void* topic_object = NULL;
    DeserializeTopic deserialize_topic = NULL;

    //TODO as key-value map
    for(uint32_t i = 0; i < state->read_request_vector_size; i++)
        if(state->read_request_vector[i].data_reader_request_id == reply->base.request_id)
        {
            DataReader* data_reader = state->read_request_vector[i].data_reader;
            on_topic = state->read_request_vector[i].on_topic;
            topic_object = state->read_request_vector[i].args;

            if(data_reader && data_reader->topic)
                deserialize_topic = data_reader->topic->deserialize_topic;
        }

    MicroBuffer reader;
    init_external_buffer(&reader, data->data, data->size);
    reader.endianness = endianness;

    AbstractTopic abstract_topic;
    if(deserialize_topic(&reader, &abstract_topic))
        on_topic(abstract_topic.topic, topic_object);
}

// ----------------------------------------------------------------------------------
//                                 COMUNICATION
// ----------------------------------------------------------------------------------

void send_to_agent(ClientState* state)
{
    uint32_t length = get_message_length(&state->output_message);
    if(length > 0)
    {
        int output_length = send_data(state->output_buffer, length, state->transport_id);
        if(output_length > 0)
        {
            PRINTL_SERIALIZATION(SEND, state->output_buffer, output_length);
            #ifndef NDEBUG
            memset(state->output_buffer, 0, state->buffer_size);
            #endif
        }

        reset_buffer(&state->output_message.writer);
    }
}

void receive_from_agent(ClientState* state)
{
    uint32_t length = receive_data(state->input_buffer, state->buffer_size, state->transport_id);
    if(length > 0)
    {
        #ifndef NDEBUG
        memset(state->input_buffer, 0, state->buffer_size);
        #endif

        parse_message(&state->input_message, length);

        PRINTL_SERIALIZATION(RECV, state->input_buffer, length);
    }
}