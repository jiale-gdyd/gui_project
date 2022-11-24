#ifndef QREFCOUNT_H
#define QREFCOUNT_H

#include "../thread/qatomic.h"

QT_BEGIN_NAMESPACE

namespace QtPrivate {
class RefCount {
public:
    inline bool ref() noexcept {
        int count = atomic.loadRelaxed();
#if !defined(QT_NO_UNSHARABLE_CONTAINERS)
        if (count == 0) {
            return false;
        }
#endif
        if (count != -1) {
            atomic.ref();
        }

        return true;
    }

    inline bool deref() noexcept {
        int count = atomic.loadRelaxed();
#if !defined(QT_NO_UNSHARABLE_CONTAINERS)
        if (count == 0) // !isSharable
            return false;
#endif
        if (count == -1) // isStatic
            return true;
        return atomic.deref();
    }

#if !defined(QT_NO_UNSHARABLE_CONTAINERS)
    bool setSharable(bool sharable) noexcept
    {
        Q_ASSERT(!isShared());
        if (sharable)
            return atomic.testAndSetRelaxed(0, 1);
        else
            return atomic.testAndSetRelaxed(1, 0);
    }

    bool isSharable() const noexcept
    {
        // Sharable === Shared ownership.
        return atomic.loadRelaxed() != 0;
    }
#endif

    bool isStatic() const noexcept
    {
        // Persistent object, never deleted
        return atomic.loadRelaxed() == -1;
    }

    bool isShared() const noexcept
    {
        int count = atomic.loadRelaxed();
        return (count != 1) && (count != 0);
    }

    void initializeOwned() noexcept { atomic.storeRelaxed(1); }
    void initializeUnsharable() noexcept { atomic.storeRelaxed(0); }

    QBasicAtomicInt atomic;
};

}

#define Q_REFCOUNT_INITIALIZE_STATIC { Q_BASIC_ATOMIC_INITIALIZER(-1) }

QT_END_NAMESPACE

#endif
