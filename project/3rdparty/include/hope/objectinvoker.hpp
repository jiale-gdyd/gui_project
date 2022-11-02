#pragma once

#include "signal.hpp"

namespace hope {
class ObjectInvoker {
public:
    ObjectInvoker() = delete;

    template<typename Object, typename ...Args>
    static void invoke(Object *obj, void(Object::*func)(Args... args), ConnectionType type, Args...args) {
        Signal<Args...> signal;
        signal.connect(obj, func, type);
        signal.emit(args...);
    }
};
}
