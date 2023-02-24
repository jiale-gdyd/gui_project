#ifndef RTSP_INTERNAL_THREAD_H
#define RTSP_INTERNAL_THREAD_H

#include <pthread.h>

namespace rtsp {
int THREAD_JOIN(pthread_t *thread);
void THREAD_DESTROY(pthread_t *thread);
int THREAD_CREATE(pthread_t *thread, void *func(void *), void *param);
}

#endif
