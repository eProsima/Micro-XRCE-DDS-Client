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

