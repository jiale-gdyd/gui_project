#include <hope/timer.hpp>
#include <hope/event.hpp>
#include <hope/threaddata.hpp>

using namespace hope;

struct TimerEvent final : public Event {
public:
    TimerEvent(Timer *event);
    ~TimerEvent() override;

    Timer *m_timer = nullptr;
};

TimerEvent::TimerEvent(Timer *event) : m_timer(event)
{

}

TimerEvent::~TimerEvent() = default;

Timer::Timer(): m_duration(std::chrono::milliseconds(0))
{

}

Timer::~Timer() = default;

std::chrono::milliseconds Timer::duration() const
{
    return m_duration;
}

void Timer::set_duration(std::chrono::milliseconds duration)
{
    m_duration = duration;
}

Signal<> &Timer::triggered()
{
    return m_triggered;
}

void Timer::start()
{
    auto lock = m_data->lock();
    ThreadDataRegistry::instance().thread_data(m_data->m_thread_id)->push_event(std::unique_ptr<Event>(new TimerEvent(this)), m_duration);
}

void Timer::on_event(Event *event)
{
    if (auto timer_event = dynamic_cast<TimerEvent *>(event)) {
        if (timer_event->m_timer == this) {
            m_triggered.emit();
        }
    } else {
        Object::on_event(event);
    }
}
