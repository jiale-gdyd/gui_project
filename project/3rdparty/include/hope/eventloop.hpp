#pragma once

#include <map>
#include <mutex>
#include <vector>
#include <memory>
#include <thread>
#include <algorithm>
#include <condition_variable>

#include "event.hpp"
#include "object.hpp"
#include "atomicwrapper.hpp"

namespace hope {
namespace detail {
class ObjectData;
}

namespace test {
class EventLoopTestHelper;
}

class EventLoop : public Object {
public:
    using Mutex = std::mutex;
    using Locker = std::unique_lock<Mutex>;
    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    EventLoop();
    ~EventLoop() override;

    bool is_running() const;

    void push_event(std::shared_ptr<Event> event, TimePoint when = Clock::now());
    void push_event(std::shared_ptr<Event> event, std::chrono::milliseconds duration, TimePoint offset = Clock::now());

    void quit(int exit_code = 0);
    int exec();

    void register_object(Object *object);
    void unregister_object(Object *object);

    void on_event(Event *event) final;

private:
    friend class EventLoopTestHelper;

    int loop();

    void process_events(const std::vector<std::shared_ptr<Event>> &events);
    void cleanup_objects();

    int                                                   m_exit_code = 0;
    bool                                                  m_exit = false;
    bool                                                  m_is_running = false;
    mutable Mutex                                         m_mutex;
    mutable Mutex                                         m_objects_mutex;
    std::condition_variable                               m_cond;
    std::multimap<TimePoint, std::shared_ptr<Event>>      m_events;
    std::map<Object *, hope::detail::AtomicWrapper<bool>> m_objects;
};
}
