#include "drm.h"

#if USE_DRM

#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <inttypes.h>
#include <sys/types.h>

#include <libdrm/xf86drm.h>
#include <libdrm/xf86drmMode.h>
#include <libdrm/drm/drm_fourcc.h>

#define DIV_ROUND_UP(n, d)                  (((n) + (d) - 1) / (d))

#ifndef drm_print
#define drm_print(msg, ...)                 fprintf(stderr, msg, ##__VA_ARGS__);
#endif

#ifndef drm_error
#define drm_error(msg, ...)                 drm_print("\033[1;31m[DRM][E]: " msg "\033[0m\n", ##__VA_ARGS__)
#endif

#ifndef drm_info
#define drm_info(msg, ...)                  drm_print("\033[1;32m[DRM][I]: " msg "\033[0m\n", ##__VA_ARGS__)
#endif

#ifndef drm_debug
#define drm_debug(msg, ...)                 {}//drm_print("\033[1;34m[DRM][D]: " msg "\033[0m\n", ##__VA_ARGS__)
#endif

struct drm_buffer {
    uint32_t          handle;
    uint32_t          pitch;
    uint32_t          offset;
    unsigned long int size;
    void              *map;
    uint32_t          fb_handle;
};

struct drm_dev {
    int                fd;
    uint32_t           conn_id;
    uint32_t           enc_id;
    uint32_t           crtc_id;
    uint32_t           plane_id;
    uint32_t           crtc_idx;
    uint32_t           width, height;
    uint32_t           mmWidth;
    uint32_t           mmHeight;
    uint32_t           fourcc;
    drmModeModeInfo    mode;
    uint32_t           blob_id;
    drmModeCrtc        *saved_crtc;
    drmModeAtomicReq   *req;
    drmEventContext    drm_event_ctx;
    drmModePlane       *plane;
    drmModeCrtc        *crtc;
    drmModeConnector   *conn;
    uint32_t           count_plane_props;
    uint32_t           count_crtc_props;
    uint32_t           count_conn_props;
    drmModePropertyPtr plane_props[128];
    drmModePropertyPtr crtc_props[128];
    drmModePropertyPtr conn_props[128];
    struct drm_buffer  drm_bufs[2];
    struct drm_buffer  *cur_bufs[2];
} drm_dev;

static uint32_t get_plane_property_id(const char *name)
{
    uint32_t i;

    drm_debug("Find plane property:[%s]", name);
    for (i = 0; i < drm_dev.count_plane_props; ++i) {
        if (!strcmp(drm_dev.plane_props[i]->name, name)) {
            return drm_dev.plane_props[i]->prop_id;
        }
    }
    drm_debug("Unknown plane property:[%s]", name);

    return 0;
}

static uint32_t get_crtc_property_id(const char *name)
{
    uint32_t i;

    drm_debug("Find crtc property:[%s]", name);
    for (i = 0; i < drm_dev.count_crtc_props; ++i) {
        if (!strcmp(drm_dev.crtc_props[i]->name, name)) {
            return drm_dev.crtc_props[i]->prop_id;
        }
    }
    drm_debug("Unknown crtc property:[%s]", name);

    return 0;
}

static uint32_t get_conn_property_id(const char *name)
{
    uint32_t i;

    drm_debug("Find conn property:[%s]", name);
    for (i = 0; i < drm_dev.count_conn_props; ++i) {
        if (!strcmp(drm_dev.conn_props[i]->name, name)) {
            return drm_dev.conn_props[i]->prop_id;
        }
    }
    drm_debug("Unknown conn property:[%s]", name);

    return 0;
}

static void page_flip_handler(int fd, unsigned int sequence, unsigned int tv_sec, unsigned int tv_usec, void *user_data)
{
    LV_UNUSED(fd);
    LV_UNUSED(sequence);
    LV_UNUSED(tv_sec);
    LV_UNUSED(tv_usec);
    LV_UNUSED(user_data);

    drm_debug("flip");
}

static int drm_get_plane_props(void)
{
    uint32_t i;

    drmModeObjectPropertiesPtr props = drmModeObjectGetProperties(drm_dev.fd, drm_dev.plane_id, DRM_MODE_OBJECT_PLANE);
    if (!props) {
        drm_error("drmModeObjectGetProperties failed");
        return -1;
    }

    drm_debug("Found [%u] plane props", props->count_props);

    drm_dev.count_plane_props = props->count_props;
    for (i = 0; i < props->count_props; i++) {
        drm_dev.plane_props[i] = drmModeGetProperty(drm_dev.fd, props->props[i]);
        drm_debug("Added plane prop [%u:%s]", drm_dev.plane_props[i]->prop_id, drm_dev.plane_props[i]->name);
    }
    drmModeFreeObjectProperties(props);

    return 0;
}

