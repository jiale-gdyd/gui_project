#pragma once

#include <map>
#include <memory>
#include <functional>

namespace Bindings {
/**
 * BindingEvaluator提供了一种机制来控制重新计算Bindings::Binding的确切时间。BindingEvaluator表示可以有选择地重新计算的绑定实例的集合。
 * 如果使用Bindings::makeBoundProperty和BindingEvaluator创建绑定，那么只有在给定的计算器上调用BindingEvaluator::evaluateAll时，绑定才会被求值。
 * 注意，BindingEvaluator的实例内部包装其绑定集合的方式是，复制BindingEvaluator并不实际复制绑定集合。
 * 因此，向BindingEvaluator的副本添加绑定也会将其添加到原始绑定中。这样做是为了便于使用，因此可以在整个代码库中轻松地传递求值器
 */
class BindingEvaluator {
    /* 我们在这里使用pimpl，这样我们就可以按值(副本)传递求值器，而每个副本都引用同一组数据 */
    struct Private {
        std::map<int, std::function<void()>> m_bindingEvalFunctions;
        int                                  m_currentId;
    };

public:
    BindingEvaluator() = default;
    BindingEvaluator(const BindingEvaluator &) noexcept = default;

    BindingEvaluator &operator=(const BindingEvaluator &) noexcept = default;

    BindingEvaluator(BindingEvaluator &&other) noexcept = delete;
    BindingEvaluator &operator=(BindingEvaluator &&other) noexcept = delete;

    /* 该函数按插入的顺序计算用该求值器构造的所有Binding实例。因此，它也将更新相关的Property实例 */
    void evaluateAll() const {
        /* map的顺序是确定的，因此绑定按插入的顺序求值，以确保正确的传递依赖性求值 */
        for (auto &[id, func] : m_d->m_bindingEvalFunctions) {
            func();
        }
    }

private:
    template<typename BindingType>
    int insert(BindingType *binding) {
        m_d->m_bindingEvalFunctions.insert({
            ++(m_d->m_currentId), [=]() {
                binding->evaluate();
            }
        });

        return m_d->m_currentId;
    }

    void remove(int id) {
        m_d->m_bindingEvalFunctions.erase(id);
    }

    std::shared_ptr<Private> m_d{ std::make_shared<Private>() };

    template<typename T, typename UpdaterT>
    friend class Binding;
};

/**
 * BindingEvaluator的这个子类本身不做任何特殊的事情。它与绑定的模板特化一起使用，以提供老派的直接模式绑定。
 * 任何用ImmediateBindingEvaluator构造的绑定都不会等待求值器调用evaluateAll，而是在它的任何可绑定(即属性实例)更改时立即求值绑定。
 * 这可能会导致在典型的GUI应用程序中使用结果之前对属性绑定进行多次计算
 */
class ImmediateBindingEvaluator final : public BindingEvaluator {
public:
    static inline ImmediateBindingEvaluator instance() {
        static ImmediateBindingEvaluator evaluator;
        return evaluator;
    }
};
}
