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
#define MICRORTPS_OBJK_TIMEOUT      10
#define MICRORTPS_MAX_ATTEMPTS     100

/* Message sizes. */
#define HEADER_SIZE     0x08
#define SUBMESSAGE_SIZE 0x04
#define STATUS_MSG_SIZE 0x18

/* Micro-RTPS status. */
#define MICRORTPS_STATUS_OK         0x00
#define MICRORTPS_ERR_MAX_ATTEMPTS  0x01
#define MICRORTPS_ERR_SERIALIZATION 0x02
#define MICRORTPS_ERR_LOCATOR       0x03
#define MICRORTPS_ERR_STREAMID      0x04
#define MICRORTPS_ERR_STREAM_EXISTS 0x05

/* Buffer sizes. */
#define MICRORTPS_MIN_BUFFER_SIZE  64
#define MICRORTPS_MTU_SIZE        512
#define MICRORTPS_MAX_MSG_NUM      16

/* Streams configuration. */
#define INPUT_BEST_EFFORT_STREAMS   1
#define OUTPUT_BEST_EFFORT_STREAMS  1
#define INPUT_RELIABLE_STREAMS      1
#define OUTPUT_RELIABLE_STREAMS     1

/*
 * Streams.
 */
typedef struct NoneStream
{
    uint8_t buf[MICRORTPS_MIN_BUFFER_SIZE];
    MicroBuffer micro_buffer;

} NoneStream;

typedef struct BestEffortStream
{
    StreamId id;
    uint8_t buf[MICRORTPS_MTU_SIZE];
    MicroBuffer micro_buffer;
    uint16_t seq_num;

    struct BestEffortStream* next_stream;
    struct BestEffortStream* prev_stream;

} BestEffortStream;

typedef struct ReliableStream
{
    StreamId id;
    uint8_t buf[MICRORTPS_MAX_MSG_NUM][MICRORTPS_MTU_SIZE];
    MicroBuffer micro_buffers[MICRORTPS_MAX_MSG_NUM];
    uint16_t seq_num;
    uint16_t bit_mask;

    struct ReliableStream* next_stream;
    struct ReliableStream* prev_stream;

} ReliableStream;

typedef struct BuiltinStreams
{
    NoneStream none_stream;
    BestEffortStream best_effort_stream;
    ReliableStream reliable_stream;

} BuiltinStreams;

typedef struct UserStreams
{
    BestEffortStream* best_effort_streams;
    ReliableStream* reliable_streams;

} UserStreams;

/*
 * New session.
 */
typedef struct SyncSession
{
    SessionId id;
    ClientKey key;
    locator_id_t transport_id;
    uint16_t request_id;

    /* Builtin streams. */
    BuiltinStreams in_builtin_streams;
    BuiltinStreams out_builtin_streams;

    /* User defined streams. */
    UserStreams in_user_streams;
    UserStreams out_user_streams;

} SyncSession;

uint8_t add_reliable_stream(SyncSession* session,
                            ReliableStream* stream,
                            const StreamId id,
                            bool output);

uint8_t add_best_effort_stream(SyncSession* session,
                            BestEffortStream* stream,
                            const StreamId id,
                            bool output);

void remove_reliable_stream(SyncSession* session,
                            const StreamId id,
                            bool output);

void remove_best_effort_stream(SyncSession* session,
                               const StreamId id,
                               bool output);

uint8_t* get_buffer(StreamId* id);
bool write_topic(SyncSession* session, StreamId id, MicroBuffer* micro_buffer);
//bool write_HelloWorld_topic(SyncSession* session, StreamId id, Serialize serialize, void* topic);

uint8_t new_udp_session_sync(SyncSession* session,
                             SessionId id,
                             ClientKey key,
                             uint16_t send_port,
                             uint16_t recv_port,
                             uint16_t remote_port,
                             const char* server_ip);

ResultStatus init_session_syn(SyncSession* session);

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
ResultStatus create_participant_by_ref(SyncSession* state,
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
ResultStatus create_topic_by_xml(SyncSession* state,
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
ResultStatus create_publisher_by_xml(SyncSession* state,
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
ResultStatus create_subscriber_by_xml(SyncSession* state,
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
ResultStatus create_datawriter_by_xml(SyncSession* state,
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
ResultStatus create_datareader_by_xml(SyncSession* state,
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
ResultStatus create_object_sync(SyncSession* state,
                                const CREATE_Payload* payload,
                                int timeout,
                                bool reuse,
                                bool replace);

#ifdef __cplusplus
}
#endif

#endif // SYNC_CLIENT_H
