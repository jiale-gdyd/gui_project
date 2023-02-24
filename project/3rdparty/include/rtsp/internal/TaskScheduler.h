#ifndef RTSP_INTERNAL_TASK_SCHEDULER_H
#define RTSP_INTERNAL_TASK_SCHEDULER_H

#include "Mutex.h"
#include "Thread.h"
#include "NetCommon.h"

namespace rtsp {
#define SOCKET_READABLE     (1<<1)
#define SOCKET_WRITABLE     (1<<2)
#define SOCKET_EXCEPTION    (1<<3)

class HandlerSet;

class TaskScheduler {
public:
    TaskScheduler();
    virtual ~TaskScheduler();

    typedef void BackgroundHandlerProc(void *clientData, int mask);

    void turnOnBackgroundReadHandling(int socketNum, BackgroundHandlerProc *handlerProc, void *clientData);
    void turnOffBackgroundReadHandling(int socketNum);

    int startEventLoop();
    void stopEventLoop();
    void doEventLoop();

    int isRunning() {
        return fTaskLoop;
    }

protected:
    virtual void SingleStep();
    void taskLock();
    void taskUnlock();

protected:
    int             fTaskLoop;
    pthread_mutex_t fMutex;
    pthread_t       fThread;
    HandlerSet      *fReadHandlers;
    int             fLastHandledSocketNum;
    int             fMaxNumSockets;
    fd_set          fReadSet;
};

class HandlerDescriptor {
    HandlerDescriptor(HandlerDescriptor *nextHandler);
    virtual ~HandlerDescriptor();

public:
    int                                  socketNum;
    TaskScheduler::BackgroundHandlerProc *handlerProc;
    void                                 *clientData;

private:
    friend class HandlerSet;
    friend class HandlerIterator;

    HandlerDescriptor *fNextHandler;
    HandlerDescriptor *fPrevHandler;
};

class HandlerSet {
public:
    HandlerSet();
    virtual ~HandlerSet();

    void assignHandler(int socketNum, TaskScheduler::BackgroundHandlerProc *handlerProc, void *clientData);
    void removeHandler(int socketNum);
    void moveHandler(int oldSocketNum, int newSocketNum);

private:
    HandlerDescriptor *lookupHandler(int socketNum);

private:
    friend class HandlerIterator;
    HandlerDescriptor fHandlers;
};

class HandlerIterator {
public:
    HandlerIterator(HandlerSet &handlerSet);
    virtual ~HandlerIterator();

    HandlerDescriptor *next();
    void reset();

private:
    HandlerSet        &fOurSet;
    HandlerDescriptor *fNextPtr;
};
}

#endif
