#include <rtsp/internal/Thread.h>

namespace rtsp {
int THREAD_CREATE(pthread_t *thread, void *func(void *), void *param)
{
    return pthread_create(thread, NULL, func, param);
}

int THREAD_JOIN(pthread_t *thread)
{
    int status;
    return pthread_join(*thread, (void **)&status);
}

void THREAD_DESTROY(pthread_t *thread)
{
    *thread = NULL;
}
}
