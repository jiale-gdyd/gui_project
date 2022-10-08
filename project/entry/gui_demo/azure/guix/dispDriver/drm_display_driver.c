#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <linux/kconfig.h>

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

#if defined(CONFIG_DRM_DISP_DRIVER)
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <libdrm/drm_fourcc.h>
#endif

#include <gx_api.h>
#include <gx_display.h>

#include "drm_display_driver.h"

#if defined(CONFIG_DRM_DISP_DRIVER)
#ifndef DRM_CARD
#define DRM_CARD                            "/dev/dri/card0"
#endif

#ifndef DRM_CONNECTOR_ID
#define DRM_CONNECTOR_ID                    -1
#endif

#define DIV_ROUND_UP(n, d)                  (((n) + (d) - 1) / (d))
#define DUMP_FOURCC(fourcc)                 (fourcc >> 0) & 0xFF, (fourcc >> 8) & 0xFF, (fourcc >> 16) & 0xFF, (fourcc >> 24) & 0xFF

struct drm_buffer {
    uint32_t      handle;
    uint32_t      pitch;
    uint32_t      offset;
    unsigned long size;
    void          *map;
    uint32_t      fb_handle;
};

struct drm_dev {
    int                fd;
    uint32_t           conn_id;
    uint32_t           enc_id;
    uint32_t           crtc_id;
    uint32_t           plane_id;
    uint32_t           crtc_idx;
    uint32_t           width;
    uint32_t           height;
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
};

static struct drm_dev g_drmDev;
static unsigned int g_colorSize = 32;
static char *gx_canvas_memory = NULL;

static uint32_t drm_get_plane_property_id(const char *name)
{
    uint32_t index = 0;

    printf("[DRM] Find plane property:[%s]\n", name);
    for (index = 0; index < g_drmDev.count_conn_props; ++index) {
        if (!strcmp(g_drmDev.plane_props[index]->name, name)) {
            return g_drmDev.plane_props[index]->prop_id;
        }
    }
    printf("[DRM] Unknown plane property:[%s]\n", name);

    return 0;
}

static uint32_t drm_get_crtc_property_id(const char *name)
{
    uint32_t index = 0;

    printf("[DRM] Find crtc property:[%s]\n", name);
    for (index = 0; index < g_drmDev.count_conn_props; ++index) {
        if (!strcmp(g_drmDev.crtc_props[index]->name, name)) {
            return g_drmDev.crtc_props[index]->prop_id;
        }
    }
    printf("[DRM] Unknown crtc property:[%s]\n", name);

    return 0;
}

static uint32_t drm_get_conn_property_id(const char *name)
{
    uint32_t index = 0;

    printf("[DRM] Find conn property:[%s]\n", name);
    for (index = 0; index < g_drmDev.count_conn_props; ++index) {
        if (!strcmp(g_drmDev.conn_props[index]->name, name)) {
            return g_drmDev.conn_props[index]->prop_id;
        }
    }
    printf("[DRM] Unknow conn property:[%s]\n", name);

    return 0;
}

static void drm_page_flip_handler(int fd, unsigned int sequence, unsigned int tv_sec, unsigned int tv_usec, void *user_data)
{

}

static int drm_get_plane_props(void)
{
    uint32_t index = 0;

    drmModeObjectPropertiesPtr props = drmModeObjectGetProperties(g_drmDev.fd, g_drmDev.plane_id, DRM_MODE_OBJECT_PLANE);
    if (!props) {
        printf("[DRM] drmModeObjectGetProperties failed\n");
        return -1;
    }

    printf("[DRM] Found %u plane props\n", props->count_props);

    g_drmDev.count_plane_props = props->count_props;
    for (index = 0; index < props->count_props; ++index) {
        g_drmDev.plane_props[index] = drmModeGetProperty(g_drmDev.fd, props->props[index]);
        printf("[DRM] Added plane prop:[%u:%s]\n", g_drmDev.plane_props[index]->prop_id, g_drmDev.plane_props[index]->name);
    }
    drmModeFreeObjectProperties(props);

    return 0;
}

