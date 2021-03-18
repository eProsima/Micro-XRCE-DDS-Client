#include <uxr/client/core/session/session.h>
#include <uxr/client/config.h>

#include "../../core/session/stream/stream_storage_internal.h"
#include "../matching/matching_internal.h"

#include <string.h>

#if defined(WIN32)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#else
#include <sys/types.h>
#endif

//==================================================================
//                             PRIVATE
//==================================================================

typedef struct uxr_interprocess_buffer_item_t{
    struct uxr_interprocess_buffer_item_t* next;
    ucdrBuffer                 data;
    uint16_t                   data_size;
} uxr_interprocess_buffer_item_t;

typedef struct uxr_interprocess_entity_t{
    uxrObjectId object_id;
    uxrSession* session;
    char hash[UXR_MATCHING_HASH_SIZE];
} uxr_interprocess_entity_t;

typedef struct uxr_interprocess_matrix_block_t{
    bool matched;
    struct uxr_interprocess_buffer_item_t * list;
} uxr_interprocess_matrix_block_t;

typedef struct uxr_interprocess_map_t{
    uxr_interprocess_entity_t entities[UXR_CONFIG_INTERPROCESS_MAX_ENTITIES];
    size_t entities_len;
    uxr_interprocess_matrix_block_t ip_matrix[UXR_CONFIG_INTERPROCESS_MAX_ENTITIES][UXR_CONFIG_INTERPROCESS_MAX_ENTITIES];
    uxr_interprocess_buffer_item_t mem_pool[UXR_CONFIG_INTERPROCESS_STATIC_MEM_SIZE];
    uxr_interprocess_buffer_item_t * mempool_freeitems;
    bool mem_pool_init;
} uxr_interprocess_map_t;

static uxr_interprocess_map_t uxr_ip_map = {0};

// Double linked pool for ucdrBuffers

void uxr_prepend_to_list(uxr_interprocess_buffer_item_t * member, uxr_interprocess_buffer_item_t ** list){
    member->next = *list;
    *list = member;
}

uxr_interprocess_buffer_item_t * uxr_pop_head_from_list(uxr_interprocess_buffer_item_t ** list){
    uxr_interprocess_buffer_item_t * member = *list;
    *list = (*list == NULL ) ? NULL : (*list)->next;
    return member;
}

void uxr_init_static_list(){
    uxr_ip_map.mem_pool_init = true;
    for (size_t i = 0; i < UXR_CONFIG_INTERPROCESS_STATIC_MEM_SIZE; i++)
    {
        uxr_prepend_to_list(&uxr_ip_map.mem_pool[i], &uxr_ip_map.mempool_freeitems);
    }
}

// API

bool uxr_interprocess_entity_compare(uxr_interprocess_entity_t* e1, uxr_interprocess_entity_t* e2)
{  
    bool ret = true;
    for (uint8_t i = 0; i < sizeof(e1->session->info.key); i++)
    {
        ret &= e1->session->info.key[i] == e2->session->info.key[i];
    }

    ret &= e1->object_id.id == e2->object_id.id;
    ret &= e1->object_id.type == e2->object_id.type;
    
    return ret;
}

ssize_t uxr_get_datawriter_index(uxrSession* session, uxrObjectId* entity_id){
    uxr_interprocess_entity_t aux = {.session = session, .object_id = *entity_id};
    for (size_t i = 0; i < uxr_ip_map.entities_len; i++)
    {
        if (uxr_interprocess_entity_compare(&aux, &uxr_ip_map.entities[i]))
        {
            return (ssize_t)i;
        }
    }
    return -1;
}

void uxr_prepare_interprocess(uxrSession* session, uxrObjectId entity_id, ucdrBuffer* ub, uint16_t data_size)
{
    if (!uxr_ip_map.mem_pool_init)
    {
        uxr_init_static_list();
    }
    
    ssize_t datawriter_index = uxr_get_datawriter_index(session, &entity_id);
    if(-1 == datawriter_index){
        return;
    }
    for (size_t i = 0; i < uxr_ip_map.entities_len; i++)
    {
        if (uxr_ip_map.ip_matrix[datawriter_index][i].matched)
        {
            uxr_interprocess_buffer_item_t * item = uxr_pop_head_from_list(&uxr_ip_map.mempool_freeitems);
            if (item != NULL)
            {
                item->data = *ub;
                item->data_size = data_size;
                uxr_prepend_to_list(item, &uxr_ip_map.ip_matrix[datawriter_index][i].list);
            }
        }
    }
}

void uxr_handle_interprocess()
{
    for (size_t i = 0; i < uxr_ip_map.entities_len; i++)
    {
        if (uxr_ip_map.entities[i].object_id.type != UXR_DATAWRITER_ID)
        {
            continue;
        }
        
        for (size_t j = 0; j < uxr_ip_map.entities_len; j++)
        {
            while (uxr_ip_map.ip_matrix[i][j].list != NULL)
            {
                uxrStreamId stream_id = {.type = UXR_INTERPROCESS_STREAM};

                uxr_interprocess_buffer_item_t * item =  uxr_pop_head_from_list(&uxr_ip_map.ip_matrix[i][j].list);
                
                switch (uxr_ip_map.entities[j].object_id.type)
                {
                case UXR_DATAREADER_ID:
                    uxr_ip_map.entities[j].session->on_topic(
                        uxr_ip_map.entities[j].session,
                        uxr_ip_map.entities[j].object_id,
                        0, // Req ID = 0 means interprocess?
                        stream_id,
                        &item->data,
                        item->data_size,
                        uxr_ip_map.entities[j].session->on_topic_args
                    );                    
                    break;
                default:
                    break;
                }

                uxr_prepend_to_list(item, &uxr_ip_map.mempool_freeitems);
            }
        }
    }
}

void uxr_update_interprocess_matching(){
    for (size_t i = 0; i < uxr_ip_map.entities_len; i++)
    {
        for (size_t j = 0; j < uxr_ip_map.entities_len; j++)
        {
            uxr_ip_map.ip_matrix[i][j].matched = 
                i != j &&
                0 == memcmp(uxr_ip_map.entities[i].hash, uxr_ip_map.entities[j].hash, UXR_MATCHING_HASH_SIZE);
        }
    }
}

void uxr_add_interprocess_entity_xml(uxrSession* session, uxrObjectId entity_id, const char* xml)
{
    if (uxr_ip_map.entities_len < UXR_CONFIG_INTERPROCESS_MAX_ENTITIES - 1)
    {
        uxr_ip_map.entities[uxr_ip_map.entities_len].object_id = entity_id;
        uxr_ip_map.entities[uxr_ip_map.entities_len].session = session;
        uxr_generate_hash_from_xml(xml, entity_id, uxr_ip_map.entities[uxr_ip_map.entities_len].hash);
        uxr_ip_map.entities_len++;
        uxr_update_interprocess_matching();
    }
}
