#ifndef TK_INPUT_DISPATCHER_H
#define TK_INPUT_DISPATCHER_H

#include "base/event_queue.h"

BEGIN_C_DECLS

typedef ret_t (*input_dispatch_t)(void *ctx, const event_queue_req_t *e, const char *msg);

ret_t input_dispatch_print(void *ctx, const event_queue_req_t *e, const char *msg);

END_C_DECLS

#endif
