#pragma once

#include <chrono>

#include "object.hpp"
#include "signal.hpp"

namespace hope {
class Timer final : public Object {
public:
    Timer();
    ~Timer() override;

    std::chrono::milliseconds duration() const;

    void set_duration(std::chrono::milliseconds duration);

    Signal<> &triggered();

    void start();

protected:
    void on_event(Event *event) final;

private:
    std::chrono::milliseconds m_duration;
    Signal<>                  m_triggered;
};
}
