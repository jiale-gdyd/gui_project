#pragma once

#include "node.hpp"
#include "make_node.hpp"
#include "node_operators.hpp"
#include "node_functions.hpp"
#include "property_updater.hpp"
#include "binding_evaluator.hpp"

namespace Bindings {
/**
 * @brief A combination of a root Node with an evaluator.
 *
 * A root Node is formed whenever multiple properties are combined inside
 * a expression and an evaluator is responsible for re-evaluating such
 * an expression whenever any of the constituent properties change.
 *
 * @tparam T The type of the value that the Binding expression evaluates to.
 * @tparam EvaluatorT The type of the evaluator that is used to evaluate the Binding.
 */
template<typename T, typename EvaluatorT = BindingEvaluator>
class Binding : public PropertyUpdater<T>, public Private::Dirtyable {
    static_assert(std::is_base_of<BindingEvaluator, EvaluatorT>::value, "The EvaluatorT type must inherit from BindingEvaluator.");

public:
    /**
     * @brief 用一个特定的求值器构造一个新的Binding
     * @param rootNode  表示绑定中包含的表达式
     * @param evaluator 用于计算绑定中包含的表达式
     */
    explicit Binding(Private::Node<T> &&rootNode, EvaluatorT const &evaluator) : m_rootNode{ std::move(rootNode) }, m_evaluator{ evaluator } {
        m_bindingId = m_evaluator.insert(this);
        m_rootNode.setParent(this);
    }

    ~Binding() override {
        m_evaluator.remove(m_bindingId);
    }

    Binding() = delete;
    Binding(Binding const &other) = delete;

    Binding &operator=(Binding const &other) = delete;

    Binding(Binding &&other) = delete;
    Binding &operator=(Binding &&other) = delete;

    /* 设置在绑定重新计算时应用于通知关联属性的函数 */
    void setUpdateFunction(std::function<void(T &&)> const &updateFunction) override {
        m_propertyUpdateFunction = updateFunction;
    }

    /* 返回绑定的当前值 */
    T get() const override {
        return m_rootNode.evaluate();
    }

    /* 重新计算绑定的值，并将更改通知所有依赖项 */
    void evaluate() {
        T value = m_rootNode.evaluate();

        /* 使用它可以通过PropertyUpdater的更新函数更新任何关联的属性 */
        m_propertyUpdateFunction(std::move(value));
    }

protected:
    Private::Dirtyable **parentVariable() override {
        return nullptr;
    }

    const bool *dirtyVariable() const override {
        return nullptr;
    }

    Private::Node<T>           m_rootNode;                                  /* 绑定的根节点表示绑定所包含的表达式 */
    EvaluatorT                 m_evaluator;                                 /* 负责计算此绑定的计算程序 */
    std::function<void (T &&)> m_propertyUpdateFunction = [](T &&) {};      /* 当绑定重新计算时，用于通知关联属性的函数 */
    int                        m_bindingId = -1;                            /* 绑定的id，用于在其求值器中跟踪绑定 */
};

/**
 * @brief 从属性创建绑定的帮助函数
 *
 * @tparam T          绑定表达式求值的值的类型
 * @tparam EvaluatorT 用于计算绑定的计算器的类型
 * @param evaluator   用于计算绑定的计算器
 * @param property    要从中创建绑定的属性
 * @return            由求值器提供的新绑定
 */
template<typename T, typename EvaluatorT>
inline std::unique_ptr<Binding<T, EvaluatorT>> makeBinding(EvaluatorT &evaluator, Property<T> &property)
{
    return std::make_unique<Binding<T, EvaluatorT>>(Private::makeNode(property), evaluator);
}

/**
 * @brief 从根节点创建绑定的帮助函数
 *
 * @tparam T          绑定表达式求值的值的类型
 * @tparam EvaluatorT 用于计算绑定的计算器的类型
 * @param evaluator   用于计算绑定的计算器
 * @param rootNode    表示将由绑定求值的表达式
 * @return            一个新的绑定，它将rootNode与计算器结合在一起
 */
template<typename T, typename EvaluatorT>
inline std::unique_ptr<Binding<T, EvaluatorT>> makeBinding(EvaluatorT &evaluator, Private::Node<T> &&rootNode)
{
    return std::make_unique<Binding<T, EvaluatorT>>(std::move(rootNode), evaluator);
}

/**
 * @brief 从函数及其参数创建Binding的帮助函数
 *
 * @tparam EvaluatorT 用于计算绑定的计算器的类型
 * @param evaluator   用于计算绑定的计算器
 * @tparam Func       函数的类型 - 可以是实现operator()的任何类型
 * @param func        函数对象
 * @tparam Args       函数参数类型
 * @param args        函数参数 - 可能的值包括:Properties, Constants和Nodes。它们将自动展开，即Property<T>将向func传递类型为T的值
 * @return            其中ReturnType是用给定参数对func求值的结果类型。绑定将由新的求值器提供支持
 */
template<typename EvaluatorT, typename Func, typename... Args, typename = std::enable_if_t<sizeof...(Args) != 0>, typename ResultType = Private::operator_node_result_t<Func, Args...>>
inline std::unique_ptr<Binding<ResultType, EvaluatorT>> makeBinding(EvaluatorT &evaluator, Func &&func, Args &&...args)
{
    return std::make_unique<Binding<ResultType, EvaluatorT>>(Private::makeNode(std::forward<Func>(func), std::forward<Args>(args)...), evaluator);
}

/**
 * 为老式的即时模式绑定提供了便利。
 * 这与不做任何事情的ImmediateBindingEvaluator类一起工作，在任何相关的可绑定对象(即属性实例)通知它们已经更改时，立即更新绑定的结果。
 * 这可能会导致在典型的GUI应用程序中使用结果之前对属性绑定进行多次计算
 *
 * @tparam T 绑定表达式求值的值的类型
 */
template<typename T>
class Binding<T, ImmediateBindingEvaluator> : public Binding<T, BindingEvaluator> {
public:
    explicit Binding(Private::Node<T> &&rootNode) : Binding<T, BindingEvaluator>(std::move(rootNode), ImmediateBindingEvaluator::instance()) {

    }

