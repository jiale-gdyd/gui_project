#pragma once

#include <functional>

namespace Bindings {
/**
 * PropertyUpdater定义用于更新属性的接口，例如从绑定表达式。
 * 该类的实例(包装在std::unique_ptr中)可以传递给Property构造函数。然后属性将变为只读，这意味着如果通过PropertyUpdater以外的任何其他方式更新属性的值，
 * 则会抛出ReadOnlyProperty的实例。
 * 为此，Property构造函数将向setUpdateFunction()传递一个函数。这个函数是更新属性而不遇到ReadOnlyProperty错误的唯一方法。
 * PropertyUpdater最典型的用法是在由makeBoundProperty()创建的Binding实例中
 */
template<typename T>
class PropertyUpdater {
public:
    PropertyUpdater() = default;
    virtual ~PropertyUpdater() = default;

    PropertyUpdater(PropertyUpdater const &other) = default;
    PropertyUpdater &operator=(PropertyUpdater const &other) = default;

    PropertyUpdater(PropertyUpdater &&other) = default;
    PropertyUpdater &operator=(PropertyUpdater &&other) = default;

    /**
     * Property将在构造该函数时调用它，并传递一个std::function作为参数，允许PropertyUpdater更新Property值。
     * PropertyUpdater通常保存此函数，并在计算的值发生变化时调用它
     */
    virtual void setUpdateFunction(std::function<void (T &&)> const &updateFunction) = 0;

    /**
     * get()函数必须返回PropertyUpdater希望分配给Property的当前值。
     * 它是从Property构造函数调用的
     */
    virtual T get() const = 0;
};
}
