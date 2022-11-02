#pragma once

#include <map>
#include <atomic>
#include <cassert>
#include <iostream>
#include <functional>

#include "optional.hpp"
#include "objectdata.hpp"
#include "threaddata.hpp"
#include "connection.hpp"
#include "queuedinvokationevent.hpp"

namespace hope {
enum class ConnectionType {
    Auto, Direct, Queued, QueuedBlocking
};

namespace detail {
template<typename ...Args>
struct BaseInvoker {
    BaseInvoker() : m_valid(true) {

    }

    virtual ~BaseInvoker() = default;

    virtual void invoke(Args... args) const = 0;

    virtual const void *receiver_pointer() const = 0;
    virtual const void *receiver_func_pointer() const = 0;

    std::atomic<bool> m_valid;
};

template<class Receiver, typename ...Args>
struct Invoker final : public BaseInvoker<Args...> {
    using ReceiverMemFunc = void(Receiver::*)(Args...);

    Invoker(Receiver *receiver, ReceiverMemFunc receiver_func, ConnectionType type) : m_receiver(receiver), m_receiver_func(receiver_func), m_type(type) {

    }

    void invoke(Args...args) const final {
        switch (m_type) {
            case ConnectionType::Auto:
                invoke_auto(std::move(args)...);
                break;

            case ConnectionType::Direct:
                invoke_direct(std::move(args)...);
                break;

            case ConnectionType::Queued:
                invoke_queued(std::move(args)...);
                break;

            case ConnectionType::QueuedBlocking:
                invoke_queued_blocking(std::move(args)...);
                break;
        }
    }

    void invoke_auto(Args...args) const {
        if (Optional<std::thread::id> thread_id = receiver_thread_id()) {
            if (std::this_thread::get_id() != thread_id) {
                invoke_queued(std::forward<Args>(args)...);
            } else {
                invoke_direct(std::forward<Args>(args)...);
            }
        }
    }

    void invoke_queued(Args...args) const {
        if (Optional<std::thread::id> thread_id = receiver_thread_id()) {
            auto event = make_queued_invokation_event(m_receiver, m_receiver_func, std::move(args)...);
            ThreadDataRegistry::instance().thread_data(thread_id)->push_event(std::move(event));
        }
    }

    void invoke_queued_blocking(Args...args) const {
        if (Optional<std::thread::id> thread_id = receiver_thread_id()) {
            auto event = make_queued_invokation_event(m_receiver, m_receiver_func, std::move(args)...);
            ThreadDataRegistry::instance().thread_data(thread_id)->push_event(event);
            event->wait();
        }
    }

    void invoke_direct(Args...args) const {
        if (is_receiver_alive()) {
            (m_receiver->*m_receiver_func)(std::move(args)...);
        }
    }

    const void *receiver_pointer() const final {
        return m_receiver;
    }

    const void *receiver_func_pointer() const final {
        return &m_receiver_func;
    }

private:
    Optional<std::thread::id> receiver_thread_id() const {
        Optional<std::thread::id> result;
        if (auto data = ObjectDataRegistry::instance().data(m_receiver).lock()) {
            auto lock = ObjectData::lock(data);
            result = data->m_thread_id;
        }

        return result;
    }

    bool is_receiver_alive() const {
        return ObjectDataRegistry::instance().data(m_receiver).lock() != nullptr;
    }

    Receiver *const       m_receiver = nullptr;
    const ReceiverMemFunc m_receiver_func = nullptr;
    const ConnectionType  m_type;
};

template<class Receiver, typename ...Args>
std::shared_ptr<BaseInvoker<Args...>> make_invoker(Receiver *receiver, void(Receiver::*func)(Args...), ConnectionType type)
{
    return std::make_shared<Invoker<Receiver, Args...>>(receiver, func, type);
}
}

template<class ... Args>
class Signal {
public:
    using SignalInvoker = std::shared_ptr<detail::BaseInvoker<Args...>>;

    Signal() = default;
    Signal(const Signal &other) = delete;
    Signal(Signal &&other) noexcept = default;

    Signal &operator=(const Signal &other) = delete;
    Signal &operator=(Signal &&other) noexcept = default;

    void emit(Args... args) {
        for (const auto &pair : objects()) {
            if (pair.second->m_valid) {
                pair.second->invoke(std::move(args)...);
            }
        }
    }

    template<typename Receiver, typename std::enable_if<std::is_base_of<Object, Receiver>::value, int>::type = 0>
    Connection connect(Receiver *receiver, void(Receiver::*func)(Args...args), ConnectionType type = ConnectionType::Auto) {
        std::lock_guard<std::mutex> lock(m_mutex);
        Connection result = get_next_connection_id();
        m_objects.emplace(result, detail::make_invoker(receiver, func, type));
        return result;
    }

    void disconnect(Connection c) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_objects.find(c);
        if (it != m_objects.end()) {
            it->second->m_valid = false;
            m_objects.erase(it);
        }
    }

    template<typename Receiver, typename ReceiverMemFunc = void(Receiver::*)(Args...args), typename std::enable_if<std::is_base_of<Object, Receiver>::value, int>::type = 0>
    void disconnect(Receiver *receiver, void(Receiver::*func)(Args...args)) {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
            const SignalInvoker& invoker = it->second;
            auto it_func = static_cast<const ReceiverMemFunc *>(invoker->receiver_func_pointer());
            if ((invoker->receiver_pointer() == receiver) && (*it_func == func)) {
                m_objects.erase(it);
                return;
            }
        }
    }

private:
    std::map<Connection, SignalInvoker> objects() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_objects;
    }

    Connection get_next_connection_id() {
        return m_next_connection_id++;
    }

    mutable std::mutex                  m_mutex;
    std::map<Connection, SignalInvoker> m_objects;
    int64_t                             m_next_connection_id = 0;
};
}
