
#include <gtest/gtest.h>

extern "C"
{
#include "../src/c/core/session/stream/seq_num.c"
#include "../src/c/core/session/stream/stream_id.c"
#include "../src/c/core/session/session.c"
}

#define BUFFER_SIZE           128
#define HISTORY               4
#define OFFSET                8

class SessionTest : public testing::Test
{
public:
    SessionTest()
    {
    }

    virtual ~SessionTest()
    {
    }

protected:
    uxrSession session;
    uint8_t ob_buffer[BUFFER_SIZE / HISTORY];
    uint8_t or_buffer[BUFFER_SIZE];
    uint8_t ir_buffer[BUFFER_SIZE];

public:
};

TEST_F(SessionTest, Reset)
{

}

// ****************************************************************************Y
//                                  MOCKS
// ****************************************************************************Y

void uxr_init_stream_storage(uxrStreamStorage* storage)
{
    (void) storage;
}

void uxr_reset_stream_storage(uxrStreamStorage* storage)
{
    (void) storage;
}

uxrStreamId uxr_add_input_best_effort_buffer(uxrStreamStorage* storage)
{
    (void) storage;
    return uxr_stream_id(0, UXR_BEST_EFFORT_STREAM, UXR_INPUT_STREAM);
}

uxrStreamId uxr_add_output_best_effort_buffer(uxrStreamStorage* storage, uint8_t* buffer, size_t size, uint8_t header_offset)
{
    (void) storage; (void) buffer; (void) size; (void) header_offset;
    return uxr_stream_id(0, UXR_BEST_EFFORT_STREAM, UXR_OUTPUT_STREAM);
}

uxrStreamId uxr_add_input_reliable_buffer(uxrStreamStorage* storage, uint8_t* buffer, size_t size, uint16_t history, OnGetFragmentationInfo on_get_fragmentation_info)
{
    (void) storage; (void) buffer; (void) size; (void) history; (void) on_get_fragmentation_info;
    return uxr_stream_id(0, UXR_RELIABLE_STREAM, UXR_INPUT_STREAM);
}

uxrStreamId uxr_add_output_reliable_buffer(uxrStreamStorage* storage, uint8_t* buffer, size_t size, uint16_t history, uint8_t header_offset, OnNewFragment on_new_fragment)
{
    (void) storage; (void) buffer; (void) size; (void) history; (void) header_offset; (void) on_new_fragment;
    return uxr_stream_id(0, UXR_RELIABLE_STREAM, UXR_OUTPUT_STREAM);
}

uxrOutputBestEffortStream* uxr_get_output_best_effort_stream(uxrStreamStorage* storage, uint8_t index)
{
    (void) storage; (void) index;
    return NULL;
}

uxrOutputReliableStream* uxr_get_output_reliable_stream(uxrStreamStorage* storage, uint8_t index)
{
    (void) storage; (void) index;
    return NULL;
}

uxrInputBestEffortStream* uxr_get_input_best_effort_stream(uxrStreamStorage* storage, uint8_t index)
{
    (void) storage; (void) index;
    return NULL;
}

uxrInputReliableStream* uxr_get_input_reliable_stream(uxrStreamStorage* storage, uint8_t index)
{
    (void) storage; (void) index;
    return NULL;
}

bool uxr_output_streams_confirmed(const uxrStreamStorage* storage)
{
    (void) storage;
    return false;
}

void uxr_init_session_info(uxrSessionInfo* info, uint8_t id, uint32_t key)
{
    (void) info; (void) id; (void) key;
}

void uxr_buffer_create_session(uxrSessionInfo* info, ucdrBuffer* ub, int64_t nanoseconds, uint16_t mtu)
{
    (void) info; (void) ub; (void) nanoseconds; (void)mtu;
}

void uxr_buffer_delete_session(uxrSessionInfo* info, ucdrBuffer* ub)
{
    (void) info; (void) ub;
}

void uxr_read_create_session_status(uxrSessionInfo* info, ucdrBuffer* ub)
{
    (void) info; (void) ub;
}

void uxr_read_delete_session_status(uxrSessionInfo* info, ucdrBuffer* ub)
{
    (void) info; (void) ub;
}

void uxr_stamp_create_session_header(const uxrSessionInfo* info, uint8_t* buffer)
{
    (void) info; (void) buffer;
}

void uxr_stamp_session_header(const uxrSessionInfo* info, uint8_t stream_id_raw, uxrSeqNum seq_num, uint8_t* buffer)
{
    (void) info; (void) stream_id_raw; (void) seq_num; (void) buffer;
}

bool uxr_read_session_header(const uxrSessionInfo* info, ucdrBuffer* ub, uint8_t* stream_id_raw, uxrSeqNum* seq_num)
{
    (void) info; (void) ub; (void) stream_id_raw; (void) seq_num;
    return false;
}

uint8_t uxr_session_header_offset(const uxrSessionInfo* info)
{
    (void) info;
    return 0;
}

void uxr_parse_base_object_request(const BaseObjectRequest* base, uxrObjectId* object_id, uint16_t* request_id)
{
    (void) base; (void) object_id; (void) request_id;
}

int64_t uxr_millis(void)
{
    return 0;
}

bool uxr_prepare_best_effort_buffer_to_write(uxrOutputBestEffortStream* stream, size_t size, struct ucdrBuffer* ub)
{
    (void) stream; (void) size; (void) ub;
    return false;
}

bool uxr_prepare_best_effort_buffer_to_send(uxrOutputBestEffortStream* stream, uint8_t** buffer, size_t* length, uint16_t* seq_num)
{
    (void) stream; (void) buffer; (void) length; (void) seq_num;
    return false;
}

bool uxr_prepare_reliable_buffer_to_write(uxrOutputReliableStream* stream, size_t size, size_t fragment_offset, struct ucdrBuffer* ub)
{
    (void) stream; (void) size; (void) fragment_offset; (void) ub;
    return true;
}

bool uxr_prepare_next_reliable_buffer_to_send(uxrOutputReliableStream* stream, uint8_t** buffer, size_t* length, uxrSeqNum* seq_num)
{
    (void) stream; (void) buffer; (void) length; (void) seq_num;
    return false;
}

bool uxr_update_output_stream_heartbeat_timestamp(uxrOutputReliableStream* stream, int64_t current_timestamp)
{
    (void) stream; (void) current_timestamp;
    return false;
}

bool uxr_buffer_submessage_header(ucdrBuffer* ub, uint8_t submessage_id, uint16_t length, uint8_t flags)
{
    (void) ub; (void) submessage_id; (void) length; (void) flags;
    return false;
}

bool uxr_read_submessage_header(ucdrBuffer* ub, uint8_t* submessage_id, uint16_t* length, uint8_t* flags)
{
    (void) ub; (void) submessage_id; (void) length; (void) flags;
    return false;
}

size_t uxr_submessage_padding(size_t length)
{
    (void) length;
    return 0;
}
