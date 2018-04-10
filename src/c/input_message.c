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
#include <micrortps/client/reliable_control.h>
#include "xrce_protocol_serialization.h"
#include "log/message.h"

#include <stdlib.h>

void process_message(Session* session, MicroBuffer* input_buffer)
{
    MessageHeader header;
    deserialize_MessageHeader(input_buffer, &header);
    if (128 > header.session_id)
    {
        ClientKey key;
        deserialize_ClientKey(input_buffer, &key);
    }

    if(0 == header.stream_id)
    {
        process_submessages(session, input_buffer);
    }
    else if(STREAMID_BUILTIN_BEST_EFFORTS == header.stream_id)
    {
        InputBestEffortStream* input_stream = &session->input_best_effort_stream;
        bool ready_to_process = receive_best_effort_message(input_stream, header.sequence_nr);
        if(ready_to_process)
        {
            process_submessages(session, input_buffer);
        }
    }
    else if(STREAMID_BUILTIN_RELIABLE == header.stream_id)
    {
        InputReliableStream* input_stream = &session->input_reliable_stream;
        bool ready_to_read = receive_reliable_message(input_stream, input_buffer, header.sequence_nr);
        if(ready_to_read)
        {
            process_submessages(session, input_buffer);
            for(uint16_t i = seq_num_add(header.sequence_nr, 1); 0 >= seq_num_cmp(i, input_stream->last_handled); i = seq_num_add(i, 1))
            {
                uint8_t current_index = i % MICRORTPS_MAX_MSG_NUM;
                MicroBuffer* current_buffer = &input_stream->buffers[current_index].micro_buffer;
                process_submessages(session, current_buffer);
                reset_micro_buffer(current_buffer);
            }
        }
    }
}

void process_submessages(Session* session, MicroBuffer* input_buffer)
{
    while(input_buffer->final - input_buffer->iterator > SUBHEADER_SIZE)
    {
        SubmessageHeader sub_header;
        deserialize_SubmessageHeader(input_buffer, &sub_header);
        input_buffer->endianness = (sub_header.flags & 0x01) ? LITTLE_ENDIANNESS : BIG_ENDIANNESS;

        if(sub_header.length > input_buffer->final - input_buffer->iterator)
        {
             break;
        }

        switch(sub_header.id)
        {
            case SUBMESSAGE_ID_STATUS:
                process_status_submessage(session, input_buffer);
                break;

            case SUBMESSAGE_ID_INFO:
                process_info_submessage(session, input_buffer);
                break;

            case SUBMESSAGE_ID_HEARTBEAT:
                process_heartbeat_submessage(session, input_buffer);
                break;

            case SUBMESSAGE_ID_ACKNACK:
                process_acknack_submessage(session, input_buffer);
                break;

            case SUBMESSAGE_ID_DATA:
                process_data_submessage(session, input_buffer);
                break;

            default:
                break;
        }

        align_to(input_buffer, 4);
    }
}

void process_status_submessage(Session* session, MicroBuffer* input_buffer)
{
    STATUS_Payload status;
    deserialize_STATUS_Payload(input_buffer, &status);

    PRINTL_STATUS_SUBMESSAGE(&status);

    session->last_status.status = status.base.result.status;
    session->last_status.implementation_status = MICRORTPS_STATUS_OK;
    session->last_status_received = true;
}

void process_info_submessage(Session* session, MicroBuffer* input_buffer)
{
    //TODO
}

void process_heartbeat_submessage(Session* session, MicroBuffer* input_buffer)
{
    HEARTBEAT_Payload heartbeat;
    deserialize_HEARTBEAT_Payload(input_buffer, &heartbeat);

    PRINTL_HEARTBEAT_SUBMESSAGE(RECV, &heartbeat);

    InputReliableStream* input_stream = &session->input_reliable_stream;
    input_heartbeat(session, input_stream, heartbeat.first_unacked_seq_nr, heartbeat.last_unacked_seq_nr);
}

void process_acknack_submessage(Session* session, MicroBuffer* input_buffer)
{
    ACKNACK_Payload acknack;
    deserialize_ACKNACK_Payload(input_buffer, &acknack);

    PRINTL_ACKNACK_SUBMESSAGE(RECV, &acknack);

    OutputReliableStream* output_stream = &session->output_reliable_stream;
    input_acknack(session, output_stream, acknack.first_unacked_seq_num, acknack.nack_bitmap);
}

void process_data_submessage(Session* session, MicroBuffer* input_buffer)
{
    DATA_Payload_Data payload;
    deserialize_DATA_Payload_Data(input_buffer, &payload);

    PRINTL_DATA_DATA_SUBMESSAGE(&payload);

    input_buffer->iterator = payload.data.data; //delete this when the topic had been deserialized out of data payload.

    session->on_topic_callback(payload.base.object_id, input_buffer, session->on_topic_args);
}

bool receive_best_effort_message(InputBestEffortStream* input_stream, const uint16_t seq_num)
{
    if(0 <= seq_num_cmp(input_stream->last_handled, seq_num))
    {
        return false;
    }

    input_stream->last_handled = seq_num;

    return true;
}

bool receive_reliable_message(InputReliableStream* input_stream, MicroBuffer* submessages, const uint16_t seq_num)
{
    bool result = false;

    if(0 <= seq_num_cmp(input_stream->last_handled, seq_num)
        || 0 > seq_num_cmp(seq_num_add(input_stream->last_handled, MICRORTPS_MAX_MSG_NUM), seq_num))
    {
        return false;
    }

    uint8_t index = seq_num % MICRORTPS_MAX_MSG_NUM;
    MicroBuffer* input_buffer = &input_stream->buffers[index].micro_buffer;
    if(seq_num_add(input_stream->last_handled, 1) == seq_num)
    {
        for(uint16_t i = 1; i < MICRORTPS_MAX_MSG_NUM; i++)
        {
            uint8_t aux_index = (seq_num + i) % MICRORTPS_MAX_MSG_NUM;
            MicroBuffer* aux_buffer = &input_stream->buffers[aux_index].micro_buffer;
            if(aux_buffer->iterator == aux_buffer->init)
            {
                input_stream->last_handled = seq_num_add(input_stream->last_handled, i);
                break;
            }
        }
        result = true;
    }
    else
    {
        serialize_array_uint8_t(input_buffer, submessages->iterator, submessages->final - submessages->iterator);
    }

    if(0 > seq_num_cmp(input_stream->last_announced, seq_num))
    {
        input_stream->last_announced = seq_num;
    }

    return result;
}
