#include "client.h"
#include "client_private.h"

#include "log/message.h"

#include <time.h>
#include <stdlib.h>

#define MAX_TOPIC_LENGTH 256

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

    init_callback_data_storage(&state->callback_data_storage, 0xFF00);

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

    free_callback_data_storage(&state->callback_data_storage);
    free_input_message(&state->input_message);
    free(state->output_buffer);
    free(state->input_buffer);
    free(state);
}

// ----------------------------------------------------------------------------------
//                              XRCE API - CLIENT TO AGENT
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

uint16_t create_topic(ClientState* state, uint16_t participant_id, String name)
{
    CreateResourcePayload payload;
    payload.request.base.request_id = ++state->next_request_id;
    payload.request.object_id = ++state->next_object_id;
    payload.representation.kind = OBJK_TOPIC;
    payload.representation._.topic.base3.format = REPRESENTATION_BY_REFERENCE;
    payload.representation._.topic.base3._.object_name.size = name.length;
    payload.representation._.topic.base3._.object_name.data = name.data;
    payload.representation._.topic.participant_id = participant_id;

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

void write_data(ClientState* state, uint16_t data_writer_id, SerializeTopic serialize_topic, void* topic)
{
    //Knows previously the topic length. Is it feasible?
    char buffer[MAX_TOPIC_LENGTH];

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

void read_data(ClientState* state, uint16_t data_reader_id, DeserializeTopic deserialize_topic,
        OnTopic on_topic, void* on_topic_args)
{
    int16_t callback_request_id = register_callback_data(&state->callback_data_storage,
            FORMAT_DATA, deserialize_topic, on_topic, on_topic_args);

    ReadDataPayload payload;
    payload.request.base.request_id = callback_request_id;
    payload.request.object_id = data_reader_id;
    payload.read_specification.optional_content_filter_expression = false;
    payload.read_specification.optional_delivery_config = FORMAT_DATA;

    add_read_data_submessage(&state->output_message, &payload);
    PRINTL_READ_DATA_SUBMESSAGE(&payload);
}

// ----------------------------------------------------------------------------------
//                              CALLBACKS - AGENT TO CLIENT
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

bool on_message_header(const MessageHeader* header, const ClientKey* key, void* vstate)
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

    CallbackData* callback_data = get_callback_data(&state->callback_data_storage, reply->base.result.request_id);
    if(callback_data)
        return callback_data->format;

    return FORMAT_BAD;
}

void on_data_payload(const BaseObjectReply* reply, const SampleData* data, void* args, Endianness endianness)
{
    ClientState* state = (ClientState*) args;
    PRINTL_DATA_SUBMESSAGE_SAMPLE_DATA(reply, data);

    CallbackData* callback_data = get_callback_data(&state->callback_data_storage, reply->base.result.request_id);
    if(callback_data)
    {
        MicroBuffer reader;
        init_external_buffer(&reader, data->data, data->size);
        reader.endianness = endianness;

        AbstractTopic abstract_topic;
        if(callback_data->deserialize_topic(&reader, &abstract_topic))
            callback_data->on_topic(abstract_topic.topic, callback_data->on_topic_args);

        remove_callback_data(&state->callback_data_storage, reply->base.result.request_id);
    }
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

// ----------------------------------------------------------------------------------------------
//    Data callback funcions
// ----------------------------------------------------------------------------------------------

void init_callback_data_storage(CallbackDataStorage* store, int16_t initial_id)
{
    store->size = 1;
    store->initial_id = initial_id;
    store->existence = calloc(store->size, sizeof(bool));
    store->callbacks = malloc(sizeof(CallbackData) * store->size);
}

void free_callback_data_storage(CallbackDataStorage* store)
{
    free(store->callbacks);
    free(store->existence);
}

uint16_t register_callback_data(CallbackDataStorage* store, uint8_t format, DeserializeTopic deserialize_topic,
        OnTopic on_topic, void* on_topic_args)
{
    uint16_t id = store->size;
    for(uint32_t i = 0; i < store->size; i++)
        if(!store->existence[i])
            id = i;

    if(id == store->size)
    {
        uint32_t new_size = store->size * 2;
        store->existence = realloc(store->existence, sizeof(bool) *new_size);
        store->callbacks = realloc(store->callbacks, sizeof(CallbackData) * new_size);

        for(uint32_t i = store->size; i < new_size; i++)
            store->existence[i] = false;

        store->size = new_size;
    }

    store->existence[id] = true;
    store->callbacks[id] = (CallbackData){format, deserialize_topic, on_topic, on_topic_args};

    return store->initial_id + id;
}

void remove_callback_data(CallbackDataStorage* store, uint16_t id)
{
    if(store->size + store->initial_id > id)
        store->existence[id - store->initial_id] = false;
}

CallbackData* get_callback_data(const CallbackDataStorage* store, uint16_t id)
{
    if(store->size + store->initial_id > id)
        return &store->callbacks[id - store->initial_id];
    return NULL;
}