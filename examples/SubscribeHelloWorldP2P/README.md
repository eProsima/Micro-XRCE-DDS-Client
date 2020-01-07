# SubscribeHelloWorldP2P example

This example will show how to subscribe to data creating a client subscriber.
It is necessary to have the following profiles enabled in `client.config` file to compile this example:

- `PROFILE_UDP_TRANSPORT`

## Usage
1. Run an agent in port *2018*.
2. Run the *PublisherHelloWorldP2P* example or some publisher that can send the *HelloWorld* topic.
3. Run the *SubscriberHelloWorldP2P* example.
   The example can be parameterized with the number of topics that will be listened to.
   If no number is given, the publisher will publish indefinitely.

## Topic

The *HelloWorld* topic has the following *IDL* representation:

```
struct HelloWorld
{
	unsigned long index;
	string message;
};
```

