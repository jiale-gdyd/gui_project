#ifndef QCOMPILERDETECTION_H
#define QCOMPILERDETECTION_H

#if defined(__GNUC__)

#define Q_CC_GNU                                            (__GNUC__ * 100 + __GNUC_MINOR__)

#if defined(__INTEL_COMPILER)

#define Q_CC_INTEL                                          (__INTEL_COMPILER)

#ifdef __clang__
#define Q_CC_CLANG                                          305
#endif

#define Q_ASSUME_IMPL(expr)                                 __assume(expr)
#define Q_UNREACHABLE_IMPL()                                __builtin_unreachable()

#if __INTEL_COMPILER >= 1300 && !defined(__APPLE__)
#define Q_DECL_DEPRECATED_X(text)                           __attribute__ ((__deprecated__(text)))
#endif

#else

#if Q_CC_GNU >= 405
#define Q_ASSUME_IMPL(expr)                                 if (expr){} else __builtin_unreachable()
#define Q_UNREACHABLE_IMPL()                                __builtin_unreachable()
#define Q_DECL_DEPRECATED_X(text)                           __attribute__ ((__deprecated__(text)))
#endif

#endif

#if defined(QT_VISIBILITY_AVAILABLE)
#define Q_DECL_EXPORT                                       __attribute__((visibility("default")))
#define Q_DECL_IMPORT                                       __attribute__((visibility("default")))
#define Q_DECL_HIDDEN                                       __attribute__((visibility("hidden")))
#endif

#define Q_FUNC_INFO                                         __PRETTY_FUNCTION__
#define Q_ALIGNOF(type)                                     __alignof__(type)
#define Q_TYPEOF(expr)                                      __typeof__(expr)
#define Q_DECL_DEPRECATED                                   __attribute__ ((__deprecated__))
#define Q_DECL_ALIGN(n)                                     __attribute__((__aligned__(n)))
#define Q_DECL_UNUSED                                       __attribute__((__unused__))
#define Q_LIKELY(expr)                                      __builtin_expect(!!(expr), true)
#define Q_UNLIKELY(expr)                                    __builtin_expect(!!(expr), false)
#define Q_NORETURN                                          __attribute__((__noreturn__))
#define Q_REQUIRED_RESULT                                   __attribute__ ((__warn_unused_result__))
#define Q_DECL_PURE_FUNCTION                                __attribute__((pure))
#define Q_DECL_CONST_FUNCTION                               __attribute__((const))
#define Q_DECL_COLD_FUNCTION                                __attribute__((cold))

#if !defined(QT_MOC_CPP)
#define Q_PACKED                                            __attribute__ ((__packed__))
#ifndef __ARM_EABI__
#define QT_NO_ARM_EABI
#endif
#endif

#if Q_CC_GNU >= 403 && !defined(Q_CC_CLANG)
#define Q_ALLOC_SIZE(x)                                     __attribute__((alloc_size(x)))
#endif

#else
#error "Qt has not been tested with this compiler - see http://www.qt-project.org/"
#endif

#ifndef __has_builtin
#define __has_builtin(x)                                    0
#endif

#ifndef __has_feature
#define __has_feature(x)                                    0
#endif

#ifndef __has_attribute
#define __has_attribute(x)                                  0
#endif

#ifndef __has_cpp_attribute
#define __has_cpp_attribute(x)                              0
#endif

#ifndef __has_include
#define __has_include(x)                                    0
#endif

#ifndef __has_include_next
#define __has_include_next(x)                               0
#endif

#define QT_HAS_BUILTIN(x)                                   __has_builtin(x)
#define QT_HAS_FEATURE(x)                                   __has_feature(x)
#define QT_HAS_ATTRIBUTE(x)                                 __has_attribute(x)
#define QT_HAS_CPP_ATTRIBUTE(x)                             __has_cpp_attribute(x)
#define QT_HAS_INCLUDE(x)                                   __has_include(x)
#define QT_HAS_INCLUDE_NEXT(x)                              __has_include_next(x)

#ifdef __cplusplus
#if __cplusplus < 201103L && !defined(Q_CC_MSVC)
#error Qt requires a C++11 compiler and yours does not seem to be that.
#endif
#endif

#if defined(Q_CC_GNU) && !defined(Q_CC_INTEL) && !defined(Q_CC_CLANG)

#define Q_COMPILER_RESTRICTED_VLA
#define Q_COMPILER_THREADSAFE_STATICS

#if Q_CC_GNU >= 403
#define Q_COMPILER_BINARY_LITERALS
#endif

#if !defined(__STRICT_ANSI__) || defined(__GXX_EXPERIMENTAL_CXX0X__) || (defined(__cplusplus) && (__cplusplus >= 201103L)) || (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L))
#define Q_COMPILER_VARIADIC_MACROS
#endif

