#pragma once

#include <type_traits>
#include "node.hpp"

namespace Bindings {
namespace Private {
template<typename T>
struct bindable_value_type_ {
    using type = T;
};

template<typename T>
struct bindable_value_type_<Property<T>> {
    using type = T;
};

template<typename T>
struct bindable_value_type_<NodeInterface<T>> {
    using type = T;
};

template<typename T>
struct bindable_value_type_<Node<T>> {
    using type = T;
};

template<typename T>
struct bindable_value_type : bindable_value_type_<std::decay_t<T>> {
};

template<typename T>
using bindable_value_type_t = typename bindable_value_type<T>::type;

/* 查找封装操作符和参数的Node的类型 */
template<typename Operator, typename... Ts>
using operator_node_result = std::decay<std::invoke_result_t<std::decay_t<Operator>, bindable_value_type_t<Ts>...>>;

template<typename Operator, typename... Ts>
using operator_node_result_t = typename operator_node_result<Operator, Ts...>::type;

/* 节点创建助手 */
template<typename T>
inline Node<std::decay_t<T>> makeNode(T &&value)
{
    return Node<std::decay_t<T>>(std::make_unique<ConstantNode<std::decay_t<T>>>(std::move(value)));
}

template<typename T>
inline Node<T> makeNode(Property<T> &property)
{
    return Node<T>(std::make_unique<PropertyNode<T>>(property));
}

template<typename T>
inline Node<T> makeNode(Node<T> &&node)
{
    return std::move(node);
}

template<typename Operator, typename... Ts, typename = std::enable_if_t<sizeof...(Ts) >= 1>, typename ResultType = operator_node_result_t<Operator, Ts...>>
inline Node<ResultType> makeNode(Operator &&op, Ts &&...args)
{
    return Node<ResultType>(std::make_unique<OperatorNode<ResultType, std::decay_t<Operator>, bindable_value_type_t<Ts>...>>(std::forward<Operator>(op), makeNode(std::forward<Ts>(args))...));
}

/* 函数和运算符助手需要 */
template<typename T>
struct is_bindable : std::integral_constant<bool, is_property<T>::value || is_node<T>::value> {

};
}
}
