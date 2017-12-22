// Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
#include <micrortps/client/xrce_protocol_spec.h>

#include <microcdr/microcdr.h>

#include <gtest/gtest.h>
#include <cstdint>
#include <unistd.h>

#include <thread>

#include <fstream>

#define MESSAGE_TRIES_WAIT 100

typedef struct HelloWorld
{
    uint32_t index;
    char* message;
} HelloTopic;

typedef struct ShapeTopic
{
    char*    color;
    uint32_t x;
    uint32_t y;
    uint32_t shapesize;
} ShapeTopic;

bool serialize_hello_topic(MicroBuffer* writer, const AbstractTopic* topic_structure);
HelloTopic* deserialize_hello_message(MicroBuffer *message);
void deallocate_hello_topic(HelloTopic* topic);


bool serialize_shape_topic(MicroBuffer* writer, const AbstractTopic* topic_structure);
ShapeTopic* deserialize_shape_message(MicroBuffer* message);
void deallocate_shape_topic(ShapeTopic* topic);

void on_shape_topic(XRCEInfo info, MicroBuffer *message, void* args);
void on_hello_topic(XRCEInfo info, MicroBuffer *message, void* args);
void on_status(XRCEInfo info, uint8_t operation, uint8_t status, void* args);

void printl_shape_topic(const ShapeTopic* shape_topic);
void printl_hello_topic(const HelloTopic* hellow_topic);

class ClientTests : public ::testing::Test
{
    public:
        ClientTests()
        {
            state = new_udp_client_state(MAX_MESSAGE_SIZE, "127.0.0.1", 2020, 2019);

            statusObjectId = 0x0000;
            statusRequestId = 0x0000;
            statusOperation = 0xFF;
            statusImplementation = 0xFF;

            lastObject = 0x0000;
            lastRequest = 0x0000;

            topicCount = 0;
        }

        ~ClientTests()
        {
            free_client_state(state);
        }

        void waitMessage()
        {
            int messageWaitCounter = 0;
            while(!receive_from_agent(state) && messageWaitCounter < MESSAGE_TRIES_WAIT)
            {
                usleep(1000);
                messageWaitCounter++;
            }

            ASSERT_LT(messageWaitCounter, MESSAGE_TRIES_WAIT);
        }

        void checkStatus(uint8_t operation)
        {
            ASSERT_EQ(statusObjectId, lastObject);
            ASSERT_EQ(statusRequestId, lastRequest);
            ASSERT_EQ(statusOperation, operation);
            ASSERT_EQ(statusImplementation, STATUS_OK);
        }

        void checkDataTopic(int expectedNumTopic)
        {
            ASSERT_EQ(expectedNumTopic, topicCount);
        }

        uint16_t createClient()
        {
            XRCEInfo info = create_client(state, on_status, this);
            lastObject = info.object_id;
            lastRequest = info.request_id;
            send_to_agent(state);

            waitMessage();

            return info.object_id;
        }

        uint16_t createParticipant()
        {
            XRCEInfo info = create_participant(state);
            lastObject = info.object_id;
            lastRequest = info.request_id;
            send_to_agent(state);

            waitMessage();

            return info.object_id;
        }

        uint16_t createTopic(const char* file_name, uint16_t participant_id)
        {
            char data[4096];
            String xml = {data, 0};
            std::ifstream in(file_name, std::ifstream::in);
            if(in.is_open())
            {
                in.seekg (0, in.end);
                xml.length = in.tellg();
                in.seekg (0, in.beg);
                in.read(data, xml.length);
            }

            XRCEInfo info = create_topic(state, participant_id, xml);
            lastObject = info.object_id;
            lastRequest = info.request_id;
            send_to_agent(state);

            waitMessage();

            return info.object_id;
        }

        uint16_t createPublisher(uint16_t participant_id)
        {
            XRCEInfo info = create_publisher(state, participant_id);
            lastObject = info.object_id;
            lastRequest = info.request_id;
            send_to_agent(state);

            waitMessage();

            return info.object_id;
        }

        uint16_t createSubscriber(uint16_t participant_id)
        {
            XRCEInfo info = create_subscriber(state, participant_id);
            lastObject = info.object_id;
            lastRequest = info.request_id;
            send_to_agent(state);

            waitMessage();

            return info.object_id;
        }

