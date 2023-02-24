#ifndef RTSP_INTERNAL_MUTEX_H
#define RTSP_INTERNAL_MUTEX_H

#include <pthread.h>

namespace rtsp {
int MUTEX_INIT(pthread_mutex_t *mutex);
int MUTEX_LOCK(pthread_mutex_t *mutex);
int MUTEX_UNLOCK(pthread_mutex_t *mutex);
int MUTEX_DESTROY(pthread_mutex_t *mutex);
}

#endif
