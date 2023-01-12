#include <pthread.h>
#include <sys/ioctl.h>
#include <rockchip/rkrgax/NormalRga.h>
#include <rockchip/rkrgax/im2d_impl.h>
#include <rockchip/rkrgax/NormalRgaContext.h>

pthread_mutex_t mMutex = PTHREAD_MUTEX_INITIALIZER;

#define RGA_SRCOVER_EN              1

volatile int32_t refCount = 0;
struct rgaContext *rgaCtx = NULL;
extern struct im2d_job_manager g_im2d_job_manager;

void NormalRgaSetLogOnceFlag(int log)
{
    struct rgaContext *ctx = NULL;
    ctx->mLogOnce = log;
}

void NormalRgaSetAlwaysLogFlag(int log)
{
    struct rgaContext *ctx = NULL;
    ctx->mLogAlways = log;
}

void is_debug_log(void)
{
    struct rgaContext *ctx = rgaCtx;
    ctx->Is_debug = get_int_property();
}

int is_out_log( void ) {
    struct rgaContext *ctx = rgaCtx;
    return ctx->Is_debug;
}

int get_int_property(void)
{
    char *level = getenv("ROCKCHIP_RGA_LOG");
    if (level == nullptr) {
        level = (char *)"0";
    }

    return atoi(level);
}

int NormalRgaOpen(void **context)
{
    int fd = -1;
    int ret = 0;
    char buf[30];
    struct rgaContext *ctx = NULL;

    if (!context) {
        ret = -EINVAL;
        goto mallocErr;
    }

    if (!rgaCtx) {
        ctx = (struct rgaContext *)malloc(sizeof(struct rgaContext));
        if (!ctx) {
            ret = -ENOMEM;
            rga_error("malloc failed:[%s]", strerror(errno));
            goto mallocErr;
        }

        fd = open("/dev/rga", O_RDWR, 0);
        if (fd < 0) {
            ret = -ENODEV;
            rga_error("failed to open RGA:[%s]", strerror(errno));
            goto rgaOpenErr;
        }
        ctx->rgaFd = fd;

        ret = ioctl(fd, RGA_IOC_GET_DRVIER_VERSION, &ctx->mDriverVersion);
        if (ret >= 0) {
            ret = ioctl(fd, RGA_IOC_GET_HW_VERSION, &ctx->mHwVersions);
            if (ret < 0) {
                rga_error("librga fail to get hw versions");
                goto getVersionError;
            }

            ctx->mVersion = (float)3.2;
            ctx->driver = RGA_DRIVER_IOC_MULTI_RGA;
        } else {
            ctx->mHwVersions.size = 1;
            ret = ioctl(fd, RGA2_GET_VERSION, ctx->mHwVersions.version[0].str);
            if (ret < 0) {
                ret = ioctl(fd, RGA_GET_VERSION, ctx->mHwVersions.version[0].str);
                if (ret < 0) {
                    rga_error("librga fail to get RGA2/RGA1 version, errstr:[%s]", strerror(ret));
                    goto getVersionError;
                }
            }

            sscanf((char *)ctx->mHwVersions.version[0].str, "%x.%x.%x", &ctx->mHwVersions.version[0].major, &ctx->mHwVersions.version[0].minor, &ctx->mHwVersions.version[0].revision);
            ctx->mVersion = atof((char *)ctx->mHwVersions.version[0].str);

            ctx->driver = RGA_DRIVER_IOC_RGA2;
            rga_error("librga fail to get driver version! Compatibility mode will be enabled");
        }

        NormalRgaInitTables();
        rgaCtx = ctx;
    } else {
        ctx = rgaCtx;
        rga_error("Had init the rga dev ctx:[%p]", ctx);
    }

    pthread_mutex_lock(&mMutex);
    refCount++;
    pthread_mutex_unlock(&mMutex);
    *context = (void *)ctx;

    return ret;

getVersionError:
rgaOpenErr:
    free(ctx);
mallocErr:
    return ret;
}

int NormalRgaClose(void **context)
{
    struct rgaContext *ctx = rgaCtx;

    if (!ctx) {
        rga_error("Try to exit uninit rgaCtx:[%p]", ctx);
        return -ENODEV;
    }

    if (!*context) {
        rga_error("Try to uninit rgaCtx:[%p]", *context);
        return -ENODEV;
    }

    if (*context != ctx) {
        rga_error("Try to exit wrong ctx:[%p]", ctx);
        return -ENODEV;
    }

    if (refCount <= 0) {
        rga_error("This can not be happened, close before init");
        return 0;
    }

    pthread_mutex_lock(&mMutex);
    refCount--;
    if (refCount < 0) {
        refCount = 0;
        pthread_mutex_unlock(&mMutex);
        return 0;
    }

    if (refCount > 0) {
        pthread_mutex_unlock(&mMutex);
        return 0;
    }
    pthread_mutex_unlock(&mMutex);

    rgaCtx = NULL;
    *context = NULL;

    close(ctx->rgaFd);
    free(ctx);

    return 0;
}

int RgaInit(void **ctx)
{
    int ret = 0;
    ret = NormalRgaOpen(ctx);

    ret = rga_check_driver(rgaCtx->mDriverVersion);
    if (ret == IM_STATUS_ERROR_VERSION) {
        return -1;
    }

    return ret;
}

int RgaDeInit(void **ctx)
{
    int ret = 0;
    ret = NormalRgaClose(ctx);
    return ret;
}

