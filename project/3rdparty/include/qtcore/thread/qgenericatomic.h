#ifndef QGENERICATOMIC_H
#define QGENERICATOMIC_H

#include "../global/qglobal.h"
#include "../global/qtypeinfo.h"

QT_BEGIN_NAMESPACE

template<int>
struct QAtomicOpsSupport {
    enum {
        IsSupported = 0
    };
};

template<>
struct QAtomicOpsSupport<4> {
    enum {
        IsSupported = 1
    };
};

template <typename T>
struct QAtomicAdditiveType {
    typedef T AdditiveT;
    static const int AddScale = 1;
};

template <typename T>
struct QAtomicAdditiveType<T *> {
    typedef qptrdiff AdditiveT;
    static const int AddScale = sizeof(T);
};

template <typename BaseClass>
struct QGenericAtomicOps {
    template <typename T>
    struct AtomicType {
        typedef T Type;
        typedef T *PointerType;
    };

    template <typename T>
    static void acquireMemoryFence(const T &_q_value) noexcept {
        BaseClass::orderedMemoryFence(_q_value);
    }

    template <typename T>
    static void releaseMemoryFence(const T &_q_value) noexcept {
        BaseClass::orderedMemoryFence(_q_value);
    }

    template <typename T>
    static void orderedMemoryFence(const T &) noexcept {

    }

    template <typename T>
    static Q_ALWAYS_INLINE T load(const T &_q_value) noexcept {
        return _q_value;
    }

    template <typename T, typename X>
    static Q_ALWAYS_INLINE void store(T &_q_value, X newValue) noexcept {
        _q_value = newValue;
    }

    template <typename T>
    static Q_ALWAYS_INLINE T loadRelaxed(const T &_q_value) noexcept {
        return _q_value;
    }

    template <typename T, typename X>
    static Q_ALWAYS_INLINE void storeRelaxed(T &_q_value, X newValue) noexcept {
        _q_value = newValue;
    }

    template <typename T>
    static Q_ALWAYS_INLINE T loadAcquire(const T &_q_value) noexcept {
        T tmp = *static_cast<const volatile T *>(&_q_value);
        BaseClass::acquireMemoryFence(_q_value);
        return tmp;
    }

    template <typename T, typename X>
    static Q_ALWAYS_INLINE void storeRelease(T &_q_value, X newValue) noexcept {
        BaseClass::releaseMemoryFence(_q_value);
        *static_cast<volatile T *>(&_q_value) = newValue;
    }

    static inline Q_DECL_CONSTEXPR bool isReferenceCountingNative() noexcept {
        return BaseClass::isFetchAndAddNative();
    }

    static inline Q_DECL_CONSTEXPR bool isReferenceCountingWaitFree() noexcept {
        return BaseClass::isFetchAndAddWaitFree();
    }

    template <typename T>
    static Q_ALWAYS_INLINE bool ref(T &_q_value) noexcept {
        return BaseClass::fetchAndAddRelaxed(_q_value, 1) != T(-1);
    }

    template <typename T>
    static Q_ALWAYS_INLINE bool deref(T &_q_value) noexcept {
         return BaseClass::fetchAndAddRelaxed(_q_value, -1) != 1;
    }

    template <typename T, typename X>
    static Q_ALWAYS_INLINE bool testAndSetAcquire(T &_q_value, X expectedValue, X newValue) noexcept {
        bool tmp = BaseClass::testAndSetRelaxed(_q_value, expectedValue, newValue);
        BaseClass::acquireMemoryFence(_q_value);
        return tmp;
    }

    template <typename T, typename X>
    static Q_ALWAYS_INLINE bool testAndSetRelease(T &_q_value, X expectedValue, X newValue) noexcept {
        BaseClass::releaseMemoryFence(_q_value);
        return BaseClass::testAndSetRelaxed(_q_value, expectedValue, newValue);
    }

    template <typename T, typename X>
    static Q_ALWAYS_INLINE bool testAndSetOrdered(T &_q_value, X expectedValue, X newValue) noexcept {
        BaseClass::orderedMemoryFence(_q_value);
        return BaseClass::testAndSetRelaxed(_q_value, expectedValue, newValue);
    }

    template <typename T, typename X>
    static Q_ALWAYS_INLINE bool testAndSetAcquire(T &_q_value, X expectedValue, X newValue, X *currentValue) noexcept {
        bool tmp = BaseClass::testAndSetRelaxed(_q_value, expectedValue, newValue, currentValue);
        BaseClass::acquireMemoryFence(_q_value);
        return tmp;
    }

