#ifndef _UTIL_MATH_H_
#define _UTIL_MATH_H_

#define MIN2(A, B)                      ((A)<(B) ? (A) : (B))
#define MAX2(A, B)                      ((A)>(B) ? (A) : (B))
#define MAX3(A, B, C)                   ((A) > (B) ? MAX2(A, C) : MAX2(B, C))

#define __align_mask(value, mask)       (((value) + (mask)) & ~(mask))
#define ALIGN(value, alignment)         __align_mask(value, (__typeof__(value))((alignment) - 1))

#endif
