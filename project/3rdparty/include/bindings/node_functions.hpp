#pragma once

#include <cmath>
#include "make_node.hpp"

namespace Bindings {
namespace Private {
template<typename... Ts>
struct any_bindables;

/* 检查单个类型是否是可绑定的(节点或属性) */
template<typename T>
struct any_bindables<T> : is_bindable<T> {

};

/* 检查类型列表的头并递归 */
template<typename HEAD, typename... Ts>
struct any_bindables<HEAD, Ts...> : std::integral_constant<bool, any_bindables<HEAD>::value || any_bindables<Ts...>::value> {

};
}

/**
 * BINDINGS_DECLARE_FUNCTION是一个helper宏，用于声明和定义数据绑定中使用的函数。
 * 这个宏可以接受任何可调用的对象或函数引用，并创建一个可以在数据绑定表达式中使用的新函数。
 * 可以使用属性或数据绑定表达式的结果调用的结果函数，以创建另一个数据绑定表达式。
 * 注意，如果函数重载，则不可能一次引用其所有重载。因此，我们建议声明一个带有模板操作符()的结构作为函数对象。
 * 有关如何做到这一点的示例，请参阅Bindings::node_abs结构
 * 
 * @param NAME 要生成的函数的名称
 * @param FUNC 包装的函数
 */
#define BINDINGS_DECLARE_FUNCTION(NAME, FUNC)                                                                                                                                                       \
    template<typename... Ts>                                                                                                                                                                        \
    inline auto NAME(Ts &&...args)->std::enable_if_t<Bindings::Private::any_bindables<Ts...>::value, Bindings::Private::Node<Bindings::Private::operator_node_result_t<decltype(FUNC), Ts...>>>     \
    {                                                                                                                                                                                               \
        return Private::makeNode(FUNC, std::forward<Ts>(args)...);                                                                                                                                  \
    }

/**
 * 一个示例结构，与BINDINGS_DECLARE_FUNCTION调用一起使用，以声明std::abs的所有重载在数据绑定中可用。
 * 由于node_abs重载它的操作符()的方式，它可以在调用BINDINGS_DECLARE_FUNCTION时像这样使用: BINDINGS_DECLARE_FUNCTION(abs, node_abs{})
 * 
 * 要为另一个函数生成这样的结构，可以使用BINDINGS_DECLARE_FUNCTION_OBJECT宏
 */
struct node_abs {
    /* operator()被重载，因此结构可以用作函数对象。因为这个操作符是模板化的，所以node_abs的一个实例可以引用std::abs的所有重载 */
    template<typename... Ts>
    auto operator()(Ts &&...x) const {
        return std::abs(std::forward<Ts>(x)...);
    }
};

BINDINGS_DECLARE_FUNCTION(abs, node_abs{})

/**
 * 此宏声明了一个可调用结构，该结构将函数及其所有重载包装起来。
 * 声明的结构可以用作BINDINGS_DECLARE_FUNCTION(NAME, FUNCTION)的FUNCTION参数，将一个函数及其所有重载传递给宏
 *
 * @param NAME     结果结构的名称
 * @param FUNCTION 包装的函数
 */
#define BINDINGS_DECLARE_FUNCTION_OBJECT(NAME, FUNCTION)    \
    struct NAME {                                           \
        template<typename... Ts>                            \
        auto operator()(Ts &&...x) const {                  \
            return FUNCTION(std::forward<Ts>(x)...);        \
        }                                                   \
    };

/**
 * @brief 此宏允许将非嵌套命名空间中的任何函数声明为数据绑定上下文中可用的函数
 *
 * @param NAMESPACE 函数所在的命名空间的名称
 * @param NAME      要包装的函数的名称
 *
 * 与BINDINGS_DECLARE_FUNCTION(NAME, FUNC)相比，这个宏将使用#BINDINGS_DECLARE_FUNCTION_OBJECT生成一个helper结构，因此函数的所有重载都一次可用。
 * #BINDINGS_DECLARE_STD_FUNCTION基本上只是调用这个宏，命名空间参数设置为'std'
 */
#define BINDINGS_DECLARE_NAMESPACED_FUNCTION(NAMESPACE, NAME)                 \
    BINDINGS_DECLARE_FUNCTION_OBJECT(node_##NAMESPACE_##NAME, NAMESPACE::NAME)  \
    BINDINGS_DECLARE_FUNCTION(NAME, node_##NAMESPACE_##NAME{})

/**
 * @brief 这个宏基于BINDINGS_DECLARE_NAMESPACED_FUNCTION(NAMESPACE, FUNC)，使声明任何标准库函数为数据绑定可用变得更容易。
 * 它使用#BINDINGS_DECLARE_NAMESPACED_FUNCTION，因此可以使'std::'函数的所有重载同时可用。
 *
 * @param NAME 'std::'命名空间中的函数名
 */
#define BINDINGS_DECLARE_STD_FUNCTION(NAME) \
    BINDINGS_DECLARE_NAMESPACED_FUNCTION(std, NAME)

BINDINGS_DECLARE_STD_FUNCTION(floor)
BINDINGS_DECLARE_STD_FUNCTION(ceil)
BINDINGS_DECLARE_STD_FUNCTION(sin)
BINDINGS_DECLARE_STD_FUNCTION(cos)
BINDINGS_DECLARE_STD_FUNCTION(tan)
BINDINGS_DECLARE_STD_FUNCTION(asin)
BINDINGS_DECLARE_STD_FUNCTION(acos)
BINDINGS_DECLARE_STD_FUNCTION(atan)
}
