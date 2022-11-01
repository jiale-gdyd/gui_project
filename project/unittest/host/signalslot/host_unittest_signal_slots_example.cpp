#include <string>
#include <iostream>
#include <bindings/signal.hpp>

#include "private.h"

static void display()
{
    std::cout << "Hello World!" << std::endl;
}

static void displayLabelled(const std::string &label, int value)
{
    std::cout << label << ": " << value << std::endl;
}

int host_unittest_sigslot_init(int argc, char *argv[])
{
    Bindings::Signal<const std::string, int> signal;

    signal.connect(display);
    signal.connect(displayLabelled);

    signal.emit("Emit value", 5);

    return 0;
}

int host_unittest_sigslot_exit(void)
{
    return -1;
}