static int drm_get_crtc_props(void)
{
    uint32_t i;

    drmModeObjectPropertiesPtr props = drmModeObjectGetProperties(drm_dev.fd, drm_dev.crtc_id, DRM_MODE_OBJECT_CRTC);
    if (!props) {
        drm_error("drmModeObjectGetProperties failed");
        return -1;
    }

    drm_debug("Found [%u] crtc props", props->count_props);

    drm_dev.count_crtc_props = props->count_props;
    for (i = 0; i < props->count_props; i++) {
        drm_dev.crtc_props[i] = drmModeGetProperty(drm_dev.fd, props->props[i]);
        drm_debug("Added crtc prop [%u:%s]", drm_dev.crtc_props[i]->prop_id, drm_dev.crtc_props[i]->name);
    }
    drmModeFreeObjectProperties(props);

    return 0;
}

static int drm_get_conn_props(void)
{
    uint32_t i;

    drmModeObjectPropertiesPtr props = drmModeObjectGetProperties(drm_dev.fd, drm_dev.conn_id, DRM_MODE_OBJECT_CONNECTOR);
    if (!props) {
        drm_error("drmModeObjectGetProperties failed");
        return -1;
    }

    drm_debug("Found [%u] connector props", props->count_props);

    drm_dev.count_conn_props = props->count_props;
    for (i = 0; i < props->count_props; i++) {
        drm_dev.conn_props[i] = drmModeGetProperty(drm_dev.fd, props->props[i]);
        drm_debug("Added connector prop [%u:%s]", drm_dev.conn_props[i]->prop_id, drm_dev.conn_props[i]->name);
    }
    drmModeFreeObjectProperties(props);

    return 0;
}

static int drm_add_plane_property(const char *name, uint64_t value)
{
    int ret;
    uint32_t prop_id = get_plane_property_id(name);

    if (!prop_id) {
        drm_error("Couldn't find plane prop:[%s]", name);
        return -1;
    }

    ret = drmModeAtomicAddProperty(drm_dev.req, drm_dev.plane_id, get_plane_property_id(name), value);
    if (ret < 0) {
        drm_error("drmModeAtomicAddProperty (%s:%" PRIu64 ") failed, return:[%d]", name, value, ret);
        return ret;
    }

    return 0;
}

static int drm_add_crtc_property(const char *name, uint64_t value)
{
    int ret;
    uint32_t prop_id = get_crtc_property_id(name);

    if (!prop_id) {
        drm_error("Couldn't find crtc prop:[%s]", name);
        return -1;
    }

    ret = drmModeAtomicAddProperty(drm_dev.req, drm_dev.crtc_id, get_crtc_property_id(name), value);
    if (ret < 0) {
        drm_error("drmModeAtomicAddProperty (%s:%" PRIu64 ") failed, return:[%d]", name, value, ret);
        return ret;
    }

    return 0;
}

static int drm_add_conn_property(const char *name, uint64_t value)
{
    int ret;
    uint32_t prop_id = get_conn_property_id(name);

    if (!prop_id) {
        drm_error("Couldn't find conn prop:[%s]", name);
        return -1;
    }

    ret = drmModeAtomicAddProperty(drm_dev.req, drm_dev.conn_id, get_conn_property_id(name), value);
    if (ret < 0) {
        drm_error("drmModeAtomicAddProperty (%s:%" PRIu64 ") failed, return:[%d]", name, value, ret);
        return ret;
    }

    return 0;
}

