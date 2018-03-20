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
#define HEADER_MIN_SIZE    0x04
#define HEADER_MAX_SIZE    0x08
#define SUBHEADER_SIZE    0x04
#define PAYLOAD_DATA_SIZE  0x04
#define STATUS_MSG_SIZE    0x18
#define HEARTBEAT_MSG_SIZE 0x04

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
#define MICRORTPS_MAX_TOPICS_READ  10

/* Streams configuration. */
#define INPUT_BEST_EFFORT_STREAMS   1
#define OUTPUT_BEST_EFFORT_STREAMS  1
#define INPUT_RELIABLE_STREAMS      1
#define OUTPUT_RELIABLE_STREAMS     1

/*
 * Streams.
 */

typedef struct BestEffortStream
{
    uint16_t seq_num;

    uint8_t buf[MICRORTPS_MTU_SIZE];
    MicroBuffer micro_buffer;

} BestEffortStream;

typedef struct ReliableStream
{
    uint16_t seq_num;

    struct Buffer
    {
        uint8_t buf[MICRORTPS_MTU_SIZE];
        MicroBuffer micro_buffer;

    } store[MICRORTPS_MAX_MSG_NUM];

} ReliableStream;

typedef void (*OnTopic)(uint32_t id, MicroBuffer* message, void *args);

typedef struct SyncSession
{
    SessionId id;
    ClientKey key;
    locator_id_t transport_id;
    uint16_t request_id;

    uint8_t header_offset;

    BestEffortStream input_best_effort_stream;
    BestEffortStream output_best_effort_stream;
    ReliableStream input_reliable_stream;
    ReliableStream output_reliable_stream;

    OnTopic on_topic_callback;
    void* on_topic_args;

    ResultStatus last_status;

} SyncSession;


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
 * @param session
 * @param object_id
 * @param ref
 * @param reuse
 * @param replace
 *
 * @return
 */
ResultStatus create_participant_by_ref(SyncSession* session,
                                       const ObjectId object_id,
                                       const char* ref,
                                       bool reuse,
                                       bool replace);
/**
 * @brief create_topic_by_xml
 *
 * @param session
 * @param object_id
 * @param xml
 * @param participant_id
 * @param reuse
 * @param replace
 *
 * @return
 */
ResultStatus create_topic_by_xml(SyncSession* session,
                                 const ObjectId object_id,
                                 const char* xml,
                                 const ObjectId participant_id,
                                 bool reuse,
                                 bool replace);
/**
 * @brief create_publisher_by_xml
 *
 * @param session
 * @param object_id
 * @param xml
 * @param participant_id
 * @param reuse
 * @param replace
 *
 * @return
 */
ResultStatus create_publisher_by_xml(SyncSession* session,
                                     const ObjectId object_id,
                                     const char* xml,
                                     const ObjectId participant_id,
                                     bool reuse,
                                     bool replace);

/**
 * @brief create_subscriber_by_xml
 *
 * @param session
 * @param object_id
 * @param xml
 * @param participant_id
 * @param reuse
 * @param replace
 *
 * @return
 */
ResultStatus create_subscriber_by_xml(SyncSession* session,
                                      const ObjectId object_id,
                                      const char* xml,
                                      const ObjectId participant_id,
                                      bool reuse,
                                      bool replace);
/**
 * @brief create_datawriter_by_xml
 *
 * @param session
 * @param object_id
 * @param xml
 * @param publisher_id
 * @param reuse
 * @param replace
 *
 * @return
 */
ResultStatus create_datawriter_by_xml(SyncSession* session,
                                      const ObjectId object_id,
                                      const char* xml,
                                      const ObjectId publisher_id,
                                      bool reuse,
                                      bool replace);

/**
 * @brief create_datareader_by_xml
 *
 * @param session
 * @param object_id
 * @param xml
 * @param subscriber_id
 * @param reuse
 * @param replace
 *
 * @return
 */
ResultStatus create_datareader_by_xml(SyncSession* session,
                                      const ObjectId object_id,
                                      const char* xml,
                                      const ObjectId subscriber_id,
                                      bool reuse,
                                      bool replace);
/**
 * @brief create_object_sync
 *
 * @param session
 * @param payload
 * @param timeout
 * @param reuse
 * @param replace
 *
 * @return
 */
ResultStatus create_object_sync(SyncSession* session,
                                const CREATE_Payload* payload,
                                int timeout,
                                bool reuse,
                                bool replace);


MicroBuffer* prepare_best_effort_stream_for_topic(BestEffortStream* output_stream, uint16_t topic_size);
MicroBuffer* prepare_reliable_stream_for_topic(ReliableStream* output_stream, uint16_t topic_size);

bool send_best_effort_message(SyncSession* session, BestEffortStream* output_stream);
bool send_reliable_message(SyncSession* session, ReliableStream* output_stream);

bool receive_best_effort(BestEffortStream* input_stream, const uint16_t seq_num);
bool receive_reliable(ReliableStream* input_stream, MicroBuffer* submessages, uint16_t seq_num);

bool send_heartbeat(SyncSession* session, ReliableStream* reference_stream);
void check_acknack(SyncSession* session, ReliableStream* output_stream, uint16_t first_unacked_seq_num, uint8_t bitmap[2]);

void process_message(SyncSession* session, MicroBuffer* input_buffer);
void process_submessages(SyncSession* session, MicroBuffer* micro_buffer);
void process_submessage(SyncSession* session, MicroBuffer* micro_buffer);

void run_communication(SyncSession* session);

#ifdef __cplusplus
}
#endif

#endif // SYNC_CLIENT_H
