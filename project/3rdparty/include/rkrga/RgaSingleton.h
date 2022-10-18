#ifndef RKRGA_RGASINGLETON_H
#define RKRGA_RGASINGLETON_H

#include "RgaMutex.h"

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wundefined-var-template"
#endif

template <typename TYPE>
class Singleton {
public:
    static TYPE &getInstance() {
        Mutex::Autolock _l(sLock);
        TYPE *instance = sInstance;

        if (instance == nullptr) {
            instance = new TYPE();
            sInstance = instance;
        }

        return *instance;
    }

    static bool hasInstance() {
        Mutex::Autolock _l(sLock);
        return sInstance != nullptr;
    }

protected:
    ~Singleton() {}
    Singleton() {}

private:
    Singleton(const Singleton &);
    Singleton &operator=(const Singleton &);

    static Mutex sLock;
    static TYPE  *sInstance;
};

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#define RGA_SINGLETON_STATIC_INSTANCE(TYPE)                             \
    template<> ::Mutex (::Singleton< TYPE >::sLock)(::Mutex::PRIVATE);  \
    template<> TYPE* ::Singleton< TYPE >::sInstance(nullptr);           \
    template class ::Singleton< TYPE >;

#endif