#  if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L

#if Q_CC_GNU >= 403
#define Q_COMPILER_DECLTYPE
#define Q_COMPILER_RVALUE_REFS
#define Q_COMPILER_STATIC_ASSERT
#endif

#if Q_CC_GNU >= 404
#define Q_COMPILER_AUTO_FUNCTION
#define Q_COMPILER_AUTO_TYPE
#define Q_COMPILER_EXTERN_TEMPLATES
#define Q_COMPILER_UNIFORM_INIT
#define Q_COMPILER_UNICODE_STRINGS
#define Q_COMPILER_VARIADIC_TEMPLATES
#endif

#if Q_CC_GNU >= 405
#define Q_COMPILER_EXPLICIT_CONVERSIONS
#define Q_COMPILER_INITIALIZER_LISTS
#define Q_COMPILER_LAMBDA
#define Q_COMPILER_RAW_STRINGS
#define Q_COMPILER_CLASS_ENUM
#endif

#if Q_CC_GNU >= 406
#define Q_COMPILER_DEFAULT_MEMBERS
#define Q_COMPILER_DELETE_MEMBERS
#define Q_COMPILER_NULLPTR
#define Q_COMPILER_UNRESTRICTED_UNIONS
#define Q_COMPILER_RANGE_FOR
#endif

#if Q_CC_GNU >= 407
#define Q_COMPILER_ATOMICS
#define Q_COMPILER_NOEXCEPT
#define Q_COMPILER_NONSTATIC_MEMBER_INIT
#define Q_COMPILER_DELEGATING_CONSTRUCTORS
#define Q_COMPILER_EXPLICIT_OVERRIDES
#define Q_COMPILER_TEMPLATE_ALIAS
#define Q_COMPILER_UDL
#endif

#if Q_CC_GNU >= 408
#define Q_COMPILER_ATTRIBUTES
#define Q_COMPILER_ALIGNAS
#define Q_COMPILER_ALIGNOF
#define Q_COMPILER_INHERITING_CONSTRUCTORS
#define Q_COMPILER_THREAD_LOCAL

#if Q_CC_GNU > 408 || __GNUC_PATCHLEVEL__ >= 1
#define Q_COMPILER_REF_QUALIFIERS
#endif
#endif

#if Q_CC_GNU >= 500
#define Q_COMPILER_CONSTEXPR
#endif

#endif

#if __cplusplus > 201103L
#if Q_CC_GNU >= 409
#define Q_COMPILER_LAMBDA_CAPTURES
#define Q_COMPILER_RETURN_TYPE_DEDUCTION
#endif
#endif

#if defined(__STDC_VERSION__) && __STDC_VERSION__ > 199901L
#if Q_CC_GNU >= 407
#define Q_COMPILER_STATIC_ASSERT
#endif

#if Q_CC_GNU >= 409 && defined(__has_include)

#if __has_include(<threads.h>)
#define Q_COMPILER_THREAD_LOCAL
#endif

#endif

#endif

#endif

#ifdef Q_COMPILER_UNICODE_STRINGS
#define Q_STDLIB_UNICODE_STRINGS
#endif

#ifdef __cplusplus
#include <utility>
#endif

#ifdef Q_COMPILER_NULLPTR
#define Q_NULLPTR                                           nullptr
#else
#define Q_NULLPTR                                           NULL
#endif

#ifdef Q_COMPILER_DEFAULT_MEMBERS
#define Q_DECL_EQ_DEFAULT                                   = default
#else
#define Q_DECL_EQ_DEFAULT
#endif

#ifdef Q_COMPILER_DELETE_MEMBERS
#define Q_DECL_EQ_DELETE                                    = delete
#else
#define Q_DECL_EQ_DELETE
#endif

#if defined(Q_COMPILER_DEFAULT_MEMBERS) && defined(Q_COMPILER_DELETE_MEMBERS)
#define Q_COMPILER_DEFAULT_DELETE_MEMBERS
#endif

#if defined Q_COMPILER_CONSTEXPR
#if defined(__cpp_constexpr) && __cpp_constexpr-0 >= 201304
#define Q_DECL_CONSTEXPR                                    constexpr
#define Q_DECL_RELAXED_CONSTEXPR                            constexpr
#define Q_CONSTEXPR                                         constexpr
#define Q_RELAXED_CONSTEXPR                                 constexpr
#else
#define Q_DECL_CONSTEXPR                                    constexpr
#define Q_DECL_RELAXED_CONSTEXPR
#define Q_CONSTEXPR                                         constexpr
#define Q_RELAXED_CONSTEXPR                                 const
#endif

#else

#define Q_DECL_CONSTEXPR
#define Q_DECL_RELAXED_CONSTEXPR
#define Q_CONSTEXPR                                         const
#define Q_RELAXED_CONSTEXPR                                 const
#endif

