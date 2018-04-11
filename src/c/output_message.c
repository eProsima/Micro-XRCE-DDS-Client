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

#include <micrortps/client/output_message.h>
#include <micrortps/client/client.h>
#include <micrortps/client/reliable_control.h>
#include "xrce_protocol_serialization.h"
#include "log/message.h"

#include <stdlib.h>

bool send_best_effort_message(Session* session, OutputBestEffortStream* output_stream)
{
    MicroBuffer* output_buffer = &output_stream->buffer.micro_buffer;

    output_stream->last_sent = seq_num_add(output_stream->last_sent, 1);
    stamp_header(session, output_buffer->init, STREAMID_BUILTIN_BEST_EFFORTS, output_stream->last_sent);

    int32_t bytes = send_data(output_buffer->init, (output_buffer->iterator - output_buffer->init), session->transport_id);
    PRINTL_SERIALIZATION(SEND, output_buffer->init, output_buffer->iterator - output_buffer->init);

    reset_micro_buffer_offset(output_buffer, session->header_offset);

    return bytes > 0;
}

bool send_reliable_message(Session* session, OutputReliableStream* output_stream)
{
    MicroBuffer* output_buffer = &output_stream->buffers[seq_num_add(output_stream->last_sent, 1) % MICRORTPS_MAX_MSG_NUM].micro_buffer;

    output_stream->last_sent = seq_num_add(output_stream->last_sent, 1);
    stamp_header(session, output_buffer->init, STREAMID_BUILTIN_RELIABLE, output_stream->last_sent);

    int32_t bytes = send_data(output_buffer->init, (output_buffer->iterator - output_buffer->init), session->transport_id);
    PRINTL_SERIALIZATION(SEND, output_buffer->init, output_buffer->iterator - output_buffer->init);

    return bytes > 0;
}

bool send_heartbeat(Session* session, OutputReliableStream* reference_stream)
{
    uint8_t buffer[MICRORTPS_MIN_BUFFER_SIZE];
    MicroBuffer output_buffer;
    init_micro_buffer_offset(&output_buffer, buffer, sizeof(buffer), session->header_offset);

    SubmessageHeader sub_header = (SubmessageHeader){SUBMESSAGE_ID_HEARTBEAT, 0, HEARTBEAT_MSG_SIZE};
    serialize_SubmessageHeader(&output_buffer, &sub_header);

    HEARTBEAT_Payload heartbeat;
    output_heartbeat(reference_stream, &heartbeat);

    serialize_HEARTBEAT_Payload(&output_buffer, &heartbeat);
    PRINTL_HEARTBEAT_SUBMESSAGE(SEND, &heartbeat);

    stamp_header(session, output_buffer.init, 0, (uint16_t)(STREAMID_BUILTIN_RELIABLE));

    int32_t bytes = send_data(output_buffer.init, (output_buffer.iterator - output_buffer.init), session->transport_id);
    PRINTL_SERIALIZATION(SEND, output_buffer.init, output_buffer.iterator - output_buffer.init);

    return bytes > 0;
}

bool send_acknack(Session* session, InputReliableStream* reference_stream)
{
    uint8_t buffer[MICRORTPS_MIN_BUFFER_SIZE];
    MicroBuffer output_buffer;
    init_micro_buffer_offset(&output_buffer, buffer, sizeof(buffer), session->header_offset);

    SubmessageHeader sub_header = (SubmessageHeader){ SUBMESSAGE_ID_ACKNACK, 0, HEARTBEAT_MSG_SIZE };
    serialize_SubmessageHeader(&output_buffer, &sub_header);

    ACKNACK_Payload acknack;
    output_acknack(reference_stream, &acknack);

    serialize_ACKNACK_Payload(&output_buffer, &acknack);
    PRINTL_ACKNACK_SUBMESSAGE(SEND, &acknack);

    stamp_header(session, output_buffer.init, 0, (uint16_t)(STREAMID_BUILTIN_RELIABLE));

    int32_t bytes = send_data(output_buffer.init, (output_buffer.iterator - output_buffer.init), session->transport_id);
    PRINTL_SERIALIZATION(SEND, output_buffer.init, output_buffer.iterator - output_buffer.init);

    return bytes > 0;
}
