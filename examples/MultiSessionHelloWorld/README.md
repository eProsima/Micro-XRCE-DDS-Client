# MultiSessionHelloWorld example

This example will show how to create multiple sessions on the same program. Both session
will have a publisher and a subscriber communicating each other.
In order to compile this example, it is necessary to have the following profiles should be enabled:

- `PROFILE_CREATE_ENTITIES_XML`
- `PROFILE_WRITE_ACCESS`
- `UCLIENT_PROFILE_UDP`

## Usage
1. Run an agent in port *2018*.
2. Run the *MultiSessionHelloWorld* example.


## Topic

The *HelloWorld* topic has the following *IDL* representation:

```
struct HelloWorld
{
	unsigned long index;
	string message;
};
```

