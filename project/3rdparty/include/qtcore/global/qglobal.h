#ifndef QGLOBAL_H
#define QGLOBAL_H

#ifdef __cplusplus
#include <cstddef>
#include <utility>
#include <type_traits>
#endif

#ifndef __ASSEMBLER__
#include <assert.h>
#include <stddef.h>
#endif

#define QT_VERSION                                  QT_VERSION_CHECK(QT_VERSION_MAJOR, QT_VERSION_MINOR, QT_VERSION_PATCH)
#define QT_VERSION_CHECK(major, minor, patch)       ((major<<16)|(minor<<8)|(patch))

#include "qconfig.h"
#include "qtcore-config.h"

#define QT_SUPPORTS(FEATURE)                        (!defined(QT_NO_##FEATURE))

#define QT_CONFIG(feature)                          (1/QT_FEATURE_##feature == 1)
#define QT_REQUIRE_CONFIG(feature)                  Q_STATIC_ASSERT_X(QT_FEATURE_##feature == 1, "Required feature " #feature " for file " __FILE__ " not available.")

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#define QT_NO_UNSHARABLE_CONTAINERS
#define QT6_VIRTUAL                                 virtual
#define QT6_NOT_VIRTUAL
#else
#define QT6_VIRTUAL
#define QT6_NOT_VIRTUAL                             virtual
#endif

#define QT_STRINGIFY2(x)                            #x
#define QT_STRINGIFY(x)                             QT_STRINGIFY2(x)

#include "qsystemdetection.h"
#include "qprocessordetection.h"
#include "qcompilerdetection.h"

#if defined(__ELF__)
#define Q_OF_ELF
#endif

#define Q_UNUSED(x)                                 (void)x;

#if defined(__cplusplus) && defined(Q_COMPILER_STATIC_ASSERT)
#define Q_STATIC_ASSERT(Condition)                  static_assert(bool(Condition), #Condition)
#define Q_STATIC_ASSERT_X(Condition, Message)       static_assert(bool(Condition), Message)
#elif defined(Q_COMPILER_STATIC_ASSERT)
#define Q_STATIC_ASSERT(Condition)                  _Static_assert(!!(Condition), #Condition)
#define Q_STATIC_ASSERT_X(Condition, Message)       _Static_assert(!!(Condition), Message)
#else
#define Q_STATIC_ASSERT_PRIVATE_JOIN(A, B)          Q_STATIC_ASSERT_PRIVATE_JOIN_IMPL(A, B)
#define Q_STATIC_ASSERT_PRIVATE_JOIN_IMPL(A, B)     A ## B
#ifdef __COUNTER__
#define Q_STATIC_ASSERT(Condition)                  typedef char Q_STATIC_ASSERT_PRIVATE_JOIN(q_static_assert_result, __COUNTER__) [(Condition) ? 1 : -1];
#else
#define Q_STATIC_ASSERT(Condition)                  typedef char Q_STATIC_ASSERT_PRIVATE_JOIN(q_static_assert_result, __LINE__) [(Condition) ? 1 : -1];
#endif
#define Q_STATIC_ASSERT_X(Condition, Message)       Q_STATIC_ASSERT(Condition)
#endif

#ifdef __cplusplus

#include <algorithm>

#if !defined(QT_NAMESPACE) || defined(Q_MOC_RUN)

#define QT_PREPEND_NAMESPACE(name)                  ::name
#define QT_USE_NAMESPACE
#define QT_BEGIN_NAMESPACE
#define QT_END_NAMESPACE
#define QT_BEGIN_INCLUDE_NAMESPACE
#define QT_END_INCLUDE_NAMESPACE

#ifndef QT_BEGIN_MOC_NAMESPACE
#define QT_BEGIN_MOC_NAMESPACE
#endif

#ifndef QT_END_MOC_NAMESPACE
#define QT_END_MOC_NAMESPACE
#endif

#define QT_FORWARD_DECLARE_CLASS(name)              class name;
#define QT_FORWARD_DECLARE_STRUCT(name)             struct name;
#define QT_MANGLE_NAMESPACE(name)                   name

#else

#define QT_PREPEND_NAMESPACE(name)                  ::QT_NAMESPACE::name
#define QT_USE_NAMESPACE                            using namespace ::QT_NAMESPACE;
#define QT_BEGIN_NAMESPACE                          namespace QT_NAMESPACE {
#define QT_END_NAMESPACE                            }
#define QT_BEGIN_INCLUDE_NAMESPACE                  }
#define QT_END_INCLUDE_NAMESPACE                    namespace QT_NAMESPACE {

#ifndef QT_BEGIN_MOC_NAMESPACE
#define QT_BEGIN_MOC_NAMESPACE                      QT_USE_NAMESPACE
#endif

#ifndef QT_END_MOC_NAMESPACE
#define QT_END_MOC_NAMESPACE
#endif

#define QT_FORWARD_DECLARE_CLASS(name)                  \
    QT_BEGIN_NAMESPACE class name; QT_END_NAMESPACE     \
    using QT_PREPEND_NAMESPACE(name);

#define QT_FORWARD_DECLARE_STRUCT(name)                 \
    QT_BEGIN_NAMESPACE struct name; QT_END_NAMESPACE    \
    using QT_PREPEND_NAMESPACE(name);

