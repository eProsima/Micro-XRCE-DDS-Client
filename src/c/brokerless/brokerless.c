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
// - Control flow of datareaders using the API of uxr_buffer_request_data and pass the request_id to the callbacks
// - If we implement binary XML entities it would be really nice to the Brokerless architecture
//       Not all QoS can be sended in Binary Format p29 s7.7.3.2 https://www.omg.org/spec/DDS-XRCE/1.0/PDF
// - Should we simplify the approach of uxr_prepare_output_stream -> serialize to something similar to the request/reply where buffer is available in uxr_buffer_request?
// - Explore cases when multiple datawriters/datareaders/requesters/repliers coexists
// - sample identity inside brokerlessMessage_t can be optimized because it is a big member

// IDEAS:
// - A P2P system that has a node with some services for create entities in the real agent.
// - A P2P system that has a node acting as a gateware to DDS. We need a P2P standart QoS which only fill type and topic to always match.


#include "./brokerless_internal.h"
#include "./brokerless_transport.h"

#include <string.h>

static uint32_t client_key;
static brokerlessMessageQueue_t brokerlessMessageQueue;
static brokerlessEntityMap_t brokerlessEntityMap;
static uint8_t brokerlessBuffer[UCLIENT_BROKERLESS_INTERNAL_BUFFER_LEN];

//==================================================================
//                        PRIVATE UTILS
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

// Find first occurrence of tag in XML
bool find_tag_xml(char * xml, size_t len, char * tag, char ** content, size_t * content_len)
{
    size_t tag_len = strlen(tag);
    bool found_begin = false;
    bool found_end = false;
    for (size_t i = 0; i < len; i++)
    {
        if (!found_begin && 0 == memcmp(&xml[i], tag, tag_len))
        {
            size_t tag_opener_len = 0;
            while(xml[i+tag_opener_len] != '>')
                tag_opener_len++;
            *content = &xml[i+tag_opener_len+1];
            found_begin = true;
        }
        else if(found_begin && 0 == memcmp(&xml[i], tag, tag_len)  )
        {
            *content_len = &xml[i-2] - *content;
            found_end = true;
            break;
        }
    }

    return found_begin && found_end;
}

// Find property in first occurrence of tag in XML
bool find_tag_property(char * xml, size_t len, char * tag, char * property, char ** content, size_t * content_len)
{
    size_t tag_len = strlen(tag);
    size_t property_len = strlen(property);

    bool found_tag = false;
    bool found_property = false;
    for (size_t i = 0; i < len; i++)
    {
        if (!found_tag && 0 == memcmp(&xml[i], tag, tag_len))
        {
            found_tag = true;
        }
        else if(found_tag && 0 == memcmp(&xml[i], property, property_len)  )
        {
            *content = &xml[i+property_len+2];
            i += property_len+2;
            *content_len = 0;
            while(xml[i + (*content_len)] != '"')
                *content_len += 1;
            found_property = true;
            break;
        }
    }

    return found_tag && found_property;
}

//==================================================================
//                             PRIVATE
//==================================================================

void init_brokerless(uint32_t key)
{
    brokerlessMessageQueue.index = 0;
    brokerlessEntityMap.index = 0;
    brokerlessEntityMap.datareaders = 0;
    brokerlessEntityMap.datawriters = 0;
    brokerlessEntityMap.requesters = 0;
    brokerlessEntityMap.repliers = 0;

    client_key = key;

    brokerless_init_transport();
}

bool add_brokerless_message(ucdrBuffer* ub, uint32_t lenght, uxrObjectId id)
{   
    SampleIdentity sample_id = {0};
    return add_brokerless_message_with_sample_id(ub, lenght, id, sample_id);
}

bool add_brokerless_message_with_sample_id(ucdrBuffer* ub, uint32_t lenght, uxrObjectId id, SampleIdentity sample_id)
{
    if (brokerlessMessageQueue.index < UCLIENT_BROKERLESS_MESSAGE_QUEUE_LEN - 1)
    {
        brokerlessMessageQueue.queue[brokerlessMessageQueue.index].data =  ub->iterator;
        brokerlessMessageQueue.queue[brokerlessMessageQueue.index].lenght =  lenght;
        brokerlessMessageQueue.queue[brokerlessMessageQueue.index].id =  id;
        brokerlessMessageQueue.queue[brokerlessMessageQueue.index].sample_id = sample_id;

        brokerlessMessageQueue.index++;

        return true;
    }
    
    return false;
}

