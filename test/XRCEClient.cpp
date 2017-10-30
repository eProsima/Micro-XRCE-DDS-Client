#include <micrortps/client/client.h>
#include <micrortps/client/xrce_protocol_spec.h>

#include <gtest/gtest.h>
#include <cstdint>
#include <unistd.h>

#include <fstream>

#define BUFFER_SIZE 1024
#define STATUS_WAIT_MICROSECONDS 3000

void on_status(XRCEInfo info, uint8_t operation, uint8_t status, void* args);
void* listen_agent(void* args);

class ClientLogicTest : public ::testing::Test
{
    public:
        ClientLogicTest()
        {
            stop_listening = false;
            state = new_udp_client_state(BUFFER_SIZE, 2020, 2019);

            statusObjectId = 0xFFFF;
            statusRequestId = 0x0000;
            statusOperation = 0xFF;
            statusReceived = false;
            last_object = 0xFFFF;
            last_request = 0x0000;

            if(pthread_create(&listening_thread, NULL, listen_agent, this))
            {
                printf("ERROR: Error creating thread\n");
            }
        }

        ~ClientLogicTest()
        {
            stop_listening = true;
            pthread_join(listening_thread, NULL);
            free_client_state(state);
        }

        void waitAndcheckStatus(uint8_t operation)
        {
            int statusWaitCounter = 0;
            while(!statusReceived && statusWaitCounter < STATUS_WAIT_MICROSECONDS)
            {
                usleep(1000);
                statusWaitCounter++;
            }

            ASSERT_LT(statusWaitCounter, STATUS_WAIT_MICROSECONDS);
            ASSERT_EQ(statusObjectId, last_object);
            ASSERT_EQ(statusRequestId, last_request);
            ASSERT_EQ(statusOperation, operation);
            ASSERT_EQ(statusImplementation, STATUS_OK);

            statusReceived = false;
        }

        uint16_t createClient()
        {
            XRCEInfo info = create_client(state, on_status, this);
            last_object = info.object_id;
            last_request = info.request_id;
            send_to_agent(state);

            return info.object_id;
        }

        uint16_t createParticipant()
        {
            XRCEInfo info = create_participant(state);
            last_object = info.object_id;
            last_request = info.request_id;
            send_to_agent(state);

            return info.object_id;
        }

        uint16_t createPublisher(uint16_t participant_id)
        {
            XRCEInfo info = create_publisher(state, participant_id);
            last_object = info.object_id;
            last_request = info.request_id;
            send_to_agent(state);

            return info.object_id;
        }

        uint16_t createSubscriber(uint16_t participant_id)
        {
            XRCEInfo info = create_subscriber(state, participant_id);
            last_object = info.object_id;
            last_request = info.request_id;
            send_to_agent(state);

            return info.object_id;
        }

        uint16_t createDataWriter(uint16_t participant_id, uint16_t publisher_id)
        {
            String xml;
            std::ifstream in("data_writer_profile");
            in.seekg (0, in.end);
            xml.length = in.tellg();
            in.seekg (0, in.beg);
            xml.data = new char[xml.length];
            in.read(xml.data, xml.length);

            XRCEInfo info = create_data_writer(state, participant_id, publisher_id, xml);
            last_object = info.object_id;
            last_request = info.request_id;
            send_to_agent(state);

            delete xml.data;
            return info.object_id;
        }

        uint16_t createDataReader(uint16_t participant_id, uint16_t subscriber_id)
        {
            String xml;
            std::ifstream in("data_reader_profile");
            in.seekg (0, in.end);
            xml.length = in.tellg();
            in.seekg (0, in.beg);
            xml.data = new char[xml.length];
            in.read(xml.data, xml.length);

            XRCEInfo info = create_data_reader(state, participant_id, subscriber_id, xml);
            last_object = info.object_id;
            last_request = info.request_id;
            send_to_agent(state);

            delete xml.data;
            return info.object_id;
        }

        void deleteXRCEObject(uint16_t id)
        {
            XRCEInfo info = delete_resource(state, id);
            last_object = info.object_id;
            last_request = info.request_id;
            send_to_agent(state);
        }

        ClientState* state;

        bool stop_listening;
        pthread_t listening_thread;

        uint16_t statusObjectId;
        uint16_t statusRequestId;
        uint8_t statusOperation;
        uint8_t statusImplementation;

        uint16_t last_request;
        uint16_t last_object;
        bool statusReceived;
};

void* listen_agent(void* args)
{
    ClientLogicTest* test = static_cast<ClientLogicTest*>(args);

    usleep(100000);
    while(!test->stop_listening)
    {
        receive_from_agent(test->state);
    }

    return NULL;
}

void on_status(XRCEInfo info, uint8_t operation, uint8_t status, void* args)
{
    ClientLogicTest* test = static_cast<ClientLogicTest*>(args);

    test->statusObjectId = info.object_id;
    test->statusRequestId = info.request_id;
    test->statusOperation = operation;
    test->statusImplementation = status;
    test->statusReceived = true;
}

TEST_F(ClientLogicTest, CreateDeleteClient)
{
    uint16_t client_id = createClient();

    waitAndcheckStatus(STATUS_LAST_OP_CREATE);

    deleteXRCEObject(client_id);

    waitAndcheckStatus(STATUS_LAST_OP_DELETE);
}


TEST_F(ClientLogicTest, CreateDeleteParticipant)
{
    uint16_t client_id = createClient();
    uint16_t participant_id = createParticipant();

    waitAndcheckStatus(STATUS_LAST_OP_CREATE);

    //deleteXRCEObject(participant_id);

    //waitAndcheckStatus(STATUS_LAST_OP_DELETE);
}

/*TEST_F(ClientLogicTest, CreateDeletePublisher)
{
    uint16_t client_id = createClient();
    uint16_t participant_id = createParticipant();
    uint16_t publisher_id = createPublisher(participant_id);

    waitAndcheckStatus(STATUS_LAST_OP_CREATE);

    deleteXRCEObject(publisher_id);

    waitAndcheckStatus(STATUS_LAST_OP_DELETE);
}

TEST_F(ClientLogicTest, CreateDeleteSubscriber)
{
    uint16_t client_id = createClient();
    uint16_t participant_id = createParticipant();
    uint16_t subscriber_id = createSubscriber(participant_id);

    waitAndcheckStatus(STATUS_LAST_OP_CREATE);

    deleteXRCEObject(subscriber_id);

    waitAndcheckStatus(STATUS_LAST_OP_DELETE);
}
*/