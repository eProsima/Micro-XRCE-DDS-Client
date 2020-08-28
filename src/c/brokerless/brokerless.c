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

// TODOs:
// - Manage brokerlessMessageQueue_t and brokerlessEntityMap_t as a container that allow push and pop
// - Implement removing entities from brokerless when they are removed from user API
// - Is the hast/objectid lookup optimal? Which container structure should we use?
// - Are the UDP broadcast sending a whole packet? Can it have fragmentation in UDP packet level?
// - What happen when uCDR buffer fragments?
// - Implement hash function for "topic recognition"
// - How can we match using XML instead of references?
// - Control flow of datareaders using the API of uxr_buffer_request_data
// - If we implement binary XML entities it would be really nice to the Brokerless architecture
//       Not all QoS can be sended in Binary Format p29 s7.7.3.2 https://www.omg.org/spec/DDS-XRCE/1.0/PDF

#include "./brokerless_internal.h"

#include <string.h>

brokerlessMessageQueue_t brokerlessMessageQueue;
brokerlessEntityMap_t brokerlessEntityMap;
uint8_t brokerlessBuffer[BROKERLESS_BUFFER_SIZE];

//==================================================================
//                             PRIVATE
//==================================================================

// djb2 by Dan Bernstein: http://www.cse.yorku.ca/~oz/hash.html
void hash_brokerless(unsigned char *str, char* hash)
{
    hash_int_t int_hash = 5381;
    int c;    

    while (c = *str++)
        int_hash = ((int_hash << 5) + int_hash) + c; /* hash * 33 + c */

    for (size_t i = 0; i < BROKERLESS_HASH_SIZE; i++){
       hash[i] = ((char*)&int_hash)[i];
    }     
}

void init_brokerless()
{
    brokerlessMessageQueue.index = 0;
    brokerlessEntityMap.index = 0;
    brokerlessEntityMap.datareaders = 0;
    brokerlessEntityMap.datawriters = 0;

    init_udp_broadcast_transport_datagram();
}

bool add_brokerless_message(ucdrBuffer* ub, uint32_t lenght, uxrObjectId id)
{
    if (brokerlessMessageQueue.index < BROKERLESS_MESSAGE_QUEUE_LEN - 1)
    {
        brokerlessMessageQueue.queue[brokerlessMessageQueue.index].data =  ub->iterator;
        brokerlessMessageQueue.queue[brokerlessMessageQueue.index].lenght =  lenght;
        brokerlessMessageQueue.queue[brokerlessMessageQueue.index].id =  id;

        brokerlessMessageQueue.index++;

        return true;
    }
    
    return false;
}

bool add_brokerless_entity_hash(char* ref, uxrObjectId id)
{
    if (brokerlessEntityMap.index < BROKERLESS_ENTITY_MAP_LEN - 1)
    {   
        hash_brokerless(ref, brokerlessEntityMap.queue[brokerlessEntityMap.index].hash);

        brokerlessEntityMap.queue[brokerlessMessageQueue.index].id =  id;

        if (id.type == UXR_DATAREADER_ID){
            brokerlessEntityMap.datareaders++;
        }else if (id.type == UXR_DATAWRITER_ID){
            brokerlessEntityMap.datawriters++;
        }
        

        brokerlessEntityMap.index++;

        return true;
    }
    
    return false;
}

int32_t find_brokerless_hash_from_id(uxrObjectId id)
{
    for (size_t i = 0; i < brokerlessEntityMap.index; i++)
    {
        if (brokerlessEntityMap.queue[i].id.id == id.id &&
            brokerlessEntityMap.queue[i].id.type == id.type)
        {
            return i;
        }
    }
    return -1;
}

int32_t find_brokerless_hash_from_hash(char* hash)
{
    for (size_t i = 0; i < brokerlessEntityMap.index; i++)
    {   
        if (0 == memcmp((void*) hash, (void*) brokerlessEntityMap.queue[i].hash, BROKERLESS_HASH_SIZE))
        {
            return i;
        }
    }
    return -1;
}

bool flush_brokerless_queues()
{   
    for (size_t i = 0; i < brokerlessMessageQueue.index; i++)
    {
        int32_t hash_index = find_brokerless_hash_from_id(brokerlessMessageQueue.queue[i].id);
        
        if (-1 != hash_index)
        {
            ucdrBuffer writer;
            ucdr_init_buffer(&writer, brokerlessBuffer, BROKERLESS_BUFFER_SIZE);

            ucdr_serialize_array_char(&writer, brokerlessEntityMap.queue[hash_index].hash, BROKERLESS_HASH_SIZE);
            ucdr_serialize_sequence_char(&writer, brokerlessMessageQueue.queue[i].data, brokerlessMessageQueue.queue[i].lenght);

            udp_broadcast_send_datagram(writer.init, ucdr_buffer_length(&writer));
        }
    }

    brokerlessMessageQueue.index = 0;
    
    return false;
}

bool listen_brokerless(uint8_t** data, size_t* length, int timeout, uxrObjectId** id)
{       
    size_t readed_bytes = 0;
    if (0 != brokerlessEntityMap.datareaders){
        readed_bytes = udp_broadcast_recv_datagram(&brokerlessBuffer, BROKERLESS_BUFFER_SIZE, timeout);
    }
    
    if(0 != readed_bytes){

        ucdrBuffer reader;
        ucdr_init_buffer(&reader, brokerlessBuffer, readed_bytes);

        char hash[BROKERLESS_HASH_SIZE];
        ucdr_deserialize_array_char(&reader, hash, BROKERLESS_HASH_SIZE);


        int32_t hash_index = find_brokerless_hash_from_hash(hash);

        if (-1 != hash_index && brokerlessEntityMap.queue[hash_index].id.type == UXR_DATAREADER_ID)
        {   
            *id = &brokerlessEntityMap.queue[hash_index];
            uint32_t aux;
            ucdr_deserialize_uint32_t(&reader, &aux);
            *length = aux;
            *data = reader.iterator;
            return true;
        }
    }

    return false;
}

