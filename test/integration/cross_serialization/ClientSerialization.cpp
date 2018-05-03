#include "ClientSerialization.hpp"
#include <micrortps/client/xrce_protocol_spec.h>
#include <xrce_protocol_serialization.h>

#define BUFFER_LENGTH 1024

std::vector<uint8_t> ClientSerialization::create_client_payload()
{
    //change in a future by client_payload_sizeof function and remove resize
    std::vector<uint8_t> buffer(BUFFER_LENGTH, 0x00);

    MicroBuffer micro_buffer;
    init_micro_buffer(&micro_buffer, &buffer.front(), buffer.capacity());

    CREATE_CLIENT_Payload payload;
    payload.base.request_id = RequestId{0x01, 0x23};
    payload.base.object_id = ObjectId{0x45, 0x67};
    payload.client_representation.xrce_cookie = XrceCookie{0x89, 0xAB, 0xCD, 0xEF};
    payload.client_representation.xrce_version = XrceVersion{0x01, 0x23};
    payload.client_representation.xrce_vendor_id = XrceVendorId{0x45, 0x67};
    payload.client_representation.client_timestamp.seconds = 0x89ABCDEF;
    payload.client_representation.client_timestamp.nanoseconds = 0x01234567;
    payload.client_representation.client_key = ClientKey{0x89, 0xAB, 0xCD, 0xEF};
    payload.client_representation.session_id = 0x01;
    payload.client_representation.optional_properties = 0x00;
    serialize_CREATE_CLIENT_Payload(&micro_buffer, &payload);

    buffer.resize(micro_buffer.iterator - micro_buffer.init);

    return buffer;
}

std::vector<uint8_t> ClientSerialization::create_payload()
{
    std::vector<uint8_t> buffer(BUFFER_LENGTH, 0x00);

    MicroBuffer micro_buffer;
    init_micro_buffer(&micro_buffer, &buffer.front(), buffer.capacity());

    CREATE_Payload payload;
    payload.base.request_id = RequestId{0x01, 0x23};
    payload.base.object_id = ObjectId{0x45, 0x67};
    payload.object_representation.kind = OBJK_PARTICIPANT;
    payload.object_representation._.participant.base.representation.format = REPRESENTATION_BY_REFERENCE;
    payload.object_representation._.participant.base.representation._.object_reference.data = (char*)"ABCDE";
    payload.object_representation._.participant.base.representation._.object_reference.size = 6;
    payload.object_representation._.participant.domain_id = (uint16_t)0x89AB;

    serialize_CREATE_Payload(&micro_buffer, &payload);

    buffer.resize(micro_buffer.iterator - micro_buffer.init);

    return buffer;
}

std::vector<uint8_t> ClientSerialization::get_info_payload()
{
    std::vector<uint8_t> buffer(BUFFER_LENGTH, 0x00);

    MicroBuffer micro_buffer;
    init_micro_buffer(&micro_buffer, &buffer.front(), buffer.capacity());

    GET_INFO_Payload payload;
    payload.base.request_id = RequestId{0x01, 0x23};
    payload.base.object_id = ObjectId{0x45, 0x67};
    payload.info_mask = 0x89ABCDEF;
    serialize_GET_INFO_Payload(&micro_buffer, &payload);

    buffer.resize(micro_buffer.iterator - micro_buffer.init);

    return buffer;
}

std::vector<uint8_t> ClientSerialization::delete_payload()
{
    std::vector<uint8_t> buffer(BUFFER_LENGTH, 0x00);

    MicroBuffer micro_buffer;
    init_micro_buffer(&micro_buffer, &buffer.front(), buffer.capacity());

    DELETE_Payload payload;
    payload.base.request_id = RequestId{0x01, 0x23};
    payload.base.object_id = ObjectId{0x45, 0x67};
    serialize_DELETE_Payload(&micro_buffer, &payload);

    buffer.resize(micro_buffer.iterator - micro_buffer.init);

    return buffer;
}

std::vector<uint8_t> ClientSerialization::status_agent_payload()
{
    //TODO
    return std::vector<uint8_t>();
}

std::vector<uint8_t> ClientSerialization::status_payload()
{
    std::vector<uint8_t> buffer(BUFFER_LENGTH, 0x00);

    MicroBuffer micro_buffer;
    init_micro_buffer(&micro_buffer, &buffer.front(), buffer.capacity());

    STATUS_Payload payload;
    payload.base.related_request.request_id = RequestId{0x01, 0x23};
    payload.base.related_request.object_id = ObjectId{0x45, 0x67};
    payload.base.result.implementation_status = 0x89;
    payload.base.result.status = 0xAB;
    serialize_STATUS_Payload(&micro_buffer, &payload);

    buffer.resize(micro_buffer.iterator - micro_buffer.init);

    return buffer;
}

std::vector<uint8_t> ClientSerialization::info_payload()
{
    //TODO
    return std::vector<uint8_t>();
}