#define QT_MANGLE_NAMESPACE0(x) x
#define QT_MANGLE_NAMESPACE1(a, b)                  a##_##b
#define QT_MANGLE_NAMESPACE2(a, b)                  QT_MANGLE_NAMESPACE1(a, b)
#define QT_MANGLE_NAMESPACE(name)                   QT_MANGLE_NAMESPACE2(QT_MANGLE_NAMESPACE0(name), QT_MANGLE_NAMESPACE0(QT_NAMESPACE))

namespace QT_NAMESPACE {

}

#ifndef QT_BOOTSTRAPPED
#ifndef QT_NO_USING_NAMESPACE
QT_USE_NAMESPACE
#endif
#endif

#endif

#else

#define QT_BEGIN_NAMESPACE
#define QT_END_NAMESPACE
#define QT_USE_NAMESPACE
#define QT_BEGIN_INCLUDE_NAMESPACE
#define QT_END_INCLUDE_NAMESPACE

#endif

#define QT_BEGIN_HEADER
#define QT_END_HEADER

#ifndef __ASSEMBLER__

QT_BEGIN_NAMESPACE

typedef signed char qint8;
typedef unsigned char quint8;

typedef short qint16;
typedef unsigned short quint16;

typedef int qint32;
typedef unsigned int quint32;

#ifdef __cplusplus
#define Q_INT64_C(c)                                static_cast<long long>(c ## LL)
#define Q_UINT64_C(c)                               static_cast<unsigned long long>(c ## ULL)
#else
#define Q_INT64_C(c)                                ((long long)(c ## LL))
#define Q_UINT64_C(c)                               ((unsigned long long)(c ## ULL))
#endif

typedef long long qint64;
typedef unsigned long long quint64;

typedef qint64 qlonglong;
typedef quint64 qulonglong;

#ifndef __cplusplus
Q_STATIC_ASSERT_X(sizeof(ptrdiff_t) == sizeof(size_t), "Weird ptrdiff_t and size_t definitions");

typedef ptrdiff_t qptrdiff;
typedef ptrdiff_t qsizetype;
typedef ptrdiff_t qintptr;
typedef size_t quintptr;
#endif

QT_BEGIN_INCLUDE_NAMESPACE
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
QT_END_INCLUDE_NAMESPACE

#if defined(QT_COORD_TYPE)
typedef QT_COORD_TYPE qreal;
#else
typedef double qreal;
#endif

#if defined(QT_NO_DEPRECATED)
#undef QT_DEPRECATED
#undef QT_DEPRECATED_X
#undef QT_DEPRECATED_VARIABLE
#undef QT_DEPRECATED_CONSTRUCTOR
#elif !defined(QT_NO_DEPRECATED_WARNINGS)
#undef QT_DEPRECATED
#define QT_DEPRECATED                               Q_DECL_DEPRECATED

#undef QT_DEPRECATED_X
#define QT_DEPRECATED_X(text)                       Q_DECL_DEPRECATED_X(text)

#undef QT_DEPRECATED_VARIABLE
#define QT_DEPRECATED_VARIABLE                      Q_DECL_VARIABLE_DEPRECATED

#undef QT_DEPRECATED_CONSTRUCTOR
#define QT_DEPRECATED_CONSTRUCTOR                   explicit Q_DECL_CONSTRUCTOR_DEPRECATED
#else
#undef QT_DEPRECATED
#define QT_DEPRECATED
#undef QT_DEPRECATED_X
#define QT_DEPRECATED_X(text)
#undef QT_DEPRECATED_VARIABLE
#define QT_DEPRECATED_VARIABLE
#undef QT_DEPRECATED_CONSTRUCTOR
#define QT_DEPRECATED_CONSTRUCTOR
#undef Q_DECL_ENUMERATOR_DEPRECATED
#define Q_DECL_ENUMERATOR_DEPRECATED
#endif

#ifndef QT_DEPRECATED_WARNINGS_SINCE
#ifdef QT_DISABLE_DEPRECATED_BEFORE
#define QT_DEPRECATED_WARNINGS_SINCE                QT_DISABLE_DEPRECATED_BEFORE
#else
#define QT_DEPRECATED_WARNINGS_SINCE                QT_VERSION
#endif
#endif

#ifndef QT_DISABLE_DEPRECATED_BEFORE
#define QT_DISABLE_DEPRECATED_BEFORE                QT_VERSION_CHECK(5, 0, 0)
#endif

#ifdef QT_DEPRECATED
#define QT_DEPRECATED_SINCE(major, minor)           (QT_VERSION_CHECK(major, minor, 0) > QT_DISABLE_DEPRECATED_BEFORE)
#else
#define QT_DEPRECATED_SINCE(major, minor)           0
#endif

#if QT_DEPRECATED_WARNINGS_SINCE >= QT_VERSION_CHECK(5, 12, 0)
#define QT_DEPRECATED_VERSION_X_5_12(text)          QT_DEPRECATED_X(text)
#define QT_DEPRECATED_VERSION_5_12                  QT_DEPRECATED
#else
#define QT_DEPRECATED_VERSION_X_5_12(text)
#define QT_DEPRECATED_VERSION_5_12
#endif

#if QT_DEPRECATED_WARNINGS_SINCE >= QT_VERSION_CHECK(5, 13, 0)
#define QT_DEPRECATED_VERSION_X_5_13(text)          QT_DEPRECATED_X(text)
#define QT_DEPRECATED_VERSION_5_13                  QT_DEPRECATED
#else
#define QT_DEPRECATED_VERSION_X_5_13(text)
#define QT_DEPRECATED_VERSION_5_13
#endif

