#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/mman.h>

#include <libdrm/xf86drm.h>
#include <libdrm/xf86drmMode.h>

#include "tkc/mem.h"
#include "base/lcd.h"
#include "awtk_global.h"
#include "base/bitmap.h"
#include "tkc/time_now.h"
#include "lcd_mem_others.h"
#include "blend/image_g2d.h"
#include "base/system_info.h"
#include "lcd/lcd_mem_special.h"

struct modeset_buf {
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t size;
    uint32_t handle;
    uint8_t  *map;
    uint32_t fb;
};

struct modeset_dev {
    struct modeset_dev *next;
    unsigned int       front_buf;
    struct modeset_buf bufs[2];
    drmModeModeInfo    mode;
    uint32_t           mode_list_size;
    drmModeModeInfo    *mode_list;
    uint32_t           conn;
    uint32_t           crtc;
    drmModeCrtc        *saved_crtc;

    bool               pflip_pending;
    bool               cleanup;
};

typedef struct _drm_info_t {
    int                fd;
    uint32_t           w;
    uint32_t           h;
    uint32_t           stride;
    struct modeset_dev *dev;
} drm_info_t;

static int s_ttyfd = -1;
static bool_t s_app_quited = FALSE;
static struct modeset_dev *modeset_list = NULL;

static int modeset_prepare(int fd);
static void modeset_cleanup(int fd);
static int modeset_open(int *out, const char *node);
static int modeset_create_fb(int fd, struct modeset_buf *buf);
static void modeset_destroy_fb(int fd, struct modeset_buf *buf);
static int modeset_setup_dev(int fd, drmModeRes *res, drmModeConnector *conn, struct modeset_dev *dev);
static int modeset_find_crtc(int fd, drmModeRes *res, drmModeConnector *conn, struct modeset_dev *dev);

static int modeset_open(int *out, const char *node)
{
    int fd, ret;
    uint64_t has_dumb;

    fd = open(node, O_RDWR | O_CLOEXEC);
    if (fd < 0) {
        ret = -errno;
        fprintf(stderr, "cannot open:[%s], errstr:[%m]\n", node);
        return ret;
    }

    if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || !has_dumb) {
        fprintf(stderr, "drm device:[%s] does not support dumb buffers\n", node);
        close(fd);
        return -EOPNOTSUPP;
    }

    *out = fd;
    return 0;
}

static int modeset_prepare(int fd)
{
    int ret;
    unsigned int i;
    drmModeRes *res;
    drmModeConnector *conn;
    struct modeset_dev *dev;

    res = drmModeGetResources(fd);
    if (!res) {
        fprintf(stderr, "cannot retrieve DRM resources, errno:[%d)], errstr:[%m]\n", errno);
        return -errno;
    }

    for (i = 0; i < res->count_connectors; ++i) {
        conn = drmModeGetConnector(fd, res->connectors[i]);
        if (!conn) {
            fprintf(stderr, "cannot retrieve DRM connector:[%u:%u], errno:[%d)], errstr:[%m]\n", i, res->connectors[i], errno);
            continue;
        }

        dev = malloc(sizeof(*dev));
        memset(dev, 0, sizeof(*dev));
        dev->conn = conn->connector_id;

        ret = modeset_setup_dev(fd, res, conn, dev);
        if (ret) {
            if (ret != -ENOENT) {
                errno = -ret;
                fprintf(stderr, "cannot setup device for connector:[%u:%u], errno:[%d)], errstr:[%m]\n", i, res->connectors[i], errno);
            }

            free(dev);
            drmModeFreeConnector(conn);
            continue;
        }

        drmModeFreeConnector(conn);
        dev->next = modeset_list;
        modeset_list = dev;
    }

    drmModeFreeResources(res);
    return 0;
}

static int modeset_fb(int fd, struct modeset_dev *dev, uint32_t connector_id)
{
    int ret = modeset_create_fb(fd, &dev->bufs[0]);
    if (ret) {
        fprintf(stderr, "cannot create framebuffer for connector_id:[%u]\n", connector_id);
        return ret;
    }

    ret = modeset_create_fb(fd, &dev->bufs[1]);
    if (ret) {
        fprintf(stderr, "cannot create framebuffer for connector_id:[%u]\n", connector_id);
        modeset_destroy_fb(fd, &dev->bufs[0]);
    }

    return ret;
}

