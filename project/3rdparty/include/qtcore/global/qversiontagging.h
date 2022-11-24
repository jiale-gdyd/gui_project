#include "qglobal.h"

#ifndef QVERSIONTAGGING_H
#define QVERSIONTAGGING_H

QT_BEGIN_NAMESPACE

#if defined(QT_BUILD_CORE_LIB) || defined(QT_BOOTSTRAPPED) || defined(QT_NO_VERSION_TAGGING) || defined(QT_STATIC)

#elif defined(Q_CC_GNU) && !defined(Q_OS_ANDROID)
#if defined(Q_PROCESSOR_X86) && (defined(Q_OS_LINUX) || defined(Q_OS_FREEBSD_KERNEL))

#if defined(Q_PROCESSOR_X86_64) && QT_POINTER_SIZE == 8
#define QT_VERSION_TAG_RELOC(sym) ".quad " QT_STRINGIFY(QT_MANGLE_NAMESPACE(sym)) "@GOT\n"
#else
#define QT_VERSION_TAG_RELOC(sym) ".long " QT_STRINGIFY(QT_MANGLE_NAMESPACE(sym)) "@GOT\n"
#endif

#define QT_VERSION_TAG(sym) \
    asm (   \
    ".section .qtversion, \"aG\", @progbits, " QT_STRINGIFY(QT_MANGLE_NAMESPACE(sym)) ", comdat\n" \
    ".align 8\n" \
    QT_VERSION_TAG_RELOC(sym) \
    ".long " QT_STRINGIFY(QT_VERSION) "\n" \
    ".align 8\n" \
    ".previous" \
    )
#endif
#endif

#if defined(QT_VERSION_TAG)
QT_VERSION_TAG(qt_version_tag);
#endif

QT_END_NAMESPACE

#endif
