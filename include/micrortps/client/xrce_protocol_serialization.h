#ifndef _MICRORTPS_CLIENT_XRCE_PROTOCOL_SERIALIZATION_H_
#define _MICRORTPS_CLIENT_XRCE_PROTOCOL_SERIALIZATION_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "xrce_protocol_spec.h"
#include "aux_memory.h"

#include <microcdr/microcdr.h>


void serialize_Time_t(MicroBuffer* buffer, Time_t* input);
void deserialize_Time_t(MicroBuffer* buffer, Time_t* output, AuxMemory* aux);

void serialize_String_t(MicroBuffer* buffer, String_t* input);
void deserialize_String_t(MicroBuffer* buffer, String_t* output, AuxMemory* aux);

void serialize_BinarySequence_t(MicroBuffer* buffer, BinarySequence_t* input);
void deserialize_BinarySequence_t(MicroBuffer* buffer, BinarySequence_t* output, AuxMemory* aux);

void serialize_StringSequence_t(MicroBuffer* buffer, StringSequence_t* input);
void deserialize_StringSequence_t(MicroBuffer* buffer, StringSequence_t* output, AuxMemory* aux);

void serialize_OBJK_CLIENT_Representation(MicroBuffer* buffer, OBJK_CLIENT_Representation* input);
void deserialize_OBJK_CLIENT_Representation(MicroBuffer* buffer, OBJK_CLIENT_Representation* output, AuxMemory* aux);

void serialize_OBJK_Representation3Formats(MicroBuffer* buffer, OBJK_Representation3Formats* input);
void deserialize_OBJK_Representation3Formats(MicroBuffer* buffer, OBJK_Representation3Formats* output, AuxMemory* aux);


#ifdef __cplusplus
}
#endif

#endif //_MICRORTPS_CLIENT_XRCE_PROTOCOL_SERIALIZATION_H_
