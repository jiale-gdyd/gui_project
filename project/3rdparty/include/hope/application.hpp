#pragma once

#include "object.hpp"
#include "eventloop.hpp"

namespace hope {
namespace detail {
class ObjectData;
}

namespace test {
class ApplicationTestHelper;
}

class Application : public Object {
public:
    Application();
    ~Application() override;

    void quit(int exit_code);

    void quit() {
        quit(0);
    }

    int exec();

private:
    friend class test::ApplicationTestHelper;

    EventLoop m_event_loop;
};
}