static int modeset_setup_dev(int fd, drmModeRes *res, drmModeConnector *conn, struct modeset_dev *dev)
{
    int ret;

    if (conn->connection != DRM_MODE_CONNECTED) {
        fprintf(stderr, "ignoring unused connector_id:[%u]\n", conn->connector_id);
        return -ENOENT;
    }

    if (conn->count_modes == 0) {
        fprintf(stderr, "no valid mode for connector_id:[%u]\n", conn->connector_id);
        return -EFAULT;
    }

    dev->mode_list = TKMEM_ZALLOCN(drmModeModeInfo, conn->count_modes);
    dev->mode_list_size = conn->count_modes;
    for (int i = 0; i < conn->count_modes; i++) {
        memcpy(&(dev->mode_list[i]), &conn->modes[i], sizeof(dev->mode));
    }

    memcpy(&dev->mode, &conn->modes[0], sizeof(dev->mode));
    dev->bufs[0].width = conn->modes[0].hdisplay;
    dev->bufs[0].height = conn->modes[0].vdisplay;
    dev->bufs[1].width = conn->modes[0].hdisplay;
    dev->bufs[1].height = conn->modes[0].vdisplay;

    fprintf(stderr, "mode for connector_id:[%u] is wxh:[%ux%u]\n", conn->connector_id, dev->bufs[0].width, dev->bufs[0].height);

    ret = modeset_find_crtc(fd, res, conn, dev);
    if (ret) {
        fprintf(stderr, "no valid crtc for connector_id:[%u]\n", conn->connector_id);
        return ret;
    }

    return modeset_fb(fd, dev, conn->connector_id);
}

static int modeset_find_crtc(int fd, drmModeRes *res, drmModeConnector *conn, struct modeset_dev *dev)
{
    int32_t crtc;
    unsigned int i, j;
    drmModeEncoder *enc;
    struct modeset_dev *iter;

    if (conn->encoder_id) {
        enc = drmModeGetEncoder(fd, conn->encoder_id);
    } else {
        enc = NULL;
    }

    if (enc) {
        if (enc->crtc_id) {
            crtc = enc->crtc_id;
            for (iter = modeset_list; iter; iter = iter->next) {
                if (iter->crtc == crtc) {
                    crtc = -1;
                    break;
                }
            }

            if (crtc >= 0) {
                drmModeFreeEncoder(enc);
                dev->crtc = crtc;
                return 0;
            }
        }

        drmModeFreeEncoder(enc);
    }

    for (i = 0; i < conn->count_encoders; ++i) {
        enc = drmModeGetEncoder(fd, conn->encoders[i]);
        if (!enc) {
            fprintf(stderr, "cannot retrieve encoder:[%u:%u], errno:[%d)], errstr:[%m]\n", i, conn->encoders[i], errno);
            continue;
        }

        for (j = 0; j < res->count_crtcs; ++j) {
            if (!(enc->possible_crtcs & (1 << j))) continue;

            crtc = res->crtcs[j];
            for (iter = modeset_list; iter; iter = iter->next) {
                if (iter->crtc == crtc) {
                    crtc = -1;
                    break;
                }
            }

            if (crtc >= 0) {
                drmModeFreeEncoder(enc);
                dev->crtc = crtc;
                return 0;
            }
        }

        drmModeFreeEncoder(enc);
    }

    fprintf(stderr, "cannot find suitable CRTC for connector:[%u]\n", conn->connector_id);
    return -ENOENT;
}