static int drm_dmabuf_set_plane(struct drm_buffer *buf)
{
    int ret;
    static int first = 1;
    uint32_t flags = DRM_MODE_PAGE_FLIP_EVENT;

    drm_dev.req = drmModeAtomicAlloc();

    // 在第一次原子提交时，做一个模式集
    if (first) {
        drm_add_conn_property("CRTC_ID", drm_dev.crtc_id);
        drm_add_crtc_property("MODE_ID", drm_dev.blob_id);
        drm_add_crtc_property("ACTIVE", 1);

        flags |= DRM_MODE_ATOMIC_ALLOW_MODESET;
        first = 0;
    }

    drm_add_plane_property("FB_ID", buf->fb_handle);
    drm_add_plane_property("CRTC_ID", drm_dev.crtc_id);
    drm_add_plane_property("SRC_X", 0);
    drm_add_plane_property("SRC_Y", 0);
    drm_add_plane_property("SRC_W", drm_dev.width << 16);
    drm_add_plane_property("SRC_H", drm_dev.height << 16);
    drm_add_plane_property("CRTC_X", 0);
    drm_add_plane_property("CRTC_Y", 0);
    drm_add_plane_property("CRTC_W", drm_dev.width);
    drm_add_plane_property("CRTC_H", drm_dev.height);

    ret = drmModeAtomicCommit(drm_dev.fd, drm_dev.req, flags, NULL);
    if (ret) {
        drm_error("drmModeAtomicCommit failed, errstr:[%s]", strerror(errno));
        drmModeAtomicFree(drm_dev.req);
        return ret;
    }

    return 0;
}

static int find_plane(unsigned int fourcc, uint32_t *plane_id, uint32_t crtc_id, uint32_t crtc_idx)
{
    LV_UNUSED(crtc_id);

    int ret = 0;
    unsigned int i, j;
    drmModePlanePtr plane;
    drmModePlaneResPtr planes;
    unsigned int format = fourcc;

    planes = drmModeGetPlaneResources(drm_dev.fd);
    if (!planes) {
        drm_error("drmModeGetPlaneResources failed");
        return -1;
    }

    drm_debug("found planes count:[%u]", planes->count_planes);

    for (i = 0; i < planes->count_planes; ++i) {
        plane = drmModeGetPlane(drm_dev.fd, planes->planes[i]);
        if (!plane) {
            drm_error("drmModeGetPlane failed, errstr:[%s]", strerror(errno));
            break;
        }

        if (!(plane->possible_crtcs & (1 << crtc_idx))) {
            drmModeFreePlane(plane);
            continue;
        }

        for (j = 0; j < plane->count_formats; ++j) {
            if (plane->formats[j] == format) {
                break;
            }
        }

        if (j == plane->count_formats) {
            drmModeFreePlane(plane);
            continue;
        }

        *plane_id = plane->plane_id;
        drmModeFreePlane(plane);

        drm_debug("found plane:[%d]", *plane_id);

        break;
    }

    if (i == planes->count_planes) {
        ret = -1;
    }

    drmModeFreePlaneResources(planes);
    return ret;
}

