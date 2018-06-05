#include <micrortps/client/session/session.h>
#include <microcdr/microcdr.h>

void read_message(MicroBuffer* message);
void read_submessages(MicroBuffer* submessages, StreamId stream);
void read_heartbeat(MicroBuffer* payload, StreamId stream);
void read_acknack(MicroBuffer* payload, StreamId stream);
void send_message(MicroBuffer* message, StreamId stream);
void send_heartbeat(OutputReliableStream* stream);
void send_acknack(InputReliableStream* stream);

