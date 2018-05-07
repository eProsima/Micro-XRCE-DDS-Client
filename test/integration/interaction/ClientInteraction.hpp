#ifndef IN_TEST_CLIENT_INTERACTION_HPP
#define IN_TEST_CLIENT_INTERACTION_HPP

#include <micrortps/client/xrce_protocol_spec.h>
#include "HelloWorld.h"
#include <chrono>
#include <atomic>
#include <thread>

#define CLIENT_MAX_NUM_ATTEMPTS    100
#define CLIENT_MAX_TIME_WAIT        10
#define TOPIC_PREFIX              0x01


class Client
{
public:
    Client(const ClientKey& client_key, const uint8_t ip[4], const uint16_t port)
        : session_(),
          client_key_(client_key),
          ip_{ip[0], ip[1], ip[2], ip[4]},
          port_(port)
    {
    }

    bool init_session()
    {
        return new_udp_session(&session_, 0x01, client_key_, ip_, port_, NULL, NULL)
                && init_session_sync(&session_);
    }

    bool close_session()
    {
        return close_session_sync(&session_);
    }

    bool create_participant()
    {
        return create_participant_sync_by_ref(&session_, participant_id_, "default_participant", false, false);
    }

    bool create_topic()
    {
        return create_topic_sync_by_xml(&session_, topic_id_, topic_xml_, participant_id_, false, false);
    }

    bool create_publisher()
    {
        return create_publisher_sync_by_xml(&session_, publisher_id_, publisher_xml_, participant_id_, false, false);
    }

    bool create_subscriber()
    {
        return create_subscriber_sync_by_xml(&session_, subscriber_id_, subscriber_xml_, participant_id_, false, false);
    }

    bool create_datawriter()
    {
        return create_datawriter_sync_by_xml(&session_, datawriter_id_, datawriter_xml_, publisher_id_, false, false);
    }

    bool create_datareader()
    {
        return create_datareader_sync_by_xml(&session_, datareader_id_, datareader_xml_, subscriber_id_, false, false);
    }

    bool delete_object(ObjectId id)
    {
        return delete_object_sync(&session_, id);
    }

    const ClientKey& get_client_key() const { return client_key_; }

    const ObjectId& get_participant_id()   const { return participant_id_; }
    const ObjectId& get_topic_id()         const { return topic_id_; }
    const ObjectId& get_publisher_id()     const { return publisher_id_; }
    const ObjectId& get_datawriter_id()    const { return datawriter_id_; }
    const ObjectId& get_subscriber_id()    const { return subscriber_id_; }
    const ObjectId& get_datareader_id()    const { return datareader_id_; }

protected:
    /* Session config. */
    Session session_;
    const ClientKey client_key_;
    const uint8_t ip_[4];
    const uint16_t port_;

    /* Object IDs. */
    static const ObjectId participant_id_;
    static const ObjectId topic_id_;
    static const ObjectId publisher_id_;
    static const ObjectId datawriter_id_;
    static const ObjectId subscriber_id_;
    static const ObjectId datareader_id_;

    /* XMLs. */
    static const char* topic_xml_;
    static const char* publisher_xml_;
    static const char* datawriter_xml_;
    static const char* subscriber_xml_;
    static const char* datareader_xml_;

    /* Utils. */
    static std::atomic<bool> running_cond_;
};

/****************************************************************************************
 * Publisher
 ****************************************************************************************/
class Publisher : public Client
{
public:
    Publisher(const ClientKey& client_key, const uint8_t ip[4], const uint16_t port)
        : Client(client_key, ip, port) {}

    bool init()
    {
        bool result = true;
        result &= new_udp_session(&session_, 0x01, client_key_, ip_, port_, NULL, NULL);
        result &= init_session_sync(&session_);
        result &= create_participant_sync_by_ref(&session_, participant_id_, "default_participant", false, false);
        result &= create_topic_sync_by_xml(&session_, topic_id_, topic_xml_, participant_id_, false, false);
        result &= create_publisher_sync_by_xml(&session_, publisher_id_, publisher_xml_, participant_id_, false, false);
        result &= create_datawriter_sync_by_xml(&session_, datawriter_id_, datawriter_xml_, publisher_id_, false, false);
        return result;
    }

    void launch()
    {
        for (uint32_t i = 0; i < CLIENT_MAX_NUM_ATTEMPTS && running_cond_; ++i)
        {
            write_HelloWorld(&session_, datawriter_id_, STREAMID_BUILTIN_RELIABLE, &topic_);
            run_communication(&session_);
            std::this_thread::sleep_for(std::chrono::milliseconds(CLIENT_MAX_TIME_WAIT));
        }
    }

    const HelloWorld& get_topic() const { return topic_; }

private:
    char topic_msg_[32] = "Hello DDS World";
    const HelloWorld topic_ = {11, topic_msg_};
};

/****************************************************************************************
 * Subscriber
 ****************************************************************************************/
class Subscriber : public Client
{
public:
    Subscriber(const ClientKey& client_key, const uint8_t ip[4], uint16_t port)
        : Client(client_key, ip, port) {}

    bool init()
    {
        bool result = true;
        result &= new_udp_session(&session_, 0x01, client_key_, ip_, port_, on_topic, NULL);
        result &= init_session_sync(&session_);
        result &= create_participant_sync_by_ref(&session_, participant_id_, "default_participant", false, false);
        result &= create_topic_sync_by_xml(&session_, topic_id_, topic_xml_, participant_id_, false, false);
        result &= create_subscriber_sync_by_xml(&session_, subscriber_id_, subscriber_xml_, participant_id_, false, false);
        result &= create_datareader_sync_by_xml(&session_, datareader_id_, datareader_xml_, subscriber_id_, false, false);
        return result;
    }

    void launch()
    {
        for (uint32_t i = 0; i < CLIENT_MAX_NUM_ATTEMPTS && running_cond_; ++i)
        {
            read_data_sync(&session_, datareader_id_, STREAMID_BUILTIN_RELIABLE);
            run_communication(&session_);
            std::this_thread::sleep_for(std::chrono::milliseconds(CLIENT_MAX_TIME_WAIT));
        }
        running_cond_ = false;
    }

    const HelloWorld& get_topic() const { return topic_; }

private:
    static void on_topic(ObjectId id, MicroBuffer* serialized_topic, void* /*args*/)
    {
        HelloWorld topic;
        if (TOPIC_PREFIX == id.data[0])
        {
            deserialize_HelloWorld_topic(serialized_topic, &topic);
            running_cond_ = false;
        }
        topic_.index = topic.index;
        memcpy(topic_.message, topic.message, strlen(topic.message));
        return;
    }

private:
    static char topic_msg_[32];
    static HelloWorld topic_;
};

#endif //IN_TEST_CLIENT_INTERACTION_HPP
