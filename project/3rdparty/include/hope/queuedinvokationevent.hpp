#pragma once

#include <tuple>
#include <condition_variable>

#include "event.hpp"
#include "indexsequence.hpp"

namespace hope {
class Object;

class QueuedInvokationEventBase : public Event {
public:
    virtual Object *object() = 0;
    virtual void invoke() = 0;
    virtual void wait() = 0;
};

template<class Object, class ...Args>
class QueuedInvokation final : public QueuedInvokationEventBase {
    using ObjectFuncPtr = void(Object::* const)(Args...);
    using ObjectFuncArgs = std::tuple<Args...>;

public:
    QueuedInvokation(Object *object, ObjectFuncPtr object_func, Args...args)
        : m_object(object), m_object_func(object_func), m_object_func_args(std::move(args)...)
    {

    }

    QueuedInvokation() = default;
    QueuedInvokation(const QueuedInvokation &) = delete;
    QueuedInvokation(QueuedInvokation &&) noexcept = default;

    QueuedInvokation &operator=(const QueuedInvokation &) = delete;
    QueuedInvokation &operator=(QueuedInvokation &&) noexcept = default;

    Object *object() final {
        return m_object;
    }

    void invoke() final {
        invoke_impl(m_object_func_args);
    }

    void wait() final {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock, [this] {
            return m_executed;
        });
    }

    template<typename Tuple, std::size_t... I>
    void invoke_impl(Tuple &a, hope::detail::index_sequence<I...>) {
        (m_object->*m_object_func)(std::move(std::get<I>(a))...);
        set_executed();
    }

    template<typename ...T, typename Indices = hope::detail::make_index_sequence<sizeof... (T)>>
    void invoke_impl(std::tuple<T...> &t) {
        invoke_impl(t, Indices{});
    }

    void set_executed() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_executed = true;
        }

        m_cond.notify_all();
    }

    Object *const           m_object = nullptr;
    ObjectFuncPtr           m_object_func = nullptr;
    ObjectFuncArgs          m_object_func_args;
    std::mutex              m_mutex;
    std::condition_variable m_cond;
    bool                    m_executed = false;
};

template<class Object>
class QueuedInvokation<Object, void> final : public QueuedInvokationEventBase {
    using ObjectFuncPtr = void(Object::* const)();

public:
    QueuedInvokation(Object *object, ObjectFuncPtr object_func) : m_object(object), m_object_func(object_func) {

    }

    QueuedInvokation() = default;
    QueuedInvokation(const QueuedInvokation &) = delete;
    QueuedInvokation(QueuedInvokation &&) noexcept = default;

    QueuedInvokation &operator=(const QueuedInvokation &) = delete;
    QueuedInvokation &operator=(QueuedInvokation &&) noexcept = default;

    Object *object() final {
        return m_object;
    }

    void invoke() final {
        (m_object->*m_object_func)();
        set_executed();
    }

    void wait() final {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock, [this] {
            return m_executed;
        });
    }

private:
    void set_executed() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_executed = true;
        }

        m_cond.notify_all();
    }

    Object *const           m_object = nullptr;
    ObjectFuncPtr           m_object_func = nullptr;
    std::mutex              m_mutex;
    std::condition_variable m_cond;
    bool                    m_executed = false;
};

template<class Object, class ...Args>
std::shared_ptr<QueuedInvokation<Object, Args...>> make_queued_invokation_event(Object *object, void(Object::*object_func)(Args...), Args...args)
{
    return std::make_shared<QueuedInvokation<Object, Args...>>(object, object_func, std::move(args)...);
}

template<class Object>
std::shared_ptr<QueuedInvokation<Object, void>> make_queued_invokation_event(Object *object, void(Object::*object_func)())
{
    return std::make_shared<QueuedInvokation<Object, void>>(object, object_func);
}
}