int RgaBlit(rga_info *src, rga_info *dst, rga_info *src1)
{
    RECT clip;
    int ret = 0;
    int rotation;
    int planeAlpha;
    int dstFd = -1;
    int srcFd = -1;
    int src1Fd = -1;
    int stretch = 0;
    float hScale = 1;
    float vScale = 1;
    unsigned int blend;
    void *srcBuf = NULL;
    void *dstBuf = NULL;
    void *src1Buf = NULL;
    bool perpixelAlpha = 0;
    unsigned int yuvToRgbMode;
    int sync_mode = RGA_BLIT_SYNC;
    struct rgaContext *ctx = rgaCtx;
    struct rga_req rgaReg, tmprgaReg;
    rga_rect_t relSrc1Rect, tmpSrc1Rect;
    int scaleMode, rotateMode, orientation, ditherEn;
    int srcVirW, srcVirH, srcActW, srcActH, srcXPos, srcYPos;
    int dstVirW, dstVirH, dstActW, dstActH, dstXPos, dstYPos;
    rga_rect_t relSrcRect, tmpSrcRect, relDstRect, tmpDstRect;
    int src1VirW, src1VirH, src1ActW, src1ActH, src1XPos,src1YPos;
    int srcType, dstType, src1Type, srcMmuFlag, dstMmuFlag, src1MmuFlag;

    if (!ctx) {
        rga_error("Try to use uninit rgaCtx:[%p]",ctx);
        return -ENODEV;
    }

    memset(&rgaReg, 0, sizeof(struct rga_req));

    srcType = dstType = srcMmuFlag = dstMmuFlag = 0;
    src1Type = src1MmuFlag = 0;
    rotation = 0;
    blend = 0;
    yuvToRgbMode = 0;

    is_debug_log();

    if (is_out_log()) {
        rga_debug("<<<<-------- print rgaLog -------->>>>");
    }

    if (!src && !dst && !src1) {
        rga_error("src:[%p], dst:[%p], src1:[%p]", src, dst, src1);
        return -EINVAL;
    }

    if (!src && !dst) {
        rga_error("src:[%p], dst:[%p]", src, dst);
        return -EINVAL;
    }

    if (src) {
        rotation = src->rotation;
        blend = src->blend;
        memcpy(&relSrcRect, &src->rect, sizeof(rga_rect_t));
    }

    if (dst) {
        memcpy(&relDstRect, &dst->rect, sizeof(rga_rect_t));
    }

    if (src1) {
        memcpy(&relSrc1Rect, &src1->rect, sizeof(rga_rect_t));
    }
    srcFd = dstFd = src1Fd = -1;

    if (is_out_log()) {
        rga_debug("src->hnd:[0x%lX] , dst->hnd:[0x%lX], src1->hnd:[0x%lX]", (unsigned long)src->hnd, (unsigned long)dst->hnd, (unsigned long)(src1 ? src1->hnd : 0));
        rga_debug("src: handle:[%d], Fd:[%.2d], phyAddr:[%p], virAddr:[%p]", src->handle, src->fd, src->phyAddr, src->virAddr);
        if (src1) {
            rga_debug("src1: handle:[%d], Fd:[%.2d], phyAddr:[%p], virAddr:[%p]", src1->handle, src1->fd, src1->phyAddr, src1->virAddr);
        }
        rga_debug("dst: handle:[%d], Fd:[%.2d], phyAddr:[%p], virAddr:[%p]", dst->handle, dst->fd, dst->phyAddr, dst->virAddr);
    }

    if (src1) {
        if ((src->handle > 0) && (dst->handle > 0) && (src1->handle > 0)) {
            rgaReg.handle_flag |= 1;
        } else if (((src->handle > 0) || (dst->handle > 0) || (src1->handle > 0)) && ((src->handle <= 0) || (dst->handle <= 0) || (src1->handle <= 0))) {
            rga_error("librga only supports the use of handles only or no handles, [src,src1,dst] = [%d, %d, %d]", src->handle, src1->handle, dst->handle);
            return -EINVAL;
        }
    } else {
        if ((src->handle > 0) && (dst->handle > 0)) {
            rgaReg.handle_flag |= 1;
        } else if (((src->handle > 0) || (dst->handle > 0)) && ((src->handle <= 0) || (dst->handle <= 0))) {
            rga_error("librga only supports the use of handles only or no handles, [src,dst] = [%d, %d]", src->handle, dst->handle);
            return -EINVAL;
        }
    }

    if (src && src->handle) {
        srcFd = src->handle;
    } else if (src && src->phyAddr) {
        srcBuf = src->phyAddr;
    } else if (src && src->fd > 0) {
        srcFd = src->fd;
        src->mmuFlag = 1;
    } else if (src && src->virAddr) {
        srcBuf = src->virAddr;
        src->mmuFlag = 1;
    }

    if ((srcFd == -1) && !srcBuf) {
        rga_error("src has not fd and address for render");
        return ret;
    }

    if ((srcFd == 0) && !srcBuf) {
        rga_error("srcFd is zero, now driver not support");
        return -EINVAL;
    }

    if (srcFd == 0) {
        srcFd = -1;
    }

    if (src1) {
        if (src1 && src1->handle) {
            src1Fd = src1->handle;
        } else if (src1 && src1->phyAddr) {
            src1Buf = src1->phyAddr;
        } else if (src1 && src1->fd > 0) {
            src1Fd = src1->fd;
            src1->mmuFlag = 1;
        } else if (src1 && src1->virAddr) {
            src1Buf = src1->virAddr;
            src1->mmuFlag = 1;
        }

        if ((src1Fd == -1) && !src1Buf) {
            rga_error("src1 has not fd and address for render");
            return ret;
        }

        if ((src1Fd == 0) && !src1Buf) {
            rga_error("src1Fd is zero, now driver not support");
            return -EINVAL;
        }

        if (src1Fd == 0) {
            src1Fd = -1;
        }
    }

    if (dst && dst->handle) {
        dstFd = dst->handle;
    } else if (dst && dst->phyAddr) {
        dstBuf = dst->phyAddr;
    } else if (dst && dst->fd > 0) {
        dstFd = dst->fd;
        dst->mmuFlag = 1;
    } else if (dst && dst->virAddr) {
        dstBuf = dst->virAddr;
        dst->mmuFlag = 1;
    }

    if ((dstFd == -1) && !dstBuf) {
        rga_error("dst has not fd and address for render");
        return ret;
    }

    if ((dstFd == 0) && !dstBuf) {
        rga_error("dstFd is zero, now driver not support");
        return -EINVAL;
    }

    if (dstFd == 0) {
        dstFd = -1;
    }

    if(is_out_log()) {
        rga_debug("handle_flag:[0x%X]", rgaReg.handle_flag);
        rga_debug("src: Fd/handle:[%.2d], buf:[%p], mmuFlag:[%d], mmuType:[%d]", srcFd, srcBuf, src->mmuFlag, srcType);
        if (src1) {
            rga_debug("src1: Fd/handle:[%.2d], buf:[%p], mmuFlag:[%d], mmuType:[%d]", src1Fd, src1Buf, src1->mmuFlag, src1Type);
        }
        rga_debug("dst: Fd/handle:[%.2d], buf:[%p], mmuFlag:[%d], mmuType:[%d]", dstFd, dstBuf, dst->mmuFlag, dstType);
    }

    relSrcRect.format = RkRgaCompatibleFormat(relSrcRect.format);
    relDstRect.format = RkRgaCompatibleFormat(relDstRect.format);
    if (isRectValid(relSrc1Rect)) {
        relSrc1Rect.format = RkRgaCompatibleFormat(relSrc1Rect.format);
    }

#if defined(CONFIG_RK3126C)
    if ((relSrcRect.width == relDstRect.width)
        && (relSrcRect.height == relDstRect.height)
        && ((relSrcRect.width + 2 * relSrcRect.xoffset) == relSrcRect.wstride)
        && ((relSrcRect.height + 2 * relSrcRect.yoffset) == relSrcRect.hstride)
        && (relSrcRect.format == HAL_PIXEL_FORMAT_YCrCb_NV12)
        && ((relSrcRect.xoffset > 0) && (relSrcRect.yoffset > 0)))
    {
        relSrcRect.width += 4;
        relSrcRect.xoffset = (relSrcRect.wstride - relSrcRect.width) / 2;
    }
#endif

    planeAlpha = (blend & 0xFF0000) >> 16;
    perpixelAlpha = NormalRgaFormatHasAlpha(RkRgaGetRgaFormat(relSrcRect.format));

    if (is_out_log()) {
        rga_error("blend:[%X], perpixelAlpha:[%d]", blend, perpixelAlpha);
    }

    switch ((blend & 0xFFFF)) {
        case 0x0001:
            NormalRgaSetAlphaEnInfo(&rgaReg, 1, 2, planeAlpha , 1, 1, 0);
            break;

        case 0x0002:
            NormalRgaSetAlphaEnInfo(&rgaReg, 1, 2, planeAlpha , 1, 2, 0);
            break;

        case 0x0105:
            if (perpixelAlpha && planeAlpha < 255) {
                NormalRgaSetAlphaEnInfo(&rgaReg, 1, 2, planeAlpha, 1, 9, 0);
            } else if (perpixelAlpha) {
                NormalRgaSetAlphaEnInfo(&rgaReg, 1, 1, 0, 1, 3, 0);
            } else {
                NormalRgaSetAlphaEnInfo(&rgaReg, 1, 0, planeAlpha, 0, 0, 0);
            }
            break;

        case 0x0405:
            if (perpixelAlpha && (planeAlpha < 255)) {
                NormalRgaSetAlphaEnInfo(&rgaReg, 1, 2, planeAlpha, 1, 9, 0);
            } else if (perpixelAlpha) {
                NormalRgaSetAlphaEnInfo(&rgaReg, 1, 1, 0, 1, 3, 0);
            } else {
                NormalRgaSetAlphaEnInfo(&rgaReg, 1, 0, planeAlpha, 0, 0, 0);
            }
            rgaReg.alpha_rop_flag |= (1 << 9);
            break;

        case 0x0501:
            if (perpixelAlpha && planeAlpha < 255) {
                NormalRgaSetAlphaEnInfo(&rgaReg, 1, 2, planeAlpha , 1, 4, 0);
            } else if (perpixelAlpha) {
                NormalRgaSetAlphaEnInfo(&rgaReg, 1, 1, planeAlpha , 1, 4, 0);
            } else {
                NormalRgaSetAlphaEnInfo(&rgaReg, 1, 3, planeAlpha , 1, 4, 0);
            }
            break;

        case 0x0504:
            if (perpixelAlpha && planeAlpha < 255) {
                NormalRgaSetAlphaEnInfo(&rgaReg, 1, 2, planeAlpha , 1, 4, 0);
            } else if (perpixelAlpha) {
                NormalRgaSetAlphaEnInfo(&rgaReg, 1, 1, planeAlpha , 1, 4, 0);
            } else {
                NormalRgaSetAlphaEnInfo(&rgaReg, 1, 3, planeAlpha , 1, 4, 0);
            }
            rgaReg.alpha_rop_flag |= (1 << 9);
            break;

        case 0x0100:
        default:
            break;
    }

    if (relSrcRect.hstride == 0) {
        relSrcRect.hstride = relSrcRect.height;
    }

    if (isRectValid(relSrc1Rect)) {
        if (relSrc1Rect.hstride == 0) {
            relSrc1Rect.hstride = relSrc1Rect.height;
        }
    }

    if (relDstRect.hstride == 0) {
        relDstRect.hstride = relDstRect.height;
    }

    if (src) {
        ret = checkRectForRga(relSrcRect);
        if (ret) {
            rga_error("Error srcRect");
            return ret;
        }
    }

    if (src1) {
        ret = checkRectForRga(relSrc1Rect);
        if (ret) {
            rga_error("Error src1Rect");
            return ret;
        }
    }

    if (dst) {
        ret = checkRectForRga(relDstRect);
        if (ret) {
            rga_error("Error dstRect");
            return ret;
        }
    }

    if (src1 && src) {
        hScale = (float)relSrcRect.width / relSrc1Rect.width;
        vScale = (float)relSrcRect.height / relSrc1Rect.height;
        if ((rotation == HAL_TRANSFORM_ROT_90) || (rotation == HAL_TRANSFORM_ROT_270)) {
            hScale = (float)relSrcRect.width / relSrc1Rect.height;
            vScale = (float)relSrcRect.height / relSrc1Rect.width;
        }

        if ((hScale < 1/16) || (hScale > 16) || (vScale < 1/16) || (vScale > 16)) {
            rga_error("Error scale HxV:[%f,%f]", hScale, vScale);
            return -EINVAL;
        }

        if ((ctx->mVersion <= 2.0) && ((hScale < 1/8) || (hScale > 8) || (vScale < 1/8) || (vScale > 8))) {
            rga_error("Error scale HxV:[%f,%f]", hScale, vScale);
            return -EINVAL;
        }

        if ((ctx->mVersion <= 1.003) && ((hScale < 1/2) || (vScale < 1/2))) {
            rga_error("Error scale HxV:[%f,%f] version:[%f]", hScale, vScale, ctx->mVersion);
            return -EINVAL;
        }
    } else if (src && dst) {
        hScale = (float)relSrcRect.width / relDstRect.width;
        vScale = (float)relSrcRect.height / relDstRect.height;
        if ((rotation == HAL_TRANSFORM_ROT_90) || (rotation == HAL_TRANSFORM_ROT_270)) {
            hScale = (float)relSrcRect.width / relDstRect.height;
            vScale = (float)relSrcRect.height / relDstRect.width;
        }

        if ((hScale < 1.0/16) || (hScale > 16) || (vScale < 1.0/16) || (vScale > 16)) {
            rga_error("Error scale HxV:[%f,%f]", hScale, vScale);
            return -EINVAL;
        }

        if ((ctx->mVersion < 2.0) && ((hScale < 1.0/8) || (hScale > 8) || (vScale < 1.0/8) || (vScale > 8))) {
            rga_error("Error scale HxV:[%f,%f]", hScale, vScale);
            return -EINVAL;
        }

        if ((ctx->mVersion <= 1.003) && ((hScale < 1.0/2) || (vScale < 1.0/2))) {
            rga_error("Error scale HxV:[%f,%f] version:[%f]", hScale, vScale, ctx->mVersion);
            return -EINVAL;
        }
    }

    scaleMode = 0;
    stretch = (hScale != 1.0f) || (vScale != 1.0f);
    if ((hScale < 1) || (vScale < 1)) {
        scaleMode = 2;
        if ((relSrcRect.format == RK_FORMAT_RGBA_8888) || (relSrcRect.format == RK_FORMAT_BGRA_8888)) {
            scaleMode = 0;
        }
    }

    if (is_out_log()) {
        rga_debug("scaleMode:[%d], stretch:[%d]",scaleMode,stretch);
    }

    switch (rotation & 0x0f) {
        case HAL_TRANSFORM_FLIP_H:
            orientation = 0;
            rotateMode = 2;
            srcVirW = relSrcRect.wstride;
            srcVirH = relSrcRect.hstride;
            srcXPos = relSrcRect.xoffset;
            srcYPos = relSrcRect.yoffset;
            srcActW = relSrcRect.width;
            srcActH = relSrcRect.height;

            src1VirW = relSrc1Rect.wstride;
            src1VirH = relSrc1Rect.hstride;
            src1XPos = relSrc1Rect.xoffset;
            src1YPos = relSrc1Rect.yoffset;
            src1ActW = relSrc1Rect.width;
            src1ActH = relSrc1Rect.height;

            dstVirW = relDstRect.wstride;
            dstVirH = relDstRect.hstride;
            dstXPos = relDstRect.xoffset;
            dstYPos = relDstRect.yoffset;
            dstActW = relDstRect.width;
            dstActH = relDstRect.height;
            break;

        case HAL_TRANSFORM_FLIP_V:
            orientation = 0;
            rotateMode = 3;
            srcVirW = relSrcRect.wstride;
            srcVirH = relSrcRect.hstride;
            srcXPos = relSrcRect.xoffset;
            srcYPos = relSrcRect.yoffset;
            srcActW = relSrcRect.width;
            srcActH = relSrcRect.height;

            src1VirW = relSrc1Rect.wstride;
            src1VirH = relSrc1Rect.hstride;
            src1XPos = relSrc1Rect.xoffset;
            src1YPos = relSrc1Rect.yoffset;
            src1ActW = relSrc1Rect.width;
            src1ActH = relSrc1Rect.height;

            dstVirW = relDstRect.wstride;
            dstVirH = relDstRect.hstride;
            dstXPos = relDstRect.xoffset;
            dstYPos = relDstRect.yoffset;
            dstActW = relDstRect.width;
            dstActH = relDstRect.height;
            break;

        case HAL_TRANSFORM_FLIP_H_V:
            orientation = 0;
            rotateMode = 4;
            srcVirW = relSrcRect.wstride;
            srcVirH = relSrcRect.hstride;
            srcXPos = relSrcRect.xoffset;
            srcYPos = relSrcRect.yoffset;
            srcActW = relSrcRect.width;
            srcActH = relSrcRect.height;

            src1VirW = relSrc1Rect.wstride;
            src1VirH = relSrc1Rect.hstride;
            src1XPos = relSrc1Rect.xoffset;
            src1YPos = relSrc1Rect.yoffset;
            src1ActW = relSrc1Rect.width;
            src1ActH = relSrc1Rect.height;

            dstVirW = relDstRect.wstride;
            dstVirH = relDstRect.hstride;
            dstXPos = relDstRect.xoffset;
            dstYPos = relDstRect.yoffset;
            dstActW = relDstRect.width;
            dstActH = relDstRect.height;
            break;

        case HAL_TRANSFORM_ROT_90:
            orientation = 90;
            rotateMode = 1;
            srcVirW = relSrcRect.wstride;
            srcVirH = relSrcRect.hstride;
            srcXPos = relSrcRect.xoffset;
            srcYPos = relSrcRect.yoffset;
            srcActW = relSrcRect.width;
            srcActH = relSrcRect.height;

            src1VirW = relSrc1Rect.wstride;
            src1VirH = relSrc1Rect.hstride;
            src1XPos = relSrc1Rect.xoffset;
            src1YPos = relSrc1Rect.yoffset;
            src1ActW = relSrc1Rect.height;
            src1ActH = relSrc1Rect.width;

            dstVirW = relDstRect.wstride;
            dstVirH = relDstRect.hstride;
            dstXPos = relDstRect.xoffset;
            dstYPos = relDstRect.yoffset;
            dstActW = relDstRect.height;
            dstActH = relDstRect.width;
            break;

        case HAL_TRANSFORM_ROT_180:
            orientation = 180;
            rotateMode = 1;
            srcVirW = relSrcRect.wstride;
            srcVirH = relSrcRect.hstride;
            srcXPos = relSrcRect.xoffset;
            srcYPos = relSrcRect.yoffset;
            srcActW = relSrcRect.width;
            srcActH = relSrcRect.height;

            src1VirW = relSrc1Rect.wstride;
            src1VirH = relSrc1Rect.hstride;
            src1XPos = relSrc1Rect.xoffset;
            src1YPos = relSrc1Rect.yoffset;
            src1ActW = relSrc1Rect.width;
            src1ActH = relSrc1Rect.height;

            dstVirW = relDstRect.wstride;
            dstVirH = relDstRect.hstride;
            dstXPos = relDstRect.xoffset;
            dstYPos = relDstRect.yoffset;
            dstActW = relDstRect.width;
            dstActH = relDstRect.height;
            break;

        case HAL_TRANSFORM_ROT_270:
            orientation = 270;
            rotateMode = 1;
            srcVirW = relSrcRect.wstride;
            srcVirH = relSrcRect.hstride;
            srcXPos = relSrcRect.xoffset;
            srcYPos = relSrcRect.yoffset;
            srcActW = relSrcRect.width;
            srcActH = relSrcRect.height;

            src1VirW = relSrc1Rect.wstride;
            src1VirH = relSrc1Rect.hstride;
            src1XPos = relSrc1Rect.xoffset;
            src1YPos = relSrc1Rect.yoffset;
            src1ActW = relSrc1Rect.height;
            src1ActH = relSrc1Rect.width;

            dstVirW = relDstRect.wstride;
            dstVirH = relDstRect.hstride;
            dstXPos = relDstRect.xoffset;
            dstYPos = relDstRect.yoffset;
            dstActW = relDstRect.height;
            dstActH = relDstRect.width;
            break;

        default:
            orientation = 0;
            rotateMode = stretch;
            srcVirW = relSrcRect.wstride;
            srcVirH = relSrcRect.hstride;
            srcXPos = relSrcRect.xoffset;
            srcYPos = relSrcRect.yoffset;
            srcActW = relSrcRect.width;
            srcActH = relSrcRect.height;

            src1VirW = relSrc1Rect.wstride;
            src1VirH = relSrc1Rect.hstride;
            src1XPos = relSrc1Rect.xoffset;
            src1YPos = relSrc1Rect.yoffset;
            src1ActW = relSrc1Rect.width;
            src1ActH = relSrc1Rect.height;

            dstVirW = relDstRect.wstride;
            dstVirH = relDstRect.hstride;
            dstXPos = relDstRect.xoffset;
            dstYPos = relDstRect.yoffset;
            dstActW = relDstRect.width;
            dstActH = relDstRect.height;
            break;
    }

    switch ((rotation & 0xF0) >> 4) {
        case HAL_TRANSFORM_FLIP_H:
            rotateMode |= (2 << 4);
            break;

        case HAL_TRANSFORM_FLIP_V:
            rotateMode |= (3 << 4);
            break;

        case HAL_TRANSFORM_FLIP_H_V:
            rotateMode |= (4 << 4);
            break;
    }

    clip.xmin = 0;
    clip.xmax = dstVirW - 1;
    clip.ymin = 0;
    clip.ymax = dstVirH - 1;

    if (NormalRgaIsRgbFormat(RkRgaGetRgaFormat(relSrcRect.format))
        && (RkRgaGetRgaFormat(relSrcRect.format) != RK_FORMAT_RGB_565
        || RkRgaGetRgaFormat(relSrcRect.format) != RK_FORMAT_BGR_565)
        && (RkRgaGetRgaFormat(relDstRect.format) == RK_FORMAT_RGB_565
        || RkRgaGetRgaFormat(relDstRect.format) == RK_FORMAT_BGR_565))
    {
        ditherEn = 1;
    } else {
        ditherEn = 0;
    }

    if (is_out_log()) {
        rga_debug("rgaVersion:[%lf], ditherEn:[%d]", ctx->mVersion, ditherEn);
    }

    if (ctx->mVersion <= (float)1.003) {
        srcMmuFlag = dstMmuFlag = src1MmuFlag = 1;

        NormalRgaSetSrcVirtualInfo(&rgaReg, (arch_type_t)srcBuf, (arch_type_t)srcBuf + srcVirW * srcVirH, (arch_type_t)srcBuf + srcVirW * srcVirH * 5/4, srcVirW, srcVirH, RkRgaGetRgaFormat(relSrcRect.format), 0);
        if (src1) {
            NormalRgaSetPatVirtualInfo(&rgaReg, (arch_type_t)src1Buf, (arch_type_t)src1Buf + src1VirW * src1VirH, (arch_type_t)src1Buf + src1VirW * src1VirH * 5/4, src1VirW, src1VirH, &clip, RkRgaGetRgaFormat(relSrc1Rect.format),0);
        }
        NormalRgaSetDstVirtualInfo(&rgaReg, (arch_type_t)dstBuf, (arch_type_t)dstBuf + dstVirW * dstVirH, (arch_type_t)dstBuf + dstVirW * dstVirH * 5/4, dstVirW, dstVirH, &clip, RkRgaGetRgaFormat(relDstRect.format),0);
    } else if (ctx->mVersion < (float)1.6) {
        if (srcFd != -1) {
            srcMmuFlag = srcType ? 1 : 0;
            if (src && (srcFd == src->fd)) {
                srcMmuFlag = src->mmuFlag ? 1 : 0;
            }

            NormalRgaSetSrcVirtualInfo(&rgaReg, 0, 0, 0, srcVirW, srcVirH, RkRgaGetRgaFormat(relSrcRect.format), 0);
            NormalRgaSetFdsOffsets(&rgaReg, srcFd, 0, 0, 0);
        } else {
            if (src && src->hnd) {
                srcMmuFlag = srcType ? 1 : 0;
            }

            if (src && (srcBuf == src->virAddr)) {
                srcMmuFlag = 1;
            }

            if (src && (srcBuf == src->phyAddr)) {
                srcMmuFlag = 0;
            }

            NormalRgaSetSrcVirtualInfo(&rgaReg, (arch_type_t)srcBuf, (arch_type_t)srcBuf + srcVirW * srcVirH, (arch_type_t)srcBuf + srcVirW * srcVirH * 5/4, srcVirW, srcVirH, RkRgaGetRgaFormat(relSrcRect.format), 0);
        }

        if (src1) {
            if (src1Fd != -1) {
                src1MmuFlag = src1Type ? 1 : 0;
                if (src1 && (src1Fd == src1->fd)) {
                    src1MmuFlag = src1->mmuFlag ? 1 : 0;
                }

                NormalRgaSetPatVirtualInfo(&rgaReg, 0, 0, 0, src1VirW, src1VirH, &clip, RkRgaGetRgaFormat(relSrc1Rect.format), 0);
                NormalRgaSetFdsOffsets(&rgaReg, 0, src1Fd, 0, 0);
            } else {
                if (src1 && src1->hnd) {
                    src1MmuFlag = src1Type ? 1 : 0;
                }

                if (src1 && (src1Buf == src1->virAddr)) {
                    src1MmuFlag = 1;
                }

                if (src1 && (src1Buf == src1->phyAddr)) {
                    src1MmuFlag = 0;
                }

                NormalRgaSetPatVirtualInfo(&rgaReg, (arch_type_t)src1Buf, (arch_type_t)src1Buf + src1VirW * src1VirH, (arch_type_t)src1Buf + src1VirW * src1VirH * 5/4, src1VirW, src1VirH, &clip, RkRgaGetRgaFormat(relSrc1Rect.format), 0);
            }
        }

        if (dstFd != -1) {
            dstMmuFlag = dstType ? 1 : 0;
            if (dst && (dstFd == dst->fd)) {
                dstMmuFlag = dst->mmuFlag ? 1 : 0;
            }

            NormalRgaSetDstVirtualInfo(&rgaReg, 0, 0, 0, dstVirW, dstVirH, &clip, RkRgaGetRgaFormat(relDstRect.format), 0);
            NormalRgaSetFdsOffsets(&rgaReg, 0, dstFd, 0, 0);
        } else {
            if (dst && dst->hnd) {
                dstMmuFlag = dstType ? 1 : 0;
            }

            if (dst && (dstBuf == dst->virAddr)) {
                dstMmuFlag = 1;
            }

            if (dst && (dstBuf == dst->phyAddr)) {
                dstMmuFlag = 0;
            }

            NormalRgaSetDstVirtualInfo(&rgaReg, (arch_type_t)dstBuf, (arch_type_t)dstBuf + dstVirW * dstVirH, (arch_type_t)dstBuf + dstVirW * dstVirH * 5/4, dstVirW, dstVirH, &clip, RkRgaGetRgaFormat(relDstRect.format), 0);
        }
    } else {
        if (src && src->hnd) {
            srcMmuFlag = srcType ? 1 : 0;
        }

        if (src && (srcBuf == src->virAddr)) {
            srcMmuFlag = 1;
        }

        if (src && (srcBuf == src->phyAddr)) {
            srcMmuFlag = 0;
        }

        if (srcFd != -1) {
            srcMmuFlag = srcType ? 1 : 0;
        }

        if (src && (srcFd == src->fd)) {
            srcMmuFlag = src->mmuFlag ? 1 : 0;
        }

        if (src1) {
            if (src1 && src1->hnd) {
                src1MmuFlag = src1Type ? 1 : 0;
            }

            if (src1 && (src1Buf == src1->virAddr)) {
                src1MmuFlag = 1;
            }

            if (src1 && (src1Buf == src1->phyAddr)) {
                src1MmuFlag = 0;
            }

            if (src1Fd != -1) {
                src1MmuFlag = src1Type ? 1 : 0;
            }

            if (src1 && (src1Fd == src1->fd)) {
                src1MmuFlag = src1->mmuFlag ? 1 : 0;
            }
        }

        if (dst && dst->hnd) {
            dstMmuFlag = dstType ? 1 : 0;
        }

        if (dst && (dstBuf == dst->virAddr)) {
            dstMmuFlag = 1;
        }

        if (dst && (dstBuf == dst->phyAddr)) {
            dstMmuFlag = 0;
        }

        if (dstFd != -1) {
            dstMmuFlag = dstType ? 1 : 0;
        }

        if (dst && (dstFd == dst->fd)) {
            dstMmuFlag = dst->mmuFlag ? 1 : 0;
        }

        NormalRgaSetSrcVirtualInfo(&rgaReg, srcFd != -1 ? srcFd : 0, (arch_type_t)srcBuf, (arch_type_t)srcBuf + srcVirW * srcVirH, srcVirW, srcVirH, RkRgaGetRgaFormat(relSrcRect.format), 0);
        if (src1) {
            NormalRgaSetPatVirtualInfo(&rgaReg, src1Fd != -1 ? src1Fd : 0, (arch_type_t)src1Buf, (arch_type_t)src1Buf + src1VirW * src1VirH, src1VirW, src1VirH, &clip, RkRgaGetRgaFormat(relSrc1Rect.format), 0);
        }
        NormalRgaSetDstVirtualInfo(&rgaReg, dstFd != -1 ? dstFd : 0, (arch_type_t)dstBuf, (arch_type_t)dstBuf + dstVirW * dstVirH, dstVirW, dstVirH, &clip, RkRgaGetRgaFormat(relDstRect.format), 0);
    }

    NormalRgaSetSrcActiveInfo(&rgaReg, srcActW, srcActH, srcXPos, srcYPos);
    NormalRgaSetDstActiveInfo(&rgaReg, dstActW, dstActH, dstXPos, dstYPos);
    if (src1) {
        NormalRgaSetPatActiveInfo(&rgaReg, src1ActW, src1ActH, src1XPos, src1YPos);
    }

    if (dst->color_space_mode & full_csc_mask) {
        NormalRgaFullColorSpaceConvert(&rgaReg, dst->color_space_mode);
    } else {
        if (src1) {
            if (NormalRgaIsYuvFormat(RkRgaGetRgaFormat(relSrcRect.format))
                && NormalRgaIsRgbFormat(RkRgaGetRgaFormat(relSrc1Rect.format))
                && NormalRgaIsRgbFormat(RkRgaGetRgaFormat(relDstRect.format)))
            {
                yuvToRgbMode |= 0x1 << 0;
            }

            if (NormalRgaIsYuvFormat(RkRgaGetRgaFormat(relSrcRect.format))
                && NormalRgaIsRgbFormat(RkRgaGetRgaFormat(relSrc1Rect.format))
                && NormalRgaIsYuvFormat(RkRgaGetRgaFormat(relDstRect.format)))
            {
                yuvToRgbMode |= 0x1 << 0;
                yuvToRgbMode |= 0x2 << 2;
            }

            if (NormalRgaIsRgbFormat(RkRgaGetRgaFormat(relSrcRect.format))
                && NormalRgaIsRgbFormat(RkRgaGetRgaFormat(relSrc1Rect.format))
                && NormalRgaIsYuvFormat(RkRgaGetRgaFormat(relDstRect.format)))
            {
                yuvToRgbMode |= 0x2 << 2;
            }
        } else {
            if (NormalRgaIsYuvFormat(RkRgaGetRgaFormat(relSrcRect.format))
                && NormalRgaIsRgbFormat(RkRgaGetRgaFormat(relDstRect.format)))
            {
                yuvToRgbMode |= 0x1 << 0;
            }

            if (NormalRgaIsRgbFormat(RkRgaGetRgaFormat(relSrcRect.format))
                && NormalRgaIsYuvFormat(RkRgaGetRgaFormat(relDstRect.format)))
            {
                yuvToRgbMode |= 0x2 << 2;
            }
        }

        if (dst->color_space_mode > 0) {
            yuvToRgbMode = dst->color_space_mode;
        }
    }

    NormalRgaSetBitbltMode(&rgaReg, scaleMode, rotateMode, orientation, ditherEn, 0, yuvToRgbMode);
    NormalRgaNNQuantizeMode(&rgaReg, dst);
    NormalRgaDitherMode(&rgaReg, dst, relDstRect.format);

    if (srcMmuFlag || dstMmuFlag) {
        NormalRgaMmuInfo(&rgaReg, 1, 0, 0, 0, 0, 2);
        NormalRgaMmuFlag(&rgaReg, srcMmuFlag, dstMmuFlag);
    }

    if (src1) {
        if (src1MmuFlag) {
            rgaReg.mmu_info.mmu_flag |= (0x1 << 11);
            rgaReg.mmu_info.mmu_flag |= (0x1 << 9);
        }

        rgaReg.bsfilter_flag = 1;
    }

    if (src->rop_code > 0) {
        rgaReg.rop_code = src->rop_code;
        rgaReg.alpha_rop_flag = 0x3;
        rgaReg.alpha_rop_mode = 0x1;
    }

    if (src->colorkey_en == 1) {
        rgaReg.alpha_rop_flag |= (1 << 9);
        switch (src->colorkey_mode) {
            case 0:
                NormalRgaSetSrcTransModeInfo(&rgaReg, 0, 1, 1, 1, 1, src->colorkey_min, src->colorkey_max, 1);
                break;

            case 1:
                NormalRgaSetSrcTransModeInfo(&rgaReg, 1, 1, 1, 1, 1, src->colorkey_min, src->colorkey_max, 1);
                break;
        }
    }

    memcpy(&rgaReg.mosaic_info, &src->mosaic_info, sizeof(struct rga_mosaic_info));
    memcpy(&rgaReg.osd_info, &src->osd_info, sizeof(struct rga_osd_info));
    memcpy(&rgaReg.pre_intr_info, &src->pre_intr, sizeof(src->pre_intr));

    if (is_out_log()) {
        rga_debug("srcMmuFlag:[%d], dstMmuFlag:[%d], rotateMode:[%d]", srcMmuFlag, dstMmuFlag, rotateMode);
        rga_debug("<<<<-------- rgaReg -------->>>>");
        NormalRgaLogOutRgaReq(rgaReg);
    }

    if ((src->sync_mode == RGA_BLIT_ASYNC) || (dst->sync_mode == RGA_BLIT_ASYNC)) {
        sync_mode = RGA_BLIT_ASYNC;
    }

    rgaReg.src.rd_mode = src->rd_mode ? src->rd_mode : raster_mode;
    rgaReg.dst.rd_mode = dst->rd_mode ? dst->rd_mode : raster_mode;
    if (src1) {
        rgaReg.pat.rd_mode = src1->rd_mode ? src1->rd_mode : raster_mode;
    }
    rgaReg.in_fence_fd = dst->in_fence_fd;
    rgaReg.core = dst->core;
    rgaReg.priority = dst->priority;

    if (dst->job_handle > 0) {
        im_rga_job_t *job = NULL;

        g_im2d_job_manager.mutex.lock();
        job = g_im2d_job_manager.job_map[dst->job_handle];
        if (job->task_count >= RGA_TASK_NUM_MAX) {
            rga_error("job:[%d] add task failed! too many tasks, count:[%d]", dst->job_handle, job->task_count);
            g_im2d_job_manager.mutex.unlock();

            return -errno;
        }

        job->req[job->task_count] = rgaReg;
        job->task_count++;
        g_im2d_job_manager.mutex.unlock();

        return 0;
    } else {
        void *ioc_req = NULL;

        switch (ctx->driver) {
            case RGA_DRIVER_IOC_RGA2:
                rga2_req compat_req;

                memset(&compat_req, 0x0, sizeof(compat_req));
                NormalRgaCompatModeConvertRga2(&compat_req, &rgaReg);
                ioc_req = &compat_req;
                break;

            case RGA_DRIVER_IOC_MULTI_RGA:
                ioc_req = &rgaReg;
                break;

            default:
                rga_error("unknow driver:[0x%X]", ctx->driver);
                return -errno;
        }

        do {
            ret = ioctl(ctx->rgaFd, sync_mode, ioc_req);
        } while ((ret == -1) && ((errno == EINTR) || (errno == 512)));

        if (ret) {
            rga_error("RGA_BLIT failed:[%s]", strerror(errno));
            return -errno;
        }
    }

    dst->out_fence_fd = rgaReg.out_fence_fd;
    return 0;
}

