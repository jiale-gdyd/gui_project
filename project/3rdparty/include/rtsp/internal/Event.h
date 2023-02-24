#ifndef RTSP_INTERNAL_EVENT_H
#define RTSP_INTERNAL_EVENT_H

#include <pthread.h>

namespace rtsp {
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    bool            triggered;
} mrevent;

#define EVENT   mrevent

void EVENT_INIT(EVENT *event);
void EVENT_DESTROY(EVENT *event);
void EVENT_WAIT(EVENT *event);
void EVENT_SET(EVENT *event);
void EVENT_RESET(EVENT *event);
}

#endif
