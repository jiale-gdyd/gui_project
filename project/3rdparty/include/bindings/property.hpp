#pragma once

#include <memory>
#include <iostream>
#include <type_traits>

#include "signal.hpp"
#include "property_updater.hpp"

namespace Bindings {
namespace Private {
template<typename X, typename Y, typename = void>
struct are_equality_comparable : std::false_type {

};

template<typename X, typename Y>
struct are_equality_comparable<X, Y, std::enable_if_t<std::is_same<std::decay_t<decltype(std::equal_to<>{}(std::declval<X>(), std::declval<Y>()))>, bool>::value>> : std::true_type {

};

template<typename X, typename Y>
constexpr bool are_equality_comparable_v = are_equality_comparable<X, Y>::value;
}

/* 当试图设置与PropertyUpdater相关联的属性的值时，将抛出ReadOnlyProperty。最常见的原因是属性保存绑定表达式的结果 */
struct ReadOnlyProperty : std::runtime_error {
    ReadOnlyProperty() = delete;

    using std::runtime_error::runtime_error;
};

template<typename T>
struct equal_to {
    auto operator()(const T &x, const T &y) const noexcept -> std::enable_if_t<Private::are_equality_comparable_v<T, T>, bool> {
        return std::equal_to<>{}(x, y);
    }

    template<typename X, typename Y>
    auto operator()(const X &, const Y &) const noexcept -> std::enable_if_t<!Private::are_equality_comparable_v<X, Y>, bool> {
        return false;
    }
};

namespace Private {
template<typename PropertyType>
class PropertyNode;
}

template<typename T>
class Property {
public:
    typedef T valuetype;

    Property() = default;
    ~Property() {
        m_destroyed.emit();
    }

    explicit Property(T value) noexcept(std::is_nothrow_move_constructible<T>::value) : m_value{ std::move(value) } {

    }

    Property(Property<T> const &other) = delete;
    Property &operator=(Property<T> const &other) = delete;

    Property(Property<T> &&other) noexcept(std::is_nothrow_move_constructible<T>::value)
        : m_value(std::move(other.m_value)),
        m_valueAboutToChange(std::move(other.m_valueAboutToChange)),
        m_valueChanged(std::move(other.m_valueChanged)),
        m_destroyed(std::move(other.m_destroyed)),
        m_updater(std::move(other.m_updater))
    {
        if (m_updater) {
            using namespace std::placeholders;
            m_updater->setUpdateFunction(std::bind(&Property<T>::setHelper, this, _1));
        }

        /* 为移出和移到属性发出移动信号 */
        m_moved.emit(*this);
        other.m_moved.emit(*this);
        m_moved = std::move(other.m_moved);
    }

    Property &operator=(Property<T> &&other) noexcept(std::is_nothrow_move_assignable<T>::value) {
        m_value = std::move(other.m_value);
        m_valueAboutToChange = std::move(other.m_valueAboutToChange);
        m_valueChanged = std::move(other.m_valueChanged);
        m_destroyed = std::move(other.m_destroyed);
        m_updater = std::move(other.m_updater);

        if (m_updater) {
            using namespace std::placeholders;
            m_updater->setUpdateFunction(std::bind(&Property<T>::setHelper, this, _1));
        }

        /* 为移出和移到属性发出移动信号 */
        m_moved.emit(*this);
        other.m_moved.emit(*this);
        m_moved = std::move(other.m_moved);

        return *this;
    }

    template<typename UpdaterT>
    explicit Property(std::unique_ptr<UpdaterT> &&updater) {
        *this = std::move(updater);
    }

    template<typename UpdaterT>
    Property &operator=(std::unique_ptr<UpdaterT> &&updater) {
        m_updater = std::move(updater);

        using namespace std::placeholders;
        m_updater->setUpdateFunction(std::bind(&Property<T>::setHelper, this, _1));
        setHelper(m_updater->get());

        return *this;
    }

    void reset() {
        m_updater.reset();
    }

    /* 返回一个将在值更改之前发出的信号。第一个发出的值是属性的当前值，第二个发出的值是属性的新值 */
    Signal<const T &, const T &> &valueAboutToChange() const {
        return m_valueAboutToChange;
    }

    /* 返回一个信号，该信号将在属性的值更改后发出。发出的值是属性的当前(新)值 */
    Signal<const T &> &valueChanged() const {
        return m_valueChanged;
    }

    /* 返回一个信号，该信号将在此属性被销毁时发出 */
    Signal<> &destroyed() const {
        return m_destroyed;
    }

    void set(T value) {
        if (m_updater) {
            throw ReadOnlyProperty{ "Cannot set value on a read-only property. This property likely holds the result of a binding expression." };
        }

        setHelper(std::move(value));
    }

    T const &get() const {
        return m_value;
    }

    Property<T> &operator=(T const &rhs) {
        set(std::move(rhs));
        return *this;
    }

    T const &operator()() const {
        return Property<T>::get();
    }

private:
    void setHelper(T value) {
        if (equal_to<T>{}(value, m_value)) {
            return;
        }

        m_valueAboutToChange.emit(m_value, value);
        m_value = std::move(value);
        m_valueChanged.emit(m_value);
    }

    T                                    m_value;
    mutable Signal<const T &, const T &> m_valueAboutToChange;  /* 属性中的信号是可变的，因为属性为"const"应该意味着它的值或绑定不会改变，而不是说没有人可以再监听它 */
    mutable Signal<const T &>            m_valueChanged;        /* 通过const ref，我们可以发出只有移动类型的T的信号 */

    template<typename PropertyType>
    friend class Private::PropertyNode;

    Signal<Property<T> &>                m_moved;

    mutable Signal<>                     m_destroyed;
    std::unique_ptr<PropertyUpdater<T>>  m_updater;
};

/* 将属性的值输出到输出流 */
template<typename T>
std::ostream &operator<<(std::ostream &stream, Property<T> const &property)
{
    stream << property.get();
    return stream;
}

/* 从输入流中读取类型为T的值，并使用set()将其赋值给属性 */
template<typename T>
std::istream &operator>>(std::istream &stream, Property<T> &prop)
{
    T temp;
    stream >> temp;
    prop.set(std::move(temp));

    return stream;
}

namespace Private {
template<typename T>
struct is_property_helper : std::false_type {

};

template<typename T>
struct is_property_helper<Property<T>> : std::true_type {

};

template<typename T>
struct is_property : is_property_helper<std::decay_t<T>> {

};
}
}
