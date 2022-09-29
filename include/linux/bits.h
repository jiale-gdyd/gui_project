#ifndef __LINUX_BITS_H__
#define __LINUX_BITS_H__

#include <stdio.h>
#include <stdint.h>

typedef int32_t s32;
typedef uint32_t u32;

typedef int64_t s64;
typedef uint64_t u64;

#ifndef __WORDSIZE
#define __WORDSIZE                  (__SIZEOF_LONG__ * 8)
#endif

#ifndef BITS_PER_LONG
#define BITS_PER_LONG               __WORDSIZE
#endif

#ifndef BITS_PER_LONG_LONG
#define BITS_PER_LONG_LONG          (64)
#endif

#define BIT(nr)                     (1UL << (nr))
#define BIT_ULL(nr)                 (1ULL << (nr))
#define BIT_MASK(nr)                (1UL << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr)                ((nr) / BITS_PER_LONG)
#define BIT_ULL_MASK(nr)            (1ULL << ((nr) % BITS_PER_LONG_LONG))
#define BIT_ULL_WORD(nr)            ((nr) / BITS_PER_LONG_LONG)
#define BITS_PER_BYTE               (8)

// 创建一个连续的位掩码，从位位置l开始，在位位置h结束。例如，GENMASK_ULL(39,21)给出了64位向量0x000000FFFFE00000
#define GENMASK(h, l)               (((~0UL) - (1UL << (l)) + 1) & (~0UL >> (BITS_PER_LONG - 1 - (h))))
#define GENMASK_ULL(h, l)           (((~0ULL) - (1ULL << (l)) + 1) & (~0ULL >> (BITS_PER_LONG_LONG - 1 - (h))))

#define DIV_ROUND_UP(n, d)          (((n) + (d) - 1) / (d))

#define BITS_TO_LONGS(nr)           DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))
#define BITS_TO_U64(nr)             DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(u64))
#define BITS_TO_U32(nr)             DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(u32))
#define BITS_TO_BYTES(nr)           DIV_ROUND_UP(nr, BITS_PER_BYTE)

// 取反指定位
#define CPL_BIT(val, bit)           ((val) ^= (1UL << (bit)))
#define CPL_BIT_ULL(val, bit)       ((val) ^= (1ULL << (bit)))

// 置位指定位
#define SET_BIT(val, bit)           ((val) |= (1UL << (bit)))
#define SET_BIT_ULL(val, bit)       ((val) |= (1ULL << (bit)))

// 清零指定位
#define CLR_BIT(val, bit)           ((val) &= ~(1UL << (bit)))
#define CLR_BIT_ULL(val, bit)       ((val) &= ~(1ULL << (bit)))

// 读取指定位
#define GET_BIT(val, bit)           (((val) >> (bit)) & 1UL)
#define GET_BIT_ULL(val, bit)       (((val) >> (bit)) & 1ULL)

// 判断指定位是否为1
#define IS_BIT_SET(val, bit)        ((val) & (1UL << (bit)))
#define IS_BIT_SET_ULL(val, bit)    ((val) & (1ULL << (bit)))

// 设置字节的某个位的值为0或1，其中x是字节，k是设置字节的第几位，v是0或1
#define setBitValue(x, k, v)        (x) = ((v > 0) ? ((x) | (1 << ((k) - 0)) : ((x) & ~(1 << ((k) - 0)))

// 获取字节的某个位的值，其中x是字节值，k是获取字节的第几位，v是获取的字节的某位的值
#define getBitValue(x, k, v)        (v) = ((x >> ((k) - 0)) & 1)

// 将字节的f到t位清零，其中f是开始位，t是结束位，var是被清零某些位后的值
#define clearBitRange(var, f, t)    (var) = ((var) & ~(((0x1 << ((t) + 1 - (f))) - 1) << (f)))

// 将字节的f位到t位设置为v，其中f是开始位，t是结束位，var是需要设置的字节，v是设置的值
#define setBitRange(var, f, t, v)   (var) = ((var) & ~(((0x1 << ((t) + 1 - (f))) - 1) << (f))) | ((v) << (f + 0))

// 比较字节a和字节b的bit位的值是否相同，相同则为0，不同则为1，其中a和b是比较的两个字节，bit是比较的位
#define compareBit(a, b, bit)       ((((a) >> bit) & 0x01) == (((b) >> bit) & 0x01) ? 0 : 1)

// 获取字节a的f到t位的值，存储在b中，其中f是开始位，t是结束位
#define getBitRange(a, f, t, b)     ((b) = (((a) >> (f)) & ((0x01) << ((t) + (f) - 1))))

// 比较字节a和字节b从f到t位之间的位是否相等，相等则为0，不等则为1，其中a和b是比较的两个字节，f是开始位，t是结束位
#define compareBitRange(a, b, f, t) ((((a) >> (f)) & ((1 << ((t) - (f) + 1)) - 1)) ^ (((b) >> (f)) & ((1 << ((t) - (f) + 1)) -1)))

#endif