#ifdef Q_COMPILER_EXPLICIT_OVERRIDES
#define Q_DECL_OVERRIDE                                     override
#define Q_DECL_FINAL                                        final
#else
#ifndef Q_DECL_OVERRIDE
#define Q_DECL_OVERRIDE
#endif

#ifndef Q_DECL_FINAL
#define Q_DECL_FINAL
#endif
#endif

#ifdef Q_COMPILER_NOEXCEPT
#define Q_DECL_NOEXCEPT                                     noexcept
#define Q_DECL_NOEXCEPT_EXPR(x)                             noexcept(x)
#else
#define Q_DECL_NOEXCEPT
#define Q_DECL_NOEXCEPT_EXPR(x)
#endif

#define Q_DECL_NOTHROW                                      Q_DECL_NOEXCEPT

#if defined(Q_COMPILER_ALIGNOF)
#undef Q_ALIGNOF
#define Q_ALIGNOF(x)                                        alignof(x)
#endif

#if defined(Q_COMPILER_ALIGNAS)
#undef Q_DECL_ALIGN
#define Q_DECL_ALIGN(n)                                     alignas(n)
#endif

#if __has_cpp_attribute(nodiscard) && !defined(Q_CC_CLANG)
#undef Q_REQUIRED_RESULT
#define Q_REQUIRED_RESULT                                   [[nodiscard]]
#endif

#if defined(__cpp_enumerator_attributes) && __cpp_enumerator_attributes >= 201411
#define Q_DECL_ENUMERATOR_DEPRECATED                        Q_DECL_DEPRECATED
#define Q_DECL_ENUMERATOR_DEPRECATED_X(x)                   Q_DECL_DEPRECATED_X(x)
#endif

#ifndef Q_NORETURN
#define Q_NORETURN
#endif

#ifndef Q_LIKELY
#define Q_LIKELY(x)                                         (x)
#endif

#ifndef Q_UNLIKELY
#define Q_UNLIKELY(x)                                       (x)
#endif

#ifndef Q_ASSUME_IMPL
#define Q_ASSUME_IMPL(expr)                                 qt_noop()
#endif

#ifndef Q_UNREACHABLE_IMPL
#define Q_UNREACHABLE_IMPL()                                qt_noop()
#endif

#ifndef Q_ALLOC_SIZE
#define Q_ALLOC_SIZE(x)
#endif

#ifndef Q_REQUIRED_RESULT
#define Q_REQUIRED_RESULT
#endif

#ifndef Q_DECL_DEPRECATED
#define Q_DECL_DEPRECATED
#endif

#ifndef Q_DECL_VARIABLE_DEPRECATED
#define Q_DECL_VARIABLE_DEPRECATED                          Q_DECL_DEPRECATED
#endif

#ifndef Q_DECL_DEPRECATED_X
#define Q_DECL_DEPRECATED_X(text)                           Q_DECL_DEPRECATED
#endif

#ifndef Q_DECL_ENUMERATOR_DEPRECATED
#define Q_DECL_ENUMERATOR_DEPRECATED
#endif

#ifndef Q_DECL_ENUMERATOR_DEPRECATED_X
#define Q_DECL_ENUMERATOR_DEPRECATED_X(x)
#endif

#ifndef Q_DECL_EXPORT
#define Q_DECL_EXPORT
#endif

#ifndef Q_DECL_IMPORT
#define Q_DECL_IMPORT
#endif

#ifndef Q_DECL_HIDDEN
#define Q_DECL_HIDDEN
#endif

#ifndef Q_DECL_UNUSED
#define Q_DECL_UNUSED
#endif

#ifndef Q_DECL_UNUSED_MEMBER
#define Q_DECL_UNUSED_MEMBER
#endif

#ifndef Q_FUNC_INFO
#if defined(Q_OS_SOLARIS) || defined(Q_CC_XLC)
#define Q_FUNC_INFO                                         __FILE__ "(line number unavailable)"
#else
#define Q_FUNC_INFO                                         __FILE__ ":" QT_STRINGIFY(__LINE__)
#endif
#endif

#ifndef Q_DECL_CF_RETURNS_RETAINED
#define Q_DECL_CF_RETURNS_RETAINED
#endif

#ifndef Q_DECL_NS_RETURNS_AUTORELEASED
#define Q_DECL_NS_RETURNS_AUTORELEASED
#endif

#ifndef Q_DECL_PURE_FUNCTION
#define Q_DECL_PURE_FUNCTION
#endif

#ifndef Q_DECL_CONST_FUNCTION
#define Q_DECL_CONST_FUNCTION                               Q_DECL_PURE_FUNCTION
#endif