        uint16_t createDataWriter(const char* file_name, uint16_t participant_id, uint16_t publisher_id)
        {
            char data[4096];
            String xml = {data, 0};
            std::ifstream in(file_name, std::ifstream::in);
            if(in.is_open())
            {
                in.seekg (0, in.end);
                xml.length = in.tellg();
                in.seekg (0, in.beg);
                in.read(data, xml.length);
            }

            XRCEInfo info = create_data_writer(state, participant_id, publisher_id, xml);
            lastObject = info.object_id;
            lastRequest = info.request_id;
            send_to_agent(state);

            waitMessage();

            return info.object_id;
        }

        uint16_t createDataReader(const char* file_name, uint16_t participant_id, uint16_t subscriber_id)
        {
            char data[4096];
            String xml = {data, 0};
            std::ifstream in(file_name, std::ifstream::in);
            if(in.is_open())
            {
                in.seekg (0, in.end);
                xml.length = in.tellg();
                in.seekg (0, in.beg);
                in.read(data, xml.length);
            }

            XRCEInfo info = create_data_reader(state, participant_id, subscriber_id, xml);
            lastObject = info.object_id;
            lastRequest = info.request_id;
            send_to_agent(state);

            waitMessage();

            return info.object_id;
        }

        void writeHelloData(uint16_t data_writer_id)
        {
            char message[] = "Hello data sample";
            HelloTopic hello_topic = {10, message};
            XRCEInfo info = write_data(state, data_writer_id, serialize_hello_topic, &hello_topic);
            printl_hello_topic(&hello_topic);
            lastObject = info.object_id;
            lastRequest = info.request_id;
            send_to_agent(state);

            waitMessage();
        }

        void readHelloData(uint16_t data_reader_id)
        {
            XRCEInfo info = read_data(state, data_reader_id, on_hello_topic, this);
            lastObject = info.object_id;
            lastRequest = info.request_id;
            send_to_agent(state);

            waitMessage();
        }

        void writeShapeData(uint16_t data_writer_id)
        {
            char topicColor[64] = "PURPLE";
            ShapeTopic shape_topic = {topicColor, 100, 100, 50};
            XRCEInfo info = write_data(state, data_writer_id, serialize_shape_topic, &shape_topic);
            printl_shape_topic(&shape_topic);
            lastObject = info.object_id;
            lastRequest = info.request_id;
            send_to_agent(state);

            waitMessage();
        }

        void readShapeData(uint16_t data_reader_id)
        {
            XRCEInfo info = read_data(state, data_reader_id, on_shape_topic, this);
            lastObject = info.object_id;
            lastRequest = info.request_id;
            send_to_agent(state);

            waitMessage();
        }

        void deleteXRCEObject(uint16_t id)
        {
            XRCEInfo info = delete_resource(state, id);
            lastObject = info.object_id;
            lastRequest = info.request_id;
            send_to_agent(state);

            waitMessage();
        }

        ClientState* state;

        uint16_t statusObjectId;
        uint16_t statusRequestId;
        uint8_t statusOperation;
        uint8_t statusImplementation;

        uint16_t lastRequest;
        uint16_t lastObject;

        int topicCount;
};

bool serialize_hello_topic(MicroBuffer* writer, const AbstractTopic* topic_structure)
{
    HelloTopic* topic = (HelloTopic*) topic_structure->topic;
    serialize_uint32_t(writer, topic->index);
    serialize_uint32_t(writer, strlen(topic->message) + 1);
    serialize_array_char(writer, topic->message, strlen(topic->message) + 1);

    return true;
}

HelloTopic* deserialize_hello_message(MicroBuffer* message)
{
    HelloTopic* topic = (HelloTopic*) malloc(sizeof(HelloTopic));
    deserialize_uint32_t(message, &topic->index);
    uint32_t size_message = 0;
    deserialize_uint32_t(message, &size_message);
    topic->message = (char*) malloc(size_message);
    deserialize_array_char(message, topic->message, size_message);

    return topic;
}

void deallocate_hello_topic(HelloTopic* topic)
{
    free(topic->message);
    free(topic);
}

bool serialize_shape_topic(MicroBuffer* writer, const AbstractTopic* topic_structure)
{
    ShapeTopic* topic = (ShapeTopic*) topic_structure->topic;
    serialize_uint32_t(writer, strlen(topic->color) + 1);
    serialize_array_char(writer, topic->color, strlen(topic->color) + 1);
    serialize_uint32_t(writer, topic->x);
    serialize_uint32_t(writer, topic->y);
    serialize_uint32_t(writer, topic->shapesize);

    return true;
}

