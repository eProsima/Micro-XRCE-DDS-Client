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

#include <micrortps/client/client.h>
#include <micrortps/client/output_message.h>

void output_heartbeat(ReliableStream* reference_stream, HEARTBEAT_Payload* heartbeat)
{
    int32_t first = -1;
    int32_t last = -1;
    for(uint16_t i = 0; i < MICRORTPS_MAX_MSG_NUM; i++)
    {
        uint16_t current_seq_num = reference_stream->seq_num + i;
        uint8_t current_index = current_seq_num % MICRORTPS_MAX_MSG_NUM;
        MicroBuffer* current_buffer = &reference_stream->store[current_index].micro_buffer;
        if(current_buffer->iterator != current_buffer->init)
        {
            if(first == -1)
            {
                first = (current_seq_num > MICRORTPS_MAX_MSG_NUM) ? current_seq_num - MICRORTPS_MAX_MSG_NUM : 0;
                last = first;
            }
            else
            {
                last = (current_seq_num > MICRORTPS_MAX_MSG_NUM) ? current_seq_num - MICRORTPS_MAX_MSG_NUM : 0;
            }
        }
    }

    heartbeat->first_unacked_seq_nr = (uint16_t) first;
    heartbeat->last_unacked_seq_nr = (uint16_t) last;
}

void output_acknack(ReliableStream* reference_stream, ACKNACK_Payload* acknack)
{
    memset(acknack->nack_bitmap, 0, 2);
    uint16_t search_buffers_size = reference_stream->last_seq_num - reference_stream->seq_num;
    for(uint16_t i = 0; i < search_buffers_size; i++)
    {
        uint16_t current_seq_num = reference_stream->seq_num + i;
        uint8_t current_index = current_seq_num % MICRORTPS_MAX_MSG_NUM;
        MicroBuffer* current_buffer = &reference_stream->store[current_index].micro_buffer;
        if(current_buffer->iterator == current_buffer->init)
        {
            if(8 > i)
            {
                acknack->nack_bitmap[1] |= (1 << i);
            }
            else
            {
                acknack->nack_bitmap[0] |= (1 << (i - 8));
            }
        }
    }

    acknack->first_unacked_seq_num = reference_stream->seq_num;
}

void input_heartbeat(Session* session, ReliableStream* reference_stream, uint16_t first_seq_num, uint16_t last_seq_num)
{
    if(first_seq_num > reference_stream->seq_num)
    {
        reference_stream->seq_num = first_seq_num;
    }

    if(last_seq_num > reference_stream->seq_num)
    {
        reference_stream->last_seq_num = last_seq_num;
    }

    send_acknack(session, reference_stream);
}

void input_acknack(Session* session, ReliableStream* reference_stream, const uint16_t first_unacked_seq_num, uint8_t bitmap[2])
{
    for(int i = 0; i < MICRORTPS_MAX_MSG_NUM; i++)
    {
        uint8_t index = first_unacked_seq_num % MICRORTPS_MAX_MSG_NUM;
        MicroBuffer* output_buffer = &reference_stream->store[index].micro_buffer;

        if(output_buffer->iterator != output_buffer->init)
        {
            bool lost = (8 > i) ? (bitmap[1] << i) : (bitmap[0] << (i - 8));
            if(lost)
            {
                send_data(output_buffer->init, (output_buffer->iterator - output_buffer->init), session->transport_id);
            }
            else
            {
                reset_micro_buffer(output_buffer);
                output_buffer->iterator += session->header_offset;
            }
        }
    }
}