static int modeset_create_fb(int fd, struct modeset_buf *buf)
{
    int ret;
    struct drm_mode_map_dumb mreq;
    struct drm_mode_create_dumb creq;
    struct drm_mode_destroy_dumb dreq;

    memset(&creq, 0, sizeof(creq));
    creq.width = buf->width;
    creq.height = buf->height;
    creq.bpp = 32;
    ret = drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
    if (ret < 0) {
        fprintf(stderr, "cannot create dumb buffer, errno:[%d)], errstr:[%m]\n", errno);
        return -errno;
    }

    buf->stride = creq.pitch;
    buf->size = creq.size;
    buf->handle = creq.handle;

    ret = drmModeAddFB(fd, buf->width, buf->height, 24, 32, buf->stride, buf->handle, &buf->fb);
    if (ret) {
        fprintf(stderr, "cannot create framebuffer, errno:[%d)], errstr:[%m]\n", errno);
        ret = -errno;
        goto err_destroy;
    }

    memset(&mreq, 0, sizeof(mreq));
    mreq.handle = buf->handle;
    ret = drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
    if (ret) {
        fprintf(stderr, "cannot map dumb buffer, errno:[%d)], errstr:[%m]\n", errno);
        ret = -errno;
        goto err_fb;
    }

    buf->map = mmap(0, buf->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, mreq.offset);
    if (buf->map == MAP_FAILED) {
        fprintf(stderr, "cannot mmap dumb buffer, errno:[%d)], errstr:[%m]\n", errno);
        ret = -errno;
        goto err_fb;
    }

    memset(buf->map, 0, buf->size);
    return 0;

err_fb:
    drmModeRmFB(fd, buf->fb);

err_destroy:
    memset(&dreq, 0, sizeof(dreq));
    dreq.handle = buf->handle;
    drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
    return ret;
}

static void modeset_destroy_fb(int fd, struct modeset_buf *buf)
{
    struct drm_mode_destroy_dumb dreq;

    munmap(buf->map, buf->size);
    drmModeRmFB(fd, buf->fb);

    memset(&dreq, 0, sizeof(dreq));
    dreq.handle = buf->handle;
    drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
}

static void modeset_page_flip_event(int fd, unsigned int frame, unsigned int sec, unsigned int usec, void *data)
{
    struct modeset_dev *dev = (struct modeset_dev *)data;
    dev->pflip_pending = false;
}

static void modeset_cleanup(int fd)
{
    int ret;
    drmEventContext ev;
    struct modeset_dev *iter;

    memset(&ev, 0, sizeof(ev));
    ev.version = DRM_EVENT_CONTEXT_VERSION;
    ev.page_flip_handler = modeset_page_flip_event;

    while (modeset_list) {
        iter = modeset_list;
        modeset_list = iter->next;

        iter->cleanup = true;
        fprintf(stderr, "wait for pending page-flip to complete...\n");

        while (iter->pflip_pending) {
            ret = drmHandleEvent(fd, &ev);
            if (ret) {
                break;
            }
        }

        if (!iter->pflip_pending) {
            drmModeSetCrtc(fd, iter->saved_crtc->crtc_id, iter->saved_crtc->buffer_id, iter->saved_crtc->x, iter->saved_crtc->y, &iter->conn, 1, &iter->saved_crtc->mode);
        }

        drmModeFreeCrtc(iter->saved_crtc);

        modeset_destroy_fb(fd, &iter->bufs[1]);
        modeset_destroy_fb(fd, &iter->bufs[0]);

        TKMEM_FREE(iter->mode_list);
        free(iter);
    }
}

static ret_t lcd_bgra8888_destroy(lcd_t *lcd)
{
    lcd_mem_special_t *special = (lcd_mem_special_t *)lcd;
    drm_info_t *info = (drm_info_t *)(special->ctx);

    modeset_cleanup(info->fd);
    TKMEM_FREE(info);

    return RET_OK;
}

static ret_t drm_vsync(int fd)
{
    fd_set fds;
    int ret = 0;
    struct timeval v;
    time_t start, cur;
    drmEventContext ev;

    FD_ZERO(&fds);
    srand(time(&start));
    memset(&v, 0, sizeof(v));
    memset(&ev, 0, sizeof(ev));

    ev.version = 2;
    ev.page_flip_handler = modeset_page_flip_event;

    while (time(&cur) < start + 5) {
        FD_SET(0, &fds);
        FD_SET(fd, &fds);
        v.tv_sec = start + 5 - cur;

        ret = select(fd + 1, &fds, NULL, NULL, &v);
        if (ret < 0) {
            fprintf(stderr, "select() failed with errno:[%d], errstr:[%m]\n", errno);
            break;
        } else if (FD_ISSET(fd, &fds)) {
            drmHandleEvent(fd, &ev);
            break;
        }
    }

    return RET_OK;
}

