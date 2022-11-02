#pragma once

#include <atomic>
#include <thread>
#include <iostream>

namespace hope {
class Event;
class Thread;

namespace detail {
class ObjectData;
}

class Object {
public:
    Object();
    virtual ~Object();

    void move_to_thread(Thread *thread);
    void move_to_thread(std::thread::id thread);

    virtual void on_event(Event *event);

protected:
    Object(bool initialize);

    void terminate();
    void initialize();

    std::atomic<bool>                         m_initialized;
    std::atomic<bool>                         m_terminated;
    std::shared_ptr<hope::detail::ObjectData> m_data;
};
}
