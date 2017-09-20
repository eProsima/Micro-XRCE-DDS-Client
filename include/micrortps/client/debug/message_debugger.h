#ifndef _DDS_XRCE_MESSAGE_DEBUGGER_H_
#define _DDS_XRCE_MESSAGE_DEBUGGER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "micrortps/client/message.h"


int print_message_header(const MessageHeaderSpec* header, void* data);
void print_submessage_header(const SubmessageHeaderSpec* header, void* data);

void print_create_submessage(const CreatePayloadSpec* payload, void* data);
void print_delete_submessage(const DeletePayloadSpec* payload, void* data);
void print_status_submessage(const StatusPayloadSpec* payload, void* data);
void print_write_data_submessage(const WriteDataPayloadSpec* payload, void* data);
void print_read_data_submessage(const ReadDataPayloadSpec* payload, void* data);
void print_data_submessage(const DataPayloadSpec* payload, void* data);

void printl_create_submessage(const CreatePayloadSpec* payload, void* data);
void printl_delete_submessage(const DeletePayloadSpec* payload, void* data);
void printl_status_submessage(const StatusPayloadSpec* payload, void* data);
void printl_write_data_submessage(const WriteDataPayloadSpec* payload, void* data);
void printl_read_data_submessage(const ReadDataPayloadSpec* payload, void* data);
void printl_data_submessage(const DataPayloadSpec* payload, void* data);

//UTIL
const char* data_to_string(const uint8_t* data, uint32_t size);


#ifdef __cplusplus
}
#endif

#endif //_DDS_XRCE_MESSAGE_DEBUGGER_H_
