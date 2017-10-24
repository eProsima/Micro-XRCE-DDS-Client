#include <micrortps/client/client.h>
#include <microcdr/microcdr.h>

#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 1024

typedef struct UserData
{

} UserData;

int main(int args, char** argv)
{
    uint8_t out_buffer[BUFFER_SIZE];
    uint8_t in_buffer[BUFFER_SIZE];

    UserData data;

    OutputMessageCallback output_callback;
    output_callback.object = &data;
    output_callback.on_initialize_message = on_initialize_message;

    InputMessageCallback input_callback = {0};
    input_callback.object = &data;
    input_callback.on_message_header = on_message_header;

    OutputMessage output_message;
    init_output_message(&output_message, output_callback, out_buffer, BUFFER_SIZE);

    InputMessage input_message;
    init_input_message(&input_message, input_callback, in_buffer, BUFFER_SIZE);


    // Writing a message
    CreateResourcePayload payload;
    payload.request.base.request_id = ++state->next_request_id;
    payload.request.object_id = ++state->next_object_id;
    payload.representation.kind = OBJK_PARTICIPANT;
    payload.representation._.participant.base2.format = REPRESENTATION_BY_REFERENCE;
    payload.representation._.participant.base2._.object_name.size = 0;

    add_create_resource_submessage(&output_message, payload); //This function calls the OutputMessageCallbacks

    // Reading a message (the message must be in in_buffer.)
    uint32_t length = 0; //message length.
    parse_message(&input_message, length); //This function calls the InputMessageCallbacks

    return 0;
}

void on_initialize_message(MessageHeader* header, ClientKey* key, void* args)
{
    UserData* data = (UserData*) args;

}

int on_message_header(const MessageHeader* header, const ClientKey* key, void* args)
{
    UserData* data = (UserData*) args;



    // Important to continue serializing
    return 1;
}