    Binding() = delete;

    virtual ~Binding() = default;

    Binding(Binding const &other) = delete;
    Binding &operator=(Binding const &other) = delete;

    Binding(Binding &&other) = delete;
    Binding &operator=(Binding &&other) = delete;

    void markDirty() override {
        Binding::evaluate();
    }
};

/**
 * @brief 从属性创建立即模式绑定的帮助函数
 *
 * @tparam T       绑定表达式求值的值的类型
 * @param property 要从中创建绑定的属性
 * @return         绑定到现有属性的新绑定，并立即求值
 */
template<typename T>
inline std::unique_ptr<Binding<T, ImmediateBindingEvaluator>> makeBinding(Property<T> &property)
{
    return std::make_unique<Binding<T, ImmediateBindingEvaluator>>(Private::makeNode(property));
}

/**
 * @brief 从根节点创建立即模式绑定的帮助函数
 *
 * @tparam T       绑定表达式求值的值的类型
 * @param rootNode 表示将由绑定求值的表达式。通常由属性上的一元/二元运算符构造
 * @return         绑定到根节点并立即计算的新绑定
 */
template<typename T>
inline std::unique_ptr<Binding<T, ImmediateBindingEvaluator>> makeBinding(Private::Node<T> &&rootNode)
{
    return std::make_unique<Binding<T, ImmediateBindingEvaluator>>(std::move(rootNode));
}

/**
 * @brief 从函数及其参数创建即时模式绑定
 *
 * @tparam Func 函数的类型 - 可以是实现operator()的任何类型
 * @param func  函数对象
 * @tparam Args 函数参数类型
 * @param args  函数参数 - 可能的值包括:Properties, Constants和Nodes。它们将自动展开，即Property<T>将向func传递类型为T的值
 * @return      其中ReturnType是用给定参数对func求值后得到的类型。绑定的特点是立即进行评估
 */
template<typename Func, typename... Args, typename = std::enable_if_t<sizeof...(Args) != 0>, typename ResultType = Private::operator_node_result_t<Func, Args...>>
inline std::unique_ptr<Binding<ResultType, ImmediateBindingEvaluator>> makeBinding(Func &&func, Args &&...args)
{
    return std::make_unique<Binding<ResultType, ImmediateBindingEvaluator>>(Private::makeNode(std::forward<Func>(func), std::forward<Args>(args)...));
}

/**
 * @brief 创建带有绑定的属性的助手函数
 *
 * 这个函数可以取:
 * - 另一个属性
 * - Node，通常通过使用操作符组合Property实例创建
 * - 带有参数(节点、常量或属性)的函数
 * 默认情况下，这将构造一个带有立即绑定求值的Property
 *
 * 另外，可以将BindingEvaluator作为该函数的第一个参数来控制何时进行求值
 */
template<typename... T>
inline auto makeBoundProperty(T &&...args)
{
    auto binding = makeBinding(std::forward<T>(args)...);
    return Property<decltype(binding->get())>(std::move(binding));
}
}
