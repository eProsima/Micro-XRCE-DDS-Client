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

#ifndef _SRC_C_BROKERLESS_BROKERLESS_INTERNAL_H_
#define _SRC_C_BROKERLESS_BROKERLESS_INTERNAL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <uxr/client/core/session/object_id.h>
#include <uxr/client/core/type/xrce_types.h>

#include <stdint.h>
#include <stdbool.h>

typedef unsigned long hash_int_t; 

#define BROKERLESS_MESSAGE_QUEUE_LEN 100
#define BROKERLESS_ENTITY_MAP_LEN 10
#define BROKERLESS_HASH_SIZE sizeof(hash_int_t)
#define BROKERLESS_BUFFER_SIZE 1000

// Outcomming message queue

typedef struct {
    uint8_t *data;
    uint32_t lenght;
    uxrObjectId id;
    SampleIdentity sample_id;
} brokerlessMessage_t;

typedef struct {
    brokerlessMessage_t queue[BROKERLESS_MESSAGE_QUEUE_LEN];
    uint8_t index;
} brokerlessMessageQueue_t;

// Entities map

typedef struct {
    char hash[BROKERLESS_HASH_SIZE];
    uxrObjectId id;
} brokerlessEntityHash_t;


typedef struct {
    brokerlessEntityHash_t queue[BROKERLESS_ENTITY_MAP_LEN];
    uint8_t index;
    uint8_t datawriters;
    uint8_t datareaders;
    uint8_t requesters;
    uint8_t repliers;
} brokerlessEntityMap_t;

// Internal API

// TODO(Pablogs9): Complete this header list

void init_brokerless();
bool add_brokerless_message(ucdrBuffer* ub, uint32_t lenght, uxrObjectId id);
bool add_brokerless_message_with_sample_id(ucdrBuffer* ub, uint32_t lenght, uxrObjectId id, SampleIdentity sample_id);
bool add_brokerless_entity_hash(char* hash, uxrObjectId id);
bool flush_brokerless_queues();


#ifdef __cplusplus
}
#endif

#endif // _SRC_C_BROKERLESS_BROKERLESS_INTERNAL_H_
