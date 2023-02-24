#include <rtsp/internal/Event.h>

namespace rtsp {
void EVENT_INIT(EVENT *event)
{
    pthread_mutex_init(&event->mutex, 0);
    pthread_cond_init(&event->cond, 0);
    event->triggered = false;
}

void EVENT_DESTROY(EVENT *event)
{
    pthread_mutex_destroy(&event->mutex);
    pthread_cond_destroy(&event->cond);
}

void EVENT_WAIT(EVENT *event)
{
    pthread_mutex_lock(&event->mutex);
    while (!event->triggered) {
        pthread_cond_wait(&event->cond, &event->mutex);
    }
    pthread_mutex_unlock(&event->mutex);
}

void EVENT_SET(EVENT *event)
{
    pthread_mutex_lock(&event->mutex);
    event->triggered = true;
    pthread_cond_signal(&event->cond);
    pthread_mutex_unlock(&event->mutex);
}

void EVENT_RESET(EVENT *event)
{
    pthread_mutex_lock(&event->mutex);
    event->triggered = false;
    pthread_mutex_unlock(&event->mutex);
}
}
