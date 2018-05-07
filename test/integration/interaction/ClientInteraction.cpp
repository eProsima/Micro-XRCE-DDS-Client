#include "ClientInteraction.hpp"

const ObjectId Client::participant_id_ = {{0x00, 0x01}};
const ObjectId Client::topic_id_       = {{0x00, 0x02}};
const ObjectId Client::publisher_id_   = {{0x00, 0x03}};
const ObjectId Client::datawriter_id_  = {{0x00, 0x05}};
const ObjectId Client::subscriber_id_  = {{TOPIC_PREFIX, 0x04}};
const ObjectId Client::datareader_id_  = {{TOPIC_PREFIX, 0x06}};

const char* Client::topic_xml_      = {"<dds><topic><name>HelloWorldTopic</name><dataType>HelloWorld</dataType></topic></dds>"};
const char* Client::publisher_xml_  = {"<publisher name=\"MyPublisher\""};
const char* Client::datawriter_xml_ = {"<profiles><publisher profile_name=\"default_xrce_publisher_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></publisher></profiles>"};
const char* Client::subscriber_xml_ = {"<publisher name=\"MySubscriber\""};
const char* Client::datareader_xml_ = {"<profiles><subscriber profile_name=\"default_xrce_subscriber_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></subscriber></profiles>"};

std::atomic<bool> Client::running_cond_{true};

char Subscriber::topic_msg_[32] = {'\0'};
HelloWorld Subscriber::topic_ = {0, Subscriber::topic_msg_};
