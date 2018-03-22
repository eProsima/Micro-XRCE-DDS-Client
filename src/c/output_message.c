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

#include <stdlib.h>

bool send_best_effort_message(Session* session, BestEffortStream* output_stream)
{
    MicroBuffer* output_buffer = &output_stream->micro_buffer;

    stamp_header(session, output_buffer, STREAMID_BUILTIN_BEST_EFFORTS, output_stream->seq_num);

    int32_t bytes = send_data(output_buffer->init, (output_buffer->iterator - output_buffer->init), session->transport_id);
    output_stream->seq_num++;

    reset_micro_buffer(output_buffer);
    output_buffer->iterator += session->header_offset;

    return bytes > 0;
}

bool send_reliable_message(Session* session, ReliableStream* output_stream)
{
    MicroBuffer* output_buffer = &output_stream->store[output_stream->seq_num].micro_buffer;

    stamp_header(session, output_buffer, STREAMID_BUILTIN_RELIABLE, output_stream->seq_num);

    int32_t bytes = send_data(output_buffer->init, (output_buffer->iterator - output_buffer->init), session->transport_id);
    output_stream->seq_num++;

    return bytes > 0;
}

bool send_heartbeat(Session* session, ReliableStream* reference_stream)
{
    uint8_t buffer[MICRORTPS_MIN_BUFFER_SIZE];
    MicroBuffer output_buffer;
    init_micro_buffer_endian(&output_buffer, buffer, sizeof(buffer), MACHINE_ENDIANNESS);

    stamp_header(session, &output_buffer, STREAMID_BUILTIN_RELIABLE, 0);

    SubmessageHeader sub_header = (SubmessageHeader){ SUBMESSAGE_ID_DATA, 0, HEARTBEAT_MSG_SIZE };
    serialize_SubmessageHeader(&output_buffer, &sub_header);

    HEARTBEAT_Payload heartbeat;
    output_heartbeat(reference_stream, &heartbeat);

    deserialize_HEARTBEAT_Payload(&output_buffer, &heartbeat);

    int32_t bytes = send_data(output_buffer.init, (output_buffer.iterator - output_buffer.init), session->transport_id);
    return bytes > 0;
}

bool send_acknack(Session* session, ReliableStream* reference_stream)
{
    uint8_t buffer[MICRORTPS_MIN_BUFFER_SIZE];
    MicroBuffer output_buffer;
    init_micro_buffer_endian(&output_buffer, buffer, sizeof(buffer), MACHINE_ENDIANNESS);

    stamp_header(session, &output_buffer, STREAMID_BUILTIN_RELIABLE, 0);

    SubmessageHeader sub_header = (SubmessageHeader){ SUBMESSAGE_ID_DATA, 0, HEARTBEAT_MSG_SIZE };
    serialize_SubmessageHeader(&output_buffer, &sub_header);

    ACKNACK_Payload acknack;
    output_acknack(reference_stream, &acknack);

    deserialize_ACKNACK_Payload(&output_buffer, &acknack);

    int32_t bytes = send_data(output_buffer.init, (output_buffer.iterator - output_buffer.init), session->transport_id);
    return bytes > 0;
}