static int drm_find_connector(void)
{
    int i;
    drmModeRes *res;
    drmModeEncoder *enc = NULL;
    drmModeConnector *conn = NULL;

    if ((res = drmModeGetResources(drm_dev.fd)) == NULL) {
        drm_error("drmModeGetResources failed, fd:[%d] failed", drm_dev.fd);
        return -1;
    }

    if (res->count_crtcs <= 0) {
        drm_error("no Crtcs");
        goto free_res;
    }

    // 找到所有可用的连接器
    for (i = 0; i < res->count_connectors; i++) {
        conn = drmModeGetConnector(drm_dev.fd, res->connectors[i]);
        if (!conn) {
            continue;
        }

#if (DRM_CONNECTOR_ID >= 0)
        if (conn->connector_id != DRM_CONNECTOR_ID) {
            drmModeFreeConnector(conn);
            continue;
        }
#endif
        if (conn->connection == DRM_MODE_CONNECTED) {
            drm_debug("connector id:[%d] connected", conn->connector_id);
        } else if (conn->connection == DRM_MODE_DISCONNECTED) {
            drm_debug("connector id:[%d] disconnected", conn->connector_id);
        } else if (conn->connection == DRM_MODE_UNKNOWNCONNECTION) {
            drm_debug("connector id:[%d] unknownconnection", conn->connector_id);
        } else {
            drm_debug("connector id:[%d] unknown", conn->connector_id);
        }

        if ((conn->connection == DRM_MODE_CONNECTED) && (conn->count_modes > 0)) {
            break;
        }

        drmModeFreeConnector(conn);
        conn = NULL;
    };

    if (!conn) {
        drm_error("suitable connector not found");
        goto free_res;
    }

    drm_dev.conn_id = conn->connector_id;
    drm_debug("conn_id:[%d]", drm_dev.conn_id);
    drm_dev.mmWidth = conn->mmWidth;
    drm_dev.mmHeight = conn->mmHeight;

    memcpy(&drm_dev.mode, &conn->modes[0], sizeof(drmModeModeInfo));

    if (drmModeCreatePropertyBlob(drm_dev.fd, &drm_dev.mode, sizeof(drm_dev.mode), &drm_dev.blob_id)) {
        drm_error("error creating mode blob, errstr:[%m]");
        goto free_res;
    }

    drm_dev.width = conn->modes[0].hdisplay;
    drm_dev.height = conn->modes[0].vdisplay;

    for (i = 0 ; i < res->count_encoders; i++) {
        enc = drmModeGetEncoder(drm_dev.fd, res->encoders[i]);
        if (!enc) {
            continue;
        }

        drm_debug("enc:[%d] enc_id:[%d] conn enc_id:[%d]", i, enc->encoder_id, conn->encoder_id);

        if (enc->encoder_id == conn->encoder_id) {
            break;
        }

        drmModeFreeEncoder(enc);
        enc = NULL;
    }

    if (enc) {
        drm_dev.enc_id = enc->encoder_id;
        drm_debug("enc_id:[%d]", drm_dev.enc_id);
        drm_dev.crtc_id = enc->crtc_id;
        drm_debug("crtc_id:[%d]", drm_dev.crtc_id);
        drmModeFreeEncoder(enc);
    } else {
        // 编码器尚未关联，查找
        for (i = 0; i < conn->count_encoders; i++) {
            int crtc, crtc_id = -1;

            enc = drmModeGetEncoder(drm_dev.fd, conn->encoders[i]);
            if (!enc) {
                continue;
            }

            for (crtc = 0; crtc < res->count_crtcs; crtc++) {
                uint32_t crtc_mask = 1 << crtc;

                crtc_id = res->crtcs[crtc];
                drm_debug("enc_id:[%d] crtc:[%d] id:[%d] mask:[%X] possible:[%X]", enc->encoder_id, crtc, crtc_id, crtc_mask, enc->possible_crtcs);
                if (enc->possible_crtcs & crtc_mask) {
                    break;
                }
            }

            if (crtc_id > 0) {
                drm_dev.enc_id = enc->encoder_id;
                drm_debug("enc_id:[%d]", drm_dev.enc_id);
                drm_dev.crtc_id = crtc_id;
                drm_debug("crtc_id:[%d]", drm_dev.crtc_id);
                break;
            }

            drmModeFreeEncoder(enc);
            enc = NULL;
        }

        if (!enc) {
            drm_error("suitable encoder not found");
            goto free_res;
        }

        drmModeFreeEncoder(enc);
    }

    drm_dev.crtc_idx = UINT32_MAX;

    for (i = 0; i < res->count_crtcs; ++i) {
        if (drm_dev.crtc_id == res->crtcs[i]) {
            drm_dev.crtc_idx = i;
            break;
        }
    }

    if (drm_dev.crtc_idx == UINT32_MAX) {
        drm_error("drm: CRTC not found");
        goto free_res;
    }

    drm_debug("crtc_idx:[%d]", drm_dev.crtc_idx);
    return 0;

free_res:
    drmModeFreeResources(res);
    return -1;
}

static int drm_open(const char *path)
{
    int ret;
    int fd, flags;
    uint64_t has_dumb;

    fd = open(path, DRM_CLOEXEC | DRM_RDWR);
    if (fd < 0) {
        drm_error("cannot open \"%s\", errstr:[%m]", path);
        return -1;
    }

    if (((flags = fcntl(fd, F_GETFD)) < 0) || (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) < 0)) {
        drm_error("fcntl FD_CLOEXEC failed, errstr:[%m]");
        goto drm_error;
    }

    ret = drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb);
    if ((ret < 0) || (has_dumb == 0)) {
        drm_error("drmGetCap DRM_CAP_DUMB_BUFFER failed or \"%s\" doesn't have dumb buffer", path);
        goto drm_error;
    }

    return fd;

drm_error:
    close(fd);
    return -1;
}

