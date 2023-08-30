#include "input_dispatcher.h"

ret_t input_dispatch_print(void *ctx, const event_queue_req_t *e, const char *msg)
{
    switch (e->event.type) {
        case EVT_POINTER_DOWN: {
            // log_debug("msg:[%s], pointer down, x:[%d], y:[%d]\n", msg, e->pointer_event.x, e->pointer_event.y);
            break;
        }

        case EVT_POINTER_UP: {
            // log_debug("msg:[%s], pointer up, x:[%d], y:[%d]\n", msg, e->pointer_event.x, e->pointer_event.y);
            break;
        }

        case EVT_POINTER_MOVE: {
            // log_debug("msg:[%s], pointer move, x:[%d], y:[%d]\n", msg, e->pointer_event.x, e->pointer_event.y);
            break;
        }

        case EVT_CONTEXT_MENU: {
            // log_debug("msg:[%s], context menu, x:[%d], y:[%d]\n", msg, e->pointer_event.x, e->pointer_event.y);
            break;
        }

        case EVT_KEY_DOWN: {
            // log_debug("msg:[%s], keydown, key:[%d]\n", msg, e->key_event.key);
            break;
        }

        case EVT_KEY_UP: {
            // log_debug("msg:[%s], keyup, key:[%d]\n", msg, e->key_event.key);
            break;
        }
    }

    return RET_OK;
}
