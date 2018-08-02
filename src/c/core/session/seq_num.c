#include <micrortps/client/core/session/seq_num.h>

#define UINT16_SIZE (1 << 16)
#define UINT16_MIDSIZE (1 << 15)

//==================================================================
//                             PUBLIC
//==================================================================
mrSeqNum seq_num_add(mrSeqNum seq_num, mrSeqNum increment)
{
    return (mrSeqNum)((seq_num + increment) % UINT16_SIZE);
}

mrSeqNum seq_num_sub(mrSeqNum seq_num, mrSeqNum decrement)
{
    return (mrSeqNum)((decrement > seq_num)
        ? (seq_num + (UINT16_SIZE - decrement)) % UINT16_SIZE
        : seq_num - decrement);
}

int seq_num_cmp(mrSeqNum seq_num_1, mrSeqNum seq_num_2)
{
    int result;
    if(seq_num_1 == seq_num_2)
    {
        result = 0;
    }
    else if((seq_num_1 < seq_num_2 && (seq_num_2 - seq_num_1) < UINT16_MIDSIZE) ||
            (seq_num_1 > seq_num_2 && (seq_num_1 - seq_num_2) > UINT16_MIDSIZE))
    {
        result = -1;
    }
    else
    {
        result = 1;
    }
    return result;
}