#if QT_DEPRECATED_WARNINGS_SINCE >= QT_VERSION_CHECK(5, 14, 0)
#define QT_DEPRECATED_VERSION_X_5_14(text)          QT_DEPRECATED_X(text)
#define QT_DEPRECATED_VERSION_5_14                  QT_DEPRECATED
#else
#define QT_DEPRECATED_VERSION_X_5_14(text)
#define QT_DEPRECATED_VERSION_5_14
#endif

#if QT_DEPRECATED_WARNINGS_SINCE >= QT_VERSION_CHECK(5, 15, 0)
#define QT_DEPRECATED_VERSION_X_5_15(text)          QT_DEPRECATED_X(text)
#define QT_DEPRECATED_VERSION_5_15                  QT_DEPRECATED
#else
#define QT_DEPRECATED_VERSION_X_5_15(text)
#define QT_DEPRECATED_VERSION_5_15
#endif

#if QT_DEPRECATED_WARNINGS_SINCE >= QT_VERSION_CHECK(6, 0, 0)
#define QT_DEPRECATED_VERSION_X_6_0(text)           QT_DEPRECATED_X(text)
#define QT_DEPRECATED_VERSION_6_0                   QT_DEPRECATED
#else
#define QT_DEPRECATED_VERSION_X_6_0(text)
#define QT_DEPRECATED_VERSION_6_0
#endif

#define QT_DEPRECATED_VERSION_X_5(minor, text)      QT_DEPRECATED_VERSION_X_5_##minor(text)
#define QT_DEPRECATED_VERSION_X(major, minor, text) QT_DEPRECATED_VERSION_X_##major##_##minor(text)

#define QT_DEPRECATED_VERSION_5(minor)              QT_DEPRECATED_VERSION_5_##minor
#define QT_DEPRECATED_VERSION(major, minor)         QT_DEPRECATED_VERSION_##major##_##minor

#ifdef __cplusplus
namespace QtPrivate {
enum class Deprecated_t {};
constexpr Q_DECL_UNUSED Deprecated_t Deprecated = {};
}
#endif

#if defined(QT_SHARED)
#ifndef QT_SHARED
#define QT_SHARED
#endif

#if defined(QT_BUILD_CORE_LIB)
#define Q_CORE_EXPORT                               Q_DECL_EXPORT
#else
#define Q_CORE_EXPORT                               Q_DECL_IMPORT
#endif
#else
#define Q_CORE_EXPORT
#endif

#define Q_DISABLE_COPY(Class)                       \
    Class(const Class &) = delete;                  \
    Class &operator=(const Class &) = delete;

#define Q_DISABLE_MOVE(Class)                       \
    Class(Class &&) = delete;                       \
    Class &operator=(Class &&) = delete;

#define Q_DISABLE_COPY_MOVE(Class)                  \
    Q_DISABLE_COPY(Class)                           \
    Q_DISABLE_MOVE(Class)

#if defined(QT_BUILD_INTERNAL) && defined(QT_BUILDING_QT) && defined(QT_SHARED)
#define Q_AUTOTEST_EXPORT                           Q_DECL_EXPORT
#elif defined(QT_BUILD_INTERNAL) && defined(QT_SHARED)
#define Q_AUTOTEST_EXPORT                           Q_DECL_IMPORT
#else
#define Q_AUTOTEST_EXPORT
#endif

