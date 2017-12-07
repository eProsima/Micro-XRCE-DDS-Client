# Micro RTPS Client

<a href="http://www.eprosima.com"><img src="https://encrypted-tbn3.gstatic.com/images?q=tbn:ANd9GcSd0PDlVz1U_7MgdTe0FRIWD0Jc9_YH-gGi0ZpLkr-qgCI6ZEoJZ5GBqQ" align="left" hspace="8" vspace="2" width="100" height="100" ></a>

In *Micro RTPS* a *Micro RTPS Client* can communicate with DDS Network as any other DDS actor could do. Clients can publish and subscribe to data topics in DDS Global Data Space. *Micro RTPS* provides you with a C API to create *Micro RTPS Clients*.

![Architecture](docs/architecture_client.png)

Within your *Micro RTPS Client* you can choose between two transport layers: UDP or SerialPort. Communication with the Agent is done through the transport you choose. The API functions to select transport are:

.. code-block:: c

    ClientState* new_serial_client_state(uint32_t buffer_size, const char* device);
    ClientState* new_udp_client_state(uint32_t buffer_size, const char* server_ip, uint16_t recv_port, uint16_t send_port);

Those two functions create a ClientState object that you should keep during a communication session with a *Micro RTPS Agent*. If you do not need that communication you can free it using:

.. code-block:: c

    void free_client_state(ClientState* state);


Your *Micro RTPS Client* can send request Operations to the *Micro RTPS Agent* using the API provided:

.. code-block:: c

    XRCEInfo create_client(ClientState* state, OnStatusReceived on_status, void* on_status_args);
    XRCEInfo create_participant(ClientState* state);
    XRCEInfo create_topic(ClientState* state, uint16_t participant_id, String xml);
    XRCEInfo create_publisher(ClientState* state, uint16_t participant_id);
    XRCEInfo create_subscriber(ClientState* state, uint16_t participant_id);
    XRCEInfo create_data_writer(ClientState* state, uint16_t participant_id, uint16_t publisher_id, String xml);
    XRCEInfo create_data_reader(ClientState* state, uint16_t participant_id, uint16_t subscriber_id, String xml);

    XRCEInfo delete_resource(ClientState* state, uint16_t resource_id);

    XRCEInfo write_data(ClientState* state, uint16_t data_writer_id, SerializeTopic serialization, void* topic);
    XRCEInfo read_data(ClientState* state, uint16_t data_reader_id, DeserializeTopic deserialization,
                       OnTopicReceived on_topic, void* on_topic_args);

All these functions return information about the entities and request identifiers inside a XRCEInfo struct.
This API functions just prepare requests for the *Micro RTPS Agent* but they don't send them. For pushing those request through the transport you need to call:

.. code-block:: c

    bool send_to_agent(ClientState* state);


In the other way of the communication (Agent -> Client) you need to give callbacks to get the responses from the Agent as well as receiving your data from DDS Global Data Space:

.. code-block:: c

    typedef void (*OnTopicReceived)(XRCEInfo info, const void* topic, void* args);
    typedef void (*OnStatusReceived)(XRCEInfo info, uint8_t operation, uint8_t status, void* args);

When You call to the receive function those callbacks are called if needed. At that moment your transport feeds your Client with new data coming from the Agent.

.. code-block:: c

    bool receive_from_agent(ClientState* state);

Installation from Sources
=========================

Clone the project from Github: ::

    $ git clone --recursive  https://github.com/eProsima/micro-RTPS-client.git
    $ mkdir micro-RTPS-client/build && cd micro-RTPS-client/build

On Linux, execute: ::

    $ cmake -DTHIRDPARTY=ON ..
    $ make
    $ sudo make install

If you want to install our *Micro RTPS Client* examples you can add "-DEPROSIMA_BUILD_EXAMPLES=ON" to the cmake command line options.

## Documentation

You can access Micro-RTPS documentation online, which is hosted on [Read the Docs](http://micro-rtps.readthedocs.io).

* [Start Page](http://micro-rtps.readthedocs.io)
* [Installation manual](http://micro-rtps.readthedocs.io/en/latest/dependencies.html)
* [User manual](http://micro-rtps.readthedocs.io/en/latest/introduction.html)

## Getting Help

If you need support you can reach us by mail at `support@eProsima.com` or by phone at `+34 91 804 34 48`.
