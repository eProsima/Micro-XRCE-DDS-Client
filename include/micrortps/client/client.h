#ifndef _MICRORTPS_CLIENT_CLIENT_H_
#define _MICRORTPS_CLIENT_CLIENT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <micrortps/client/config.h>
#include <micrortps/client/xrce_client.h>
#include <micrortps/transport/micrortps_transport.h>
#include "xrce_protocol_serialization.h"

#include <stddef.h>
#include <stdint.h>

/* Create object defines. */
#define MICRORTPS_TIMEOUT           10
#define MICRORTPS_MAX_ATTEMPTS     100

/* Message sizes. */
#define HEADER_MIN_SIZE    0x04
#define HEADER_MAX_SIZE    0x08
#define SUBHEADER_SIZE     0x04
#define PAYLOAD_DATA_SIZE  0x08
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

/* Streams configuration. */
#define INPUT_BEST_EFFORT_STREAMS   1
#define OUTPUT_BEST_EFFORT_STREAMS  1
#define INPUT_RELIABLE_STREAMS      1
#define OUTPUT_RELIABLE_STREAMS     1

/*
 * Streams.
 */

bool create_reliable_object_sync(Session* session, ReliableStream* output_stream, const CREATE_Payload* payload, bool reuse, bool replace);

MicroBuffer* prepare_reliable_stream(ReliableStream* output_stream, uint8_t submessage_id, uint16_t payload_size);

void stamp_header(Session* session, MicroBuffer* output_buffer, StreamId id, uint16_t seq_num);

bool run_until_status(Session* session);

uint16_t get_num_request_id(RequestId request_id);
RequestId get_raw_request_id(uint16_t request_id);
uint16_t get_num_object_id(ObjectId object_id);
ObjectId get_raw_object_id(uint16_t object_id);

#ifdef __cplusplus
}
#endif

#endif // _MICRORTPS_CLIENT_CLIENT_H
