#include <micrortps/client/session/session.h>
#include <microcdr/microcdr.h>

void read_message(MicroBuffer* message);
void read_submessages(MicroBuffer* submessages, int stream_id);
void read_heartbeat(MicroBuffer* payload, int stream_id);
void read_acknack(MicroBuffer* payload, int stream_id);
void send_message(MicroBuffer* message, int stream_id);
void send_heartbeat(OutputReliableStream* stream);
void send_acknack(InputReliableStream* stream);
