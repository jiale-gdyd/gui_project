#pragma once

#include <atomic>
#include <utility>

namespace hope {
namespace detail {
template<typename T>
class AtomicWrapper {
public:
    AtomicWrapper() = default;

    AtomicWrapper(T &&t) : m_atomic(std::move(t)) {

    }

    AtomicWrapper(const AtomicWrapper &other) : m_atomic(other.m_atomic.load()) {

    }

    AtomicWrapper(AtomicWrapper &&other) noexcept = delete;

    AtomicWrapper &operator=(const AtomicWrapper &other) {
        if (&other != this) {
            *this = AtomicWrapper(other);
        }

        return *this;
    }

    AtomicWrapper &operator=(AtomicWrapper &&other) noexcept = delete;

    const std::atomic<T> &value() const {
        return m_atomic;
    }

    std::atomic<T> &value() {
        return m_atomic;
    }

private:
    std::atomic<T> m_atomic;
};
}
}
