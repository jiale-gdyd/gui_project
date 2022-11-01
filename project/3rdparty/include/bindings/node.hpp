#pragma once

#include <memory>
#include <utility>
#include <stdexcept>
#include <functional>
#include <type_traits>

#include "signal.hpp"
#include "property.hpp"

namespace Bindings {
/* 每当计算引用不再存在的属性的绑定时，都会抛出PropertyDestroyedError */
class PropertyDestroyedError : public std::runtime_error {
public:
    PropertyDestroyedError() = delete;

    using std::runtime_error::runtime_error;
};

namespace Private {
class Dirtyable {
public:
    Dirtyable() = default;
    virtual ~Dirtyable() = default;

    void setParent(Dirtyable *newParent) {
        auto **parentVar = parentVariable();
        if (parentVar) {
            *parentVar = newParent;
        }
    }

    virtual void markDirty() {
        auto *dirtyVar = dirtyVariable();
        if (dirtyVar) {
            if (*dirtyVar) {
                return;
            }

            /* 我们只想对dirtyVariable有一个重写，也就是const，所以我们必须在这里进行const强制转换 */
            *const_cast<bool *>(dirtyVar) = true;
        }

        auto **parentVar = parentVariable();
        if (parentVar && *parentVar) {
            (*parentVar)->markDirty();
        }
    }

    bool isDirty() const {
        auto *dirtyVar = dirtyVariable();
        return dirtyVar && *dirtyVar;
    }

protected:
    virtual Dirtyable **parentVariable() = 0;
    virtual const bool *dirtyVariable() const = 0;
};

template<typename ResultType>
class NodeInterface : public Dirtyable {
public:
    /* 返回一个引用，因为我们缓存了每个求值。const，因为它不应该修改AST的返回值 */
    virtual const ResultType &evaluate() const = 0;

protected:
    NodeInterface() = default;
};

template<typename ResultType>
class Node {
public:
    Node(std::unique_ptr<NodeInterface<ResultType>> &&interface) : m_interface(std::move(interface)) {

    }

    const ResultType &evaluate() const {
        return m_interface->evaluate();
    }

    void setParent(Dirtyable *newParent) {
        m_interface->setParent(newParent);
    }

    bool isDirty() const {
        return m_interface->isDirty();
    }

private:
    std::unique_ptr<NodeInterface<ResultType>> m_interface;
};

template<typename T>
class ConstantNode : public NodeInterface<T> {
public:
    explicit ConstantNode(const T &value) : m_value{ value } {

    }

    const T &evaluate() const override {
        return m_value;
    }

protected:
    /* 常量永远不可能是脏的，所以它不需要知道它的父元素，因为它不需要通知它 */
    Dirtyable **parentVariable() override {
        return nullptr;
    }

    const bool *dirtyVariable() const override {
        return nullptr;
    }

private:
    T m_value;
};

template<typename PropertyType>
class PropertyNode : public NodeInterface<PropertyType> {
public:
    explicit PropertyNode(Property<PropertyType> &property) : m_parent(nullptr), m_dirty(false) {
        setProperty(property);
    }

    PropertyNode(PropertyNode<PropertyType> &&) = delete;

    PropertyNode(const PropertyNode<PropertyType> &other) : Dirtyable(other.isDirty()) {
        setProperty(*other.m_property);
    }

    virtual ~PropertyNode() {
        m_valueChangedHandle.disconnect();
        m_movedHandle.disconnect();
        m_destroyedHandle.disconnect();
    }

    const PropertyType &evaluate() const override {
        if (!m_property) {
            throw PropertyDestroyedError("The Property this node refers to no longer exists!");
        }

        m_dirty = false;
        return m_property->get();
    }

    /* 它当前必须接受一个const引用，因为"moved"信号会发出一个const& */
    void propertyMoved(Property<PropertyType> &property) {
        if (&property != m_property) {
            m_property = &property;
        } else {
            /* 另一个属性被移动到该节点引用的属性中。因此，它将不再更新此节点 */
            m_property = nullptr;
        }
    }

    void propertyDestroyed() {
        m_property = nullptr;
    }

protected:
    Dirtyable **parentVariable() override {
        return &m_parent;
    }

    const bool *dirtyVariable() const override {
        return &m_dirty;
    }

private:
    void setProperty(Property<PropertyType> &property) {
        m_property = &property;
        m_valueChangedHandle = m_property->valueChanged().connect(&PropertyNode<PropertyType>::markDirty, this);
        m_movedHandle = m_property->m_moved.connect(&PropertyNode<PropertyType>::propertyMoved, this);
        m_destroyedHandle = m_property->destroyed().connect(&PropertyNode<PropertyType>::propertyDestroyed, this);
    }

    Property<PropertyType> *m_property;
    ConnectionHandle       m_movedHandle;
    ConnectionHandle       m_valueChangedHandle;
    ConnectionHandle       m_destroyedHandle;

    Dirtyable              *m_parent;
    mutable bool           m_dirty;
};

template<typename ResultType, typename Operator, typename... Ts>
class OperatorNode : public NodeInterface<ResultType> {
public:
    /* 为Operator类型添加另一个typename模板，这样它就可以成为通用引用 */
    template<typename Op>
    explicit OperatorNode(Op &&op, Node<Ts> &&...arguments)
        : m_parent{ nullptr }, m_dirty{ true }, m_op{ std::move(op) }, m_values{ std::move(arguments)... }, m_result(reevaluate())
    {
        static_assert(std::is_convertible_v<decltype(m_op(std::declval<Ts>()...)), ResultType>, "The result of the Operator must be convertible to the ReturnType of the Node");
        setParents<0>();
    }

    template<std::size_t I>
    auto setParents() -> std::enable_if_t<I == sizeof...(Ts)> {

    }

    template<std::size_t I>
    auto setParents() -> std::enable_if_t<I < sizeof...(Ts)> {
        std::get<I>(m_values).setParent(this);
        setParents<I + 1>();
    }

    virtual ~OperatorNode() = default;

    const ResultType &evaluate() const override {
        if (Dirtyable::isDirty()) {
            m_result = reevaluate();
        }

        return m_result;
    }

protected:
    Dirtyable **parentVariable() override {
        return &m_parent;
    }

    const bool *dirtyVariable() const override {
        return &m_dirty;
    }

private:
    template<std::size_t... Is>
    ResultType reevaluate_helper(std::index_sequence<Is...>) const {
        return m_op(std::get<Is>(m_values).evaluate()...);
    }

    ResultType reevaluate() const {
        m_dirty = false;
        return reevaluate_helper(std::make_index_sequence<sizeof...(Ts)>());
    }

    Dirtyable               *m_parent;
    mutable bool            m_dirty;

    Operator                m_op;
    std::tuple<Node<Ts>...> m_values;

    // 注意: 重要的是m_result是最后计算的!否则，在构造函数中重新求值的调用将失败
    mutable ResultType      m_result;
};

template<typename T>
struct is_node_helper : std::false_type {

};

template<typename T>
struct is_node_helper<Node<T>> : std::true_type {

};

template<typename T>
struct is_node : is_node_helper<T> {

};
}
}