bool add_brokerless_entity_hash_from_xml(const char* xml, uxrObjectId id)
{
    bool found = true;
    char name_type_buffer[100];

    if (id.type == UXR_DATAWRITER_ID || id.type == UXR_DATAREADER_ID)
    {
        char xml_strings[3][12] = { "dds", 
                                    "data_writer", 
                                    "topic"
                                  };
        if (id.type == UXR_DATAREADER_ID){
            memcpy(xml_strings[1], "data_reader\0", 12);
        }
        
        char * content_in = xml;
        char * content_out;
        size_t content_len_in = strlen(content_in);
        size_t content_len_out;

        for (size_t i = 0; i < 3; i++)
        {   
            if(find_tag_xml(content_in, content_len_in, xml_strings[i], &content_out, &content_len_out)){
                content_in = content_out;
                content_len_in = content_len_out;
            } else {
                return false;
            }
        }

        size_t topic_name_len;
        size_t type_name_len;

        found &= find_tag_xml(content_in, content_len_in, "name", &content_out, &topic_name_len);
        memcpy(name_type_buffer, content_out, topic_name_len);

        found &= find_tag_xml(content_in, content_len_in, "dataType", &content_out, &type_name_len);
        memcpy(&name_type_buffer[topic_name_len], content_out, type_name_len);

        name_type_buffer[topic_name_len+type_name_len] = '\0';

        found &= add_brokerless_entity_hash(name_type_buffer, id);   
    } 
    else if (id.type == UXR_REQUESTER_ID || id.type == UXR_REPLIER_ID)
    {   
        char * content_out;
        size_t service_name_len;
        size_t request_type_name_len;
        size_t reply_type_name_len;

        found &= find_tag_property( xml, 
                                    strlen(xml), 
                                    (id.type == UXR_REQUESTER_ID) ? "requester" : "replier", 
                                    "service_name", 
                                    &content_out, 
                                    &service_name_len);
        
        memcpy(name_type_buffer, content_out, service_name_len);

        found &= find_tag_property( xml, 
                            strlen(xml), 
                            (id.type == UXR_REQUESTER_ID) ? "requester" : "replier", 
                            "request_type", 
                            &content_out, 
                            &request_type_name_len);
        
        memcpy(&name_type_buffer[service_name_len], content_out, service_name_len);

        found &= find_tag_property( xml, 
                    strlen(xml), 
                    (id.type == UXR_REQUESTER_ID) ? "requester" : "replier", 
                    "reply_type", 
                    &content_out, 
                    &reply_type_name_len);
        
        memcpy(&name_type_buffer[service_name_len+request_type_name_len], content_out, service_name_len);

        name_type_buffer[service_name_len+request_type_name_len+reply_type_name_len] = '\0';

        found &= add_brokerless_entity_hash(name_type_buffer, id);   
    }

    return found;
}


bool add_brokerless_entity_hash(char* ref, uxrObjectId id)
{
    if (brokerlessEntityMap.index < UCLIENT_BROKERLESS_ENTITY_MAP_LEN - 1)
    {   
        hash_brokerless((unsigned char*) ref, brokerlessEntityMap.queue[brokerlessEntityMap.index].hash);

        brokerlessEntityMap.queue[brokerlessMessageQueue.index].id =  id;
        
        if (id.type == UXR_DATAREADER_ID){
            brokerlessEntityMap.datareaders++;
        }else if (id.type == UXR_DATAWRITER_ID){
            brokerlessEntityMap.datawriters++;
        }else if (id.type == UXR_REQUESTER_ID){
            brokerlessEntityMap.requesters++;
        }else if (id.type == UXR_REPLIER_ID){
            brokerlessEntityMap.repliers++;
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
            return (int32_t) i;
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
            return (int32_t) i;
        }
    }
    return -1;
}

bool check_brokerless_sample_id(SampleIdentity sample_id)
{
    // TODO (pablogs9): Check if requester id stored in the sample_id still exists
    
    return  !memcmp(&sample_id.writer_guid.entityId.entityKey, (uint8_t*)(&client_key), 3) &&
            !memcmp(&sample_id.writer_guid.entityId.entityKind, (uint8_t*)(&client_key) + 3, 1);
}

void fill_brokerless_sample_id(SampleIdentity* sample_id, uxrObjectId id)
{
    // TODO (pablogs9): Improve this in order to take into account the endianness

    memcpy(&sample_id->writer_guid.entityId.entityKey, (uint8_t*)(&client_key), 3);
    memcpy(&sample_id->writer_guid.entityId.entityKind, (uint8_t*)(&client_key) + 3, 1);

    memcpy(&sample_id->writer_guid.guidPrefix.data, (uint8_t*)(&id.id), 2);
}

