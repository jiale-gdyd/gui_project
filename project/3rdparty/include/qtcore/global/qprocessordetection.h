#ifndef QPROCESSORDETECTION_H
#define QPROCESSORDETECTION_H

#if defined(__ORDER_BIG_ENDIAN__)
#define Q_BIG_ENDIAN                            __ORDER_BIG_ENDIAN__
#else
#define Q_BIG_ENDIAN                            4321
#endif

#if defined(__ORDER_LITTLE_ENDIAN__)
#define Q_LITTLE_ENDIAN                         __ORDER_LITTLE_ENDIAN__
#else
#define Q_LITTLE_ENDIAN                         1234
#endif

#if defined(__arm__) || defined(__TARGET_ARCH_ARM) || defined(_M_ARM) || defined(_M_ARM64) || defined(__aarch64__) || defined(__ARM64__)

#if defined(__aarch64__) || defined(__ARM64__) || defined(_M_ARM64)
#define Q_PROCESSOR_ARM_64
#define Q_PROCESSOR_WORDSIZE                    8
#else
#define Q_PROCESSOR_ARM_32
#endif

#if defined(__ARM_ARCH) && __ARM_ARCH > 1
#define Q_PROCESSOR_ARM                         __ARM_ARCH
#elif defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM > 1
#define Q_PROCESSOR_ARM                         __TARGET_ARCH_ARM
#elif defined(_M_ARM) && _M_ARM > 1
#define Q_PROCESSOR_ARM                         _M_ARM
#elif defined(__ARM64_ARCH_8__) || defined(__aarch64__) || defined(__ARMv8__) || defined(__ARMv8_A__) || defined(_M_ARM64)
#define Q_PROCESSOR_ARM                         8
#elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__) || defined(_ARM_ARCH_7) || defined(__CORE_CORTEXA__)
#define Q_PROCESSOR_ARM                         7
#elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6T2__) || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6ZK__) || defined(__ARM_ARCH_6M__)
#define Q_PROCESSOR_ARM                         6
#elif defined(__ARM_ARCH_5TEJ__) || defined(__ARM_ARCH_5TE__)
#define Q_PROCESSOR_ARM                         5
#else
#define Q_PROCESSOR_ARM                         0
#endif

#if Q_PROCESSOR_ARM >= 8
#define Q_PROCESSOR_ARM_V8
#endif

#if Q_PROCESSOR_ARM >= 7
#define Q_PROCESSOR_ARM_V7
#endif

#if Q_PROCESSOR_ARM >= 6
#define Q_PROCESSOR_ARM_V6
#endif

#if Q_PROCESSOR_ARM >= 5
#define Q_PROCESSOR_ARM_V5
#else
#error "ARM architecture too old"
#endif

#if defined(__ARMEL__) || defined(_M_ARM64)
#define Q_BYTE_ORDER                            Q_LITTLE_ENDIAN
#elif defined(__ARMEB__)
#define Q_BYTE_ORDER                            Q_BIG_ENDIAN
#endif

#elif defined(__i386) || defined(__i386__) || defined(_M_IX86)

#define Q_PROCESSOR_X86_32
#define Q_BYTE_ORDER                            Q_LITTLE_ENDIAN
#define Q_PROCESSOR_WORDSIZE                    4

#if defined(_M_IX86)
#define Q_PROCESSOR_X86                         (_M_IX86/100)
#elif defined(__i686__) || defined(__athlon__) || defined(__SSE__) || defined(__pentiumpro__)
#define Q_PROCESSOR_X86                         6
#elif defined(__i586__) || defined(__k6__) || defined(__pentium__)
#define Q_PROCESSOR_X86                         5
#elif defined(__i486__) || defined(__80486__)
#define Q_PROCESSOR_X86                         4
#else
#define Q_PROCESSOR_X86                         3
#endif

#elif defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64)

#define Q_PROCESSOR_X86                         6
#define Q_PROCESSOR_X86_64
#define Q_BYTE_ORDER                            Q_LITTLE_ENDIAN
#define Q_PROCESSOR_WORDSIZE                    8

#elif defined(__ia64) || defined(__ia64__) || defined(_M_IA64)

#define Q_PROCESSOR_IA64
#define Q_PROCESSOR_WORDSIZE                    8

#elif defined(__mips) || defined(__mips__) || defined(_M_MRX000)

#define Q_PROCESSOR_MIPS