    template <typename T, typename X>
    static Q_ALWAYS_INLINE bool testAndSetRelease(T &_q_value, X expectedValue, X newValue, X *currentValue) noexcept {
        BaseClass::releaseMemoryFence(_q_value);
        return BaseClass::testAndSetRelaxed(_q_value, expectedValue, newValue, currentValue);
    }

    template <typename T, typename X>
    static Q_ALWAYS_INLINE bool testAndSetOrdered(T &_q_value, X expectedValue, X newValue, X *currentValue) noexcept {
        BaseClass::orderedMemoryFence(_q_value);
        return BaseClass::testAndSetRelaxed(_q_value, expectedValue, newValue, currentValue);
    }

    static inline Q_DECL_CONSTEXPR bool isFetchAndStoreNative() noexcept {
        return false;
    }

    static inline Q_DECL_CONSTEXPR bool isFetchAndStoreWaitFree() noexcept {
        return false;
    }

    template <typename T, typename X>
    static Q_ALWAYS_INLINE T fetchAndStoreRelaxed(T &_q_value, X newValue) noexcept {
        Q_FOREVER {
            T tmp = loadRelaxed(_q_value);
            if (BaseClass::testAndSetRelaxed(_q_value, tmp, newValue)) {
                return tmp;
            }
        }
    }

    template <typename T, typename X>
    static Q_ALWAYS_INLINE T fetchAndStoreAcquire(T &_q_value, X newValue) noexcept {
        T tmp = BaseClass::fetchAndStoreRelaxed(_q_value, newValue);
        BaseClass::acquireMemoryFence(_q_value);
        return tmp;
    }

    template <typename T, typename X>
    static Q_ALWAYS_INLINE T fetchAndStoreRelease(T &_q_value, X newValue) noexcept {
        BaseClass::releaseMemoryFence(_q_value);
        return BaseClass::fetchAndStoreRelaxed(_q_value, newValue);
    }

    template <typename T, typename X>
    static Q_ALWAYS_INLINE T fetchAndStoreOrdered(T &_q_value, X newValue) noexcept {
        BaseClass::orderedMemoryFence(_q_value);
        return BaseClass::fetchAndStoreRelaxed(_q_value, newValue);
    }

    static inline Q_DECL_CONSTEXPR bool isFetchAndAddNative() noexcept {
        return false;
    }

    static inline Q_DECL_CONSTEXPR bool isFetchAndAddWaitFree() noexcept {
        return false;
    }

    template <typename T>
    static Q_ALWAYS_INLINE T fetchAndAddRelaxed(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd) noexcept {
        Q_FOREVER {
            T tmp = BaseClass::loadRelaxed(_q_value);
            if (BaseClass::testAndSetRelaxed(_q_value, tmp, T(tmp + valueToAdd))) {
                return tmp;
            }
        }
    }

    template <typename T>
    static Q_ALWAYS_INLINE T fetchAndAddAcquire(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd) noexcept {
        T tmp = BaseClass::fetchAndAddRelaxed(_q_value, valueToAdd);
        BaseClass::acquireMemoryFence(_q_value);
        return tmp;
    }

    template <typename T>
    static Q_ALWAYS_INLINE T fetchAndAddRelease(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd) noexcept {
        BaseClass::releaseMemoryFence(_q_value);
        return BaseClass::fetchAndAddRelaxed(_q_value, valueToAdd);
    }

    template <typename T>
    static Q_ALWAYS_INLINE T fetchAndAddOrdered(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT valueToAdd) noexcept {
        BaseClass::orderedMemoryFence(_q_value);
        return BaseClass::fetchAndAddRelaxed(_q_value, valueToAdd);
    }

QT_WARNING_PUSH
QT_WARNING_DISABLE_MSVC(4146)
    template <typename T>
    static Q_ALWAYS_INLINE T fetchAndSubRelaxed(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT operand) noexcept {
        return fetchAndAddRelaxed(_q_value, -operand);
    }
QT_WARNING_POP

    template <typename T>
    static Q_ALWAYS_INLINE T fetchAndSubAcquire(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT operand) noexcept {
        T tmp = BaseClass::fetchAndSubRelaxed(_q_value, operand);
        BaseClass::acquireMemoryFence(_q_value);
        return tmp;
    }

    template <typename T>
    static Q_ALWAYS_INLINE T fetchAndSubRelease(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT operand) noexcept {
        BaseClass::releaseMemoryFence(_q_value);
        return BaseClass::fetchAndSubRelaxed(_q_value, operand);
    }

    template <typename T>
    static Q_ALWAYS_INLINE T fetchAndSubOrdered(T &_q_value, typename QAtomicAdditiveType<T>::AdditiveT operand) noexcept {
        BaseClass::orderedMemoryFence(_q_value);
        return BaseClass::fetchAndSubRelaxed(_q_value, operand);
    }