int RgaFlush()
{
    struct rgaContext *ctx = rgaCtx;

    if (!ctx) {
        rga_error("Try to use uninit rgaCtx:[%p]", ctx);
        return -ENODEV;
    }

    if (ioctl(ctx->rgaFd, RGA_FLUSH, NULL)) {
        rga_error("RGA_FLUSH failed:[%s]", strerror(errno));
        return -errno;
    }

    return 0;
}

int RgaCollorFill(rga_info *dst)
{
    RECT clip;
    int ret = 0;
    int dstFd = -1;
    void *dstBuf = NULL;
    COLOR_FILL fillColor;
    struct rga_req rgaReg;
    int scaleMode,ditherEn;
    int dstType,dstMmuFlag;
    int sync_mode = RGA_BLIT_SYNC;
    struct rgaContext *ctx = rgaCtx;
    unsigned int color = 0x00000000;
    rga_rect_t relDstRect,tmpDstRect;
    int dstVirW, dstVirH, dstActW, dstActH, dstXPos, dstYPos;

    if (!ctx) {
        rga_error("Try to use uninit rgaCtx:[%p]", ctx);
        return -ENODEV;
    }

    memset(&rgaReg, 0, sizeof(struct rga_req));
    dstType = dstMmuFlag = 0;

    if (!dst) {
        rga_error("dst:[%p]", dst);
        return -EINVAL;
    }

    if (dst) {
        color = dst->color;
        memcpy(&relDstRect, &dst->rect, sizeof(rga_rect_t));
    }

    dstFd = -1;

    if (relDstRect.hstride == 0) {
        relDstRect.hstride = relDstRect.height;
    }

    if (dst && (dstFd < 0)) {
        if (dst->handle > 0) {
            dstFd = dst->handle;
            rgaReg.handle_flag |= 1;
        } else {
            dstFd = dst->fd;
        }
    }

    if (dst && dst->phyAddr) {
        dstBuf = dst->phyAddr;
    } else if (dst && dst->virAddr) {
        dstBuf = dst->virAddr;
    }

    if (dst && (dstFd == -1) && !dstBuf) {
        rga_error("dst has not fd and address for render");
        return ret;
    }

    if (dst && (dstFd == 0) && !dstBuf) {
        rga_error("dstFd is zero, now driver not support");
        return -EINVAL;
    }

    relDstRect.format = RkRgaCompatibleFormat(relDstRect.format);
    if (dstFd == 0) {
        dstFd = -1;
    }

    if (relDstRect.hstride == 0) {
        relDstRect.hstride = relDstRect.height;
    }

    dstVirW = relDstRect.wstride;
    dstVirH = relDstRect.hstride;
    dstXPos = relDstRect.xoffset;
    dstYPos = relDstRect.yoffset;
    dstActW = relDstRect.width;
    dstActH = relDstRect.height;

    clip.xmin = 0;
    clip.xmax = dstActW - 1;
    clip.ymin = 0;
    clip.ymax = dstActH - 1;

    if (ctx->mVersion <= 1.003) {
        NormalRgaSetDstVirtualInfo(&rgaReg, (arch_type_t)dstBuf, (arch_type_t)dstBuf + dstVirW * dstVirH, (arch_type_t)dstBuf + dstVirW * dstVirH * 5/4, dstVirW, dstVirH, &clip, RkRgaGetRgaFormat(relDstRect.format), 0);
    } else if (ctx->mVersion < 1.6 ) {
        if (dstFd != -1) {
            dstMmuFlag = dstType ? 1 : 0;
            if (dst && (dstFd == dst->fd)) {
                dstMmuFlag = dst->mmuFlag ? 1 : 0;
            }

            NormalRgaSetDstVirtualInfo(&rgaReg, 0, 0, 0, dstVirW, dstVirH, &clip, RkRgaGetRgaFormat(relDstRect.format), 0);
            NormalRgaSetFdsOffsets(&rgaReg, 0, dstFd, 0, 0);
        } else {
            if (dst && dst->hnd) {
                dstMmuFlag = dstType ? 1 : 0;
            }

            if (dst && (dstBuf == dst->virAddr)) {
                dstMmuFlag = 1;
            }

            if (dst && (dstBuf == dst->phyAddr)) {
                dstMmuFlag = 0;
            }

            NormalRgaSetDstVirtualInfo(&rgaReg, (arch_type_t)dstBuf, (arch_type_t)dstBuf + dstVirW * dstVirH, (arch_type_t)dstBuf + dstVirW * dstVirH * 5/4, dstVirW, dstVirH, &clip, RkRgaGetRgaFormat(relDstRect.format), 0);
        }
    } else {
        if (dst && dst->hnd) {
            dstMmuFlag = dstType ? 1 : 0;
        }

        if (dst && (dstBuf == dst->virAddr)) {
            dstMmuFlag = 1;
        }

        if (dst && (dstBuf == dst->phyAddr)) {
            dstMmuFlag = 0;
        }

        if (dstFd != -1) {
            dstMmuFlag = dstType ? 1 : 0;
        }

        if (dst && (dstFd == dst->fd)) {
            dstMmuFlag = dst->mmuFlag ? 1 : 0;
        }

        NormalRgaSetDstVirtualInfo(&rgaReg, dstFd != -1 ? dstFd : 0, (arch_type_t)dstBuf, (arch_type_t)dstBuf + dstVirW * dstVirH, dstVirW, dstVirH, &clip, RkRgaGetRgaFormat(relDstRect.format), 0);
    }

    if (NormalRgaIsYuvFormat(RkRgaGetRgaFormat(relDstRect.format))) {
        rgaReg.yuv2rgb_mode |= 0x2 << 2;
    }

    if (dst->color_space_mode > 0) {
        rgaReg.yuv2rgb_mode = dst->color_space_mode;
    }
    NormalRgaSetDstActiveInfo(&rgaReg, dstActW, dstActH, dstXPos, dstYPos);

    memset(&fillColor, 0x0, sizeof(COLOR_FILL));
    NormalRgaSetColorFillMode(&rgaReg, &fillColor, 0, 0, color, 0, 0, 0, 0, 0);

    if (dstMmuFlag) {
        NormalRgaMmuInfo(&rgaReg, 1, 0, 0, 0, 0, 2);
        NormalRgaMmuFlag(&rgaReg, dstMmuFlag, dstMmuFlag);
    }

#if __DEBUG
    NormalRgaLogOutRgaReq(rgaReg);
#endif

    if (dst->sync_mode == RGA_BLIT_ASYNC) {
        sync_mode = dst->sync_mode;
    }

    rgaReg.dst.rd_mode = dst->rd_mode ? dst->rd_mode : raster_mode;
    rgaReg.in_fence_fd = dst->in_fence_fd;
    rgaReg.core = dst->core;
    rgaReg.priority = dst->priority;

    if (dst->job_handle > 0) {
        im_rga_job_t *job = NULL;

        g_im2d_job_manager.mutex.lock();
        job = g_im2d_job_manager.job_map[dst->job_handle];
        if (job->task_count >= RGA_TASK_NUM_MAX) {
            rga_error("job:[%d] add task failed! too many tasks, count:[%d]", dst->job_handle, job->task_count);
            g_im2d_job_manager.mutex.unlock();

            return -errno;
        }

        job->req[job->task_count] = rgaReg;
        job->task_count++;
        g_im2d_job_manager.mutex.unlock();

        return 0;
    } else {
        void *ioc_req = NULL;

        switch (ctx->driver) {
            case RGA_DRIVER_IOC_RGA2:
                rga2_req compat_req;

                memset(&compat_req, 0x0, sizeof(compat_req));
                NormalRgaCompatModeConvertRga2(&compat_req, &rgaReg);

                ioc_req = &compat_req;
                break;

            case RGA_DRIVER_IOC_MULTI_RGA:
                ioc_req = &rgaReg;
                break;

            default:
                rga_error("unknow driver:[0x%X]", ctx->driver);
                return -errno;
        }

        do {
            ret = ioctl(ctx->rgaFd, sync_mode, ioc_req);
        } while ((ret == -1) && ((errno == EINTR) || (errno == 512)));

        if (ret) {
            rga_error("RGA_COLORFILL failed:[%s]", strerror(errno));
            return -errno;
        }
    }

    return 0;
}

