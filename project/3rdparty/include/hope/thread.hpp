#pragma once

#include <thread>
#include <vector>
#include <condition_variable>

namespace hope {
class Object;
class EventLoop;

class Thread {
public:
    Thread();
    ~Thread();

    std::thread::id id() const;

    void quit();
    void start();

    void wait();
    void move_to_thread(std::unique_ptr<Object> obj);

private:
    void exec();

    enum class State {
        Starting,
        Started,
        Stopping,
        Stopped
    };

    mutable std::mutex                   m_mutex;
    State                                m_state = State::Stopped;
    std::condition_variable              m_cond;
    std::thread                          m_thread;
    std::unique_ptr<EventLoop>           m_event_loop = nullptr;
    std::vector<std::unique_ptr<Object>> m_children;
};
}