    template <typename T>
    static Q_ALWAYS_INLINE T fetchAndAndRelaxed(T &_q_value, typename std::enable_if<QTypeInfo<T>::isIntegral, T>::type operand) noexcept {
        T tmp = BaseClass::loadRelaxed(_q_value);
        Q_FOREVER {
            if (BaseClass::testAndSetRelaxed(_q_value, tmp, T(tmp & operand), &tmp)) {
                return tmp;
            }
        }
    }

    template <typename T>
    static Q_ALWAYS_INLINE T fetchAndAndAcquire(T &_q_value, typename std::enable_if<QTypeInfo<T>::isIntegral, T>::type operand) noexcept {
        T tmp = BaseClass::fetchAndAndRelaxed(_q_value, operand);
        BaseClass::acquireMemoryFence(_q_value);
        return tmp;
    }

    template <typename T>
    static Q_ALWAYS_INLINE T fetchAndAndRelease(T &_q_value, typename std::enable_if<QTypeInfo<T>::isIntegral, T>::type operand) noexcept {
        BaseClass::releaseMemoryFence(_q_value);
        return BaseClass::fetchAndAndRelaxed(_q_value, operand);
    }

    template <typename T>
    static Q_ALWAYS_INLINE T fetchAndAndOrdered(T &_q_value, typename std::enable_if<QTypeInfo<T>::isIntegral, T>::type operand) noexcept {
        BaseClass::orderedMemoryFence(_q_value);
        return BaseClass::fetchAndAndRelaxed(_q_value, operand);
    }

    template <typename T>
    static Q_ALWAYS_INLINE T fetchAndOrRelaxed(T &_q_value, typename std::enable_if<QTypeInfo<T>::isIntegral, T>::type operand) noexcept {
        T tmp = BaseClass::loadRelaxed(_q_value);
        Q_FOREVER {
            if (BaseClass::testAndSetRelaxed(_q_value, tmp, T(tmp | operand), &tmp)) {
                return tmp;
            }
        }
    }

    template <typename T>
    static Q_ALWAYS_INLINE T fetchAndOrAcquire(T &_q_value, typename std::enable_if<QTypeInfo<T>::isIntegral, T>::type operand) noexcept {
        T tmp = BaseClass::fetchAndOrRelaxed(_q_value, operand);
        BaseClass::acquireMemoryFence(_q_value);
        return tmp;
    }

    template <typename T>
    static Q_ALWAYS_INLINE T fetchAndOrRelease(T &_q_value, typename std::enable_if<QTypeInfo<T>::isIntegral, T>::type operand) noexcept {
        BaseClass::releaseMemoryFence(_q_value);
        return BaseClass::fetchAndOrRelaxed(_q_value, operand);
    }

    template <typename T>
    static Q_ALWAYS_INLINE T fetchAndOrOrdered(T &_q_value, typename std::enable_if<QTypeInfo<T>::isIntegral, T>::type operand) noexcept {
        BaseClass::orderedMemoryFence(_q_value);
        return BaseClass::fetchAndOrRelaxed(_q_value, operand);
    }

    template <typename T>
    static Q_ALWAYS_INLINE T fetchAndXorRelaxed(T &_q_value, typename std::enable_if<QTypeInfo<T>::isIntegral, T>::type operand) noexcept {
        T tmp = BaseClass::loadRelaxed(_q_value);
        Q_FOREVER {
            if (BaseClass::testAndSetRelaxed(_q_value, tmp, T(tmp ^ operand), &tmp)) {
                return tmp;
            }
        }
    }

    template <typename T>
    static Q_ALWAYS_INLINE T fetchAndXorAcquire(T &_q_value, typename std::enable_if<QTypeInfo<T>::isIntegral, T>::type operand) noexcept {
        T tmp = BaseClass::fetchAndXorRelaxed(_q_value, operand);
        BaseClass::acquireMemoryFence(_q_value);
        return tmp;
    }

    template <typename T>
    static Q_ALWAYS_INLINE T fetchAndXorRelease(T &_q_value, typename std::enable_if<QTypeInfo<T>::isIntegral, T>::type operand) noexcept {
        BaseClass::releaseMemoryFence(_q_value);
        return BaseClass::fetchAndXorRelaxed(_q_value, operand);
    }

    template <typename T>
    static Q_ALWAYS_INLINE T fetchAndXorOrdered(T &_q_value, typename std::enable_if<QTypeInfo<T>::isIntegral, T>::type operand) noexcept {
        BaseClass::orderedMemoryFence(_q_value);
        return BaseClass::fetchAndXorRelaxed(_q_value, operand);
    }
};

QT_END_NAMESPACE

#endif
