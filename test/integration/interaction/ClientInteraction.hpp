#ifndef IN_TEST_CLIENT_INTERACTION_HPP
#define IN_TEST_CLIENT_INTERACTION_HPP

#include "HelloWorld.h"
#include "Gateway.hpp"

#include <uxr/client/client.h>
#include <microcdr/microcdr.h>

#include <gtest/gtest.h>
#include <iostream>

class Client
{
public:
    Client(uint16_t port, float lost, uint16_t history)
    : gateway_(lost)
    , port_(port)
    , client_key_(++next_client_key_)
    , history_(history)
    {
        output_best_effort_stream_buffer_ = new uint8_t[UXR_CONFIG_UDP_TRANSPORT_MTU * UXR_CONFIG_MAX_OUTPUT_BEST_EFFORT_STREAMS]{0};
        output_reliable_stream_buffer_ = new uint8_t[UXR_CONFIG_UDP_TRANSPORT_MTU * history_ * UXR_CONFIG_MAX_OUTPUT_RELIABLE_STREAMS]{0};
        input_reliable_stream_buffer_ = new uint8_t[UXR_CONFIG_UDP_TRANSPORT_MTU * history_ * UXR_CONFIG_MAX_INPUT_RELIABLE_STREAMS]{0};

        init();
    }

    virtual ~Client()
    {
        close();

        delete[] output_best_effort_stream_buffer_;
        delete[] output_reliable_stream_buffer_;
        delete[] input_reliable_stream_buffer_;
    }

    void create_entities(uint8_t id, uint8_t stream_id_raw, uint8_t expected_status, uint8_t flags)
    {
        uxrStreamId output_stream_id = uxr_stream_id_from_raw(stream_id_raw, UXR_OUTPUT_STREAM);
        uint16_t request_id; uint8_t status;

        uxrObjectId participant_id = uxr_object_id(id, UXR_PARTICIPANT_ID);
        request_id = uxr_write_create_participant_ref(&session_, output_stream_id, participant_id, 0, "default_xrce_participant_profile", flags);
        ASSERT_NE(UXR_INVALID_REQUEST_ID, request_id);
        uxr_run_session_until_all_status(&session_, 60000, &request_id, &status, 1);
        ASSERT_EQ(expected_status, status);

        uxrObjectId topic_id = uxr_object_id(id, UXR_TOPIC_ID);
        request_id = uxr_write_configure_topic_xml(&session_, output_stream_id, topic_id, participant_id, topic_xml_, flags);
        ASSERT_NE(UXR_INVALID_REQUEST_ID, request_id);
        uxr_run_session_until_all_status(&session_, 60000, &request_id, &status, 1);
        ASSERT_EQ(expected_status, status);

        uxrObjectId publisher_id = uxr_object_id(id, UXR_PUBLISHER_ID);
        request_id = uxr_write_configure_publisher_xml(&session_, output_stream_id, publisher_id, participant_id, publisher_xml_, flags);
        ASSERT_NE(UXR_INVALID_REQUEST_ID, request_id);
        uxr_run_session_until_all_status(&session_, 60000, &request_id, &status, 1);
        ASSERT_EQ(expected_status, status);

        uxrObjectId datawriter_id = uxr_object_id(id, UXR_DATAWRITER_ID);
        request_id = uxr_write_configure_datawriter_xml(&session_, output_stream_id, datawriter_id, publisher_id, datawriter_xml_, flags);
        ASSERT_NE(UXR_INVALID_REQUEST_ID, request_id);
        uxr_run_session_until_all_status(&session_, 60000, &request_id, &status, 1);
        ASSERT_EQ(expected_status, status);

        uxrObjectId subscriber_id = uxr_object_id(id, UXR_SUBSCRIBER_ID);
        request_id = uxr_write_configure_subscriber_xml(&session_, output_stream_id, subscriber_id, participant_id, subscriber_xml_, flags);
        ASSERT_NE(UXR_INVALID_REQUEST_ID, request_id);
        uxr_run_session_until_all_status(&session_, 60000, &request_id, &status, 1);
        ASSERT_EQ(expected_status, status);

        uxrObjectId datareader_id = uxr_object_id(id, UXR_DATAREADER_ID);
        request_id = uxr_write_configure_datareader_xml(&session_, output_stream_id, datareader_id, subscriber_id, datareader_xml_, flags);
        ASSERT_NE(UXR_INVALID_REQUEST_ID, request_id);
        uxr_run_session_until_all_status(&session_, 60000, &request_id, &status, 1);
        ASSERT_EQ(expected_status, status);
    }

    void publish(uint8_t id, uint8_t stream_id_raw, size_t number)
    {
        //Used only for waiting the RTPS subscriber matching
        (void) uxr_run_session_time(&session_, 50);

        uxrStreamId output_stream_id = uxr_stream_id_from_raw(stream_id_raw, UXR_OUTPUT_STREAM);
        uxrObjectId datawriter_id = uxr_object_id(id, UXR_DATAWRITER_ID);

        for(size_t i = 0; i < number; ++i)
        {
            HelloWorld topic = {static_cast<uint32_t>(i), "Hello DDS world!"};

            ucdrBuffer mb;
            uint32_t topic_size = HelloWorld_size_of_topic(&topic, 0);
            bool prepared = uxr_prepare_output_stream(&session_, output_stream_id, datawriter_id, &mb, topic_size);
            ASSERT_TRUE(prepared);
            bool written = HelloWorld_serialize_topic(&mb, &topic);
            ASSERT_TRUE(written);
            bool sent = uxr_run_session_until_confirm_delivery(&session_, 60000);
            ASSERT_TRUE(sent);
            std::cout << "topic sent: " << i << std::endl;
        }
    }