#ifndef Q_DECL_COLD_FUNCTION
#define Q_DECL_COLD_FUNCTION
#endif

#ifndef QT_MAKE_UNCHECKED_ARRAY_ITERATOR
#define QT_MAKE_UNCHECKED_ARRAY_ITERATOR(x)                 (x)
#endif

#ifndef QT_MAKE_CHECKED_ARRAY_ITERATOR
#define QT_MAKE_CHECKED_ARRAY_ITERATOR(x, N)                (x)
#endif

#define QT_DO_PRAGMA(text)                                  _Pragma(#text)

#if defined(Q_CC_GNU) && (__GNUC__ * 100 + __GNUC_MINOR__ >= 406)
#define QT_WARNING_PUSH                                     QT_DO_PRAGMA(GCC diagnostic push)
#define QT_WARNING_POP                                      QT_DO_PRAGMA(GCC diagnostic pop)
#define QT_WARNING_DISABLE_GCC(text)                        QT_DO_PRAGMA(GCC diagnostic ignored text)
#define QT_WARNING_DISABLE_CLANG(text)
#define QT_WARNING_DISABLE_INTEL(number)
#define QT_WARNING_DISABLE_MSVC(number)
#define QT_WARNING_DISABLE_DEPRECATED                       QT_WARNING_DISABLE_GCC("-Wdeprecated-declarations")
#else
#define QT_WARNING_DISABLE_GCC(text)
#define QT_WARNING_PUSH
#define QT_WARNING_POP
#define QT_WARNING_DISABLE_INTEL(number)
#define QT_WARNING_DISABLE_MSVC(number)
#define QT_WARNING_DISABLE_CLANG(text)
#define QT_WARNING_DISABLE_GCC(text)
#define QT_WARNING_DISABLE_DEPRECATED
#endif

#ifndef QT_IGNORE_DEPRECATIONS
#define QT_IGNORE_DEPRECATIONS(statement)   \
    QT_WARNING_PUSH                         \
    QT_WARNING_DISABLE_DEPRECATED           \
    statement                               \
    QT_WARNING_POP
#endif

#ifndef QT_NO_KEYWORDS
#if defined(Q_CC_MIPS) || (defined(Q_CC_HPACC) && defined(__ia64))
#define for                                                 if (0) {} else for
#endif
#endif

#ifdef Q_COMPILER_RVALUE_REFS
#define qMove(x)                                            std::move(x)
#else
#define qMove(x)                                            (x)
#endif

#define Q_UNREACHABLE()                                                                 \
    do {                                                                                \
        Q_ASSERT_X(false, "Q_UNREACHABLE()", "Q_UNREACHABLE was reached");              \
        Q_UNREACHABLE_IMPL();                                                           \
    } while (false)

#define Q_ASSUME(Expr)                                                                  \
    do {                                                                                \
        const bool valueOfExpression = Expr;                                            \
        Q_ASSERT_X(valueOfExpression, "Q_ASSUME()", "Assumption in Q_ASSUME(\"" #Expr "\") was not correct");\
        Q_ASSUME_IMPL(valueOfExpression);                                               \
    } while (false)

#if defined(__cplusplus)
#if __has_cpp_attribute(clang::fallthrough)
#define Q_FALLTHROUGH()                                     [[clang::fallthrough]]
#elif __has_cpp_attribute(gnu::fallthrough)
#define Q_FALLTHROUGH()                                     [[gnu::fallthrough]]
#elif __has_cpp_attribute(fallthrough)
#define Q_FALLTHROUGH()                                     [[fallthrough]]
#endif
#endif

#ifndef Q_FALLTHROUGH
#if (defined(Q_CC_GNU) && Q_CC_GNU >= 700) && !defined(Q_CC_INTEL)
#define Q_FALLTHROUGH()                                     __attribute__((fallthrough))
#else
#define Q_FALLTHROUGH()                                     (void)0
#endif
#endif

#if !defined(Q_PROCESSOR_X86)
#undef QT_COMPILER_SUPPORTS_SSE2
#undef QT_COMPILER_SUPPORTS_SSE3
#undef QT_COMPILER_SUPPORTS_SSSE3
#undef QT_COMPILER_SUPPORTS_SSE4_1
#undef QT_COMPILER_SUPPORTS_SSE4_2
#undef QT_COMPILER_SUPPORTS_AVX
#undef QT_COMPILER_SUPPORTS_AVX2
#endif

#if !defined(Q_PROCESSOR_ARM)
#undef QT_COMPILER_SUPPORTS_NEON
#endif

#if !defined(Q_PROCESSOR_MIPS)
#undef QT_COMPILER_SUPPORTS_MIPS_DSP
#undef QT_COMPILER_SUPPORTS_MIPS_DSPR2
#endif

#endif
