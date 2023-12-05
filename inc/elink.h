#ifndef JOS_INC_LINK_H
#define JOS_INC_LINK_H
#include <inc/types.h>

// we use a queue and EmbedLink nodes to repersent blocking IPC senders
// remember to wake up these pending senders in env_free
// also, modify ipc_send & receive 

typedef struct EmbedLink {
    struct EmbedLink* prev, * next;
} EmbedLink;

static inline void
elink_init(EmbedLink* ln)
{
    ln->prev = ln;
    ln->next = ln;
}

static inline EmbedLink*
elink_remove(EmbedLink* ln)
{
    ln->prev->next = ln->next;
    ln->next->prev = ln->prev;
    elink_init(ln);
    return ln;
}

static inline void
elink_insert(EmbedLink* pos, EmbedLink* ln)
{
    ln->prev = pos;
    ln->next = pos->next;
    ln->next->prev = ln;
    ln->prev->next = ln;
}

static inline int
elink_empty(EmbedLink* ln)
{
    if (ln->prev == ln) {
        assert(ln->next == ln);
        return 1;
    }
    else {
        return 0;
    }
}


// wrapper functions, we call these functions when we manage the queue

static inline EmbedLink*
elink_queue_head(EmbedLink* que)
{
    return que->next;
}

static inline void
elink_enqueue(EmbedLink* que, EmbedLink* ln)
{
    elink_insert(que->prev, ln);
}

static inline EmbedLink*
elink_dequeue(EmbedLink* que)
{
    return elink_remove(elink_queue_head(que));
}

#define offset(_t, _m) ((uint32_t)(&((_t*)0)->_m))
#define master(_x, _t, _m) ((_t*)((void*)(_x) - offset(_t, _m)))

#endif