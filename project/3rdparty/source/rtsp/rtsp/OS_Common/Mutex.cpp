#include <rtsp/internal/Mutex.h>

namespace rtsp {
int MUTEX_INIT(pthread_mutex_t *mutex)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    return pthread_mutex_init(mutex, &attr);
}

int MUTEX_DESTROY(pthread_mutex_t *mutex)
{
    return pthread_mutex_destroy(mutex);
}

int MUTEX_LOCK(pthread_mutex_t *mutex)
{
    return pthread_mutex_lock(mutex);
}

int MUTEX_UNLOCK(pthread_mutex_t *mutex)
{
    return pthread_mutex_unlock(mutex);
}
}
