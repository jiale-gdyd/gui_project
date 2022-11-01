#pragma once

#include <utility>
#include <functional>
#include <type_traits>

namespace Bindings {
namespace Private {
/**
 * get_arity是一个模板函数，它返回(几乎)任何可调用对象的实参数量。最简单的方法是为可调用类型T调用get_arity<T>()。
 * 它需要是constexpr，以便可以在模板实参中使用
 */

/* 要重载get_arity，它需要一个标记类型，因为c++不允许部分函数专门化 */
template<typename T>
struct TypeMarker {
    constexpr TypeMarker() = default;
};

/* get_arity的基本实现通过为其专门化的TypeMarker使用重载来引用每种可调用对象类型的专门化实现 */
template<typename T>
constexpr size_t get_arity()
{
    return get_arity(TypeMarker<std::decay_t<T>>{});
}

/* get_arity的语法糖版本，允许将任何可调用对象传递给get_arity，而不必将其decltype作为模板参数传递 */
template<typename T>
constexpr size_t get_arity(const T &)
{
    return get_arity<T>();
}

/* 函数指针的arity就是它的参数个数 */
template<typename Return, typename... Arguments>
constexpr size_t get_arity(TypeMarker<Return (*)(Arguments...)>)
{
    return sizeof...(Arguments);
}

template<typename Return, typename... Arguments>
constexpr size_t get_arity(TypeMarker<Return (*)(Arguments...) noexcept>)
{
    return sizeof...(Arguments);
}

/* 泛型可调用对象的arity是它的operator() - 1的arity，因为this指针对于这样的对象是已知的 */
template<typename T>
constexpr size_t get_arity(TypeMarker<T>)
{
    return get_arity(TypeMarker<decltype(&T::operator())>{}) - 1;
}

/* 帮助定义可能的成员函数限定符的大多数组合。给sizeof...(Arguments)加上1，因为"this"指针是指向任何成员函数的隐式参数 */
#define BINDINGS_DEFINE_MEMBER_GET_ARITY(MODIFIERS)                                                             \
    template<typename Return, typename Class, typename... Arguments>                                            \
    constexpr size_t get_arity(::Bindings::Private::TypeMarker<Return (Class::*)(Arguments...) MODIFIERS>)      \
    {                                                                                                           \
        return sizeof...(Arguments) + 1;                                                                        \
    }

/* 在不使用宏的情况下定义不带修饰符的get_arity版本 */
template<typename Return, typename Class, typename... Arguments>
constexpr size_t get_arity(::Bindings::Private::TypeMarker<Return (Class::*)(Arguments...)>)
{
    return sizeof...(Arguments) + 1;
}

BINDINGS_DEFINE_MEMBER_GET_ARITY(const)
BINDINGS_DEFINE_MEMBER_GET_ARITY(&)
BINDINGS_DEFINE_MEMBER_GET_ARITY(const &)
BINDINGS_DEFINE_MEMBER_GET_ARITY(&&)
BINDINGS_DEFINE_MEMBER_GET_ARITY(const &&)

BINDINGS_DEFINE_MEMBER_GET_ARITY(volatile)
BINDINGS_DEFINE_MEMBER_GET_ARITY(volatile const)
BINDINGS_DEFINE_MEMBER_GET_ARITY(volatile &)
BINDINGS_DEFINE_MEMBER_GET_ARITY(volatile const &)
BINDINGS_DEFINE_MEMBER_GET_ARITY(volatile &&)
BINDINGS_DEFINE_MEMBER_GET_ARITY(volatile const &&)

BINDINGS_DEFINE_MEMBER_GET_ARITY(noexcept)
BINDINGS_DEFINE_MEMBER_GET_ARITY(const noexcept)
BINDINGS_DEFINE_MEMBER_GET_ARITY(&noexcept)
BINDINGS_DEFINE_MEMBER_GET_ARITY(const &noexcept)
BINDINGS_DEFINE_MEMBER_GET_ARITY(&&noexcept)
BINDINGS_DEFINE_MEMBER_GET_ARITY(const &&noexcept)

BINDINGS_DEFINE_MEMBER_GET_ARITY(volatile noexcept)
BINDINGS_DEFINE_MEMBER_GET_ARITY(volatile const noexcept)
BINDINGS_DEFINE_MEMBER_GET_ARITY(volatile &noexcept)
BINDINGS_DEFINE_MEMBER_GET_ARITY(volatile const &noexcept)
BINDINGS_DEFINE_MEMBER_GET_ARITY(volatile &&noexcept)
BINDINGS_DEFINE_MEMBER_GET_ARITY(volatile const &&noexcept)

/* 占位符提供了一种构造等价于std::placeholders::_N的方法，其中N作为模板参数。由于占位符从1开始，因此placeholder<0>不是有效的占位符 */
template<int>
struct placeholder {

};

template<typename Func, typename... Args, std::size_t... Is>
auto bind_first_helper(std::index_sequence<Is...>, Func &&fun, Args... args)
{
    return std::bind(std::forward<Func>(fun), std::forward<Args>(args)..., placeholder<Is + 1>{}...);
}

/**
 * bind_first将可调用对象(即函数)的第一个参数绑定到args提供的值。
 * 返回值是一个接受get_arity<Func> - sizeof...(Args)多个参数的新函数，第一个sizeof...(Args)参数绑定到Args的值。
 * 这与std::bind(fun, args...)的调用不同，因为在这种情况下，由std::bind创建的可调用对象现在将接受零参数，而bind_first仍然期望提供剩余的参数。
 * 目前，在Args中提供std::placeholders的实例是不允许的，因为如果sizeof...(Args) != get_arity<Func>的含义还不清楚。enable_if_t确保没有一个Args值是占位符。
 * 将来，我们可以提供这个函数的另一个重载，允许占位符，只要所有实参都被绑定
 */
template<typename Func, typename... Args, typename = std::enable_if_t<std::conjunction_v<std::negation<std::is_placeholder<Args>>...>>>
auto bind_first(Func &&fun, Args &&...args)
{
    return bind_first_helper(std::make_index_sequence<get_arity<Func>() - sizeof...(Args)>{}, std::forward<Func>(fun), std::forward<Args>(args)...);
}
}
}

namespace std {
/* 这允许使用占位符替换std::placeholders */
template<int N>
struct is_placeholder<Bindings::Private::placeholder<N>> : integral_constant<int, N> {

};
}
