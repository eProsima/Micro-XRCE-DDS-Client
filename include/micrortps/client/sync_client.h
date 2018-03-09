#ifndef SYNC_CLIENT_H
#define SYNC_CLIENT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <micrortps/client/xrce_protocol_serialization.h>
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

ResultStatus create_participant_by_ref(ClientState* state,
                                       const ObjectId object_id,
                                       const char* ref,
                                       bool reuse,
                                       bool replace);

ResultStatus create_topic_by_xml(ClientState* state,
                                 const ObjectId object_id,
                                 const char* xml,
                                 const ObjectId participant_id,
                                 bool reuse,
                                 bool replace);

ResultStatus create_publisher_by_xml(ClientState* state,
                                     const ObjectId object_id,
                                     const char* xml,
                                     const ObjectId participant_id,
                                     bool reuse,
                                     bool replace);

ResultStatus create_subscriber_by_xml(ClientState* state,
                                      const ObjectId object_id,
                                      const char* xml,
                                      const ObjectId participant_id,
                                      bool reuse,
                                      bool replace);

ResultStatus create_datawriter_by_xml(ClientState* state,
                                      const ObjectId object_id,
                                      const char* xml,
                                      const ObjectId publisher_id,
                                      bool reuse,
                                      bool replace);

ResultStatus create_datareader_by_xml(ClientState* state,
                                      const ObjectId object_id,
                                      const char* xml,
                                      const ObjectId subscriber_id,
                                      bool reuse,
                                      bool replace);

ResultStatus create_object_sync(ClientState* state,
                                const CREATE_Payload* payload,
                                int timeout,
                                bool reuse,
                                bool replace);

#ifdef __cplusplus
}
#endif

#endif // SYNC_CLIENT_H