static int drm_get_crtc_props(void)
{
    uint32_t index;

    drmModeObjectPropertiesPtr props = drmModeObjectGetProperties(g_drmDev.fd, g_drmDev.crtc_id, DRM_MODE_OBJECT_CRTC);
    if (!props) {
        printf("[DRM] drmModeObjectGetProperties failed\n");
        return -1;
    }

    printf("[DRM] Found %u crtc props\n", props->count_props);

    g_drmDev.count_crtc_props = props->count_props;
    for (index = 0; index < props->count_props; ++index) {
        g_drmDev.crtc_props[index] = drmModeGetProperty(g_drmDev.fd, props->props[index]);
        printf("[DRM] Added crtc prop:[%u:%s]\n", g_drmDev.crtc_props[index]->prop_id, g_drmDev.crtc_props[index]->name);
    }
    drmModeFreeObjectProperties(props);

    return 0;
}

static int drm_get_conn_props(void)
{
    uint32_t index;

    drmModeObjectPropertiesPtr props = drmModeObjectGetProperties(g_drmDev.fd, g_drmDev.conn_id, DRM_MODE_OBJECT_CONNECTOR);
    if (!props) {
        printf("[DRM] drmModeObjectGetProperties failed\n");
        return -1;
    }

    printf("[DRM] Found %u connector props\n", props->count_props);

    g_drmDev.count_conn_props = props->count_props;
    for (index = 0; index < props->count_props; ++index) {
        g_drmDev.conn_props[index] = drmModeGetProperty(g_drmDev.fd, props->props[index]);
        printf("[DRM] Added connector prop:[%u:%s]\n", g_drmDev.conn_props[index]->prop_id, g_drmDev.conn_props[index]->name);
    }
    drmModeFreeObjectProperties(props);

    return 0;
}

static int drm_add_plane_property(const char *name, uint64_t value)
{
    int ret;
    uint32_t prop_id;

    prop_id = drm_get_plane_property_id(name);
    if (!prop_id) {
        printf("[DRM] Couldn't find plane prop:[%s]\n", name);
        return -1;
    }

    ret = drmModeAtomicAddProperty(g_drmDev.req, g_drmDev.plane_id, prop_id, value);
    if (ret < 0) {
        printf("[DRM] drmModeAtomicAddProperty (%s:%" PRIu64 ") failed, return:[%d]\n", name, value, ret);
        return ret;
    }

    return 0;
}

static int drm_add_crtc_property(const char *name, uint64_t value)
{
    int ret;
    uint32_t prop_id;

    prop_id = drm_get_crtc_property_id(name);
    if (!prop_id) {
        printf("[DRM] Couldn't find crtc prop:[%s]\n", name);
        return -1;
    }

    ret = drmModeAtomicAddProperty(g_drmDev.req, g_drmDev.crtc_id, prop_id, value);
    if (ret < 0) {
        printf("[DRM] drmModeAtomicAddProperty (%s:%" PRIu64 ") failed, return:[%d]\n", name, value, ret);
        return ret;
    }

    return 0;
}

static int drm_add_conn_property(const char *name, uint64_t value)
{
    int ret;
    uint32_t prop_id;

    prop_id = drm_get_conn_property_id(name);
    if (!prop_id) {
        printf("[DRM] Couldn't find conn prop:[%s]\n", name);
        return -1;
    }

    ret = drmModeAtomicAddProperty(g_drmDev.req, g_drmDev.conn_id, prop_id, value);
    if (ret < 0) {
        printf("[DRM] drmModeAtomicAddProperty (%s:%" PRIu64 ") failed, return:[%d]", name, value, ret);
        return ret;
    }

    return 0;
}

