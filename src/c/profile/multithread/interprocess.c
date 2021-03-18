#include <uxr/client/core/session/session.h>
#include <uxr/client/config.h>

#include "../../core/session/stream/stream_storage_internal.h"
#include "../matching/matching_internal.h"

#include <string.h>

//==================================================================
//                             PRIVATE
//==================================================================

// Limitations:
// Only one dw/dr topic can be save at the same time
// Dw should run session after serializing in one buffer

typedef struct uxr_interprocess_entity_t{
    uxrObjectId object_id;
    uxrSession* session;
    char hash[UXR_MATCHING_HASH_SIZE];
} uxr_interprocess_entity_t;

typedef struct uxr_interprocess_matrix_block_t{
    bool matched;
    ucdrBuffer data;
    uint16_t data_size;
} uxr_interprocess_matrix_block_t;

typedef struct uxr_interprocess_map_t{
    uxr_interprocess_entity_t entities[UXR_CONFIG_INTERPROCESS_MAX_ENTITIES];
    size_t entities_len;
    uxr_interprocess_matrix_block_t ip_matrix[UXR_CONFIG_INTERPROCESS_MAX_ENTITIES][UXR_CONFIG_INTERPROCESS_MAX_ENTITIES];
} uxr_interprocess_map_t;

static uxr_interprocess_map_t uxr_ip_map = {0};

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

size_t uxr_get_datawriter_index(uxrSession* session, uxrObjectId* entity_id){
    uxr_interprocess_entity_t aux = {.session = session, .object_id = *entity_id};
    for (size_t i = 0; i < uxr_ip_map.entities_len; i++)
    {
        if (uxr_interprocess_entity_compare(&aux, &uxr_ip_map.entities[i]))
        {
            return i;
        }
    }
    // TODO(pablogs): Handling errors here?
    return 0;
}

void uxr_prepare_interprocess(uxrSession* session, uxrObjectId entity_id, ucdrBuffer* ub, uint16_t data_size)
{
    size_t datawriter_index = uxr_get_datawriter_index(session, &entity_id);
    for (size_t i = 0; i < uxr_ip_map.entities_len; i++)
    {
        if (uxr_ip_map.ip_matrix[datawriter_index][i].matched)
        {
            uxr_ip_map.ip_matrix[datawriter_index][i].data = *ub;
            uxr_ip_map.ip_matrix[datawriter_index][i].data_size = data_size;
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
            if (uxr_ip_map.ip_matrix[i][j].data.init != NULL)
            {
                uxrStreamId stream_id = {0};

                switch (uxr_ip_map.entities[j].object_id.type)
                {
                case UXR_DATAREADER_ID:
                    uxr_ip_map.entities[j].session->on_topic(
                        uxr_ip_map.entities[j].session,
                        uxr_ip_map.entities[j].object_id,
                        0, // Req ID = 0 means interprocess?
                        stream_id,
                        &uxr_ip_map.ip_matrix[i][j].data,
                        uxr_ip_map.ip_matrix[i][j].data_size,
                        uxr_ip_map.entities[j].session->on_topic_args
                    );                    
                    break;
                default:
                    break;
                }

                uxr_ip_map.ip_matrix[i][j].data.init = NULL;
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
