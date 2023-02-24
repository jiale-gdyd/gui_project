#include <rtsp/internal/Semaphore.h>

namespace rtsp {
int SEM_INIT(sem_t *sem, int init, int max)
{
    return sem_init(sem, 0, init);
}

int SEM_DESTROY(sem_t *sem)
{
    return sem_destroy(sem);
}

int SEM_WAIT(sem_t *sem)
{
    return sem_wait(sem);
}

int SEM_POST(sem_t *sem)
{
    return sem_post(sem);
}
}
