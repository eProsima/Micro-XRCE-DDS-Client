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

#include "aux_memory.h"

#include <stdlib.h>
#include <string.h>

#ifndef NDEBUG
#include <stdio.h>
#endif

void init_aux_memory(AuxMemory* memory)
{
    memory->buffer = NULL;
    memory->buffer_alloc = 0;
}

void increase_aux_memory(AuxMemory* memory, uint32_t memory_size)
{
    if(memory->buffer_alloc < memory_size)
    {
        memory->buffer = realloc(memory->buffer, memory_size);
        memory->buffer_alloc = memory_size;
    }
    memory->size = 0;
}

void* request_aux_memory(AuxMemory* memory, uint32_t size)
{
    #ifndef NDEBUG
    if(memory->size + size > memory->buffer_alloc)
    {
        fprintf(stderr, "ERROR: AuxMemory oveflow.\n");
        return NULL;
    }
    #endif

    uint32_t init = memory->size;
    memory->size += size;
    return ((uint8_t*)memory->buffer) + init;
}

void free_aux_memory(AuxMemory* memory)
{
    memory->buffer = NULL;
    free(memory->buffer);
}