static ret_t lcd_bgra8888_flush(lcd_t *lcd)
{
    int ret = 0;
    bitmap_t src_img;
    bitmap_t dst_img;
    static int inited = 0;
    rect_t dr = {0, 0, lcd->w, lcd->h};
    lcd_mem_special_t *special = (lcd_mem_special_t *)lcd;
    drm_info_t *info = (drm_info_t *)(special->ctx);
    lcd_orientation_t o = system_info()->lcd_orientation;

    struct modeset_dev *dev = info->dev;
    struct modeset_buf *buf = &dev->bufs[dev->front_buf ^ 1];

    int fd = info->fd;
    int dst_line_length = info->stride;
    uint32_t *dst = (uint32_t *)(buf->map);
    uint32_t *src = (uint32_t *)lcd_mem_get_offline_fb((lcd_mem_t *)lcd);

    dev->front_buf ^= 1;
    if (inited) {
        dev->pflip_pending = true;
        drm_vsync(fd);
    } else {
        inited = 1;
    }

    if (o == LCD_ORIENTATION_0 || o == LCD_ORIENTATION_180) {
        bitmap_init(&dst_img, lcd->w, lcd->h, special->format, (uint8_t *)dst);
    } else {
        bitmap_init(&dst_img, lcd->h, lcd->w, special->format, (uint8_t *)dst);
    }

    bitmap_set_line_length(&dst_img, dst_line_length);
    bitmap_init(&src_img, lcd->w, lcd->h, special->format, (uint8_t *)src);

    if (o == LCD_ORIENTATION_0) {
        image_copy(&dst_img, &src_img, &dr, dr.x, dr.y);
    } else {
        image_rotate(&dst_img, &src_img, &dr, o);
    }

    ret = drmModePageFlip(fd, dev->crtc, buf->fb, DRM_MODE_PAGE_FLIP_EVENT, dev);
    if (src_img.buffer != NULL) {
        graphic_buffer_destroy(src_img.buffer);
    }

    if (dst_img.buffer != NULL) {
        graphic_buffer_destroy(dst_img.buffer);
    }

    if (ret) {
        log_error("cannot flip CRTC for connector:[%u], errno:[%d)], errstr:[%m]\n", dev->conn, errno);
        return RET_FAIL;
    } else {
        return RET_OK;
    }
}

static void on_app_exit(void)
{
    if (s_ttyfd >= 0) {
        ioctl(s_ttyfd, KDSETMODE, KD_TEXT);
    }

    close(s_ttyfd);
    log_debug("on_app_exit\n");
}

static void on_signal_int(int sig)
{
    s_app_quited = TRUE;
    tk_quit();
}

static ret_t drm_do_resize(lcd_t *lcd, wh_t w, wh_t h)
{
    int i = 0;
    int ret = 0;
    drmEventContext ev;
    int32_t find_number = -1;
    lcd_mem_special_t *special = (lcd_mem_special_t *)lcd;
    drm_info_t *info = (drm_info_t *)(special->ctx);
    struct modeset_dev *dev = info->dev;

    for (; i < dev->mode_list_size; i++) {
        if (dev->mode_list[i].hdisplay == w && dev->mode_list[i].vdisplay == h) {
            find_number = i;
            break;
        }
    }

    return_value_if_fail(find_number >= 0, RET_NOT_FOUND);

    memset(&ev, 0, sizeof(ev));
    ev.version = DRM_EVENT_CONTEXT_VERSION;
    ev.page_flip_handler = modeset_page_flip_event;

    while (dev->pflip_pending) {
        ret = drmHandleEvent(info->fd, &ev);
        if (ret) {
            break;
        }
    }

    if (!dev->pflip_pending) {
        drmModeSetCrtc(info->fd, dev->saved_crtc->crtc_id, dev->saved_crtc->buffer_id, dev->saved_crtc->x, dev->saved_crtc->y, &dev->conn, 1, &dev->saved_crtc->mode);
    }
    drmModeFreeCrtc(dev->saved_crtc);

    modeset_destroy_fb(info->fd, &(dev->bufs[1]));
    modeset_destroy_fb(info->fd, &(dev->bufs[0]));

    dev->bufs[0].width = w;
    dev->bufs[0].height = h;
    dev->bufs[1].width = w;
    dev->bufs[1].height = h;
    modeset_fb(info->fd, dev, dev->conn);

    dev->front_buf = 0;
    info->w = dev->bufs[0].width;
    info->h = dev->bufs[0].height;
    info->stride = dev->bufs[0].stride;

    dev->saved_crtc = drmModeGetCrtc(info->fd, dev->crtc);
    memcpy(&(dev->mode), &(dev->mode_list[find_number]), sizeof(dev->mode));
    ret = drmModeSetCrtc(info->fd, dev->crtc, dev->bufs[0].fb, 0, 0, &dev->conn, 1, &dev->mode);

    return RET_OK;
}

