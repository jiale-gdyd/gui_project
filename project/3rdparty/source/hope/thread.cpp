#include <future>
#include <cassert>
#include <iostream>

#include <hope/thread.hpp>
#include <hope/object.hpp>
#include <hope/eventloop.hpp>

using namespace hope;

Thread::Thread() = default;

Thread::~Thread()
{
    quit();
    wait();
}

std::thread::id Thread::id() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_thread.get_id();
}

void Thread::start()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if ((m_state == State::Started) || (m_state == State::Starting)) {
        std::cerr << "Thread already started" << std::endl;
        return;
    }

    if (m_state == State::Stopping) {
        m_cond.wait(lock, [this] {
            return (m_state == State::Stopped);
        });
    }

    if (m_thread.joinable()) {
        m_thread.join();
    }

    assert(m_state == State::Stopped);

    m_state = State::Starting;
    m_cond.notify_all();

    m_thread = std::thread([this] {
        exec();
    });

    m_cond.wait(lock, [this] {
        return (m_state == State::Started);
    });

    assert(m_state == State::Started);
}

void Thread::quit()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if ((m_state == State::Stopping) || (m_state == State::Stopped)) {
        return;
    }

    if (m_state == State::Starting) {
        m_cond.wait(lock, [this] {
            return (m_state == State::Started);
        });
    }

    assert(m_state == State::Started);

    m_state = State::Stopping;
    m_cond.notify_all();
    m_event_loop->quit();
}

void Thread::wait()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_state != State::Stopped) {
        m_cond.wait(lock, [this] {
            return (m_state == State::Stopped);
        });
    }

    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void Thread::move_to_thread(std::unique_ptr<Object> obj)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if ((m_state == State::Stopping) || (m_state == State::Stopped)) {
        std::cerr << "Trying to move and object to a thread that has not been started" << std::endl;
    }

    if (m_state == State::Starting) {
        m_cond.wait(lock, [this] {
            return (m_state == State::Started);
        });
    }

    obj->move_to_thread(m_thread.get_id());
    assert(m_state == State::Started);
    m_children.push_back(std::move(obj));
}

void Thread::exec()
{
    {
        m_mutex.lock();
        assert(m_state == State::Starting);
        m_event_loop.reset(new EventLoop());
        m_state = State::Started;
        m_cond.notify_all();
        m_mutex.unlock();
    }

    m_event_loop->exec();

    {
        m_mutex.lock();
        assert(m_state == State::Stopping);
        m_children.clear();
        m_event_loop.reset();
        m_state = State::Stopped;
        m_cond.notify_all();
        m_mutex.unlock();
    }
}