static int drm_setup(unsigned int fourcc)
{
    int ret;
    const char *device_path = NULL;

    device_path = getenv("DRM_CARD");
    if (!device_path) {
        device_path = DRM_CARD;
    }

    drm_dev.fd = drm_open(device_path);
    if (drm_dev.fd < 0) {
        return -1;
    }

    ret = drmSetClientCap(drm_dev.fd, DRM_CLIENT_CAP_ATOMIC, 1);
    if (ret) {
        drm_error("No atomic modesetting support, errstr:[%s]", strerror(errno));
        goto drm_error;
    }

    ret = drm_find_connector();
    if (ret) {
        drm_error("available drm devices not found");
        goto drm_error;
    }

    ret = find_plane(fourcc, &drm_dev.plane_id, drm_dev.crtc_id, drm_dev.crtc_idx);
    if (ret) {
        drm_error("Cannot find plane");
        goto drm_error;
    }

    drm_dev.plane = drmModeGetPlane(drm_dev.fd, drm_dev.plane_id);
    if (!drm_dev.plane) {
        drm_error("Cannot get plane");
        goto drm_error;
    }

    drm_dev.crtc = drmModeGetCrtc(drm_dev.fd, drm_dev.crtc_id);
    if (!drm_dev.crtc) {
        drm_error("Cannot get crtc");
        goto drm_error;
    }

    drm_dev.conn = drmModeGetConnector(drm_dev.fd, drm_dev.conn_id);
    if (!drm_dev.conn) {
        drm_error("Cannot get connector");
        goto drm_error;
    }

    ret = drm_get_plane_props();
    if (ret) {
        drm_error("Cannot get plane props");
        goto drm_error;
    }

    ret = drm_get_crtc_props();
    if (ret) {
        drm_error("Cannot get crtc props");
        goto drm_error;
    }

    ret = drm_get_conn_props();
    if (ret) {
        drm_error("Cannot get connector props");
        goto drm_error;
    }

    drm_dev.drm_event_ctx.version = DRM_EVENT_CONTEXT_VERSION;
    drm_dev.drm_event_ctx.page_flip_handler = page_flip_handler;
    drm_dev.fourcc = fourcc;

    drm_info("Found plane_id:[%u] connector_id:[%d] crtc_id:[%d]", drm_dev.plane_id, drm_dev.conn_id, drm_dev.crtc_id);
    drm_info("wxh:[%dx%d]-(%dmm X %dmm) pixel format:[%c%c%c%c]", drm_dev.width, drm_dev.height, drm_dev.mmWidth, drm_dev.mmHeight, (fourcc >> 0) & 0xFF, (fourcc >> 8) & 0xFF, (fourcc >> 16) & 0xFF, (fourcc >> 24) & 0xFF);

    return 0;

drm_error:
    close(drm_dev.fd);
    return -1;
}

