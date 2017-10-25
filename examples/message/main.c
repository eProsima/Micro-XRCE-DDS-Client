#include <micrortps/client/input_message.h>
#include <micrortps/client/output_message.h>
#include <microcdr/microcdr.h>

#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 1024

typedef struct UserData
{
    uint16_t expected_sequence_number_send;
    uint16_t expected_sequence_number_received;

} UserData;

// Callbacks definitions
void on_initialize_message(MessageHeader* header, ClientKey* key, void* args);
bool on_message_header(const MessageHeader* header, const ClientKey* key, void* args);

int main(int args, char** argv)
{
    UserData user_data;
    user_data.expected_sequence_number_send = 0;
    user_data.expected_sequence_number_received = 0;

    // ------------------------------------
    //     Writing a message
    // ------------------------------------
    uint8_t out_buffer[BUFFER_SIZE];

    OutputMessageCallback output_callback = {0};
    output_callback.object = &user_data;
    output_callback.on_initialize_message = on_initialize_message;

    OutputMessage output_message;
    init_output_message(&output_message, output_callback, out_buffer, BUFFER_SIZE);

    // Message payload
    CreateResourcePayload payload;
    payload.request.base.request_id = 0x0001;
    payload.request.object_id = 0x0001;
    payload.representation.kind = OBJK_PARTICIPANT;
    payload.representation._.participant.base2.format = REPRESENTATION_BY_REFERENCE;
    payload.representation._.participant.base2._.object_name.size = 0;

    add_create_resource_submessage(&output_message, &payload, 0);


    // ------------------------------------
    //     Reading a message
    // ------------------------------------
    uint8_t in_buffer[BUFFER_SIZE];

    InputMessageCallback input_callback = {0};
    input_callback.object = &user_data;
    input_callback.on_message_header = on_message_header;

    InputMessage input_message;
    init_input_message(&input_message, input_callback, in_buffer, BUFFER_SIZE);

    uint32_t length = 0; //message length.
    parse_message(&input_message, length);

    return 0;
}

void on_initialize_message(MessageHeader* header, ClientKey* key, void* args)
{
    printf("Message header send\n");

    UserData* user_data = (UserData*) args;
    header->sequence_nr = user_data->expected_sequence_number_send++;
}

bool on_message_header(const MessageHeader* header, const ClientKey* key, void* args)
{
    printf("Message header received\n");

    UserData* user_data = (UserData*) args;
    if(header->sequence_nr == user_data->expected_sequence_number_received)
    {
        user_data->expected_sequence_number_received++;
        return true; // Continue serializing
    }

    return false; // Stop serializing
}