int RgaCollorPalette(rga_info *src, rga_info *dst, rga_info *lut)
{
    RECT clip;
    int ret = 0;
    int dstFd = -1;
    int srcFd = -1;
    int lutFd = -1;
    void *srcBuf = NULL;
    void *dstBuf = NULL;
    void *lutBuf = NULL;
    struct rga_req  Rga_Request;
    struct rga_req  Rga_Request2;
    struct rgaContext *ctx = rgaCtx;
    struct rga_req rgaReg,tmprgaReg;
    int srcVirW, srcVirH, srcActW, srcActH, srcXPos, srcYPos;
    int dstVirW, dstVirH, dstActW, dstActH, dstXPos, dstYPos;
    int lutVirW, lutVirH, lutActW, lutActH, lutXPos, lutYPos;
    int srcType, dstType, lutType, srcMmuFlag, dstMmuFlag, lutMmuFlag;
    rga_rect_t relSrcRect, tmpSrcRect, relDstRect, tmpDstRect, relLutRect, tmpLutRect;

    if (!ctx) {
        rga_error("Try to use uninit rgaCtx:[%p]",ctx);
        return -ENODEV;
    }

    memset(&rgaReg, 0, sizeof(struct rga_req));
    srcType = dstType = lutType = srcMmuFlag = dstMmuFlag = lutMmuFlag = 0;

    is_debug_log();
    if (is_out_log()) {
        rga_debug("<<<<-------- print rgaLog -------->>>>");
    }

    if (!src && !dst) {
        rga_error("src:[%p], dst:[%p], lut:[%p]", src, dst, lut);
        return -EINVAL;
    }

    if (src) {
        memcpy(&relSrcRect, &src->rect, sizeof(rga_rect_t));
    }

    if (dst) {
        memcpy(&relDstRect, &dst->rect, sizeof(rga_rect_t));
    }

    if (lut) {
        memcpy(&relLutRect, &lut->rect, sizeof(rga_rect_t));
    }
    srcFd = dstFd = lutFd = -1;

    if (is_out_log()) {
        rga_debug("src->hnd:[0x%lX], dst->hnd:[0x%lX], lut->hnd:[0x%lX]", (unsigned long)src->hnd, (unsigned long)dst->hnd, (unsigned long)lut->hnd);
        rga_debug("src: Fd:[%.2d], phyAddr:[%p], virAddr:[%p]", src->fd, src->phyAddr, src->virAddr);
        rga_debug("dst: Fd:[%.2d], phyAddr:[%p], virAddr:[%p]", dst->fd, dst->phyAddr, dst->virAddr);
        rga_debug("lut: Fd:[%.2d], phyAddr:[%p], virAddr:[%p]", lut->fd, lut->phyAddr, lut->virAddr);
    }

    if (lut) {
        if ((src->handle > 0) && (dst->handle > 0) && (lut->handle > 0)) {
            if ((src->handle <= 0) || (dst->handle <= 0) || (lut->handle <= 0)) {
                rga_error("librga only supports the use of handles only or no handles, [src,lut,dst] = [%d, %d, %d]", src->handle, lut->handle, dst->handle);
                return -EINVAL;
            }

            rgaReg.handle_flag |= 1;
        }
    } else if ((src->handle > 0) && (dst->handle > 0)) {
        if ((src->handle <= 0) || (dst->handle <= 0)) {
            rga_error("librga only supports the use of handles only or no handles, [src,dst] = [%d, %d]", src->handle, dst->handle);
            return -EINVAL;
        }

        rgaReg.handle_flag |= 1;
    }

    if (src && src->handle) {
        srcFd = src->handle;
    } else if (src && src->phyAddr) {
        srcBuf = src->phyAddr;
    } else if (src && (src->fd > 0)) {
        srcFd = src->fd;
        src->mmuFlag = 1;
    } else if (src && src->virAddr) {
        srcBuf = src->virAddr;
        src->mmuFlag = 1;
    }

    if ((srcFd == -1) && !srcBuf) {
        rga_error("src has not fd and address for render");
        return ret;
    }

    if ((srcFd == 0) && !srcBuf) {
        rga_error("srcFd is zero, now driver not support");
        return -EINVAL;
    }

    if (srcFd == 0) {
        srcFd = -1;
    }

    if (dst && dst->handle) {
        dstFd = dst->handle;
    } else if (dst && dst->phyAddr) {
        dstBuf = dst->phyAddr;
    } else if (dst && (dst->fd > 0)) {
        dstFd = dst->fd;
        dst->mmuFlag = 1;
    } else if (dst && dst->virAddr) {
        dstBuf = dst->virAddr;
        dst->mmuFlag = 1;
    }

    if ((dstFd == -1) && !dstBuf) {
        rga_error("dst has not fd and address for render");
        return ret;
    }

    if ((dstFd == 0) && !dstBuf) {
        rga_error("dstFd is zero, now driver not support");
        return -EINVAL;
    }

    if (dstFd == 0) {
        dstFd = -1;
    }

    if (lut && lut->handle) {
        lutFd = lut->handle;
    } else if (lut && lut->phyAddr) {
        lutBuf = lut->phyAddr;
    } else if (lut && (lut->fd > 0)) {
        lutFd = lut->fd;
        lut->mmuFlag = 1;
    } else if (lut && lut->virAddr) {
        lutBuf = lut->virAddr;
        lut->mmuFlag = 1;
    }

    if (lutFd == 0) {
        lutFd = -1;
    }

    if (is_out_log()) {
        rga_debug("src: Fd:[%.2d], buf:[%p], mmuFlag:[%d], mmuType:[%d]", srcFd, srcBuf, src->mmuFlag, srcType);
        rga_debug("dst: Fd:[%.2d], buf:[%p], mmuFlag:[%d], mmuType:[%d]", dstFd, dstBuf, dst->mmuFlag, dstType);
        rga_debug("lut: Fd:[%.2d], buf:[%p], mmuFlag:[%d], mmuType:[%d]", lutFd, lutBuf, lut->mmuFlag, lutType);
    }

    relSrcRect.format = RkRgaCompatibleFormat(relSrcRect.format);
    relDstRect.format = RkRgaCompatibleFormat(relDstRect.format);
    relLutRect.format = RkRgaCompatibleFormat(relLutRect.format);

#if defined(CONFIG_RK3126C)
    if ((relSrcRect.width == relDstRect.width)
        && (relSrcRect.height == relDstRect.height)
        && ((relSrcRect.width + 2 * relSrcRect.xoffset) == relSrcRect.wstride)
        && ((relSrcRect.height + 2 * relSrcRect.yoffset) == relSrcRect.hstride)
        && (relSrcRect.format == HAL_PIXEL_FORMAT_YCrCb_NV12)
        && ((relSrcRect.xoffset > 0) && (relSrcRect.yoffset > 0)))
    {
        relSrcRect.width += 4;
        relSrcRect.xoffset = (relSrcRect.wstride - relSrcRect.width) / 2;
    }
#endif

    if (relSrcRect.hstride == 0) {
        relSrcRect.hstride = relSrcRect.height;
    }

    if (relDstRect.hstride == 0) {
        relDstRect.hstride = relDstRect.height;
    }

    if (src) {
        ret = checkRectForRga(relSrcRect);
        if (ret) {
            rga_error("Error srcRect");
            return ret;
        }
    }

    if (dst) {
        ret = checkRectForRga(relDstRect);
        if (ret) {
            rga_error("Error dstRect");
            return ret;
        }
    }

    srcVirW = relSrcRect.wstride;
    srcVirH = relSrcRect.hstride;
    srcXPos = relSrcRect.xoffset;
    srcYPos = relSrcRect.yoffset;
    srcActW = relSrcRect.width;
    srcActH = relSrcRect.height;

    dstVirW = relDstRect.wstride;
    dstVirH = relDstRect.hstride;
    dstXPos = relDstRect.xoffset;
    dstYPos = relDstRect.yoffset;
    dstActW = relDstRect.width;
    dstActH = relDstRect.height;

    lutVirW = relLutRect.wstride;
    lutVirH = relLutRect.hstride;
    lutXPos = relLutRect.xoffset;
    lutYPos = relLutRect.yoffset;
    lutActW = relLutRect.width;
    lutActH = relLutRect.height;

    clip.xmin = 0;
    clip.xmax = dstVirW - 1;
    clip.ymin = 0;
    clip.ymax = dstVirH - 1;

    if (ctx->mVersion <= (float)1.003) {
        srcMmuFlag = dstMmuFlag = lutMmuFlag = 1;

        NormalRgaSetSrcVirtualInfo(&rgaReg, (unsigned long)srcBuf, (arch_type_t)srcBuf + srcVirW * srcVirH, (arch_type_t)srcBuf + srcVirW * srcVirH * 5/4, srcVirW, srcVirH, RkRgaGetRgaFormat(relSrcRect.format), 0);
        NormalRgaSetDstVirtualInfo(&rgaReg, (unsigned long)dstBuf, (arch_type_t)dstBuf + dstVirW * dstVirH, (arch_type_t)dstBuf + dstVirW * dstVirH * 5/4, dstVirW, dstVirH, &clip, RkRgaGetRgaFormat(relDstRect.format), 0);
        NormalRgaSetPatVirtualInfo(&rgaReg, (unsigned long)lutBuf, (arch_type_t)lutBuf + lutVirW * lutVirH, (arch_type_t)lutBuf + lutVirW * lutVirH * 5/4, lutVirW, lutVirH, &clip, RkRgaGetRgaFormat(relLutRect.format), 0);
    } else if (ctx->mVersion < (float)1.6) {
        if (srcFd != -1) {
            srcMmuFlag = srcType ? 1 : 0;
            if (src && (srcFd == src->fd)) {
                srcMmuFlag = src->mmuFlag ? 1 : 0;
            }

            NormalRgaSetSrcVirtualInfo(&rgaReg, 0, 0, 0, srcVirW, srcVirH, RkRgaGetRgaFormat(relSrcRect.format), 0);
            NormalRgaSetFdsOffsets(&rgaReg, srcFd, 0, 0, 0);
        } else {
            if (src && src->hnd) {
                srcMmuFlag = srcType ? 1 : 0;
            }

            if (src && (srcBuf == src->virAddr)) {
                srcMmuFlag = 1;
            }

            if (src && (srcBuf == src->phyAddr)) {
                srcMmuFlag = 0;
            }

            NormalRgaSetSrcVirtualInfo(&rgaReg, (arch_type_t)srcBuf, (arch_type_t)srcBuf + srcVirW * srcVirH, (arch_type_t)srcBuf + srcVirW * srcVirH * 5/4, srcVirW, srcVirH, RkRgaGetRgaFormat(relSrcRect.format), 0);
        }

        if (dstFd != -1) {
            dstMmuFlag = dstType ? 1 : 0;
            if (dst && (dstFd == dst->fd)) {
                dstMmuFlag = dst->mmuFlag ? 1 : 0;
            }

            NormalRgaSetDstVirtualInfo(&rgaReg, 0, 0, 0, dstVirW, dstVirH, &clip, RkRgaGetRgaFormat(relDstRect.format), 0);
            NormalRgaSetFdsOffsets(&rgaReg, 0, dstFd, 0, 0);
        } else {
            if (dst && dst->hnd) {
                dstMmuFlag = dstType ? 1 : 0;
            }

            if (dst && (dstBuf == dst->virAddr)) {
                dstMmuFlag = 1;
            }

            if (dst && (dstBuf == dst->phyAddr)) {
                dstMmuFlag = 0;
            }

            NormalRgaSetDstVirtualInfo(&rgaReg, (arch_type_t)dstBuf, (arch_type_t)dstBuf + dstVirW * dstVirH, (arch_type_t)dstBuf + dstVirW * dstVirH * 5/4, dstVirW, dstVirH, &clip, RkRgaGetRgaFormat(relDstRect.format), 0);
        }

        if (lutFd != -1) {
            lutMmuFlag = lutType ? 1 : 0;
            if (lut && (lutFd == lut->fd)) {
                lutMmuFlag = lut->mmuFlag ? 1 : 0;
            }

            NormalRgaSetPatVirtualInfo(&rgaReg, 0, 0, 0, lutVirW, lutVirH, &clip, RkRgaGetRgaFormat(relLutRect.format), 0);
            NormalRgaSetFdsOffsets(&rgaReg, 0, lutFd, 0, 0);
        } else {
            if (lut && lut->hnd) {
                lutMmuFlag = lutType ? 1 : 0;
            }

            if (lut && (lutBuf == lut->virAddr)) {
                lutMmuFlag = 1;
            }

            if (lut && (lutBuf == lut->phyAddr)) {
                lutMmuFlag = 0;
            }

            NormalRgaSetPatVirtualInfo(&rgaReg, (arch_type_t)lutBuf, (arch_type_t)lutBuf + lutVirW * lutVirH, (arch_type_t)lutBuf + lutVirW * lutVirH * 5/4, lutVirW, lutVirH, &clip, RkRgaGetRgaFormat(relLutRect.format), 0);
        }
    } else {
        if (src && src->hnd) {
            srcMmuFlag = srcType ? 1 : 0;
        }

        if (src && (srcBuf == src->virAddr)) {
            srcMmuFlag = 1;
        }

        if (src && (srcBuf == src->phyAddr)) {
            srcMmuFlag = 0;
        }

        if (srcFd != -1) {
            srcMmuFlag = srcType ? 1 : 0;
        }

        if (src && (srcFd == src->fd)) {
            srcMmuFlag = src->mmuFlag ? 1 : 0;
        }

        if (dst && dst->hnd) {
            dstMmuFlag = dstType ? 1 : 0;
        }

        if (dst && (dstBuf == dst->virAddr)) {
            dstMmuFlag = 1;
        }

        if (dst && (dstBuf == dst->phyAddr)) {
            dstMmuFlag = 0;
        }

        if (dstFd != -1) {
            dstMmuFlag = dstType ? 1 : 0;
        }

        if (dst && (dstFd == dst->fd)) {
            dstMmuFlag = dst->mmuFlag ? 1 : 0;
        }

        if (lut && lut->hnd) {
            lutMmuFlag = lutType ? 1 : 0;
        }

        if (lut && (lutBuf == lut->virAddr)) {
            lutMmuFlag = 1;
        }

        if (lut && (lutBuf == lut->phyAddr)) {
            lutMmuFlag = 0;
        }

        if (lutFd != -1) {
            lutMmuFlag = lutType ? 1 : 0;
        }

        if (lut && (lutFd == lut->fd)) {
            lutMmuFlag = lut->mmuFlag ? 1 : 0;
        }

        NormalRgaSetSrcVirtualInfo(&rgaReg, srcFd != -1 ? srcFd : 0, (arch_type_t)srcBuf, (arch_type_t)srcBuf + srcVirW * srcVirH, srcVirW, srcVirH, RkRgaGetRgaFormat(relSrcRect.format), 0);
        NormalRgaSetDstVirtualInfo(&rgaReg, dstFd != -1 ? dstFd : 0, (arch_type_t)dstBuf, (arch_type_t)dstBuf + dstVirW * dstVirH, dstVirW, dstVirH, &clip, RkRgaGetRgaFormat(relDstRect.format), 0);
        NormalRgaSetPatVirtualInfo(&rgaReg, lutFd != -1 ? lutFd : 0, (arch_type_t)lutBuf, (arch_type_t)lutBuf + lutVirW * lutVirH, lutVirW, lutVirH, &clip, RkRgaGetRgaFormat(relLutRect.format), 0);
    }

    NormalRgaSetSrcActiveInfo(&rgaReg, srcActW, srcActH, srcXPos, srcYPos);
    NormalRgaSetDstActiveInfo(&rgaReg, dstActW, dstActH, dstXPos, dstYPos);
    NormalRgaSetPatActiveInfo(&rgaReg, lutActW, lutActH, lutXPos, lutYPos);

    if (srcMmuFlag || dstMmuFlag || lutMmuFlag) {
        NormalRgaMmuInfo(&rgaReg, 1, 0, 0, 0, 0, 2);
        NormalRgaMmuFlag(&rgaReg, srcMmuFlag, dstMmuFlag);

        if (lutMmuFlag) {
            rgaReg.mmu_info.mmu_flag |= (0x1 << 11);
            rgaReg.mmu_info.mmu_flag |= (0x1 << 9);
        }
    }

    if (is_out_log()) {
        rga_debug("srcMmuFlag:[%d], dstMmuFlag:[%d], lutMmuFlag:[%d]", srcMmuFlag, dstMmuFlag, lutMmuFlag);
        rga_debug("<<<<-------- rgaReg -------->>>>");
        NormalRgaLogOutRgaReq(rgaReg);
    }

    switch (RkRgaGetRgaFormat(relSrcRect.format)) {
        case RK_FORMAT_BPP1:
            rgaReg.palette_mode = 0;
            break;

        case RK_FORMAT_BPP2:
            rgaReg.palette_mode = 1;
            break;

        case RK_FORMAT_BPP4:
            rgaReg.palette_mode = 2;
            break;

        case RK_FORMAT_BPP8:
            rgaReg.palette_mode = 3;
            break;
    }

    rgaReg.src.rd_mode = src->rd_mode ? src->rd_mode : raster_mode;
    rgaReg.dst.rd_mode = dst->rd_mode ? dst->rd_mode : raster_mode;
    if (lut) {
        rgaReg.pat.rd_mode = lut->rd_mode ? lut->rd_mode : raster_mode;
    }
    rgaReg.in_fence_fd = dst->in_fence_fd;
    rgaReg.core = dst->core;
    rgaReg.priority = dst->priority;

    if (!((lutFd == -1) && (lutBuf == NULL))) {
        rgaReg.fading.g = 0xff;
        rgaReg.render_mode = update_palette_table_mode;

        if (ioctl(ctx->rgaFd, RGA_BLIT_SYNC, &rgaReg) != 0) {
            rga_error("update palette table mode ioctl error");
            return -1;
        }
    }

    rgaReg.render_mode = color_palette_mode;
    rgaReg.endian_mode = 1;

    void *ioc_req = NULL;

    switch (ctx->driver) {
        case RGA_DRIVER_IOC_RGA2:
            rga2_req compat_req;

            memset(&compat_req, 0x0, sizeof(compat_req));
            NormalRgaCompatModeConvertRga2(&compat_req, &rgaReg);

            ioc_req = &compat_req;
            break;

        case RGA_DRIVER_IOC_MULTI_RGA:
            ioc_req = &rgaReg;
            break;

        default:
            rga_error("unknow driver:[0x%X]", ctx->driver);
            return -errno;
    }

    do {
        ret = ioctl(ctx->rgaFd, RGA_BLIT_SYNC, &ioc_req);
    } while ((ret == -1) && ((errno == EINTR) || (errno == 512)));

    if (ret) {
        rga_error("RGA_COLOR_PALETTE failed:[%s]", strerror(errno));
        return -errno;
    }

    return 0;
}

int NormalRgaScale()
{
    return 1;
}

int NormalRgaRoate()
{
    return 1;
}

int NormalRgaRoateScale()
{
    return 1;
}