static int drm_dmabuf_set_plane(struct drm_buffer *buf)
{
    int ret;
    static int first = 1;
    uint32_t flags = DRM_MODE_PAGE_FLIP_EVENT;

    g_drmDev.req = drmModeAtomicAlloc();

    if (first) {
        drm_add_conn_property("CRTC_ID", g_drmDev.crtc_id);
        drm_add_crtc_property("MODE_ID", g_drmDev.blob_id);
        drm_add_crtc_property("ACTIVE", 1);

        flags |= DRM_MODE_ATOMIC_ALLOW_MODESET;
        first = 0;
    }

    drm_add_plane_property("FB_ID", buf->fb_handle);
    drm_add_plane_property("CRTC_ID", g_drmDev.crtc_id);
    drm_add_plane_property("SRC_X", 0);
    drm_add_plane_property("SRC_Y", 0);
    drm_add_plane_property("SRC_W", g_drmDev.width << 16);
    drm_add_plane_property("SRC_H", g_drmDev.height << 16);
    drm_add_plane_property("CRTC_X", 0);
    drm_add_plane_property("CRTC_Y", 0);
    drm_add_plane_property("CRTC_W", g_drmDev.width);
    drm_add_plane_property("CRTC_H", g_drmDev.height);

    // 提交数据进行显示
    ret = drmModeAtomicCommit(g_drmDev.fd, g_drmDev.req, flags, NULL);
    if (ret) {
        printf("[DRM] drmModeAtomicCommit failed: errstr:[%s]\n", strerror(errno));
        drmModeAtomicFree(g_drmDev.req);
        return ret;
    }

    return 0;
}

static int drm_find_plane(unsigned int fourcc, uint32_t *plane_id, uint32_t crtc_id, uint32_t crtc_idx)
{
    int ret = 0;
    unsigned int i, j;
    drmModePlanePtr plane;
    drmModePlaneResPtr planes;
    unsigned int format = fourcc;

    planes = drmModeGetPlaneResources(g_drmDev.fd);
    if (!planes) {
        printf("[DRM] drmModeGetPlaneResources failed\n");
        return -1;
    }

    printf("[DRM] found planes:[%u]\n", planes->count_planes);

    for (i = 0; i < planes->count_planes; ++i) {
        plane = drmModeGetPlane(g_drmDev.fd, planes->planes[i]);
        if (!plane) {
            printf("[DRM] drmModeGetPlane failed, errstr:[%s]\n", strerror(errno));
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

        printf("[DRM] found plane id:[%d]\n", *plane_id);
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

    if ((res = drmModeGetResources(g_drmDev.fd)) == NULL) {
        printf("[DRM] drmModeGetResources failed\n");
        return -1;
    }

    if (res->count_crtcs <= 0) {
        printf("[DRM] no Crtcs\n");
        goto free_res;
    }
    for (i = 0; i < res->count_connectors; i++) {
        conn = drmModeGetConnector(g_drmDev.fd, res->connectors[i]);
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
            printf("[DRM] connector:[%d] connected\n", conn->connector_id);
        } else if (conn->connection == DRM_MODE_DISCONNECTED) {
            printf("[DRM] connector:[%d] disconnected\n", conn->connector_id);
        } else if (conn->connection == DRM_MODE_UNKNOWNCONNECTION) {
            printf("[DRM] connector:[%d] unknownconnection\n", conn->connector_id);
        } else {
            printf("[DRM] connector:[%d] unknown\n", conn->connector_id);
        }

        if ((conn->connection == DRM_MODE_CONNECTED) && (conn->count_modes > 0)) {
            break;
        }

        drmModeFreeConnector(conn);
        conn = NULL;
    };

    if (!conn) {
        printf("[DRM] suitable connector not found\n");
        goto free_res;
    }

    g_drmDev.conn_id = conn->connector_id;
    printf("[DRM] conn_id:[%d]\n", g_drmDev.conn_id);
    g_drmDev.mmWidth = conn->mmWidth;
    g_drmDev.mmHeight = conn->mmHeight;

    memcpy(&g_drmDev.mode, &conn->modes[0], sizeof(drmModeModeInfo));

    if (drmModeCreatePropertyBlob(g_drmDev.fd, &g_drmDev.mode, sizeof(g_drmDev.mode), &g_drmDev.blob_id)) {
        printf("[DRM] error creating mode blob\n");
        goto free_res;
    }

    g_drmDev.width = conn->modes[0].hdisplay;
    g_drmDev.height = conn->modes[0].vdisplay;

    for (i = 0 ; i < res->count_encoders; i++) {
        enc = drmModeGetEncoder(g_drmDev.fd, res->encoders[i]);
        if (!enc) {
            continue;
        }

        printf("[DRM] enc:[%d], enc_id:[%d], conn enc_id:[%d]\n", i, enc->encoder_id, conn->encoder_id);

        if (enc->encoder_id == conn->encoder_id) {
            break;
        }

        drmModeFreeEncoder(enc);
        enc = NULL;
    }

    if (enc) {
        g_drmDev.enc_id = enc->encoder_id;
        printf("[DRM] enc_id:[%d]\n", g_drmDev.enc_id);

        g_drmDev.crtc_id = enc->crtc_id;
        printf("[DRM] crtc_id:[%d]\n", g_drmDev.crtc_id);

        drmModeFreeEncoder(enc);
    } else {
        for (i = 0; i < conn->count_encoders; i++) {
            int crtc, crtc_id = -1;

            enc = drmModeGetEncoder(g_drmDev.fd, conn->encoders[i]);
            if (!enc) {
                continue;
            }

            for (crtc = 0 ; crtc < res->count_crtcs; crtc++) {
                uint32_t crtc_mask = 1 << crtc;

                crtc_id = res->crtcs[crtc];
                printf("[DRM] enc_id:[%d], crtc:[%d], id:[%d], mask:[%X], possible:[%X]\n", enc->encoder_id, crtc, crtc_id, crtc_mask, enc->possible_crtcs);

                if (enc->possible_crtcs & crtc_mask) {
                    break;
                }
            }

            if (crtc_id > 0) {
                g_drmDev.enc_id = enc->encoder_id;
                printf("[DRM] enc_id:[%d]\n", g_drmDev.enc_id);

                g_drmDev.crtc_id = crtc_id;
                printf("[DRM] crtc_id:[%d]\n", g_drmDev.crtc_id);
                break;
            }

            drmModeFreeEncoder(enc);
            enc = NULL;
        }

        if (!enc) {
            printf("[DRM] suitable encoder not found\n");
            goto free_res;
        }

        drmModeFreeEncoder(enc);
    }

    g_drmDev.crtc_idx = -1;
    for (i = 0; i < res->count_crtcs; ++i) {
        if (g_drmDev.crtc_id == res->crtcs[i]) {
            g_drmDev.crtc_idx = i;
            break;
        }
    }

    if (g_drmDev.crtc_idx == -1) {
        printf("[DRM] CRTC not found\n");
        goto free_res;
    }

    printf("[DRM] crtc_idx:[%d]\n", g_drmDev.crtc_idx);
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

    fd = open(path, O_RDWR);
    if (fd < 0) {
        printf("[DRM] cannot open \"%s\"\n", path);
        return -1;
    }

    if (((flags = fcntl(fd, F_GETFD)) < 0) || (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) < 0)) {
        printf("[DRM] fcntl FD_CLOEXEC failed\n");
        goto err;
    }

    ret = drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb);
    if ((ret < 0) || (has_dumb == 0)) {
        printf("[DRM] drmGetCap DRM_CAP_DUMB_BUFFER failed or \"%s\" doesn't have dumb buffer\n", path);
        goto err;
    }

    return fd;

