#ifndef RTSP_INTERNAL_MY_SEMAPHORE_H
#define RTSP_INTERNAL_MY_SEMAPHORE_H

#include <semaphore.h>

namespace rtsp {
int SEM_WAIT(sem_t *sem);
int SEM_POST(sem_t *sem);
int SEM_DESTROY(sem_t *sem);
int SEM_INIT(sem_t *sem, int init, int max);
}

#endif
