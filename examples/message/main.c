// Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <micrortps/client/input_message.h>
#include <micrortps/client/client.h>
#include <micrortps/client/output_message.h>
#include <microcdr/microcdr.h>

#include <stdio.h>
#include <string.h>

typedef struct UserData
{
    uint16_t expected_sequence_number_send;
    uint16_t expected_sequence_number_received;

} UserData;

// Callbacks definitions
void on_initialize_message(MessageHeader *header, ClientKey *key, void *args);
bool on_message_header(const MessageHeader *header, const ClientKey *key, void *args);

int main(int args, char **argv)
{
    (void)args;
    (void)argv;
    UserData user_data;
    user_data.expected_sequence_number_send = 0;
    user_data.expected_sequence_number_received = 0;

    // ------------------------------------
    //     Writing a message
    // ------------------------------------
    uint8_t out_buffer[MAX_MESSAGE_SIZE];

    OutputMessageCallback output_callback = {0};
    output_callback.args = &user_data;
    output_callback.on_initialize_message = on_initialize_message;

    OutputMessage output_message;
    init_output_message(&output_message, output_callback, out_buffer, MAX_MESSAGE_SIZE);

    // Message payload
    CreateResourcePayload payload;
    payload.request.base.request_id = (RequestId){{0x00, 0x01}};
    payload.request.object_id = (ObjectId){{0x00, 0x01}};
    payload.representation.kind = OBJK_PARTICIPANT;
    payload.representation._.participant.base2.format = REPRESENTATION_BY_REFERENCE;
    payload.representation._.participant.base2._.object_name.size = 0;

    add_create_resource_submessage(&output_message, &payload, 0);

    // ------------------------------------
    //     Reading a message
    // ------------------------------------
    uint8_t in_buffer[MAX_MESSAGE_SIZE];

    InputMessageCallback input_callback = {0};
    input_callback.args = &user_data;
    input_callback.on_message_header = on_message_header;

    InputMessage input_message;
    init_input_message(&input_message, input_callback, in_buffer, MAX_MESSAGE_SIZE);

    uint32_t length = 0; //message length.
    parse_message(&input_message, length);

    return 0;
}

void on_initialize_message(MessageHeader *header, ClientKey *key, void *args)
{
    (void)key;
    printf("Message header send\n");

    UserData *user_data = (UserData *)args;
    header->sequence_nr = user_data->expected_sequence_number_send++;
}

bool on_message_header(const MessageHeader *header, const ClientKey *key, void *args)
{
    (void)key;
    printf("Message header received\n");

    UserData *user_data = (UserData *)args;
    if (header->sequence_nr == user_data->expected_sequence_number_received)
    {
        user_data->expected_sequence_number_received++;
        return true; // Continue serializing
    }

    return false; // Stop serializing
}