ShapeTopic* deserialize_shape_message(MicroBuffer* message)
{
    ShapeTopic* topic = (ShapeTopic*) malloc(sizeof(ShapeTopic)); 
    uint32_t size_color = 0;
    deserialize_uint32_t(message, &size_color);
    topic->color = (char*) malloc(size_color);
    deserialize_array_char(message, topic->color, size_color);
    deserialize_uint32_t(message, &topic->x);
    deserialize_uint32_t(message, &topic->y);
    deserialize_uint32_t(message, &topic->shapesize);

    return topic;
}

void deallocate_shape_topic(ShapeTopic* topic)
{
    free(topic->color);
    free(topic);
}

void on_hello_topic(XRCEInfo info, MicroBuffer *message, void* args)
{
    (void) info;
    ClientTests* test = static_cast<ClientTests*>(args);
    
    HelloTopic* topic = deserialize_hello_message(message);
    printl_hello_topic(topic);
    
    test->topicCount++;

    deallocate_hello_topic(topic);
}

void on_shape_topic(XRCEInfo info, MicroBuffer* message, void* args)
{
    (void) info;
    ClientTests* test = static_cast<ClientTests*>(args);

    ShapeTopic* topic = deserialize_shape_message(message);
    printl_shape_topic(topic);

    test->topicCount++;

    deallocate_shape_topic(topic);
}

void on_status(XRCEInfo info, uint8_t operation, uint8_t status, void* args)
{
    ClientTests* test = static_cast<ClientTests*>(args);

    test->statusObjectId = info.object_id;
    test->statusRequestId = info.request_id;
    test->statusOperation = operation;
    test->statusImplementation = status;
}

void printl_shape_topic(const ShapeTopic* shape_topic)
{
    printf("        %s[%s | x: %u | y: %u | shapesize: %u]%s\n",
            "\x1B[1;34m",
            shape_topic->color,
            shape_topic->x,
            shape_topic->y,
            shape_topic->shapesize,
            "\x1B[0m");
}

void printl_hello_topic(const HelloTopic* hello_topic)
{
    printf("        %s[%s | index: %u]%s\n",
            "\x1B[1;34m",
            hello_topic->message,
            hello_topic->index,
            "\x1B[0m");
}

TEST_F(ClientTests, CreateDeleteClient)
{
    uint16_t client_id = createClient();
    checkStatus(STATUS_LAST_OP_CREATE);

    deleteXRCEObject(client_id);
    checkStatus(STATUS_LAST_OP_DELETE);
}

TEST_F(ClientTests, CreateDeleteParticipant)
{
    uint16_t client_id = createClient();
    uint16_t participant_id = createParticipant();
    checkStatus(STATUS_LAST_OP_CREATE);

    deleteXRCEObject(participant_id);
    checkStatus(STATUS_LAST_OP_DELETE);
    deleteXRCEObject(client_id);
}

TEST_F(ClientTests, CreateDeleteTopic)
{
    uint16_t client_id = createClient();
    uint16_t participant_id = createParticipant();
    uint16_t topic_id = createTopic("shape_topic.xml", participant_id);
    checkStatus(STATUS_LAST_OP_CREATE);

    deleteXRCEObject(topic_id);
    checkStatus(STATUS_LAST_OP_DELETE);
    deleteXRCEObject(participant_id);
    deleteXRCEObject(client_id);
}

TEST_F(ClientTests, CreateDeletePublisher)
{
    uint16_t client_id = createClient();
    uint16_t participant_id = createParticipant();
    uint16_t publisher_id = createPublisher(participant_id);
    checkStatus(STATUS_LAST_OP_CREATE);

    deleteXRCEObject(publisher_id);
    checkStatus(STATUS_LAST_OP_DELETE);
    deleteXRCEObject(participant_id);
    deleteXRCEObject(client_id);
}

TEST_F(ClientTests, CreateDeleteSubscriber)
{
    uint16_t client_id = createClient();
    uint16_t participant_id = createParticipant();
    uint16_t subscriber_id = createSubscriber(participant_id);
    checkStatus(STATUS_LAST_OP_CREATE);

    deleteXRCEObject(subscriber_id);
    checkStatus(STATUS_LAST_OP_DELETE);
    deleteXRCEObject(participant_id);
    deleteXRCEObject(client_id);
}

