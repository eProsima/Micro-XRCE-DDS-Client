#include <gtest/gtest.h>

extern "C"
{
#include "../src/c/core/serialization/xrce_protocol.c"
#include "../src/c/core/serialization/xrce_header.c"
#include "../src/c/core/serialization/xrce_subheader.c"
#include "../src/c/core/session/object_id.c"
#include "../src/c/core/session/submessage.c"
#include "../src/c/core/session/session_info.c"
}

#define BUFFER_SIZE 512

TEST(SessionInfoTest, CreateSessionSize)
{
    uint8_t buffer[BUFFER_SIZE];
    ucdrBuffer ub;
    ucdr_init_buffer(&ub, buffer, BUFFER_SIZE);

    uxrSessionInfo info;
    uxr_buffer_create_session(&info, &ub, 0, 512);
    EXPECT_EQ(info.last_request_id, UXR_REQUEST_LOGIN);
    EXPECT_EQ(SUBHEADER_SIZE + CREATE_CLIENT_PAYLOAD_SIZE, ucdr_buffer_length(&ub));
}

TEST(SessionInfoTest, DeleteSessionSize)
{
    uint8_t buffer[BUFFER_SIZE];
    ucdrBuffer ub;
    ucdr_init_buffer(&ub, buffer, BUFFER_SIZE);

    uxrSessionInfo info;

    uxr_buffer_delete_session(&info, &ub);
    EXPECT_EQ(info.last_request_id, UXR_REQUEST_LOGOUT);
    EXPECT_EQ(SUBHEADER_SIZE + DELETE_CLIENT_PAYLOAD_SIZE, ucdr_buffer_length(&ub));
}

TEST(SessionInfoTest, RequestIdGeneration)
{
    uxrSessionInfo info;
    info.last_request_id = RESERVED_REQUESTS_ID;
    uint16_t request_id = generate_request_id(&info);
    EXPECT_EQ(RESERVED_REQUESTS_ID + 1, request_id);
    EXPECT_EQ(RESERVED_REQUESTS_ID + 1, info.last_request_id);

    info.last_request_id = 20;
    request_id = generate_request_id(&info);
    EXPECT_EQ(21, request_id);
    EXPECT_EQ(21, info.last_request_id);

    info.last_request_id = UINT16_MAX;
    request_id = generate_request_id(&info);
    EXPECT_EQ(RESERVED_REQUESTS_ID + 1, request_id);
    EXPECT_EQ(RESERVED_REQUESTS_ID + 1, info.last_request_id);
}

TEST(SessionInfoTest, BaseObjectRequestConversion)
{
    uxrSessionInfo info;
    info.last_request_id = RESERVED_REQUESTS_ID;

    uxrObjectId object_id = uxr_object_id(0x0AAA, 0x0B);
    BaseObjectRequest base = {};
    uint16_t request_id = uxr_init_base_object_request(&info, object_id, &base);

    uxrObjectId read_object_id = {};
    uint16_t read_request_id {};
    uxr_parse_base_object_request(&base, &read_object_id, &read_request_id);

    EXPECT_EQ(request_id, read_request_id);
    EXPECT_EQ(info.last_request_id, read_request_id);
    EXPECT_EQ(object_id.id, read_object_id.id);
    EXPECT_EQ(object_id.type, read_object_id.type);
}

TEST(SessionInfoTest, SessionHeaderOffset)
{
    uxrSessionInfo info;
    info.id = 0x79;
    EXPECT_EQ(uxr_session_header_offset(&info), MAX_HEADER_SIZE);

    info.id = 0x80;
    EXPECT_EQ(uxr_session_header_offset(&info), MIN_HEADER_SIZE);
}

TEST(SessionInfoTest, WriteReadSessionHeaderWithKey)
{
    uint8_t buffer[BUFFER_SIZE];
    ucdrBuffer ub;
    ucdr_init_buffer(&ub, buffer, BUFFER_SIZE);

    uxrSessionInfo info;
    uxr_init_session_info(&info, 0x00, 0xAABBCCDD);
    uint8_t stream_id = 1;
    uxrSeqNum seq_num = 2;
    uxr_stamp_session_header(&info, stream_id, seq_num, buffer);

    uint8_t read_stream_id = 0;
    uxrSeqNum read_seq_num = 0;
    ASSERT_TRUE(uxr_read_session_header(&info, &ub, &read_stream_id, &read_seq_num));
    EXPECT_EQ(stream_id, read_stream_id);
    EXPECT_EQ(seq_num, read_seq_num);
    EXPECT_EQ(MAX_HEADER_SIZE, ucdr_buffer_length(&ub));
}

TEST(SessionInfoTest, WriteReadSessionHeaderWithoutKey)
{
    uint8_t buffer[BUFFER_SIZE];
    ucdrBuffer ub;
    ucdr_init_buffer(&ub, buffer, BUFFER_SIZE);

    uxrSessionInfo info;
    uxr_init_session_info(&info, 0x80, 0xAABBCCDD);
    uint8_t stream_id = 1;
    uxrSeqNum seq_num = 2;
    uxr_stamp_session_header(&info, stream_id, seq_num, buffer);

    uint8_t read_stream_id = 0;
    uxrSeqNum read_seq_num = 0;
    ASSERT_TRUE(uxr_read_session_header(&info, &ub, &read_stream_id, &read_seq_num));
    EXPECT_EQ(stream_id, read_stream_id);
    EXPECT_EQ(seq_num, read_seq_num);
    EXPECT_EQ(MIN_HEADER_SIZE, ucdr_buffer_length(&ub));
}
