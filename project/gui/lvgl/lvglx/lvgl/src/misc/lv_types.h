#ifndef LV_TYPES_H
#define LV_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__UINTPTR_MAX__) && (__UINTPTR_MAX__ > 0xFFFFFFFF)
#define LV_ARCH_64
#elif defined(UINTPTR_MAX) && (UINTPTR_MAX > 0xFFFFFFFF)
#define LV_ARCH_64
// 否则使用依赖于编译器的方法来确定拱形尺寸
#elif defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__) || defined (__aarch64__)
#define LV_ARCH_64
#endif

enum {
    LV_RES_INV = 0,     // 通常表示对象在动作函数中被删除(变为无效)或操作失败
    LV_RES_OK,          // 动作后对象有效(未删除)
};
typedef uint8_t lv_res_t;

#if defined(__cplusplus) || (__STDC_VERSION__ >= 199901L)
// 如果是c99或更新版本，则直接使用<stdint.h>中的uintptr_t定义
typedef uintptr_t lv_uintptr_t;
typedef intptr_t lv_intptr_t;
#else
// 否则，使用拱形尺寸确定
#ifdef LV_ARCH_64
typedef uint64_t lv_uintptr_t;
typedef int64_t lv_intptr_t;
#else
typedef uint32_t lv_uintptr_t;
typedef int32_t lv_intptr_t;
#endif

#endif

#define LV_UNUSED(x)                            ((void)x)

#define _LV_CONCAT(x, y)                        x ## y
#define LV_CONCAT(x, y)                         _LV_CONCAT(x, y)

#define _LV_CONCAT3(x, y, z)                    x ## y ## z
#define LV_CONCAT3(x, y, z)                     _LV_CONCAT3(x, y, z)

#if defined(PYCPARSER) || defined(__CC_ARM)
#define LV_FORMAT_ATTRIBUTE(fmtstr, vararg)
#elif defined(__GNUC__) && ((__GNUC__ == 4 && __GNUC_MINOR__ >= 4) || __GNUC__ > 4)
#define LV_FORMAT_ATTRIBUTE(fmtstr, vararg)     __attribute__((format(gnu_printf, fmtstr, vararg)))
#elif (defined(__clang__) || defined(__GNUC__) || defined(__GNUG__))
#define LV_FORMAT_ATTRIBUTE(fmtstr, vararg)     __attribute__((format(printf, fmtstr, vararg)))
#else
#define LV_FORMAT_ATTRIBUTE(fmtstr, vararg)
#endif

#ifdef __cplusplus
}
#endif

#endif
