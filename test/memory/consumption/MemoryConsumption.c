// Copyright 2018 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <micrortps/client/client.h>
#include <micrortps/client/core/serialization/xrce_protocol.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define STREAM_HISTORY  2
#define BUFFER_SIZE     MR_CONFIG_SERIAL_TRANSPORT_MTU * STREAM_HISTORY

void on_topic(mrSession* session, mrObjectId object_id, uint16_t request_id, mrStreamId stream_id, struct MicroBuffer* mb, void* args)
{
    (void) session;
    (void) object_id;
    (void) request_id;
    (void) stream_id;
    (void) mb;
    (void) args;
}

int main()
{
    // Open file descriptor and alloc memory.
    if (0 < mkfifo("/tmp/serial_fifo", S_IRWXU | S_IRWXG | S_IRWXO))
    {
        unlink("/tmp/serial_fifo");
        return 1;
    }
    int fd = open("/tmp/serial_fifo", O_RDWR | O_NONBLOCK);
    int* memory_flag = malloc(sizeof(int));

    // Transport.
    mrSerialTransport transport = {0};
    (void) mr_init_serial_transport_fd(&transport, fd, 0, 1);

    // Session.
    mrSession session;
    mr_init_session(&session, &transport.comm, 0xAAAAAAAA);
    mr_set_topic_callback(&session, on_topic, NULL);

    // Streams
    uint8_t output_reliable_stream_buffer[BUFFER_SIZE] = {0};
    mrStreamId reliable_out = mr_create_output_reliable_stream(&session, output_reliable_stream_buffer, BUFFER_SIZE, STREAM_HISTORY);

    uint8_t input_reliable_stream_buffer[BUFFER_SIZE] = {0};
    mrStreamId reliable_in = mr_create_input_reliable_stream(&session, input_reliable_stream_buffer, BUFFER_SIZE, STREAM_HISTORY);

    // Create entities.
    mrObjectId participant_id = mr_object_id(0x01, MR_PARTICIPANT_ID);
    const char* participant_ref = "default_xrce_participant_profile";
    uint16_t participant_req = mr_write_create_participant_ref(&session, reliable_out, participant_id, 0, participant_ref, MR_REPLACE);

    mrObjectId topic_id = mr_object_id(0x01, MR_TOPIC_ID);
    const char* topic_xml = "<dds><topic><name>HelloWorldTopic</name><dataType>HelloWorld</dataType></topic></dds>";
    uint16_t topic_req = mr_write_configure_topic_xml(&session, reliable_out, topic_id, participant_id, topic_xml, MR_REPLACE);

    mrObjectId publisher_id = mr_object_id(0x01, MR_PUBLISHER_ID);
    const char* publisher_xml = "<publisher name=\"MyPublisher\">";
    uint16_t publisher_req = mr_write_configure_publisher_xml(&session, reliable_out, publisher_id, participant_id, publisher_xml, MR_REPLACE);

    mrObjectId datawriter_id = mr_object_id(0x01, MR_DATAWRITER_ID);
    const char* datawriter_xml = "<profiles><publisher profile_name=\"default_xrce_publisher_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></publisher></profiles>";
    uint16_t datawriter_req = mr_write_configure_datawriter_xml(&session, reliable_out, datawriter_id, publisher_id, datawriter_xml, MR_REPLACE);

    mrObjectId subscriber_id = mr_object_id(0x01, MR_SUBSCRIBER_ID);
    const char* subscriber_xml = "<subscriber name=\"MySubscriber\">";
    uint16_t subscriber_req = mr_write_configure_subscriber_xml(&session, reliable_out, subscriber_id, participant_id, subscriber_xml, MR_REPLACE);

    mrObjectId datareader_id = mr_object_id(0x01, MR_DATAREADER_ID);
    const char* datareader_xml = "<profiles><subscriber profile_name=\"default_xrce_subscriber_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></subscriber></profiles>";
    uint16_t datareader_req = mr_write_configure_datareader_xml(&session, reliable_out, datareader_id, subscriber_id, datareader_xml, MR_REPLACE);

    uint8_t status[6];
    uint16_t request[6] = {participant_req, topic_req, publisher_req, datawriter_req, subscriber_req, datareader_req};
    (void) mr_run_session_until_status(&session, 0, request, status, 6);

    // Request topics
    mrDeliveryControl delivery_control = {0};
    delivery_control.max_samples = MR_MAX_SAMPLES_UNLIMITED;
    (void) mr_write_request_data(&session, reliable_out, datareader_id, reliable_in, &delivery_control);

    // Delete resources.
    mr_delete_session(&session);
    mr_close_serial_transport(&transport);

    // Free resources.
    int b = *memory_flag;
    free(memory_flag);
    unlink("/tmp/serial_fifo");

    return b;
}
