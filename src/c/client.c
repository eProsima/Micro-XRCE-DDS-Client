#include "micrortps/client/client.h"

#include "micrortps/client/input_message.h"
#include "micrortps/client/output_message.h"

#include <time.h>
#include <stdlib.h>

// aux includes
#include <stdio.h>

#define BUFFER_LENGTH 1024

typedef struct ClientState
{
    uint8_t* input_buffer;
    uint8_t* output_buffer;

    uint8_t session_id;
    uint8_t stream_id;
    uint16_t output_sequence_number;
    uint16_t input_sequence_number;
    ClientKey key;

    OutputMessage output_message;
    InputMessage input_message;

    InputMessageCallback input_callback;

} ClientState;

void on_initialize_message(MessageHeader* header, ClientKey* key, void* vstate);
int on_message_header(const MessageHeader* header, const ClientKey* key, void* vstate);


// ----------------------------------------------------------------------------------
//                                 PUBLIC API
// ----------------------------------------------------------------------------------
ClientState* new_client_state()
{
    ClientState* state = malloc(sizeof(ClientState));

    state->input_buffer = malloc(BUFFER_LENGTH);
    state->output_buffer = malloc(BUFFER_LENGTH);

    state->session_id = SESSIONID_NONE_WITH_CLIENT_KEY;
    state->stream_id = STREAMID_NONE;
    state->output_sequence_number = 0;
    state->input_sequence_number = 0;
    state->key = (ClientKey){0xFF, 0xFF, 0xFF, 0xFF};

    OutputMessageCallback output_callback;
    output_callback.object = state;
    output_callback.on_initialize_message = on_initialize_message;

    InputMessageCallback input_callback = {0};
    input_callback.object = state;
    input_callback.on_message_header = on_message_header;

    init_output_message(&state->output_message, output_callback, state->output_buffer, BUFFER_LENGTH);
    init_input_message(&state->input_message, input_callback, state->input_buffer, BUFFER_LENGTH);

    return state;
}

void free_client_state(ClientState* state)
{
    free_input_message(&state->input_message);
    free(state->output_buffer);
    free(state->input_buffer);
    free(state);
}

void create_xrce_client(ClientState* state)
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
}

void create_participant(ClientState* state)
{

}

void received_data(ClientState* state)
{
    parse_message(&state->input_message, BUFFER_LENGTH);
}

// ----------------------------------------------------------------------------------
//                                 CALLBACKS
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