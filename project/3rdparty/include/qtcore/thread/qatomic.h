#include "../global/qglobal.h"

#ifndef QATOMIC_H
#define QATOMIC_H

#include "qbasicatomic.h"

QT_BEGIN_NAMESPACE

QT_WARNING_PUSH
QT_WARNING_DISABLE_GCC("-Wextra")

#ifdef Q_CLANG_QDOC
#undef QT_BASIC_ATOMIC_HAS_CONSTRUCTORS
#endif

template <typename T>
class QAtomicInteger : public QBasicAtomicInteger<T> {
public:
#ifdef QT_BASIC_ATOMIC_HAS_CONSTRUCTORS
    constexpr QAtomicInteger(T value = 0) noexcept : QBasicAtomicInteger<T>(value) {

    }
#else
    inline QAtomicInteger(T value = 0) noexcept {
        this->_q_value = value;
    }
#endif

    inline QAtomicInteger(const QAtomicInteger &other) noexcept
#ifdef QT_BASIC_ATOMIC_HAS_CONSTRUCTORS
        : QBasicAtomicInteger<T>()
#endif
    {
        this->storeRelease(other.loadAcquire());
    }

    inline QAtomicInteger &operator=(const QAtomicInteger &other) noexcept {
        this->storeRelease(other.loadAcquire());
        return *this;
    }

#ifdef Q_CLANG_QDOC
    T load() const;
    T loadRelaxed() const;
    T loadAcquire() const;

    void store(T newValue);
    void storeRelaxed(T newValue);
    void storeRelease(T newValue);

    operator T() const;
    QAtomicInteger &operator=(T);

    static Q_DECL_CONSTEXPR bool isReferenceCountingNative();
    static Q_DECL_CONSTEXPR bool isReferenceCountingWaitFree();

    bool ref();
    bool deref();

    static Q_DECL_CONSTEXPR bool isTestAndSetNative();
    static Q_DECL_CONSTEXPR bool isTestAndSetWaitFree();

    bool testAndSetRelaxed(T expectedValue, T newValue);
    bool testAndSetAcquire(T expectedValue, T newValue);
    bool testAndSetRelease(T expectedValue, T newValue);
    bool testAndSetOrdered(T expectedValue, T newValue);

    static Q_DECL_CONSTEXPR bool isFetchAndStoreNative();
    static Q_DECL_CONSTEXPR bool isFetchAndStoreWaitFree();

    T fetchAndStoreRelaxed(T newValue);
    T fetchAndStoreAcquire(T newValue);
    T fetchAndStoreRelease(T newValue);
    T fetchAndStoreOrdered(T newValue);

    static Q_DECL_CONSTEXPR bool isFetchAndAddNative();
    static Q_DECL_CONSTEXPR bool isFetchAndAddWaitFree();

    T fetchAndAddRelaxed(T valueToAdd);
    T fetchAndAddAcquire(T valueToAdd);
    T fetchAndAddRelease(T valueToAdd);
    T fetchAndAddOrdered(T valueToAdd);

    T fetchAndSubRelaxed(T valueToSub);
    T fetchAndSubAcquire(T valueToSub);
    T fetchAndSubRelease(T valueToSub);
    T fetchAndSubOrdered(T valueToSub);

    T fetchAndOrRelaxed(T valueToOr);
    T fetchAndOrAcquire(T valueToOr);
    T fetchAndOrRelease(T valueToOr);
    T fetchAndOrOrdered(T valueToOr);

    T fetchAndAndRelaxed(T valueToAnd);
    T fetchAndAndAcquire(T valueToAnd);
    T fetchAndAndRelease(T valueToAnd);
    T fetchAndAndOrdered(T valueToAnd);

    T fetchAndXorRelaxed(T valueToXor);
    T fetchAndXorAcquire(T valueToXor);
    T fetchAndXorRelease(T valueToXor);
    T fetchAndXorOrdered(T valueToXor);

    T operator++();
    T operator++(int);
    T operator--();
    T operator--(int);
    T operator+=(T value);
    T operator-=(T value);
    T operator|=(T value);
    T operator&=(T value);
    T operator^=(T value);
#endif
};

class QAtomicInt : public QAtomicInteger<int> {
public:
#ifdef QT_BASIC_ATOMIC_HAS_CONSTRUCTORS
    constexpr
#endif
    QAtomicInt(int value = 0) noexcept : QAtomicInteger<int>(value) {

    }
};

template <typename T>
class QAtomicPointer : public QBasicAtomicPointer<T> {
public:
#ifdef QT_BASIC_ATOMIC_HAS_CONSTRUCTORS
    constexpr QAtomicPointer(T *value = nullptr) noexcept : QBasicAtomicPointer<T>(value) {}
#else
    inline QAtomicPointer(T *value = nullptr) noexcept {
        this->storeRelaxed(value);
    }
#endif
    inline QAtomicPointer(const QAtomicPointer<T> &other) noexcept
#ifdef QT_BASIC_ATOMIC_HAS_CONSTRUCTORS
        : QBasicAtomicPointer<T>()
#endif
    {
        this->storeRelease(other.loadAcquire());
    }

    inline QAtomicPointer<T> &operator=(const QAtomicPointer<T> &other) noexcept {
        this->storeRelease(other.loadAcquire());
        return *this;
    }

#ifdef Q_QDOC
    T *load() const;
    T *loadAcquire() const;
    T *loadRelaxed() const;
    void store(T *newValue);
    void storeRelaxed(T *newValue);
    void storeRelease(T *newValue);

    static Q_DECL_CONSTEXPR bool isTestAndSetNative();
    static Q_DECL_CONSTEXPR bool isTestAndSetWaitFree();

    bool testAndSetRelaxed(T *expectedValue, T *newValue);
    bool testAndSetAcquire(T *expectedValue, T *newValue);
    bool testAndSetRelease(T *expectedValue, T *newValue);
    bool testAndSetOrdered(T *expectedValue, T *newValue);

    static Q_DECL_CONSTEXPR bool isFetchAndStoreNative();
    static Q_DECL_CONSTEXPR bool isFetchAndStoreWaitFree();

    T *fetchAndStoreRelaxed(T *newValue);
    T *fetchAndStoreAcquire(T *newValue);
    T *fetchAndStoreRelease(T *newValue);
    T *fetchAndStoreOrdered(T *newValue);

    static Q_DECL_CONSTEXPR bool isFetchAndAddNative();
    static Q_DECL_CONSTEXPR bool isFetchAndAddWaitFree();

    T *fetchAndAddRelaxed(qptrdiff valueToAdd);
    T *fetchAndAddAcquire(qptrdiff valueToAdd);
    T *fetchAndAddRelease(qptrdiff valueToAdd);
    T *fetchAndAddOrdered(qptrdiff valueToAdd);
#endif
};

QT_WARNING_POP

#ifdef QT_BASIC_ATOMIC_HAS_CONSTRUCTORS
#undef QT_BASIC_ATOMIC_HAS_CONSTRUCTORS
#endif

template <typename T>
inline void qAtomicAssign(T *&d, T *x)
{
    if (d == x) {
        return;
    }

    x->ref.ref();
    if (!d->ref.deref()) {
        delete d;
    }

    d = x;
}

template <typename T>
inline void qAtomicDetach(T *&d)
{
    if (d->ref.loadRelaxed() == 1) {
        return;
    }

    T *x = d;
    d = new T(*d);
    if (!x->ref.deref()) {
        delete x;
    }
}

QT_END_NAMESPACE

#endif
