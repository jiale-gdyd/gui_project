#ifndef RKRGA_RGAAPI_H
#define RKRGA_RGAAPI_H

#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <linux/stddef.h>

#include "rga.h"
#include "drmrga.h"

#ifdef __cplusplus
extern "C"{
#endif

#define RgaInit(ctx)                            \
    ({                                          \
        int ret = 0;                            \
        ret = c_RkRgaInit();                    \
        c_RkRgaGetContext(ctx);                 \
        ret;                                    \
    })

#define RgaDeInit(ctx)                          \
    {                                           \
        (void)ctx;                              \
        c_RkRgaDeInit();                        \
    }

#define RgaBlit(...)            c_RkRgaBlit(__VA_ARGS__)
#define RgaCollorFill(...)      c_RkRgaColorFill(__VA_ARGS__)
#define RgaFlush()              c_RkRgaFlush()

int  c_RkRgaInit();
void c_RkRgaDeInit();
void c_RkRgaGetContext(void **ctx);
int c_RkRgaBlit(rga_info_t *src, rga_info_t *dst, rga_info_t *src1);
int c_RkRgaColorFill(rga_info_t *dst);
int c_RkRgaFlush();

int c_RkRgaGetAllocBuffer(bo_t *bo_info, int width, int height, int bpp);
int c_RkRgaGetAllocBufferCache(bo_t *bo_info, int width, int height, int bpp);

int c_RkRgaUnmap(bo_t *bo_info);
int c_RkRgaGetMmap(bo_t *bo_info);

int c_RkRgaFree(bo_t *bo_info);
int c_RkRgaGetBufferFd(bo_t *bo_info, int *fd);

#ifdef __cplusplus
}
#endif

#endif
