#include <iostream>
#include <boost/signals2.hpp>
#include <boost/bind/bind.hpp>

#include "private.hpp"

static void slots1()
{
    std::cout << "slots1 called" << std::endl;
}
 
static void slots2()
{
    std::cout << "slots2 called" << std::endl;
}

class ClassA {
public:
    static int staticMemberFunc(int param) {
        std::cout << "ClassA::staticMemberFunc called, param: " << param << std::endl;
        return 0;
    }

    int memberFunc(int param) {
        std::cout << "ClassA::memberFunc called, param: " << param << std::endl;
        return 0;
    }
};

int host_boost_signal_slots_demo_exit(void)
{
    return 0;
}

int host_boost_signal_slots_demo_init(int argc, char *argv[])
{
    ClassA classA;
    boost::signals2::signal<void ()> sig;
    boost::signals2::signal<int (int)> sig2;

    boost::signals2::connection c1 = sig.connect(&slots1);
    boost::signals2::connection c2 = sig.connect(&slots2);
    sig();

    if (c2.connected()) {
        c2.disconnect();
    }

    std::cout << "second call ......" << std::endl;
    sig();

    boost::signals2::connection c3 = sig2.connect(&ClassA::staticMemberFunc);
    boost::signals2::connection c4 = sig2.connect(boost::bind(&ClassA::memberFunc, &classA, boost::placeholders::_1));

    std::cout << "return code is: " << std::endl << *sig2(44) << std::endl;
    return 0;
}