err:
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

    g_drmDev.fd = drm_open(device_path);
    if (g_drmDev.fd < 0) {
        return -1;
    }

    ret = drmSetClientCap(g_drmDev.fd, DRM_CLIENT_CAP_ATOMIC, 1);
    if (ret) {
        printf("[DRM] No atomic modesetting support, errstr:[%s]\n", strerror(errno));
        goto err;
    }

    ret = drm_find_connector();
    if (ret) {
        printf("[DRM] available drm devices not found\n");
        goto err;
    }

    ret = drm_find_plane(fourcc, &g_drmDev.plane_id, g_drmDev.crtc_id, g_drmDev.crtc_idx);
    if (ret) {
        printf("[DRM] Cannot find plane\n");
        goto err;
    }

    g_drmDev.plane = drmModeGetPlane(g_drmDev.fd, g_drmDev.plane_id);
    if (!g_drmDev.plane) {
        printf("[DRM] Cannot get plane\n");
        goto err;
    }

    g_drmDev.crtc = drmModeGetCrtc(g_drmDev.fd, g_drmDev.crtc_id);
    if (!g_drmDev.crtc) {
        printf("[DRM] Cannot get crtc\n");
        goto err;
    }

    g_drmDev.conn = drmModeGetConnector(g_drmDev.fd, g_drmDev.conn_id);
    if (!g_drmDev.conn) {
        printf("[DRM] Cannot get connector\n");
        goto err;
    }

    ret = drm_get_plane_props();
    if (ret) {
        printf("[DRM] Cannot get plane props\n");
        goto err;
    }

    ret = drm_get_crtc_props();
    if (ret) {
        printf("[DRM] Cannot get crtc props\n");
        goto err;
    }

    ret = drm_get_conn_props();
    if (ret) {
        printf("[DRM] Cannot get connector props\n");
        goto err;
    }

    g_drmDev.drm_event_ctx.version = DRM_EVENT_CONTEXT_VERSION;
    g_drmDev.drm_event_ctx.page_flip_handler = drm_page_flip_handler;
    g_drmDev.fourcc = fourcc;

    printf("[DRM] Found plane_id:[%u], connector_id:[%d], crtc_id:[%d]\n", g_drmDev.plane_id, g_drmDev.conn_id, g_drmDev.crtc_id);
    printf("[DRM] wxh:[%dx%d]-(%dmm X %dmm) pixel format %c%c%c%c\n", g_drmDev.width, g_drmDev.height, g_drmDev.mmWidth, g_drmDev.mmHeight, DUMP_FOURCC(fourcc));

    return 0;

