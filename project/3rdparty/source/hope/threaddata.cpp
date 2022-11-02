#include <hope/threaddata.hpp>

namespace hope {
ThreadDataRegistry &ThreadDataRegistry::instance()
{
    static ThreadDataRegistry instance;
    return instance;
}
}