TEST_F(ClientTests, CreateDeleteDataWriter)
{
    uint16_t client_id = createClient();
    uint16_t participant_id = createParticipant();
    uint16_t publisher_id = createPublisher(participant_id);
    uint16_t data_writer_id = createDataWriter("data_writer_profile.xml", participant_id, publisher_id);
    checkStatus(STATUS_LAST_OP_CREATE);

    deleteXRCEObject(data_writer_id);
    checkStatus(STATUS_LAST_OP_DELETE);
    deleteXRCEObject(publisher_id);
    deleteXRCEObject(participant_id);
    deleteXRCEObject(client_id);
}

TEST_F(ClientTests, CreateDeleteDataReader)
{
    uint16_t client_id = createClient();
    uint16_t participant_id = createParticipant();
    uint16_t subscriber_id = createSubscriber(participant_id);
    uint16_t data_reader_id = createDataReader("data_reader_profile.xml", participant_id, subscriber_id);
    checkStatus(STATUS_LAST_OP_CREATE);

    deleteXRCEObject(data_reader_id);
    checkStatus(STATUS_LAST_OP_DELETE);
    deleteXRCEObject(subscriber_id);
    deleteXRCEObject(participant_id);
    deleteXRCEObject(client_id);
}

TEST_F(ClientTests, WriteData)
{
    uint16_t client_id = createClient();
    uint16_t participant_id = createParticipant();
    uint16_t publisher_id = createPublisher(participant_id);
    uint16_t data_writer_id = createDataWriter("data_writer_profile.xml", participant_id, publisher_id);
    writeShapeData(data_writer_id);
    checkStatus(STATUS_LAST_OP_WRITE);

    deleteXRCEObject(data_writer_id);
    deleteXRCEObject(publisher_id);
    deleteXRCEObject(participant_id);
    deleteXRCEObject(client_id);
}

TEST_F(ClientTests, WriteHelloData)
{
    uint16_t client_id = createClient();
    uint16_t participant_id = createParticipant();
    uint16_t publisher_id = createPublisher(participant_id);
    uint16_t data_writer_id = createDataWriter("hello_data_writer_profile.xml", participant_id, publisher_id);
    writeHelloData(data_writer_id);
    checkStatus(STATUS_LAST_OP_WRITE);

    deleteXRCEObject(data_writer_id);
    deleteXRCEObject(publisher_id);
    deleteXRCEObject(participant_id);
    deleteXRCEObject(client_id);
}

TEST_F(ClientTests, ReadData)
{
    uint16_t client_id = createClient();
    uint16_t participant_id = createParticipant();
    uint16_t subscriber_id = createSubscriber(participant_id);
    uint16_t data_reader_id = createDataReader("data_reader_profile.xml", participant_id, subscriber_id);
    readShapeData(data_reader_id);
    checkStatus(STATUS_LAST_OP_READ);
    waitMessage();
    checkDataTopic(1);
    deleteXRCEObject(data_reader_id);
    deleteXRCEObject(subscriber_id);
    deleteXRCEObject(participant_id);
    deleteXRCEObject(client_id);
}

TEST_F(ClientTests, ReadHelloData)
{
    uint16_t client_id = createClient();
    uint16_t participant_id = createParticipant();
    uint16_t subscriber_id = createSubscriber(participant_id);
    uint16_t data_reader_id = createDataReader("hello_data_reader_profile.xml", participant_id, subscriber_id);
    readHelloData(data_reader_id);
    checkStatus(STATUS_LAST_OP_READ);
    waitMessage();
    checkDataTopic(1);
    deleteXRCEObject(data_reader_id);
    deleteXRCEObject(subscriber_id);
    deleteXRCEObject(participant_id);
    deleteXRCEObject(client_id);
}

TEST_F(ClientTests, ReadMultiData)
{
    uint16_t client_id = createClient();
    uint16_t participant_id = createParticipant();
    uint16_t subscriber_id = createSubscriber(participant_id);
    uint16_t data_reader_id = createDataReader("data_reader_profile.xml", participant_id, subscriber_id);
    readShapeData(data_reader_id);
    checkStatus(STATUS_LAST_OP_READ);
    {
        waitMessage();
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        checkDataTopic(1);
    }
    deleteXRCEObject(data_reader_id);
    deleteXRCEObject(subscriber_id);
    deleteXRCEObject(participant_id);
    deleteXRCEObject(client_id);
}
