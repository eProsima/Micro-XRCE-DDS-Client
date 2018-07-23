#include "ClientInteraction.hpp"

const char* Client::topic_xml_      = {"<dds><topic><name>HelloWorldTopic</name><dataType>HelloWorld</dataType></topic></dds>"};
const char* Client::publisher_xml_  = {"<publisher name=\"MyPublisher\""};
const char* Client::datawriter_xml_ = {"<profiles><publisher profile_name=\"default_xrce_publisher_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>10</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></publisher></profiles>"};
const char* Client::subscriber_xml_ = {"<publisher name=\"MySubscriber\""};
const char* Client::datareader_xml_ = {"<profiles><subscriber profile_name=\"default_xrce_subscriber_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>10</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></subscriber></profiles>"};

uint32_t Client::next_client_key_ = 0;


