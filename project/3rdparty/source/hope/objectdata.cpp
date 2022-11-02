#include <hope/objectdata.hpp>

namespace hope {
namespace detail {
ObjectDataRegistry &ObjectDataRegistry::instance()
{
    static ObjectDataRegistry instance;
    return instance;
}
}
}