std::vector<uint8_t> ClientSerialization::read_data_payload()
{
    std::vector<uint8_t> buffer(BUFFER_LENGTH, 0x00);

    MicroBuffer micro_buffer;
    init_micro_buffer(&micro_buffer, &buffer.front(), buffer.capacity());

    READ_DATA_Payload payload;
    payload.base.request_id = RequestId{0x01, 0x23};
    payload.base.object_id = ObjectId{0x45, 0x67};
    payload.read_specification.optional_content_filter_expression = 0x00;
    payload.read_specification.content_filter_expression.data = (char*)"ABCDE";
    payload.read_specification.content_filter_expression.size = 6;
    payload.read_specification.data_format = 0x89;
    payload.read_specification.optional_delivery_control = 0x00;
    payload.read_specification.delivery_control.max_bytes_per_seconds = 0xABCDEF01;
    payload.read_specification.delivery_control.max_elapsed_time = 0x23456789;
    payload.read_specification.delivery_control.max_samples = 0xABCD;
    payload.read_specification.delivery_control.min_pace_period = 0xEF012345;
    serialize_READ_DATA_Payload(&micro_buffer, &payload);

    buffer.resize(micro_buffer.iterator - micro_buffer.init);

    return buffer;
}

std::vector<uint8_t> ClientSerialization::write_data_payload_data()
{
    std::vector<uint8_t> buffer(BUFFER_LENGTH, 0x00);

    MicroBuffer micro_buffer;
    init_micro_buffer(&micro_buffer, &buffer.front(), buffer.capacity());

    WRITE_DATA_Payload_Data payload;
    payload.base.request_id = RequestId{0x01, 0x23};
    payload.base.object_id = ObjectId{0x45, 0x67};
    payload.data.data = (uint8_t*) "BYTES";
    payload.data.size = 5;
    serialize_WRITE_DATA_Payload_Data(&micro_buffer, &payload);

    buffer.resize(micro_buffer.iterator - micro_buffer.init);

    return buffer;
}

std::vector<uint8_t> ClientSerialization::write_data_payload_sample()
{
    //TODO
    return std::vector<uint8_t>();
}

std::vector<uint8_t> ClientSerialization::write_data_payload_data_seq()
{
    //TODO
    return std::vector<uint8_t>();
}

std::vector<uint8_t> ClientSerialization::write_data_payload_sample_seq()
{
    //TODO
    return std::vector<uint8_t>();
}

std::vector<uint8_t> ClientSerialization::write_data_payload_packed_samples()
{
    //TODO
    return std::vector<uint8_t>();
}

std::vector<uint8_t> ClientSerialization::data_payload_data()
{
    std::vector<uint8_t> buffer(BUFFER_LENGTH, 0x00);

    MicroBuffer micro_buffer;
    init_micro_buffer(&micro_buffer, &buffer.front(), buffer.capacity());

    DATA_Payload_Data payload;
    payload.base.request_id = RequestId{0x01, 0x23};
    payload.base.object_id = ObjectId{0x45, 0x67};
    payload.data.data = (uint8_t*) "BYTES";
    payload.data.size = 5;
    serialize_DATA_Payload_Data(&micro_buffer, &payload);

    buffer.resize(micro_buffer.iterator - micro_buffer.init);

    return buffer;
}

std::vector<uint8_t> ClientSerialization::data_payload_sample()
{
    //TODO
    return std::vector<uint8_t>();
}

std::vector<uint8_t> ClientSerialization::data_payload_data_seq()
{
    //TODO
    return std::vector<uint8_t>();
}

std::vector<uint8_t> ClientSerialization::data_payload_sample_seq()
{
    //TODO
    return std::vector<uint8_t>();
}

std::vector<uint8_t> ClientSerialization::data_payload_packed_samples()
{
    //TODO
    return std::vector<uint8_t>();
}

std::vector<uint8_t> ClientSerialization::acknack_payload()
{
    std::vector<uint8_t> buffer(BUFFER_LENGTH, 0x00);

    MicroBuffer micro_buffer;
    init_micro_buffer(&micro_buffer, &buffer.front(), buffer.capacity());

    ACKNACK_Payload payload;
    payload.first_unacked_seq_num = (uint16_t)0x0123;
    payload.nack_bitmap[0] = 0x45;
    payload.nack_bitmap[1] = 0x67;
    serialize_ACKNACK_Payload(&micro_buffer, &payload);

    buffer.resize(micro_buffer.iterator - micro_buffer.init);

    return buffer;
}

std::vector<uint8_t> ClientSerialization::heartbeat_payload()
{
    std::vector<uint8_t> buffer(BUFFER_LENGTH, 0x00);

    MicroBuffer micro_buffer;
    init_micro_buffer(&micro_buffer, &buffer.front(), buffer.capacity());

    HEARTBEAT_Payload payload;
    payload.first_unacked_seq_nr = (uint16_t)0x0123;
    payload.last_unacked_seq_nr = (uint16_t)0x4567;
    serialize_HEARTBEAT_Payload(&micro_buffer, &payload);

    buffer.resize(micro_buffer.iterator - micro_buffer.init);

    return buffer;
}

