#include <stdio.h>
#include <rtsp/internal/TaskScheduler.h>
#include <rtsp/internal/RTSPCommonEnv.h>

namespace rtsp {
void *DoEventThread(void *lpParam)
{
    TaskScheduler *scheduler = (TaskScheduler *)lpParam;
    scheduler->doEventLoop();
    return NULL;
}

TaskScheduler::TaskScheduler()
{
    fTaskLoop = 0;
    MUTEX_INIT(&fMutex);
    FD_ZERO(&fReadSet);
    fMaxNumSockets = 0;
    fThread = NULL;

    fReadHandlers = new HandlerSet();
}

TaskScheduler::~TaskScheduler()
{
    stopEventLoop();

    delete fReadHandlers;

    THREAD_DESTROY(&fThread);
    MUTEX_DESTROY(&fMutex);
}

void TaskScheduler::taskLock()
{
    MUTEX_LOCK(&fMutex);
}

void TaskScheduler::taskUnlock()
{
    MUTEX_UNLOCK(&fMutex);
}

void TaskScheduler::turnOnBackgroundReadHandling(int socketNum, BackgroundHandlerProc *handlerProc, void *clientData) 
{
    taskLock();

    if (socketNum < 0) {
        goto func_exit;
    }

    FD_SET((unsigned)socketNum, &fReadSet);
    fReadHandlers->assignHandler(socketNum, handlerProc, clientData);

    if ((socketNum + 1) > fMaxNumSockets) {
        fMaxNumSockets = socketNum + 1;
    }

func_exit:
    taskUnlock();
}

void TaskScheduler::turnOffBackgroundReadHandling(int socketNum) 
{
    taskLock();

    if (socketNum < 0) {
        goto func_exit;
    }

    FD_CLR((unsigned)socketNum, &fReadSet);
    fReadHandlers->removeHandler(socketNum);

    if ((socketNum + 1) == fMaxNumSockets) {
        --fMaxNumSockets;
    }

func_exit:
    taskUnlock();
}

int TaskScheduler::startEventLoop()
{
    if (fTaskLoop != 0) {
        return -1;
    }

    fTaskLoop = 1;
    THREAD_CREATE(&fThread, DoEventThread, this);
    if (!fThread) {
        DPRINTF("failed to create event loop thread\n");
        fTaskLoop = 0;
        return -1;
    }

    return 0;
}

void TaskScheduler::stopEventLoop()
{
    fTaskLoop = 0;

    THREAD_JOIN(&fThread);
    THREAD_DESTROY(&fThread);
}

void TaskScheduler::doEventLoop() 
{
    while (fTaskLoop) {
        SingleStep();
    }
}

void TaskScheduler::SingleStep()
{
    taskLock();

    fd_set readSet = fReadSet;

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    int selectResult = select(fMaxNumSockets, &readSet, NULL, NULL, &timeout);
    if (selectResult < 0) {
        int err = WSAGetLastError();
        if (err != 0) {
            // exit(0);
        }
    }

    HandlerDescriptor *handler;
    HandlerIterator iter(*fReadHandlers);

    if (fLastHandledSocketNum >= 0) {
        while ((handler = iter.next()) != NULL) {
            if (handler->socketNum == fLastHandledSocketNum) {
                break;
            }
        }

        if (handler == NULL) {
            fLastHandledSocketNum = -1;
            iter.reset();
        }
    }

    while ((handler = iter.next()) != NULL) {
        if (FD_ISSET(handler->socketNum, &readSet) && FD_ISSET(handler->socketNum, &fReadSet) && (handler->handlerProc != NULL)) {
                fLastHandledSocketNum = handler->socketNum;
                (*handler->handlerProc)(handler->clientData, SOCKET_READABLE);
                break;
        }
    }

    if ((handler == NULL) && (fLastHandledSocketNum >= 0)) {
        iter.reset();

        while ((handler = iter.next()) != NULL) {
            if (FD_ISSET(handler->socketNum, &readSet) && FD_ISSET(handler->socketNum, &fReadSet) && (handler->handlerProc != NULL)) {
                    fLastHandledSocketNum = handler->socketNum;
                    (*handler->handlerProc)(handler->clientData, SOCKET_READABLE);
                    break;
            }
        }

        if (handler == NULL) {
            fLastHandledSocketNum = -1;
        }
    }

    taskUnlock();
    if (fLastHandledSocketNum == -1) {
        usleep(1);
    }
}


HandlerDescriptor::HandlerDescriptor(HandlerDescriptor* nextHandler) : handlerProc(NULL)
{
    if (nextHandler == this) {
        fNextHandler = fPrevHandler = this;
    } else {
        fNextHandler = nextHandler;
        fPrevHandler = nextHandler->fPrevHandler;
        nextHandler->fPrevHandler = this;
        fPrevHandler->fNextHandler = this;
    }
}

HandlerDescriptor::~HandlerDescriptor()
{
    fNextHandler->fPrevHandler = fPrevHandler;
    fPrevHandler->fNextHandler = fNextHandler;
}

HandlerSet::HandlerSet() : fHandlers(&fHandlers)
{
    fHandlers.socketNum = -1;
}

HandlerSet::~HandlerSet()
{
    while (fHandlers.fNextHandler != &fHandlers) {
        delete fHandlers.fNextHandler;
    }
}

void HandlerSet::assignHandler(int socketNum, TaskScheduler::BackgroundHandlerProc *handlerProc, void *clientData)
{
    HandlerDescriptor *handler = lookupHandler(socketNum);
    if (handler == NULL) {
        handler = new HandlerDescriptor(fHandlers.fNextHandler);
        handler->socketNum = socketNum;
    }

    handler->handlerProc = handlerProc;
    handler->clientData = clientData;
}

void HandlerSet::removeHandler(int socketNum)
{
    HandlerDescriptor *handler = lookupHandler(socketNum);
    delete handler;
}

void HandlerSet::moveHandler(int oldSocketNum, int newSocketNum)
{
    HandlerDescriptor *handler = lookupHandler(oldSocketNum);
    if (handler != NULL) {
        handler->socketNum = newSocketNum;
    }
}

HandlerDescriptor* HandlerSet::lookupHandler(int socketNum)
{
    HandlerDescriptor* handler;
    HandlerIterator iter(*this);

    while ((handler = iter.next()) != NULL) {
        if (handler->socketNum == socketNum) {
            break;
        }
    }

    return handler;
}

HandlerIterator::HandlerIterator(HandlerSet &handlerSet) : fOurSet(handlerSet)
{
    reset();
}

HandlerIterator::~HandlerIterator()
{

}

void HandlerIterator::reset()
{
    fNextPtr = fOurSet.fHandlers.fNextHandler;
}

HandlerDescriptor *HandlerIterator::next()
{
    HandlerDescriptor *result = fNextPtr;

    if (result == &fOurSet.fHandlers) {
        result = NULL;
    } else {
        fNextPtr = fNextPtr->fNextHandler;
    }

    return result;
}
}