    void subscribe(uint8_t id, uint8_t stream_id_raw, size_t number)
    {
        expected_topic_index_ = 0;
        last_topic_stream_id_ = uxr_stream_id_from_raw(0, UXR_OUTPUT_STREAM);
        last_topic_object_id_ = uxr_object_id(255, 15);

        uxrStreamId output_stream_id = uxr_stream_id(0, UXR_RELIABLE_STREAM, UXR_OUTPUT_STREAM);
        uxrStreamId input_stream_id = uxr_stream_id_from_raw(stream_id_raw, UXR_INPUT_STREAM);
        uxrObjectId datareader_id = uxr_object_id(id, UXR_DATAREADER_ID);

        uxrDeliveryControl delivery_control = {};
        delivery_control.max_samples = UXR_MAX_SAMPLES_UNLIMITED;
        uint16_t request_id = uxr_write_request_data(&session_, output_stream_id, datareader_id, input_stream_id, &delivery_control);
        ASSERT_NE(UXR_INVALID_REQUEST_ID, request_id);

        while(expected_topic_index_ < number)
        {
            uint8_t status;
            bool received_ok = uxr_run_session_until_all_status(&session_, 60000, &request_id, &status, 1);
            ASSERT_EQ(UXR_STATUS_OK, status);
            ASSERT_TRUE(received_ok);
            //ASSERT_EQ(last_topic_object_id_, datareader_id);
            //ASSERT_EQ(last_topic_stream_id_, input_stream_id);
        }
    }

private:
    static void on_topic_dispatcher(uxrSession* session_, uxrObjectId object_id, uint16_t request_id, uxrStreamId stream_id, struct ucdrBuffer* serialization, void* args)
    {
        static_cast<Client*>(args)->on_topic(session_, object_id, request_id, stream_id, serialization);
    }

    void init()
    {
        ASSERT_TRUE(uxr_init_udp_transport(&transport_, "127.0.0.1", port_));

        uxr_init_session(&session_, gateway_.monitorize(&transport_.comm), client_key_);
        uxr_set_topic_callback(&session_, on_topic_dispatcher, this);

        ASSERT_TRUE(uxr_create_session(&session_));
        ASSERT_EQ(UXR_STATUS_OK, session_.info.last_requested_status);

        for(int i = 0; i < UXR_CONFIG_MAX_OUTPUT_BEST_EFFORT_STREAMS; ++i)
        {
            uint8_t* buffer = output_best_effort_stream_buffer_ + UXR_CONFIG_UDP_TRANSPORT_MTU * i;
            (void) uxr_create_output_best_effort_stream(&session_, buffer, transport_.comm.mtu);
        }
        for(int i = 0; i < UXR_CONFIG_MAX_INPUT_BEST_EFFORT_STREAMS; ++i)
        {
            (void) uxr_create_input_best_effort_stream(&session_);
        }
        for(int i = 0; i < UXR_CONFIG_MAX_OUTPUT_RELIABLE_STREAMS; ++i)
        {
            uint8_t* buffer = output_reliable_stream_buffer_ + UXR_CONFIG_UDP_TRANSPORT_MTU * history_ * i;
            (void) uxr_create_output_reliable_stream(&session_, buffer , static_cast<size_t>(transport_.comm.mtu * history_), history_);
        }
        for(int i = 0; i < UXR_CONFIG_MAX_INPUT_RELIABLE_STREAMS; ++i)
        {
            uint8_t* buffer = input_reliable_stream_buffer_ + UXR_CONFIG_UDP_TRANSPORT_MTU * history_ * i;
            (void) uxr_create_input_reliable_stream(&session_, buffer, static_cast<size_t>(transport_.comm.mtu * history_), history_);
        }
    }

    void close()
    {
        bool deleted = uxr_delete_session(&session_);
        if(0.0f == gateway_.get_lost_value()) //because the agent only send one status to a delete in stream 0.
        {
            ASSERT_TRUE(deleted);
            ASSERT_EQ(UXR_STATUS_OK, session_.info.last_requested_status);
        }
        ASSERT_TRUE(uxr_close_udp_transport(&transport_));
    }


    void on_topic(uxrSession* /*session_*/, uxrObjectId object_id, uint16_t /*request_id*/, uxrStreamId stream_id, struct ucdrBuffer* serialization)
    {
        HelloWorld topic;
        HelloWorld_deserialize_topic(serialization, &topic);
        ASSERT_EQ(topic.index, expected_topic_index_);
        ASSERT_STREQ(topic.message, "Hello DDS world!");
        last_topic_object_id_ = object_id;
        last_topic_stream_id_ = stream_id;
        expected_topic_index_++;
        std::cout << "topic received: " << topic.index << std::endl;
    }

    static uint32_t next_client_key_;
    static const char* topic_xml_;
    static const char* publisher_xml_;
    static const char* subscriber_xml_;
    static const char* datawriter_xml_;
    static const char* datareader_xml_;

    Gateway gateway_;
    uint16_t port_;

    uint32_t client_key_;
    uint16_t history_;

    uxrUDPTransport transport_;
    uxrSession session_;

    uint8_t* output_best_effort_stream_buffer_;
    uint8_t* output_reliable_stream_buffer_;
    uint8_t* input_reliable_stream_buffer_;

    uxrObjectId last_topic_object_id_;
    uxrStreamId last_topic_stream_id_;
    size_t expected_topic_index_;

};

#endif //IN_TEST_CLIENT_INTERACTION_HPP
