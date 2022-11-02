#pragma once

namespace hope {
namespace detail {
template<class T>
class Optional {
public:
    Optional() : m_ok(false) {
    }

    Optional(T &&data) : m_ok(true), m_data(std::move(data)) {

    }

    Optional(const T &data) : m_ok(true), m_data(data) {

    }

    template<typename ...Args>
    Optional(Args && ... args) : m_ok(true), m_data(std::forward<Args...>(args...)) {

    }

    Optional(const Optional &) = default;
    Optional(Optional &&) noexcept = default;

    Optional &operator=(const Optional &) = default;
    Optional &operator=(Optional &&) noexcept = default;

    Optional &operator=(const T &data) {
        m_ok = true;
        m_data = data;
        return *this;
    }

    Optional& operator=(T &&data) {
        m_ok = true;
        m_data = std::move(data);
        return *this;
    }

    void reset() {
        m_ok = false;
    }

    T &data() {
        return m_data;
    }

    const T &data() const {
        return m_data;
    }

    bool is_some() const {
        return m_ok;
    }

    bool is_none() const {
        return !m_ok;
    }

    operator T() {
        return m_data;
    }

    operator bool() {
        return m_ok;
    }

    T *operator->() {
        return m_ok ? &m_data : nullptr;
    }

    const T *operator->() const {
        return m_ok ? &m_data : nullptr;
    }

    bool operator==(const Optional &other) const {
        return (m_ok == other.m_ok) && (m_data == other.m_data);
    }

    bool operator!=(const Optional &other) const {
        return !operator==(other);
    }

private:
    bool m_ok = false;
    T    m_data;
};
}
}
