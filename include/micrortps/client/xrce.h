#ifndef _DDS_XRCE_CLIENT_XRCE_H_
#define _DDS_XRCE_CLIENT_XRCE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define OBJECT_PARTICIPANT   0x01
#define OBJECT_DATA_WRITER   0x03
#define OBJECT_DATA_READER   0x07
#define OBJECT_SUBSCRIBER    0x08
#define OBJECT_PUBLISHER     0x09

void xrce_create(uint8_t object_kind, uint16_t id, char* dds_xml);
void xrce_delete(uint16_t id);

void update(uint16_t id, char* dds_xml);
void write_data(uint16_t id, void* topic);

#ifdef __cplusplus
}
#endif

#endif //_DDS_XRCE_CLIENT_XRCE_H_
