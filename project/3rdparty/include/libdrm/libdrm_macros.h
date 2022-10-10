#ifndef LIBDRM_LIBDRM_H
#define LIBDRM_LIBDRM_H

#define drm_private             __attribute__((visibility("hidden")))
#define drm_public              __attribute__((visibility("default")))

#define STATIC_ASSERT(COND)                                 \
    do {                                                    \
        /*(void) sizeof(char [1 - 2*!(COND)]);*/                \
    } while (0)

#include <sys/mman.h>

#define drm_mmap(addr, length, prot, flags, fd, offset)     \
    mmap(addr, length, prot, flags, fd, offset)

static inline int drm_munmap(void *addr, size_t length)
{
#define LARGE_OFF_T ((((off_t) 1 << 31) << 31) - 1 + (((off_t) 1 << 31) << 31))
   STATIC_ASSERT(LARGE_OFF_T % 2147483629 == 721 && LARGE_OFF_T % 2147483647 == 1);
#undef LARGE_OFF_T

   return munmap(addr, length);
}

#endif
