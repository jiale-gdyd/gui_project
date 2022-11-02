#include <hope/eventloop.hpp>
#include <hope/threaddata.hpp>
#include <hope/objectdata.hpp>

using namespace hope;
using namespace detail;

namespace {
class RegisterEvent final : public Event {
public:
    RegisterEvent(Object *object) : m_object(object) {

    }

    Object *m_object = nullptr;
};
}

EventLoop::EventLoop() : Object(false)
{
    {
        auto lock = m_data->lock();
        ThreadDataRegistry::instance().thread_data(m_data->m_thread_id)->set_event_loop(this);
    }

    initialize();
}

EventLoop::~EventLoop()
{
    terminate();
    {
        auto lock = m_data->lock();
        ThreadDataRegistry::instance().thread_data(m_data->m_thread_id)->set_event_loop(nullptr);
    }
}

bool EventLoop::is_running() const
{
    Locker locker(m_mutex);
    return m_is_running;
}

void EventLoop::push_event(std::shared_ptr<Event> event, EventLoop::TimePoint when)
{
    Locker lock(m_mutex);
    m_events.emplace(when, std::move(event));
    m_cond.notify_one();
}

void EventLoop::push_event(std::shared_ptr<Event> event, std::chrono::milliseconds duration, EventLoop::TimePoint offset)
{
    Locker lock(m_mutex);
    m_events.emplace(offset + duration, std::move(event));
    m_cond.notify_one();
}

void EventLoop::quit(int exit_code)
{
    Locker lock(m_mutex);
    m_exit = true;
    m_exit_code = exit_code;
    m_cond.notify_one();
}

int EventLoop::exec()
{
    return loop();
}

void EventLoop::register_object(Object *object)
{
    std::unique_ptr<Event> event(new RegisterEvent(object));
    push_event(std::move(event));
}

void EventLoop::unregister_object(Object *object)
{
    Locker lock(m_objects_mutex);
    auto it = m_objects.find(object);
    if (it != m_objects.end()) {
        it->second.value() = false;
    }
}

void EventLoop::on_event(Event *event)
{
    if (auto registerEvent = dynamic_cast<RegisterEvent *>(event)) {
        Locker lock(m_objects_mutex);
        m_objects.emplace(registerEvent->m_object, true);
    } else {
        Object::on_event(event);
    }
}

int EventLoop::loop()
{
    {
        Locker lock(m_mutex);
        m_is_running = true;
    }

    std::vector<std::shared_ptr<Event>> events;

    while (true) {
        while (true) {
            Locker lock(m_mutex);
            if (m_exit) {
                m_is_running = false;
                return m_exit_code;
            } else if (m_events.empty()) {
                m_cond.wait(lock);
            } else {
                const TimePoint now = Clock::now();
                while (!m_events.empty() && (m_events.begin()->first <= now)) {
                    events.emplace_back(std::move(m_events.begin()->second));
                    m_events.erase(m_events.begin());
                }

                if (!events.empty()) {
                    break;
                }

                m_cond.wait_until(lock, m_events.begin()->first);
            }
        }

        process_events(events);
        cleanup_objects();

        events.clear();
    }
}

void EventLoop::process_events(const std::vector<std::shared_ptr<Event> > &events)
{
    for (const std::shared_ptr<Event> &event : events) {
        on_event(event.get());

        for (const auto &object : m_objects) {
            if (object.second.value()) {
                object.first->on_event(event.get());
            }
        }
    }
}

void EventLoop::cleanup_objects()
{
    Locker lock(m_objects_mutex);
    for (auto it = m_objects.begin(); it != m_objects.end(); ) {
        if (!it->second.value()) {
            it = m_objects.erase(it);
        } else {
            ++it;
        }
    }
}
