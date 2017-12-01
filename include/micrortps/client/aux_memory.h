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

#ifndef _MICRORTPS_CLIENT_AUX_MEMORY_H_
#define _MICRORTPS_CLIENT_AUX_MEMORY_H_

#ifdef __cplusplus
extern "C"
{
#endif


#include <stdint.h>

typedef struct AuxMemory
{
    void* buffer;
    uint32_t size;
    uint32_t buffer_alloc;

} AuxMemory;

void init_aux_memory(AuxMemory* memory);
void increase_aux_memory(AuxMemory* memory, uint32_t memory_size);
void* request_aux_memory(AuxMemory* memory, uint32_t size);
void free_aux_memory(AuxMemory* memory);

#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_AUX_MEMORY_H_