#if defined(_MIPS_ARCH_MIPS1) || (defined(__mips) && __mips - 0 >= 1)
#define Q_PROCESSOR_MIPS_I
#endif

#if defined(_MIPS_ARCH_MIPS2) || (defined(__mips) && __mips - 0 >= 2)
#define Q_PROCESSOR_MIPS_II
#endif

#if defined(_MIPS_ARCH_MIPS3) || (defined(__mips) && __mips - 0 >= 3)
#define Q_PROCESSOR_MIPS_III
#endif

#if defined(_MIPS_ARCH_MIPS4) || (defined(__mips) && __mips - 0 >= 4)
#define Q_PROCESSOR_MIPS_IV
#endif

#if defined(_MIPS_ARCH_MIPS5) || (defined(__mips) && __mips - 0 >= 5)
#define Q_PROCESSOR_MIPS_V
#endif

#if defined(_MIPS_ARCH_MIPS32) || defined(__mips32) || (defined(__mips) && __mips - 0 >= 32)
#define Q_PROCESSOR_MIPS_32
#endif

#if defined(_MIPS_ARCH_MIPS64) || defined(__mips64)
#define Q_PROCESSOR_MIPS_64
#define Q_PROCESSOR_WORDSIZE                    8
#endif

#if defined(__MIPSEL__)
#define Q_BYTE_ORDER                            Q_LITTLE_ENDIAN
#elif defined(__MIPSEB__)
#define Q_BYTE_ORDER                            Q_BIG_ENDIAN
#endif

#elif defined(__ppc__) || defined(__ppc) || defined(__powerpc__) || defined(_ARCH_COM) || defined(_ARCH_PWR) || defined(_ARCH_PPC) || defined(_M_MPPC) || defined(_M_PPC)

#define Q_PROCESSOR_POWER

#if defined(__ppc64__) || defined(__powerpc64__) || defined(__64BIT__)
#define Q_PROCESSOR_POWER_64
#define Q_PROCESSOR_WORDSIZE                    8
#else
#define Q_PROCESSOR_POWER_32
#endif

#elif defined(__riscv)

#define Q_PROCESSOR_RISCV

#if __riscv_xlen == 64
#define Q_PROCESSOR_RISCV_64
#else
#define Q_PROCESSOR_RISCV_32
#endif

#define Q_BYTE_ORDER                            Q_LITTLE_ENDIAN

#elif defined(__s390__)

#define Q_PROCESSOR_S390

#if defined(__s390x__)
#define Q_PROCESSOR_S390_X
#endif

#define Q_BYTE_ORDER                            Q_BIG_ENDIAN

#elif defined(__sparc__)

#define Q_PROCESSOR_SPARC

#if defined(__sparc_v9__)
#define Q_PROCESSOR_SPARC_V9
#endif

#if defined(__sparc64__)
#define Q_PROCESSOR_SPARC_64
#endif

#define Q_BYTE_ORDER                            Q_BIG_ENDIAN

#elif defined(__EMSCRIPTEN__)

#define Q_PROCESSOR_WASM
#define Q_BYTE_ORDER                            Q_LITTLE_ENDIAN
#define Q_PROCESSOR_WORDSIZE                    8

#endif

#if !defined(Q_BYTE_ORDER)
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == Q_BIG_ENDIAN || __BYTE_ORDER__ == Q_LITTLE_ENDIAN)
#define Q_BYTE_ORDER                            __BYTE_ORDER__
#elif defined(__BIG_ENDIAN__) || defined(_big_endian__) || defined(_BIG_ENDIAN)
#define Q_BYTE_ORDER                            Q_BIG_ENDIAN
#elif defined(__LITTLE_ENDIAN__) || defined(_little_endian__) || defined(_LITTLE_ENDIAN) || defined(WINAPI_FAMILY)
#define Q_BYTE_ORDER                            Q_LITTLE_ENDIAN
#else
#error "Unable to determine byte order!"
#endif
#endif

#if defined __SIZEOF_POINTER__
#define QT_POINTER_SIZE                         __SIZEOF_POINTER__
#elif defined(__LP64__) || defined(_LP64)
#define QT_POINTER_SIZE                         8
#elif defined(Q_PROCESSOR_WORDSIZE)
#define QT_POINTER_SIZE                         Q_PROCESSOR_WORDSIZE
#else
#define QT_POINTER_SIZE                         4
#endif

#ifndef Q_PROCESSOR_WORDSIZE
#define Q_PROCESSOR_WORDSIZE                    QT_POINTER_SIZE
#endif

#endif