bool flush_brokerless_queues()
{   
    for (size_t i = 0; i < brokerlessMessageQueue.index; i++)
    {
        int32_t hash_index = find_brokerless_hash_from_id(brokerlessMessageQueue.queue[i].id);
        
        if (-1 != hash_index)
        {
            ucdrBuffer writer;
            ucdr_init_buffer(&writer, brokerlessBuffer, UCLIENT_BROKERLESS_INTERNAL_BUFFER_LEN);
            ucdr_serialize_array_char(&writer, brokerlessEntityMap.queue[hash_index].hash, BROKERLESS_HASH_SIZE);

            if (brokerlessMessageQueue.queue[i].id.type == UXR_REQUESTER_ID || brokerlessMessageQueue.queue[i].id.type == UXR_REPLIER_ID){

                ucdr_serialize_bool(&writer, brokerlessMessageQueue.queue[i].id.type == UXR_REQUESTER_ID);

                if (brokerlessMessageQueue.queue[i].id.type == UXR_REQUESTER_ID)
                {
                    fill_brokerless_sample_id(&brokerlessMessageQueue.queue[i].sample_id, brokerlessMessageQueue.queue[i].id);
                }
                
                uxr_serialize_SampleIdentity(&writer, &brokerlessMessageQueue.queue[i].sample_id);
            }

            ucdr_serialize_sequence_char(&writer, (char*) brokerlessMessageQueue.queue[i].data, brokerlessMessageQueue.queue[i].lenght);

            brokerless_broadcast_send(writer.init, ucdr_buffer_length(&writer));
        }
    }

    brokerlessMessageQueue.index = 0;
    
    return false;
}

bool listen_brokerless(uxrSession* session, int timeout)
{
    size_t readed_bytes = 0;
    if (brokerlessEntityMap.datareaders || brokerlessEntityMap.requesters || brokerlessEntityMap.repliers){
        readed_bytes = brokerless_broadcast_recv(brokerlessBuffer, UCLIENT_BROKERLESS_INTERNAL_BUFFER_LEN, timeout);
    }

    if(0 != readed_bytes){

        ucdrBuffer reader;
        ucdr_init_buffer(&reader, brokerlessBuffer, readed_bytes);

        char hash[BROKERLESS_HASH_SIZE];
        ucdr_deserialize_array_char(&reader, hash, BROKERLESS_HASH_SIZE);

        int32_t hash_index = find_brokerless_hash_from_hash(hash);

        if (-1 != hash_index && brokerlessEntityMap.queue[hash_index].id.type != UXR_DATAWRITER_ID)
        {    
            uxrObjectId * object_id = &brokerlessEntityMap.queue[hash_index].id;

            // TODO (pablogs9): request_id is related to the uxr_buffer_request_data request, so it can determine some limitations imposed into the communication -> NOT IMPLEMENTED BY NOW
            if (object_id->type == UXR_DATAREADER_ID)
            {
                uxrStreamId stream = {0, 0, UXR_BROKERLESS, UXR_INPUT_STREAM};
                uint32_t length;
                ucdr_deserialize_uint32_t(&reader, &length);
                session->on_topic(session, *object_id, 0, stream, &reader, length, session->on_topic_args);
            }
            else
            {
                bool is_from_requester;
                SampleIdentity sample_id;
                uint32_t length;

                ucdr_deserialize_bool(&reader, &is_from_requester);

                // sample_id deserialization is done inside conditional in order to not deserialize when message should be dropped

                if (is_from_requester && object_id->type == UXR_REPLIER_ID)
                {   
                    uxr_deserialize_SampleIdentity(&reader, &sample_id);          
                    ucdr_deserialize_uint32_t(&reader, &length);
                    session->on_request(session, *object_id, 0, &sample_id, &reader, length, session->on_request_args);
                }
                else if(!is_from_requester && object_id->type == UXR_REQUESTER_ID)
                {
                    uxr_deserialize_SampleIdentity(&reader, &sample_id);
                    if (check_brokerless_sample_id(sample_id))
                    {
                        ucdr_deserialize_uint32_t(&reader, &length);
                        session->on_reply(session, *object_id, 0, sample_id.sequence_number.low, &reader, length, session->on_reply_args);
                    }
                }
                
            }
            return true;
        }
    }

    return false;
}

