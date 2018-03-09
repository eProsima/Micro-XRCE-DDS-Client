#ifndef SYNC_CLIENT_H
#define SYNC_CLIENT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "xrce_protocol_serialization.h"
#include <micrortps/client/client.h>

#include <stddef.h>
#include <stdint.h>

/* Create object defines. */
#define MICRORTPS_OBJK_TIMEOUT     100
#define MICRORTPS_OBJK_MAX_TRIES   100

/* Message sizes. */
#define HEADER_SIZE     0x08
#define SUBMESSAGE_SIZE 0x04
#define STATUS_MSG_SIZE 0x18

/* Micro-RTPS status. */
#define MICRORTPS_STATUS_OK         0x00
#define MICRORTPS_ERR_MAX_TRIES     0x01
#define MICRORTPS_ERR_SERIALIZATION 0x02

/* Buffer sizes. */
#define MIN_BUFFER_SIZE  64
#define MTU_SIZE        512



typedef struct NoneStream
{
    uint8_t buf[MIN_BUFFER_SIZE];
    MicroBuffer micro_buffer;

} NoneStream;

typedef struct BestEffordStream
{
    uint8_t buf[MTU_SIZE];
    MicroBuffer micro_buffer;
    uint16_t seq_num;

} BestEfford;

typedef struct ReliableStream
{
    uint8_t buf[MTU_SIZE];
    MicroBuffer micro_buffer;
    uint16_t seq_num;

} ReliableStream;

typedef struct UdpSession
{
    SessionId id;
    ClientKey key;
    locator_id_t tranport_id;
    /* TODO (Julian): add built-in streams. */
} SyncSession;

/**
 * @brief create_participant_by_ref
 *
 * @param state
 * @param object_id
 * @param ref
 * @param reuse
 * @param replace
 *
 * @return
 */
ResultStatus create_participant_by_ref(Session* state,
                                       const ObjectId object_id,
                                       const char* ref,
                                       bool reuse,
                                       bool replace);
/**
 * @brief create_topic_by_xml
 *
 * @param state
 * @param object_id
 * @param xml
 * @param participant_id
 * @param reuse
 * @param replace
 *
 * @return
 */
ResultStatus create_topic_by_xml(Session* state,
                                 const ObjectId object_id,
                                 const char* xml,
                                 const ObjectId participant_id,
                                 bool reuse,
                                 bool replace);
/**
 * @brief create_publisher_by_xml
 *
 * @param state
 * @param object_id
 * @param xml
 * @param participant_id
 * @param reuse
 * @param replace
 *
 * @return
 */
ResultStatus create_publisher_by_xml(Session* state,
                                     const ObjectId object_id,
                                     const char* xml,
                                     const ObjectId participant_id,
                                     bool reuse,
                                     bool replace);

/**
 * @brief create_subscriber_by_xml
 *
 * @param state
 * @param object_id
 * @param xml
 * @param participant_id
 * @param reuse
 * @param replace
 *
 * @return
 */
ResultStatus create_subscriber_by_xml(Session* state,
                                      const ObjectId object_id,
                                      const char* xml,
                                      const ObjectId participant_id,
                                      bool reuse,
                                      bool replace);
/**
 * @brief create_datawriter_by_xml
 *
 * @param state
 * @param object_id
 * @param xml
 * @param publisher_id
 * @param reuse
 * @param replace
 *
 * @return
 */
ResultStatus create_datawriter_by_xml(Session* state,
                                      const ObjectId object_id,
                                      const char* xml,
                                      const ObjectId publisher_id,
                                      bool reuse,
                                      bool replace);

/**
 * @brief create_datareader_by_xml
 *
 * @param state
 * @param object_id
 * @param xml
 * @param subscriber_id
 * @param reuse
 * @param replace
 *
 * @return
 */
ResultStatus create_datareader_by_xml(Session* state,
                                      const ObjectId object_id,
                                      const char* xml,
                                      const ObjectId subscriber_id,
                                      bool reuse,
                                      bool replace);
/**
 * @brief create_object_sync
 *
 * @param state
 * @param payload
 * @param timeout
 * @param reuse
 * @param replace
 *
 * @return
 */
ResultStatus create_object_sync(Session* state,
                                const CREATE_Payload* payload,
                                int timeout,
                                bool reuse,
                                bool replace);

#ifdef __cplusplus
}
#endif

#endif // SYNC_CLIENT_H