static ret_t (*lcd_drm_linux_resize_defalut)(lcd_t *lcd, wh_t w, wh_t h, uint32_t line_length);

static ret_t lcd_drm_linux_resize(lcd_t *lcd, wh_t w, wh_t h, uint32_t line_length)
{
    ret_t ret = RET_OK;

    ret = drm_do_resize(lcd, w, h);
    return_value_if_fail(ret == RET_OK, ret);

    if (lcd_drm_linux_resize_defalut != NULL) {
        lcd_drm_linux_resize_defalut(lcd, w, h, line_length);
    }

    return ret;
}

lcd_t *lcd_linux_drm_create(const char *card)
{
    struct modeset_buf *buf = NULL;
    struct modeset_dev *iter = NULL;
    int ret = 0, fd = 0, connector_id = -1;
    drm_info_t *drm = TKMEM_ZALLOC(drm_info_t);

    return_value_if_fail(drm != NULL, NULL);

    log_info("using card:[%s]\n", card);

    ret = modeset_open(&fd, card);
    log_info("modeset_open return:[%d]\n", ret);
    goto_error_if_fail(ret == 0);

    ret = modeset_prepare(fd);
    log_info("modeset_prepare return:[%d]\n", ret);
    goto_error_if_fail(ret == 0);

#ifdef DRM_DEVICE_CONNECTOR_ID
    connector_id = DRM_DEVICE_CONNECTOR_ID;
#endif

    for (iter = modeset_list; iter; iter = iter->next) {
        iter->saved_crtc = drmModeGetCrtc(fd, iter->crtc);
        buf = &iter->bufs[iter->front_buf];
        ret = drmModeSetCrtc(fd, iter->crtc, buf->fb, 0, 0, &iter->conn, 1, &iter->mode);

        if (ret) {
            fprintf(stderr, "cannot set CRTC for connector:[%u], errno:[%d], errstr:[%m]\n", iter->conn, errno);
        } else {
            drm->fd = fd;
            drm->dev = iter;
            drm->w = buf->width;
            drm->h = buf->height;
            drm->stride = buf->stride;
            if (connector_id == -1 || connector_id == iter->conn) {
                log_info("use dev:[%p], width:[%d], height:[%d]\n", iter, drm->w, drm->h);
                break;
            }
        }
    }

    s_ttyfd = open("/dev/tty1", O_RDWR);
    if (s_ttyfd >= 0) {
        ioctl(s_ttyfd, KDSETMODE, KD_GRAPHICS);
    }

    lcd_t *lcd = lcd_mem_special_create(drm->w, drm->h, BITMAP_FMT_BGRA8888, lcd_bgra8888_flush, NULL, lcd_bgra8888_destroy, drm);
    if (lcd) {
        lcd_drm_linux_resize_defalut = lcd->resize;
        lcd->resize = lcd_drm_linux_resize;
    }

    atexit(on_app_exit);
    signal(SIGINT, on_signal_int);
    return lcd;

error:
    TKMEM_FREE(drm);
    if (fd > 0) {
        close(fd);
    }

    return NULL;
}