err:
    close(g_drmDev.fd);
    return -1;
}

static int drm_allocate_dumb(struct drm_buffer *buf)
{
    int ret;
    struct drm_mode_map_dumb mreq;
    struct drm_mode_create_dumb creq;
    uint32_t handles[4] = {0}, pitches[4] = {0}, offsets[4] = {0};

    memset(&creq, 0x00, sizeof(creq));
    creq.width = g_drmDev.width;
    creq.height = g_drmDev.height;
    creq.bpp = g_colorSize;
    ret = drmIoctl(g_drmDev.fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
    if (ret < 0) {
        printf("[DRM] DRM_IOCTL_MODE_CREATE_DUMB failed\n");
        return -1;
    }

    buf->handle = creq.handle;
    buf->pitch = creq.pitch;
    printf("[DRM] pitch:[%u]\n", buf->pitch);
    buf->size = creq.size;
    printf("[DRM] size:[%lu]\n", buf->size);

    memset(&mreq, 0x00, sizeof(mreq));
    mreq.handle = creq.handle;
    ret = drmIoctl(g_drmDev.fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
    if (ret) {
        printf("[DRM] DRM_IOCTL_MODE_MAP_DUMB failed\n");
        return -1;
    }

    buf->offset = mreq.offset;
    buf->map = mmap(0, creq.size, PROT_READ | PROT_WRITE, MAP_SHARED, g_drmDev.fd, mreq.offset);
    if (buf->map == MAP_FAILED) {
        printf("[DRM] mmap failed\n");
        return -1;
    }

    memset(buf->map, 0x00, creq.size);

    handles[0] = creq.handle;
    pitches[0] = creq.pitch;
    offsets[0] = 0;
    ret = drmModeAddFB2(g_drmDev.fd, g_drmDev.width, g_drmDev.height, g_drmDev.fourcc, handles, pitches, offsets, &buf->fb_handle, 0);
    if (ret) {
        printf("[DRM] drmModeAddFB failed, errstr:[%s]\n", strerror(errno));
        return -1;
    }

    return 0;
}

static int drm_setup_buffers(void)
{
    int ret;

    ret = drm_allocate_dumb(&g_drmDev.drm_bufs[0]);
    if (ret) {
        return ret;
    }

    ret = drm_allocate_dumb(&g_drmDev.drm_bufs[1]);
    if (ret) {
        return ret;
    }

    g_drmDev.cur_bufs[0] = NULL;
    g_drmDev.cur_bufs[1] = &g_drmDev.drm_bufs[0];

    return 0;
}

static void drm_wait_vsync(void)
{
    int ret;
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(g_drmDev.fd, &fds);

    do {
        ret = select(g_drmDev.fd + 1, &fds, NULL, NULL, NULL);
    } while ((ret == -1) && (errno == EINTR));

    if (ret < 0) {
        printf("[DRM] select failed, errstr:[%s]", strerror(errno));
        drmModeAtomicFree(g_drmDev.req);
        g_drmDev.req = NULL;
        return;
    }

    if (FD_ISSET(g_drmDev.fd, &fds)) {
        drmHandleEvent(g_drmDev.fd, &g_drmDev.drm_event_ctx);
    }

    drmModeAtomicFree(g_drmDev.req);
    g_drmDev.req = NULL;
}

static void drm_get_sizes(uint32_t *width, uint32_t *height, uint32_t *dpi)
{
    if (width) {
        *width = g_drmDev.width;
    }

    if (height) {
        *height = g_drmDev.height;
    }

    if (dpi && g_drmDev.mmWidth) {
        *dpi = DIV_ROUND_UP(g_drmDev.width * 25400, g_drmDev.mmWidth * 1000);
    }
}

static int drm_init(unsigned int fourcc)
{
    int ret;

    ret = drm_setup(fourcc);
    if (ret) {
        close(g_drmDev.fd);
        g_drmDev.fd = -1;
        return -1;
    }

    ret = drm_setup_buffers();
    if (ret) {
        close(g_drmDev.fd);
        g_drmDev.fd = -1;
        return -1;
    }

    printf("[DRM] drm subsystem and buffer mapped successfully\n");
    return 0;
}

static void drm_exit(void)
{
    munmap(g_drmDev.drm_bufs[0].map, g_drmDev.drm_bufs[0].size);
    munmap(g_drmDev.drm_bufs[1].map, g_drmDev.drm_bufs[1].size);

    close(g_drmDev.fd);
    g_drmDev.fd = -1;
}

static void drm_flush(const char *gx_canvas_memory, struct GX_CANVAS_STRUCT *canvas)
{
    int i, y;
    struct drm_buffer *fbuf = g_drmDev.cur_bufs[1];
    uint32_t width = canvas->gx_canvas_x_resolution;
    uint32_t height = canvas->gx_canvas_y_resolution;
    int y1 = canvas->gx_canvas_dirty_area.gx_rectangle_top;
    int x1 = canvas->gx_canvas_dirty_area.gx_rectangle_left;
    int x2 = canvas->gx_canvas_dirty_area.gx_rectangle_right;
    int y2 = canvas->gx_canvas_dirty_area.gx_rectangle_bottom;

    if (gx_canvas_memory == NULL) {
        return;
    }

    printf("[DRM] left:[%d], top:[%d], right:[%d], bottom:[%d], width:[%d], height:[%d]\n",
        canvas->gx_canvas_dirty_area.gx_rectangle_left, canvas->gx_canvas_dirty_area.gx_rectangle_top,
        canvas->gx_canvas_dirty_area.gx_rectangle_right, canvas->gx_canvas_dirty_area.gx_rectangle_bottom,
        width, height);

    // 部分更新
    if (((width != g_drmDev.width) || (height != g_drmDev.height)) && g_drmDev.cur_bufs[0]) {
        memcpy(fbuf->map, g_drmDev.cur_bufs[0]->map, fbuf->size);
    }

    for (y = 0, i = y1 ; i <= y2 ; ++i, ++y) {
        memcpy((uint8_t *)fbuf->map + (x1 * (g_colorSize / 8)) + (fbuf->pitch * i), (uint8_t *)gx_canvas_memory + (width * (g_colorSize / 8) * y), width * (g_colorSize / 8));
    }

    if (g_drmDev.req) {
        drm_wait_vsync();
    }

    if (drm_dmabuf_set_plane(fbuf)) {
        printf("[DRM] Flush failed\n");
        return;
    } else {
        printf("[DRM] Flush done\n");
    }

    if (!g_drmDev.cur_bufs[0]) {
        g_drmDev.cur_bufs[1] = &g_drmDev.drm_bufs[1];
    } else {
        g_drmDev.cur_bufs[1] = g_drmDev.cur_bufs[0];
    }

    g_drmDev.cur_bufs[0] = fbuf;
}

static void _gx_copy_canvas_to_buffer_332rgb(char *dest, GX_CANVAS *canvas, GX_RECTANGLE *dirty_area)
{
    GX_VALUE x, y;
    GX_VALUE width, height;
    GX_UBYTE red, green, blue;
    GX_COLOR *put_ptr, *put_row;
    GX_UBYTE *read_ptr, *read_row;

    width = canvas->gx_canvas_x_resolution;
    height = canvas->gx_canvas_y_resolution;

    // 计算目标的起始地址
    put_row = (GX_COLOR *)dest;
    put_row += dirty_area->gx_rectangle_top * width;
    put_row += dirty_area->gx_rectangle_left;

    // 计算源的起始地址
    read_row = (GX_UBYTE *)canvas->gx_canvas_memory;
    read_row += dirty_area->gx_rectangle_top * width + dirty_area->gx_rectangle_left;

    for (y = dirty_area->gx_rectangle_top; y < dirty_area->gx_rectangle_bottom; y++) {
        put_ptr = put_row;
        read_ptr = read_row;

        for (x = dirty_area->gx_rectangle_left; x < dirty_area->gx_rectangle_right; x++) {
            red = (*read_ptr) & 0xe0;
            if (red & 0x20) {
                red |= 0x1f;
            }

            green = ((*read_ptr) & 0x1c) << 3;
            if (green & 0x20) {
                green |= 0x1f;
            }

            blue = ((*read_ptr) & 0x03) << 6;
            if (blue & 0x40) {
                blue |= 0x3f;
            }

            *put_ptr++ = ASSEMBLECOLOR_32BPP(red, green, blue);
            read_ptr++;
        }

        put_row += width;
        read_row += width;
    }
}

static void _gx_copy_canvas_to_buffer_565rgb(char *dest, GX_CANVAS *canvas, GX_RECTANGLE *dirty_area)
{
    GX_VALUE x, y;
    GX_VALUE width, height;
    GX_UBYTE red, green, blue;
    USHORT *read_ptr, *read_row;
    GX_COLOR *put_ptr, *put_row;

    width = canvas->gx_canvas_x_resolution;
    height = canvas->gx_canvas_y_resolution;

    // 计算目标的起始地址
    put_row = (GX_COLOR *)dest;
    put_row += dirty_area->gx_rectangle_top * width;
    put_row += dirty_area->gx_rectangle_left;

    // 计算源的起始地址
    read_row = (USHORT *)canvas->gx_canvas_memory;
    read_row += dirty_area->gx_rectangle_top * width + dirty_area->gx_rectangle_left;

    for (y = dirty_area->gx_rectangle_top; y < dirty_area->gx_rectangle_bottom; y++) {
        put_ptr = put_row;
        read_ptr = read_row;

        for (x = dirty_area->gx_rectangle_left; x < dirty_area->gx_rectangle_right; x++) {
            red = REDVAL_16BPP(*read_ptr) << 3;
            if (red & 0x08) {
                red |= 0x07;
            }

            green = GREENVAL_16BPP(*read_ptr) << 2;
            if (green & 0x04) {
                green |= 0x03;
            }

            blue = BLUEVAL_16BPP(*read_ptr) << 3;
            if (blue & 0x08) {
                blue |= 0x07;
            }

            *put_ptr++ = ASSEMBLECOLOR_32BPP(red, green, blue);
            read_ptr++;
        }

        put_row += width;
        read_row += width;
    }
}

static void _gx_copy_canvas_to_buffer_24xrgb(char *dest, GX_CANVAS *canvas, GX_RECTANGLE *dirty_area)
{
    GX_VALUE x, y;
    GX_VALUE width, height;
    GX_UBYTE red, green, blue;
    GX_COLOR *put_ptr, *put_row;
    GX_COLOR *read_ptr, *read_row;

    width = canvas->gx_canvas_x_resolution;
    height = canvas->gx_canvas_y_resolution;

    // 计算目标起始地址
    put_row = (GX_COLOR *)dest;
    put_row += dirty_area->gx_rectangle_top * width;
    put_row += dirty_area->gx_rectangle_left;

    // 计算源起始地址
    read_row = (GX_COLOR *)canvas->gx_canvas_memory;
    read_row += dirty_area->gx_rectangle_top * width + dirty_area->gx_rectangle_left;

    for (y = dirty_area->gx_rectangle_top; y < dirty_area->gx_rectangle_bottom; y++) {
        put_ptr = put_row;
        read_ptr = read_row;

        for (x = dirty_area->gx_rectangle_left; x < dirty_area->gx_rectangle_right; x++) {
            *put_ptr++ = *read_ptr++;
        }

        put_row += width;
        read_row += width;
    }
}

static void gx_drm_buffer_toggle(struct GX_CANVAS_STRUCT *canvas, GX_RECTANGLE *dirty_area)
{
    GX_VALUE format;
    GX_VALUE width, height;
    GX_RECTANGLE limitRect, overLapArea;

    width = canvas->gx_canvas_x_resolution;
    height = canvas->gx_canvas_y_resolution;

    gx_utility_rectangle_define(&limitRect, 0, 0, width - 1, height - 1);
    gx_utility_rectangle_overlap_detect(&limitRect, &canvas->gx_canvas_dirty_area, &overLapArea);

    if (canvas->gx_canvas_display) {
        format = canvas->gx_canvas_display->gx_display_color_format;
    }

    switch (format) {
        case GX_COLOR_FORMAT_8BIT_PACKED_PIXEL:
            _gx_copy_canvas_to_buffer_332rgb(gx_canvas_memory, canvas, &overLapArea);
            break;

        case GX_COLOR_FORMAT_565RGB:
            _gx_copy_canvas_to_buffer_565rgb(gx_canvas_memory, canvas, &overLapArea);
            break;

        case GX_COLOR_FORMAT_24XRGB:
        case GX_COLOR_FORMAT_32ARGB:
            _gx_copy_canvas_to_buffer_24xrgb(gx_canvas_memory, canvas, &overLapArea);
            break;

        default:
            printf("[DRM] Unknown color format:[%d]\n", format);
            return;
    }

    // 刷新缓存到DRM
    drm_flush(gx_canvas_memory, canvas);
}

static GX_BOOL _gx_allocate_x_canvas(GX_VALUE width, GX_VALUE height)
{
    if ((width > GX_MAX_DISPLAY_RESOLUTION) || (height > GX_MAX_DISPLAY_RESOLUTION)) {
        printf("[DRM] Invalid canvas size\n");
        return GX_FALSE;
    }

    UINT wHorzQuant = width * sizeof(GX_COLOR);
    gx_canvas_memory = (char *)malloc(wHorzQuant * height);
    if (!gx_canvas_memory) {
        printf("[DRM] Failed to allocate memory\n");
        return GX_FALSE;
    }

    return GX_TRUE;
}

static void _gx_x11_graphics_driver_setup(GX_DISPLAY *display)
{
    GX_VALUE width, height;

    width = display->gx_display_width;
    height = display->gx_display_height;

    _gx_allocate_x_canvas(width, height);
}
#endif

UINT gx_drm_graphics_driver_setup_24xrgb(GX_DISPLAY *display)
{
#if defined(CONFIG_DRM_DISP_DRIVER)
    static int drm_init_flags = 0;

    if (drm_init_flags == 0) {
        g_colorSize = 32;
        int ret = drm_init(DRM_FORMAT_ARGB8888);
        if (ret == 0) {
            drm_init_flags = 1;
        }
    }

    _gx_display_driver_24xrgb_setup(display, GX_NULL, gx_drm_buffer_toggle);
    _gx_x11_graphics_driver_setup(display);

    return GX_SUCCESS;
#else
    return GX_FAILURE;
#endif
}

UINT gx_drm_graphics_driver_setup_565rgb(GX_DISPLAY *display)
{
#if defined(CONFIG_DRM_DISP_DRIVER)
    static int drm_init_flags = 0;

    if (drm_init_flags == 0) {
        g_colorSize = 16;
        int ret = drm_init(DRM_FORMAT_RGB565);
        if (ret == 0) {
            drm_init_flags = 1;
        }
    }

    _gx_display_driver_565rgb_setup(display, GX_NULL, gx_drm_buffer_toggle);
    _gx_x11_graphics_driver_setup(display);

    return GX_SUCCESS;
#else
    return GX_FAILURE;
#endif
}

UINT gx_drm_graphics_driver_setup_332rgb(GX_DISPLAY *display)
{
#if defined(CONFIG_DRM_DISP_DRIVER)
    static int drm_init_flags = 0;

    if (drm_init_flags == 0) {
        g_colorSize = 8;
        int ret = drm_init(DRM_FORMAT_RGB332);
        if (ret == 0) {
            drm_init_flags = 1;
        }
    }

    _gx_display_driver_332rgb_setup(display, GX_NULL, gx_drm_buffer_toggle);
    _gx_x11_graphics_driver_setup(display);

    return GX_SUCCESS;
#else
    return GX_FAILURE;
#endif
}
