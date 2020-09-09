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
#include <uxr/client/core/session/session.h>
#include <uxr/client/config.h>

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t hash_int_t;
#define BROKERLESS_HASH_SIZE sizeof(hash_int_t)

// Outcomming message queue

typedef struct
{
    uint8_t *data;
    size_t lenght;
    uxrObjectId id;
    SampleIdentity sample_id;
} brokerlessMessage_t;

typedef struct
{
    brokerlessMessage_t queue[UCLIENT_BROKERLESS_MESSAGE_QUEUE_LEN];
    size_t index;
} brokerlessMessageQueue_t;

// Entities map

typedef struct
{
    char hash[BROKERLESS_HASH_SIZE];
    uxrObjectId id;
} brokerlessEntityHash_t;


typedef struct
{
    brokerlessEntityHash_t queue[UCLIENT_BROKERLESS_ENTITY_MAP_LEN];
    size_t index;
    uint8_t datawriters;
    uint8_t datareaders;
    uint8_t requesters;
    uint8_t repliers;
} brokerlessEntityMap_t;

// Internal API (TODO: add documentation)

void init_brokerless(uint32_t key);
bool add_brokerless_message(ucdrBuffer* ub, uint32_t lenght, uxrObjectId id);
bool add_brokerless_message_with_sample_id(ucdrBuffer* ub, size_t lenght, uxrObjectId id, SampleIdentity sample_id);
bool add_brokerless_entity_hash_from_xml(const char* xml, uxrObjectId id);
bool add_brokerless_entity_hash(const char* ref, uxrObjectId id);
int32_t find_brokerless_hash_from_id(uxrObjectId id);
int32_t find_brokerless_hash_from_hash(char* hash);
bool check_brokerless_sample_id(SampleIdentity sample_id);
void fill_brokerless_sample_id(SampleIdentity* sample_id, uxrObjectId id);
bool flush_brokerless_queues();
bool listen_brokerless(uxrSession* session, int timeout);

#ifdef __cplusplus
}
#endif

#endif // _SRC_C_BROKERLESS_BROKERLESS_INTERNAL_H_