#define Q_INIT_RESOURCE(name)                                           \
    do {                                                                \
        extern int QT_MANGLE_NAMESPACE(qInitResources_ ## name)();      \
        QT_MANGLE_NAMESPACE(qInitResources_ ## name)();                 \
    } while (false)

#define Q_CLEANUP_RESOURCE(name)                                        \
    do {                                                                \
        extern int QT_MANGLE_NAMESPACE(qCleanupResources_ ## name)();   \
        QT_MANGLE_NAMESPACE(qCleanupResources_ ## name)();              \
    } while (false)

#if !defined(QT_NAMESPACE) && defined(__cplusplus) && !defined(Q_QDOC)
extern "C"
#endif

Q_CORE_EXPORT Q_DECL_CONST_FUNCTION const char *qVersion(void) Q_DECL_NOEXCEPT;

#if defined(__cplusplus)

#ifndef Q_CONSTRUCTOR_FUNCTION
#define Q_CONSTRUCTOR_FUNCTION0(AFUNC)                                  \
    namespace {                                                         \
        static const struct AFUNC ## _ctor_class_ {                     \
            inline AFUNC ## _ctor_class_() { AFUNC(); }                 \
        } AFUNC ## _ctor_instance_;                                     \
    }

#define Q_CONSTRUCTOR_FUNCTION(AFUNC)               Q_CONSTRUCTOR_FUNCTION0(AFUNC)
#endif

#ifndef Q_DESTRUCTOR_FUNCTION
#define Q_DESTRUCTOR_FUNCTION0(AFUNC)                                   \
    namespace {                                                         \
        static const struct AFUNC ## _dtor_class_ {                     \
            inline AFUNC ## _dtor_class_() { }                          \
            inline ~ AFUNC ## _dtor_class_() { AFUNC(); }               \
        } AFUNC ## _dtor_instance_;                                     \
    }

#define Q_DESTRUCTOR_FUNCTION(AFUNC)                Q_DESTRUCTOR_FUNCTION0(AFUNC)
#endif

namespace QtPrivate {
    template <class T>
    struct AlignOfHelper {
        char c;
        T type;

        AlignOfHelper();
        ~AlignOfHelper();
    };

    template <class T>
    struct AlignOf_Default {
        enum { Value = sizeof(AlignOfHelper<T>) - sizeof(T) };
    };

    template <class T> struct AlignOf : AlignOf_Default<T> { };
    template <class T> struct AlignOf<T &> : AlignOf<T> {};
    template <class T> struct AlignOf<T &&> : AlignOf<T> {};
    template <size_t N, class T> struct AlignOf<T[N]> : AlignOf<T> {};

#if defined(Q_PROCESSOR_X86_32) && !defined(Q_OS_WIN)

    template <class T> struct AlignOf_WorkaroundForI386Abi { enum { Value = sizeof(T) }; };

    template <> struct AlignOf<double>  : AlignOf_WorkaroundForI386Abi<double> {};
    template <> struct AlignOf<qint64>  : AlignOf_WorkaroundForI386Abi<qint64> {};
    template <> struct AlignOf<quint64> : AlignOf_WorkaroundForI386Abi<quint64> {};
#ifdef Q_CC_CLANG
    template <size_t N> struct AlignOf<double[N]>   : AlignOf_Default<double> {};
    template <size_t N> struct AlignOf<qint64[N]>   : AlignOf_Default<qint64> {};
    template <size_t N> struct AlignOf<quint64[N]>  : AlignOf_Default<quint64> {};
#endif
#endif
}

#define QT_EMULATED_ALIGNOF(T) \
    (size_t(QT_PREPEND_NAMESPACE(QtPrivate)::AlignOf<T>::Value))

#ifndef Q_ALIGNOF
#define Q_ALIGNOF(T)                                QT_EMULATED_ALIGNOF(T)
#endif

template <int> struct QIntegerForSize;
template <>    struct QIntegerForSize<1> { typedef quint8  Unsigned; typedef qint8  Signed; };
template <>    struct QIntegerForSize<2> { typedef quint16 Unsigned; typedef qint16 Signed; };
template <>    struct QIntegerForSize<4> { typedef quint32 Unsigned; typedef qint32 Signed; };
template <>    struct QIntegerForSize<8> { typedef quint64 Unsigned; typedef qint64 Signed; };

#if defined(Q_CC_GNU) && defined(__SIZEOF_INT128__)
template <>    struct QIntegerForSize<16> { __extension__ typedef unsigned __int128 Unsigned; __extension__ typedef __int128 Signed; };
#endif

template <class T> struct QIntegerForSizeof: QIntegerForSize<sizeof(T)> { };
typedef QIntegerForSize<Q_PROCESSOR_WORDSIZE>::Signed qregisterint;
typedef QIntegerForSize<Q_PROCESSOR_WORDSIZE>::Unsigned qregisteruint;
typedef QIntegerForSizeof<void*>::Unsigned quintptr;
typedef QIntegerForSizeof<void*>::Signed qptrdiff;
typedef qptrdiff qintptr;
using qsizetype = QIntegerForSizeof<std::size_t>::Signed;

#ifndef QT_MOC_COMPAT
#define QT_MOC_COMPAT
#else
#undef QT_MOC_COMPAT
#define QT_MOC_COMPAT
#endif

#ifdef QT_ASCII_CAST_WARNINGS
#define QT_ASCII_CAST_WARN                          Q_DECL_DEPRECATED_X("Use fromUtf8, QStringLiteral, or QLatin1String")
#else
#define QT_ASCII_CAST_WARN
#endif

#ifdef Q_PROCESSOR_X86_32
#if defined(Q_CC_GNU)
#define QT_FASTCALL                             __attribute__((regparm(3)))
#elif defined(Q_CC_MSVC)
#define QT_FASTCALL                             __fastcall
#else
#define QT_FASTCALL
#endif
#else
#define QT_FASTCALL
#endif

#if defined(Q_CC_GNU) && !defined(__INSURE__)
#if defined(Q_CC_MINGW) && !defined(Q_CC_CLANG)
#define Q_ATTRIBUTE_FORMAT_PRINTF(A, B)         __attribute__((format(gnu_printf, (A), (B))))
#else
#define Q_ATTRIBUTE_FORMAT_PRINTF(A, B)         __attribute__((format(printf, (A), (B))))
#endif
#else
#define Q_ATTRIBUTE_FORMAT_PRINTF(A, B)
#endif

#if defined(Q_CC_GNU)
#define Q_NEVER_INLINE                          __attribute__((noinline))
#define Q_ALWAYS_INLINE                         inline __attribute__((always_inline))
#else
#define Q_NEVER_INLINE
#define Q_ALWAYS_INLINE                         inline
#endif

#if defined(Q_CC_GNU) && defined(Q_OS_WIN) && !defined(QT_NO_DATA_RELOCATION)
#define QT_INIT_METAOBJECT                      __attribute__((init_priority(101)))
#else
#define QT_INIT_METAOBJECT
#endif

typedef int QNoImplicitBoolCast;

template <typename T>
Q_DECL_CONSTEXPR inline T qAbs(const T &t)
{
    return t >= 0 ? t : -t;
}

Q_DECL_CONSTEXPR inline int qRound(double d)
{
    return d >= 0.0 ? int(d + 0.5) : int(d - double(int(d-1)) + 0.5) + int(d-1);
}

Q_DECL_CONSTEXPR inline int qRound(float d)
{
    return d >= 0.0f ? int(d + 0.5f) : int(d - float(int(d-1)) + 0.5f) + int(d-1);
}

Q_DECL_CONSTEXPR inline qint64 qRound64(double d)
{
    return d >= 0.0 ? qint64(d + 0.5) : qint64(d - double(qint64(d-1)) + 0.5) + qint64(d-1);

}

Q_DECL_CONSTEXPR inline qint64 qRound64(float d)
{
    return d >= 0.0f ? qint64(d + 0.5f) : qint64(d - float(qint64(d-1)) + 0.5f) + qint64(d-1);
}

template <typename T>
constexpr inline const T &qMin(const T &a, const T &b) { return (a < b) ? a : b; }
template <typename T>
constexpr inline const T &qMax(const T &a, const T &b) { return (a < b) ? b : a; }
template <typename T>
constexpr inline const T &qBound(const T &min, const T &val, const T &max) { return qMax(min, qMin(max, val)); }

#ifndef Q_FORWARD_DECLARE_OBJC_CLASS
#ifdef __OBJC__
#define Q_FORWARD_DECLARE_OBJC_CLASS(classname) @class classname
#else
#define Q_FORWARD_DECLARE_OBJC_CLASS(classname) typedef struct objc_object classname
#endif
#endif

#ifndef Q_FORWARD_DECLARE_CF_TYPE
#define Q_FORWARD_DECLARE_CF_TYPE(type)         typedef const struct __ ## type * type ## Ref
#endif

#ifndef Q_FORWARD_DECLARE_MUTABLE_CF_TYPE
#define Q_FORWARD_DECLARE_MUTABLE_CF_TYPE(type) typedef struct __ ## type * type ## Ref
#endif

#ifndef Q_FORWARD_DECLARE_CG_TYPE
#define Q_FORWARD_DECLARE_CG_TYPE(type)         typedef const struct type *type ## Ref;
#endif

#ifndef Q_FORWARD_DECLARE_MUTABLE_CG_TYPE
#define Q_FORWARD_DECLARE_MUTABLE_CG_TYPE(type) typedef struct type *type ## Ref;
#endif

#define QT_DARWIN_PLATFORM_SDK_EQUAL_OR_ABOVE(macos, ios, tvos, watchos)    (0)
#define QT_MACOS_IOS_PLATFORM_SDK_EQUAL_OR_ABOVE(macos, ios)                (0)
#define QT_MACOS_PLATFORM_SDK_EQUAL_OR_ABOVE(macos)                         (0)
#define QT_IOS_PLATFORM_SDK_EQUAL_OR_ABOVE(ios)                             (0)
#define QT_TVOS_PLATFORM_SDK_EQUAL_OR_ABOVE(tvos)                           (0)
#define QT_WATCHOS_PLATFORM_SDK_EQUAL_OR_ABOVE(watchos)                     (0)

#define QT_MAC_PLATFORM_SDK_EQUAL_OR_ABOVE(osx, ios)                        (0)
#define QT_OSX_PLATFORM_SDK_EQUAL_OR_ABOVE(osx)                             (0)

class QDataStream;

inline void qt_noop(void) {}

#if !defined(QT_NO_EXCEPTIONS)
#if !defined(Q_MOC_RUN)
#if (defined(Q_CC_CLANG) && !defined(Q_CC_INTEL) && !__has_feature(cxx_exceptions)) || (defined(Q_CC_GNU) && !defined(__EXCEPTIONS))
#define QT_NO_EXCEPTIONS
#endif
#elif defined(QT_BOOTSTRAPPED)
#define QT_NO_EXCEPTIONS
#endif
#endif

#ifdef QT_NO_EXCEPTIONS
#define QT_TRY                                      if (true)
#define QT_CATCH(A)                                 else
#define QT_THROW(A)                                 qt_noop()
#define QT_RETHROW                                  qt_noop()
#define QT_TERMINATE_ON_EXCEPTION(expr)             do { expr; } while (false)
#else
#define QT_TRY                                      try
#define QT_CATCH(A)                                 catch (A)
#define QT_THROW(A)                                 throw A
#define QT_RETHROW                                  throw

Q_NORETURN Q_DECL_COLD_FUNCTION Q_CORE_EXPORT void qTerminate() noexcept;
#ifdef Q_COMPILER_NOEXCEPT
#define QT_TERMINATE_ON_EXCEPTION(expr)             do { expr; } while (false)
#else
#define QT_TERMINATE_ON_EXCEPTION(expr)             do { try { expr; } catch (...) { qTerminate(); } } while (false)
#endif
#endif

Q_CORE_EXPORT Q_DECL_CONST_FUNCTION bool qSharedBuild() noexcept;

#ifndef Q_OUTOFLINE_TEMPLATE
#define Q_OUTOFLINE_TEMPLATE
#endif

#ifndef Q_INLINE_TEMPLATE
#define Q_INLINE_TEMPLATE                           inline
#endif

#if !defined(QT_NO_DEBUG) && !defined(QT_DEBUG)
#define QT_DEBUG
#endif

#ifndef qPrintable
#define qPrintable(string)                          QtPrivate::asString(string).toLocal8Bit().constData()
#endif

#ifndef qUtf8Printable
#define qUtf8Printable(string)                      QtPrivate::asString(string).toUtf8().constData()
#endif

#ifndef qUtf16Printable
#define qUtf16Printable(string)                     static_cast<const wchar_t*>(static_cast<const void*>(QString(string).utf16()))
#endif

class QString;
Q_DECL_COLD_FUNCTION
Q_CORE_EXPORT QString qt_error_string(int errorCode = -1);

#ifndef Q_CC_MSVC
Q_NORETURN
#endif
Q_DECL_COLD_FUNCTION
Q_CORE_EXPORT void qt_assert(const char *assertion, const char *file, int line) noexcept;

#if !defined(Q_ASSERT)
#if defined(QT_NO_DEBUG) && !defined(QT_FORCE_ASSERTS)
#define Q_ASSERT(cond)                              static_cast<void>(false && (cond))
#else
#define Q_ASSERT(cond)                              ((cond) ? static_cast<void>(0) : qt_assert(#cond, __FILE__, __LINE__))
#endif
#endif

#ifndef Q_CC_MSVC
Q_NORETURN
#endif
Q_DECL_COLD_FUNCTION
Q_CORE_EXPORT void qt_assert_x(const char *where, const char *what, const char *file, int line) noexcept;

#if !defined(Q_ASSERT_X)
#if defined(QT_NO_DEBUG) && !defined(QT_FORCE_ASSERTS)
#define Q_ASSERT_X(cond, where, what)               static_cast<void>(false && (cond))
#else
#define Q_ASSERT_X(cond, where, what)               ((cond) ? static_cast<void>(0) : qt_assert_x(where, what, __FILE__, __LINE__))
#endif
#endif

Q_NORETURN Q_CORE_EXPORT void qt_check_pointer(const char *, int) noexcept;

Q_DECL_COLD_FUNCTION
Q_CORE_EXPORT void qBadAlloc();

#ifdef QT_NO_EXCEPTIONS
#if defined(QT_NO_DEBUG) && !defined(QT_FORCE_ASSERTS)
#define Q_CHECK_PTR(p)                              qt_noop()
#else
#define Q_CHECK_PTR(p)                              do {if (!(p)) qt_check_pointer(__FILE__,__LINE__);} while (false)
#endif
#else
#define Q_CHECK_PTR(p)                              do { if (!(p)) qBadAlloc(); } while (false)
#endif

template <typename T>
inline T *q_check_ptr(T *p) { Q_CHECK_PTR(p); return p; }

typedef void (*QFunctionPointer)();

#if !defined(Q_UNIMPLEMENTED)
#define Q_UNIMPLEMENTED()                           qWarning("Unimplemented code.")
#endif

Q_REQUIRED_RESULT Q_DECL_CONSTEXPR static inline Q_DECL_UNUSED bool qFuzzyCompare(double p1, double p2)
{
    return (qAbs(p1 - p2) * 1000000000000. <= qMin(qAbs(p1), qAbs(p2)));
}

Q_REQUIRED_RESULT Q_DECL_CONSTEXPR static inline Q_DECL_UNUSED bool qFuzzyCompare(float p1, float p2)
{
    return (qAbs(p1 - p2) * 100000.f <= qMin(qAbs(p1), qAbs(p2)));
}

Q_REQUIRED_RESULT Q_DECL_CONSTEXPR static inline Q_DECL_UNUSED bool qFuzzyIsNull(double d)
{
    return qAbs(d) <= 0.000000000001;
}

Q_REQUIRED_RESULT Q_DECL_CONSTEXPR static inline Q_DECL_UNUSED  bool qFuzzyIsNull(float f)
{
    return qAbs(f) <= 0.00001f;
}

QT_WARNING_PUSH
QT_WARNING_DISABLE_CLANG("-Wfloat-equal")
QT_WARNING_DISABLE_GCC("-Wfloat-equal")
QT_WARNING_DISABLE_INTEL(1572)

Q_REQUIRED_RESULT Q_DECL_CONSTEXPR static inline Q_DECL_UNUSED bool qIsNull(double d) noexcept
{
    return d == 0.0;
}

Q_REQUIRED_RESULT Q_DECL_CONSTEXPR static inline Q_DECL_UNUSED bool qIsNull(float f) noexcept
{
    return f == 0.0f;
}

QT_WARNING_POP

#ifdef Q_FULL_TEMPLATE_INSTANTIATION
#define Q_DUMMY_COMPARISON_OPERATOR(C)                      \
    bool operator==(const C&) const {                       \
        qWarning(#C"::operator==(const "#C"&) was called"); \
        return false;                                       \
    }
#else
#define Q_DUMMY_COMPARISON_OPERATOR(C)
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_GCC("-Wnoexcept")

namespace QtPrivate {
namespace SwapExceptionTester {
    using std::swap;
    template <typename T>
    void checkSwap(T &t) noexcept(noexcept(swap(t, t)));
}
}

template <typename T>
inline void qSwap(T &value1, T &value2) noexcept(noexcept(QtPrivate::SwapExceptionTester::checkSwap(value1)))
{
    using std::swap;
    swap(value1, value2);
}

QT_WARNING_POP

#if QT_DEPRECATED_SINCE(5, 0)
Q_CORE_EXPORT QT_DEPRECATED void *qMalloc(size_t size) Q_ALLOC_SIZE(1);
Q_CORE_EXPORT QT_DEPRECATED void qFree(void *ptr);
Q_CORE_EXPORT QT_DEPRECATED void *qRealloc(void *ptr, size_t size) Q_ALLOC_SIZE(2);
Q_CORE_EXPORT QT_DEPRECATED void *qMemCopy(void *dest, const void *src, size_t n);
Q_CORE_EXPORT QT_DEPRECATED void *qMemSet(void *dest, int c, size_t n);
#endif
Q_CORE_EXPORT void *qMallocAligned(size_t size, size_t alignment) Q_ALLOC_SIZE(1);
Q_CORE_EXPORT void *qReallocAligned(void *ptr, size_t size, size_t oldsize, size_t alignment) Q_ALLOC_SIZE(2);
Q_CORE_EXPORT void qFreeAligned(void *ptr);

#if !defined(QT_CC_WARNINGS)
#define QT_NO_WARNINGS
#endif

#define QT_3ARG_ALG(alg, f1, l1, f2, l2)      \
    [&f1, &l1, &f2, &l2]() {                  \
        Q_UNUSED(l2);                         \
        return std::alg(f1, l1, f2);          \
    }()

template <typename ForwardIterator1, typename ForwardIterator2>
inline bool qt_is_permutation(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, ForwardIterator2 last2)
{
    return QT_3ARG_ALG(is_permutation, first1, last1, first2, last2);
}

#undef QT_3ARG_ALG

template <typename T>
Q_DECL_CONSTEXPR typename std::add_const<T>::type &qAsConst(T &t) noexcept { return t; }

template <typename T>
void qAsConst(const T &&) = delete;

template <typename T, typename U = T>
Q_DECL_RELAXED_CONSTEXPR T qExchange(T &t, U &&newValue)
{
    T old = std::move(t);
    t = std::forward<U>(newValue);
    return old;
}

#ifndef QT_NO_FOREACH

namespace QtPrivate {
template <typename T>
class QForeachContainer {
    Q_DISABLE_COPY(QForeachContainer)

public:
    QForeachContainer(const T &t) : c(t), i(qAsConst(c).begin()), e(qAsConst(c).end()) {}
    QForeachContainer(T &&t) : c(std::move(t)), i(qAsConst(c).begin()), e(qAsConst(c).end())  {}

    QForeachContainer(QForeachContainer &&other)
        : c(std::move(other.c)), i(qAsConst(c).begin()), e(qAsConst(c).end()), control(std::move(other.control))
    {

    }

    QForeachContainer &operator=(QForeachContainer &&other) {
        c = std::move(other.c);
        i = qAsConst(c).begin();
        e = qAsConst(c).end();
        control = std::move(other.control);
        return *this;
    }

    T c;
    typename T::const_iterator i, e;
    int control = 1;
};

template<typename T>
QForeachContainer<typename std::decay<T>::type> qMakeForeachContainer(T &&t)
{
    return QForeachContainer<typename std::decay<T>::type>(std::forward<T>(t));
}
}

#if __cplusplus >= 201703L
#define Q_FOREACH(variable, container)                                   \
for (auto _container_ = QtPrivate::qMakeForeachContainer(container);     \
     _container_.i != _container_.e;  ++_container_.i)                   \
    if (variable = *_container_.i; false) {} else
#else
#define Q_FOREACH(variable, container)                                  \
for (auto _container_ = QtPrivate::qMakeForeachContainer(container);    \
     _container_.control && _container_.i != _container_.e;             \
     ++_container_.i, _container_.control ^= 1)                         \
    for (variable = *_container_.i; _container_.control; _container_.control = 0)
#endif
#endif

#define Q_FOREVER                                   for(;;)

#ifndef QT_NO_KEYWORDS
#ifndef QT_NO_FOREACH
#ifndef foreach
#define foreach                                     Q_FOREACH
#endif
#endif
#ifndef forever
#define forever                                     Q_FOREVER
#endif
#endif

template <typename T> inline T *qGetPtrHelper(T *ptr) { return ptr; }
template <typename Ptr> inline auto qGetPtrHelper(Ptr &ptr) -> decltype(ptr.operator->()) { return ptr.operator->(); }

#define Q_CAST_IGNORE_ALIGN(body)                   QT_WARNING_PUSH QT_WARNING_DISABLE_GCC("-Wcast-align") body QT_WARNING_POP

#define Q_DECLARE_PRIVATE(Class)                                                                            \
    inline Class##Private *d_func()                                                                         \
    {                                                                                                       \
        Q_CAST_IGNORE_ALIGN(return reinterpret_cast<Class##Private *>(qGetPtrHelper(d_ptr));)               \
    }                                                                                                       \
                                                                                                            \
    inline const Class##Private *d_func() const                                                             \
    {                                                                                                       \
        Q_CAST_IGNORE_ALIGN(return reinterpret_cast<const Class##Private *>(qGetPtrHelper(d_ptr));)         \
    }                                                                                                       \
    friend class Class##Private;

#define Q_DECLARE_PRIVATE_D(Dptr, Class)                                                                    \
    inline Class##Private *d_func()                                                                         \
    {                                                                                                       \
        Q_CAST_IGNORE_ALIGN(return reinterpret_cast<Class##Private *>(qGetPtrHelper(Dptr));)                \
    }                                                                                                       \
                                                                                                            \
    inline const Class##Private *d_func() const                                                             \
    {                                                                                                       \
        Q_CAST_IGNORE_ALIGN(return reinterpret_cast<const Class##Private *>(qGetPtrHelper(Dptr));)          \
    }                                                                                                       \
    friend class Class##Private;

#define Q_DECLARE_PUBLIC(Class)                                                                             \
    inline Class *q_func() {                                                                                \
        return static_cast<Class *>(q_ptr);                                                                 \
    }                                                                                                       \
                                                                                                            \
    inline const Class *q_func() const {                                                                    \
        return static_cast<const Class *>(q_ptr);                                                           \
    }                                                                                                       \
    friend class Class;

#define Q_D(Class)                                  Class##Private * const d = d_func()
#define Q_Q(Class)                                  Class * const q = q_func()

#define QT_TR_NOOP(x)                               x
#define QT_TR_NOOP_UTF8(x)                          x
#define QT_TRANSLATE_NOOP(scope, x)                 x
#define QT_TRANSLATE_NOOP_UTF8(scope, x)            x
#define QT_TRANSLATE_NOOP3(scope, x, comment)       {x, comment}
#define QT_TRANSLATE_NOOP3_UTF8(scope, x, comment)  {x, comment}

#ifndef QT_NO_TRANSLATION

#define QT_TR_N_NOOP(x)                             x
#define QT_TRANSLATE_N_NOOP(scope, x)               x
#define QT_TRANSLATE_N_NOOP3(scope, x, comment)     {x, comment}

Q_CORE_EXPORT QString qtTrId(const char *id, int n = -1);

#define QT_TRID_NOOP(id) id

#endif

#ifdef Q_QDOC

template<typename T>
auto qOverload(T functionPointer);

template<typename T>
auto qConstOverload(T memberFunctionPointer);

template<typename T>
auto qNonConstOverload(T memberFunctionPointer);

#elif defined(Q_COMPILER_VARIADIC_TEMPLATES)

template <typename... Args>
struct QNonConstOverload {
    template <typename R, typename T>
    Q_DECL_CONSTEXPR auto operator()(R (T::*ptr)(Args...)) const noexcept -> decltype(ptr) {
        return ptr;
    }

    template <typename R, typename T>
    static Q_DECL_CONSTEXPR auto of(R (T::*ptr)(Args...)) noexcept -> decltype(ptr) {
        return ptr;
    }
};

template <typename... Args>
struct QConstOverload {
    template <typename R, typename T>
    Q_DECL_CONSTEXPR auto operator()(R (T::*ptr)(Args...) const) const noexcept -> decltype(ptr) {
        return ptr;
    }

    template <typename R, typename T>
    static Q_DECL_CONSTEXPR auto of(R (T::*ptr)(Args...) const) noexcept -> decltype(ptr) {
        return ptr;
    }
};

template <typename... Args>
struct QOverload : QConstOverload<Args...>, QNonConstOverload<Args...> {
    using QConstOverload<Args...>::of;
    using QConstOverload<Args...>::operator();
    using QNonConstOverload<Args...>::of;
    using QNonConstOverload<Args...>::operator();

    template <typename R>
    Q_DECL_CONSTEXPR auto operator()(R (*ptr)(Args...)) const noexcept -> decltype(ptr) {
        return ptr;
    }

    template <typename R>
    static Q_DECL_CONSTEXPR auto of(R (*ptr)(Args...)) noexcept -> decltype(ptr) {
        return ptr;
    }
};

#if defined(__cpp_variable_templates) && __cpp_variable_templates >= 201304
template <typename... Args> Q_CONSTEXPR Q_DECL_UNUSED QOverload<Args...> qOverload = {};
template <typename... Args> Q_CONSTEXPR Q_DECL_UNUSED QConstOverload<Args...> qConstOverload = {};
template <typename... Args> Q_CONSTEXPR Q_DECL_UNUSED QNonConstOverload<Args...> qNonConstOverload = {};
#endif
#endif

class QByteArray;
Q_CORE_EXPORT QByteArray qgetenv(const char *varName);

Q_CORE_EXPORT QString qEnvironmentVariable(const char *varName);
Q_CORE_EXPORT QString qEnvironmentVariable(const char *varName, const QString &defaultValue);

Q_CORE_EXPORT bool qputenv(const char *varName, const QByteArray& value);
Q_CORE_EXPORT bool qunsetenv(const char *varName);

Q_CORE_EXPORT bool qEnvironmentVariableIsEmpty(const char *varName) noexcept;
Q_CORE_EXPORT bool qEnvironmentVariableIsSet(const char *varName) noexcept;
Q_CORE_EXPORT int  qEnvironmentVariableIntValue(const char *varName, bool *ok=nullptr) noexcept;

inline int qIntCast(double f) {
    return int(f);
}

inline int qIntCast(float f) {
    return int(f);
}

#if QT_DEPRECATED_SINCE(5, 15)
Q_CORE_EXPORT QT_DEPRECATED_VERSION_X_5_15("use QRandomGenerator instead") void qsrand(uint seed);
Q_CORE_EXPORT QT_DEPRECATED_VERSION_X_5_15("use QRandomGenerator instead") int qrand();
#endif

#define QT_MODULE(x)

#if !defined(QT_BOOTSTRAPPED) && defined(QT_REDUCE_RELOCATIONS) && defined(__ELF__) && (!defined(__PIC__) || (defined(__PIE__) && defined(Q_CC_GNU) && Q_CC_GNU >= 500))
#error "You must build your code with position independent code if Qt was built with -reduce-relocations. Compile your code with -fPIC (and not with -fPIE)."
#endif

namespace QtPrivate {
template <bool B, typename T = void> struct QEnableIf;
template <typename T> struct QEnableIf<true, T> { typedef T Type; };
}

QT_END_NAMESPACE

#include "qtypeinfo.h"
#include "qsysinfo.h"
#include "qlogging.h"

#include "qflags.h"

#include "../thread/qatomic.h"
#include "qglobalstatic.h"
#include "qnumeric.h"
#include "qversiontagging.h"

#endif
#endif

#endif
