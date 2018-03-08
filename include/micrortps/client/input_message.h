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

#ifndef _MICRORTPS_CLIENT_INPUT_MESSAGE_H_
#define _MICRORTPS_CLIENT_INPUT_MESSAGE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "xrce_protocol_spec.h"

#include <microcdr/microcdr.h>


typedef struct InputMessageCallback
{
    // Headers
    bool  (*on_message_header)   (const MessageHeader* header, const ClientKey* key, void* args);
    void (*on_submessage_header)(const SubmessageHeader* header, void* args);

    // Submessages
    void (*on_status_submessage)(const STATUS_Payload* payload, void* args);
    void (*on_info_submessage)  (const INFO_Payload* payload, void* args);
    DataFormat (*on_data_submessage)  (const BaseObjectReply* reply, void* args);

    // Data payloads
    void (*on_data_payload)           (const BaseObjectReply* reply, const SampleData* data,
            void* args, Endianness endianness);
    void (*on_sample_payload)         (const BaseObjectReply* reply, const Sample* sample,
            void* args, Endianness endianness);
    void (*on_data_sequence_payload)  (const BaseObjectReply* reply, const SampleDataSeq* data_sequence,
            void* args, Endianness endianness);
    void (*on_sample_sequence_payload)(const BaseObjectReply* reply, const SampleSeq* sample_sequence,
            void* args, Endianness endianness);
    void (*on_packed_samples_payload) (const BaseObjectReply* reply, const PackedSamples* packed_samples,
            void* args, Endianness endianness);

    void* args;

} InputMessageCallback;

typedef struct InputMessage
{
    InputMessageCallback callback;
    MicroBuffer reader;

    uint8_t* buffer;
    uint32_t buffer_size;

} InputMessage;

void init_input_message(InputMessage* message, InputMessageCallback callback, uint8_t* in_buffer, uint32_t in_buffer_size);
void free_input_message(InputMessage* message);

void parse_data_payload(InputMessage* message, DataFormat format, const BaseObjectReply* reply);
int parse_submessage(InputMessage* message);

int parse_message(InputMessage* message, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_INPUT_MESSAGE_H_
