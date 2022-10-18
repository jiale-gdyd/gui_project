#ifndef RKRGA_ROCKCHIPRGA_H
#define RKRGA_ROCKCHIPRGA_H

#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <linux/stddef.h>

#include "rga.h"
#include "drmrga.h"
#include "RgaUtils.h"
#include "RgaSingleton.h"

class RockchipRga :public Singleton<RockchipRga> {
public:
    static inline RockchipRga &get() {
        return getInstance();
    }

    int RkRgaInit();
    void RkRgaDeInit();
    void RkRgaGetContext(void **ctx);

    int RkRgaAllocBuffer(int drm_fd, bo_t *bo_info, int width, int height, int bpp, int flags);
    int RkRgaFreeBuffer(int drm_fd, bo_t *bo_info);
    int RkRgaGetAllocBuffer(bo_t *bo_info, int width, int height, int bpp);
    int RkRgaGetAllocBufferExt(bo_t *bo_info, int width, int height, int bpp, int flags);
    int RkRgaGetAllocBufferCache(bo_t *bo_info, int width, int height, int bpp);
    int RkRgaGetMmap(bo_t *bo_info);
    int RkRgaUnmap(bo_t *bo_info);
    int RkRgaFree(bo_t *bo_info);
    int RkRgaGetBufferFd(bo_t *bo_info, int *fd);

    int RkRgaBlit(rga_info *src, rga_info *dst, rga_info *src1);
    int RkRgaCollorFill(rga_info *dst);
    int RkRgaCollorPalette(rga_info *src, rga_info *dst, rga_info *lut);
    int RkRgaFlush();

    void RkRgaSetLogOnceFlag(int log) {
        mLogOnce = log;
    }

    void RkRgaSetAlwaysLogFlag(bool log) {
        mLogAlways = log;
    }

    void RkRgaLogOutRgaReq(struct rga_req rgaReg);
    int RkRgaLogOutUserPara(rga_info *rgaInfo);

    inline bool RkRgaIsReady() {
        return mSupportRga;
    }

    RockchipRga();
    ~RockchipRga();

private:
    bool mSupportRga;
    int  mLogOnce;
    int  mLogAlways;
    void *mContext;

    friend class Singleton<RockchipRga>;
};

#endif
