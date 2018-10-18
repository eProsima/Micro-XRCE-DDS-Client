#include "ClientInteraction.hpp"


const char* Client::participant_xml_ = "<dds>"
                                           "<participant>"
                                               "<rtps>"
                                                   "<name>default_xrce_participant</name>"
                                               "</rtps>"
                                           "</participant>"
                                       "</dds>";

const char* Client::topic_xml_ = "<dds>"
                                     "<topic>"
                                         "<name>HelloWorldTopic</name>"
                                         "<dataType>HelloWorld</dataType>"
                                     "</topic>"
                                 "</dds>";

const char* Client::publisher_xml_  = "";
const char* Client::datawriter_xml_ = "<dds>"
                                          "<data_writer>"
                                              "<topic>"
                                                  "<kind>NO_KEY</kind>"
                                                  "<name>HelloWorldTopic</name>"
                                                  "<dataType>HelloWorld</dataType>"
                                                  "<historyQos>"
                                                      "<kind>KEEP_LAST</kind>"
                                                      "<depth>10</depth>"
                                                  "</historyQos>"
                                                  "<durability>"
                                                      "<kind>TRANSIENT_LOCAL</kind>"
                                                  "</durability>"
                                              "</topic>"
                                          "</data_writer>"
                                      "</dds>";


const char* Client::subscriber_xml_ = "";
const char* Client::datareader_xml_ = "<dds>"
                                          "<data_reader>"
                                              "<topic>"
                                                  "<kind>NO_KEY</kind>"
                                                  "<name>HelloWorldTopic</name>"
                                                  "<dataType>HelloWorld</dataType>"
                                                  "<historyQos>"
                                                      "<kind>KEEP_LAST</kind>"
                                                      "<depth>10</depth>"
                                                  "</historyQos>"
                                                  "<durability>"
                                                      "<kind>TRANSIENT_LOCAL</kind>"
                                                  "</durability>"
                                              "</topic>"
                                          "</data_reader>"
                                      "</dds>";


uint32_t Client::next_client_key_ = 0;