static int drm_allocate_dumb(struct drm_buffer *buf)
{
    int ret;
    struct drm_mode_map_dumb mreq;
    struct drm_mode_create_dumb creq;
    uint32_t handles[4] = {0}, pitches[4] = {0}, offsets[4] = {0};

    // 创建哑缓冲区
    memset(&creq, 0x00, sizeof(creq));
    creq.width = drm_dev.width;
    creq.height = drm_dev.height;
    creq.bpp = LV_COLOR_DEPTH;

    ret = drmIoctl(drm_dev.fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
    if (ret < 0) {
        drm_error("DRM_IOCTL_MODE_CREATE_DUMB failed, return:[%d], errstr:[%m]", ret);
        return -1;
    }

    buf->handle = creq.handle;
    buf->pitch = creq.pitch;
    buf->size = creq.size;
    drm_debug("pitch:[%d], size:[%d]", buf->pitch, (int)buf->size);

    // 为内存映射准备缓冲区
    memset(&mreq, 0x00, sizeof(mreq));
    mreq.handle = creq.handle;
    ret = drmIoctl(drm_dev.fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
    if (ret) {
        drm_error("DRM_IOCTL_MODE_MAP_DUMB failed, return:[%d], errstr:[%m]", ret);
        return -1;
    }

    buf->offset = mreq.offset;

    // 执行实际的内存映射
    buf->map = mmap(0, creq.size, PROT_READ | PROT_WRITE, MAP_SHARED, drm_dev.fd, mreq.offset);
    if (buf->map == MAP_FAILED) {
        drm_error("mmap fail, errstr:[%m]");
        return -1;
    }

    // 将帧缓冲区清除为0(=ARGB8888中的完全透明)
    memset(buf->map, 0x00, creq.size);

    // 为哑缓冲区创建帧缓冲区对象
    handles[0] = creq.handle;
    pitches[0] = creq.pitch;
    offsets[0] = 0;
    ret = drmModeAddFB2(drm_dev.fd, drm_dev.width, drm_dev.height, drm_dev.fourcc, handles, pitches, offsets, &buf->fb_handle, 0);
    if (ret) {
        drm_error("drmModeAddFB2 failed, return:[%d], errstr:[%m]", ret);
        return -1;
    }

    return 0;
}

static int drm_setup_buffers(void)
{
    int ret;

    // 分配DUMB缓冲区
    ret = drm_allocate_dumb(&drm_dev.drm_bufs[0]);
    if (ret) {
        return ret;
    }

    ret = drm_allocate_dumb(&drm_dev.drm_bufs[1]);
    if (ret) {
        return ret;
    }

    // 设置缓冲处理
    drm_dev.cur_bufs[0] = NULL;
    drm_dev.cur_bufs[1] = &drm_dev.drm_bufs[0];

    return 0;
}

void drm_wait_vsync(lv_disp_t *disp_drv)
{
    LV_UNUSED(disp_drv);

    int ret;
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(drm_dev.fd, &fds);

    do {
        ret = select(drm_dev.fd + 1, &fds, NULL, NULL, NULL);
    } while ((ret == -1) && (errno == EINTR));

    if (ret < 0) {
        drm_error("select failed, errstr:[%s]", strerror(errno));
        drmModeAtomicFree(drm_dev.req);
        drm_dev.req = NULL;

        return;
    }

    if (FD_ISSET(drm_dev.fd, &fds)) {
        drmHandleEvent(drm_dev.fd, &drm_dev.drm_event_ctx);
    }

    drmModeAtomicFree(drm_dev.req);
    drm_dev.req = NULL;
}

void drm_flush(lv_disp_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    int i, y;
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    struct drm_buffer *fbuf = drm_dev.cur_bufs[1];

    drm_debug("x1:[%d] x2:[%d] y1:[%d] y2:[%d] w:[%d] h:[%d]", area->x1, area->x2, area->y1, area->y2, w, h);

    // 部分更新
    if (((w != drm_dev.width) || (h != drm_dev.height)) && drm_dev.cur_bufs[0]) {
        memcpy(fbuf->map, drm_dev.cur_bufs[0]->map, fbuf->size);
    }

    for (y = 0, i = area->y1 ; i <= area->y2 ; ++i, ++y) {
        memcpy((uint8_t *)fbuf->map + (area->x1 * (LV_COLOR_SIZE / 8)) + (fbuf->pitch * i), (uint8_t *)color_p + (w * (LV_COLOR_SIZE / 8) * y), w * (LV_COLOR_SIZE / 8));
    }

    if (drm_dev.req) {
        drm_wait_vsync(disp_drv);
    }

    // 显示fbuf平面
    if (drm_dmabuf_set_plane(fbuf)) {
        drm_error("Flush fail");
        return;
    } else {
        drm_debug("Flush done");
    }

    if (!drm_dev.cur_bufs[0]) {
        drm_dev.cur_bufs[1] = &drm_dev.drm_bufs[1];
    } else {
        drm_dev.cur_bufs[1] = drm_dev.cur_bufs[0];
    }
    drm_dev.cur_bufs[0] = fbuf;

    lv_disp_flush_ready(disp_drv);
}

#if (LV_COLOR_DEPTH == 32)
#if defined(CONFIG_X86_64)
#define DRM_FOURCC      DRM_FORMAT_XRGB8888
#else
#define DRM_FOURCC      DRM_FORMAT_ARGB8888
#endif
#elif (LV_COLOR_DEPTH == 16)
#define DRM_FOURCC      DRM_FORMAT_RGB565
#else
#error LV_COLOR_DEPTH not supported
#endif

void drm_get_sizes(lv_coord_t *width, lv_coord_t *height, uint32_t *dpi)
{
    if (width) {
        *width = drm_dev.width;
    }

    if (height) {
        *height = drm_dev.height;
    }

    if (dpi && drm_dev.mmWidth) {
        *dpi = DIV_ROUND_UP(drm_dev.width * 25400, drm_dev.mmWidth * 1000);
    }
}

void drm_init(void)
{
    int ret;

    ret = drm_setup(DRM_FOURCC);
    if (ret) {
        close(drm_dev.fd);
        drm_dev.fd = -1;

        return;
    }

    ret = drm_setup_buffers();
    if (ret) {
        drm_error("DRM buffer allocation failed");
        close(drm_dev.fd);
        drm_dev.fd = -1;

        return;
    }

    drm_info("DRM subsystem and buffer mapped successfully");
}

void drm_exit(void)
{
    close(drm_dev.fd);
    drm_dev.fd = -1;
}

#endif
