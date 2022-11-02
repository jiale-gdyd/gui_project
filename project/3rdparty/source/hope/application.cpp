#include <hope/objectdata.hpp>
#include <hope/threaddata.hpp>
#include <hope/application.hpp>
#include <hope/queuedinvokationevent.hpp>

namespace hope {
Application::Application() : Object(false)
{
    initialize();
}

Application::~Application()
{
    terminate();
}

void Application::quit(int exit_code)
{
    m_event_loop.quit(exit_code);
}

int Application::exec()
{
    return m_event_loop.exec();
}
}
