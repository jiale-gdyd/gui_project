#ifndef LIBDRM_DISPLAY_MESSAGE_H
#define LIBDRM_DISPLAY_MESSAGE_H

#include <stdint.h>
#include <string.h>
#include <sys/time.h>

#include <thread>
#include <vector>

#include "lock.h"
#include "message_type.h"

namespace libdrm {
class Flow;
class EventMessage;

class EventParam {
public:
    EventParam() = delete;
    EventParam(int id, int param = 0) : id_(id), param_(param), params_(nullptr) {}

    ~EventParam() {
        if (params_) {
            free(params_);
            params_ = nullptr;
        }
    }

    int SetParams(void *params, int size) {
        if (params_) {
            free(params_);
        }

        params_ = params;
        params_size_ = size;
        return 0;
    }

    int GetId() {
        return id_;
    }

    int GetParam() {
        return param_;
    }

    void *GetParams() {
        return params_;
    }

    int GetParamsSize() {
        return params_size_;
    }

private:
    int  id_;
    int  param_;
    void *params_;
    int  params_size_;
};

typedef std::shared_ptr<EventParam> EventParamPtr;

class EventMessage {
public:
    EventMessage();
    EventMessage(void *sender, EventParamPtr param, int type = 0) : sender_(sender), param_(param), type_(type) {}
    ~EventMessage() {}

    void *GetSender() {
        return sender_;
    }

    EventParamPtr GetEventParam() {
        return param_;
    }

    int GetType() {
        return type_;
    }

private:
    void          *sender_;
    EventParamPtr param_;
    int           type_;
};

typedef int (*EventHook)(std::shared_ptr<Flow> flow, bool &loop);
typedef std::shared_ptr<EventMessage> MessagePtr;
typedef std::vector<MessagePtr> MessagePtrQueue;

class EventHandler {
public:
    EventHandler() {}
    virtual ~EventHandler() {}

    void RegisterEventHook(std::shared_ptr<Flow> flow, EventHook proc);
    void UnRegisterEventHook();
    void EventHookWait();
    void SignalEventHook();

    void CleanRepeatMessage(MessagePtr msg);
    void InsertMessage(MessagePtr msg, bool front = false);
    MessagePtr GetEventMessage();
    void NotifyToEventHandler(MessagePtr msg);

private:
    EventHook                    process_;
    bool                         event_thread_loop_;
    std::unique_ptr<std::thread> event_thread_;
    MessagePtrQueue              event_msgs_;
    ConditionLockMutex           event_cond_mtx_;
    ReadWriteLockMutex           event_queue_mtx_;
};
}

#endif
