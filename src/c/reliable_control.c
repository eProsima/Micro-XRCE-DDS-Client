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
#include <string.h>
#include "log/message.h"

void output_heartbeat(OutputReliableStream* output_stream, HEARTBEAT_Payload* heartbeat)
{
    heartbeat->first_unacked_seq_nr = seq_num_add(output_stream->last_acknown, 1);
    heartbeat->last_unacked_seq_nr = output_stream->last_sent;
}

void input_acknack(Session* session, OutputReliableStream* output_stream, const uint16_t first_unacked_seq_num, uint8_t bitmap[2])
{
    /* Clear buffers */
    for(uint16_t i = seq_num_add(output_stream->last_acknown, 1); 0 > seq_num_cmp(i, first_unacked_seq_num); i = seq_num_add(i, 1))
    {
        uint8_t index = i % MICRORTPS_MAX_MSG_NUM;
        MicroBuffer* output_buffer = &output_stream->buffers[index].micro_buffer;
        reset_micro_buffer_offset(output_buffer, session->header_offset);
    }
    output_stream->last_acknown = seq_num_sub(first_unacked_seq_num, 1);

    /* Send lost */
    for(int i = 0; i < MICRORTPS_MAX_MSG_NUM; i++)
    {
        bool lost = (8 > i) ? (bitmap[1] & (0x01 << i)) : (bitmap[0] & (0x01 << (i - 8)));
        if(lost)
        {
            uint8_t index = (first_unacked_seq_num + i) % MICRORTPS_MAX_MSG_NUM;
            MicroBuffer* output_buffer = &output_stream->buffers[index].micro_buffer;
            if((output_buffer->iterator - output_buffer->init) > session->header_offset)
            {
                send_data(output_buffer->init, (output_buffer->iterator - output_buffer->init), session->transport_id);
                PRINTL_SERIALIZATION(SEND, output_buffer->init, (uint32_t)(output_buffer->iterator - output_buffer->init));
            }
        }
    }
}

void input_heartbeat(Session* session, InputReliableStream* input_stream, uint16_t first_seq_num, uint16_t last_seq_num)
{
    if(0 > seq_num_cmp(seq_num_add(input_stream->last_handled, 1), first_seq_num))
    {
        input_stream->last_handled = seq_num_sub(first_seq_num, 1);
    }

    if(0 > seq_num_cmp(input_stream->last_announced, last_seq_num))
    {
        input_stream->last_announced = last_seq_num;
    }

    send_acknack(session, input_stream);
}

void output_acknack(InputReliableStream* input_stream, ACKNACK_Payload* acknack)
{
    memset(acknack->nack_bitmap, 0, 2);
    uint16_t search_buffers_size = seq_num_sub(input_stream->last_announced, input_stream->last_handled);
    for(uint16_t i = 0; i < search_buffers_size; i++)
    {
        uint8_t current_index = seq_num_add(input_stream->last_handled, i + 1) % MICRORTPS_MAX_MSG_NUM;
        MicroBuffer* current_buffer = &input_stream->buffers[current_index].micro_buffer;
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

    acknack->first_unacked_seq_num = seq_num_add(input_stream->last_handled, 1);
}
