#ifndef LIBDRM_XF86DRMMODE_H
#define LIBDRM_XF86DRMMODE_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "drm.h"
#include "drm_mode.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define DRM_MODE_FEATURE_KMS                                    1
#define DRM_MODE_FEATURE_DIRTYFB                                1

typedef struct _drmModeRes {
    int      count_fbs;
    uint32_t *fbs;
    int      count_crtcs;
    uint32_t *crtcs;
    int      count_connectors;
    uint32_t *connectors;
    int      count_encoders;
    uint32_t *encoders;
    uint32_t min_width, max_width;
    uint32_t min_height, max_height;
} drmModeRes, *drmModeResPtr;

typedef struct _drmModeModeInfo {
    uint32_t clock;
    uint16_t hdisplay, hsync_start, hsync_end, htotal, hskew;
    uint16_t vdisplay, vsync_start, vsync_end, vtotal, vscan;
    uint32_t vrefresh;
    uint32_t flags;
    uint32_t type;
    char     name[DRM_DISPLAY_MODE_LEN];
} drmModeModeInfo, *drmModeModeInfoPtr;

typedef struct _drmModeFB {
    uint32_t fb_id;
    uint32_t width, height;
    uint32_t pitch;
    uint32_t bpp;
    uint32_t depth;

    uint32_t handle;
} drmModeFB, *drmModeFBPtr;

typedef struct _drmModeFB2 {
    uint32_t fb_id;
    uint32_t width, height;
    uint32_t pixel_format;
    uint64_t modifier;
    uint32_t flags;

    uint32_t handles[4];
    uint32_t pitches[4];
    uint32_t offsets[4];
} drmModeFB2, *drmModeFB2Ptr;

typedef struct drm_clip_rect drmModeClip, *drmModeClipPtr;

typedef struct _drmModePropertyBlob {
    uint32_t id;
    uint32_t length;
    void     *data;
} drmModePropertyBlobRes, *drmModePropertyBlobPtr;

typedef struct _drmModeProperty {
    uint32_t                      prop_id;
    uint32_t                      flags;
    char                          name[DRM_PROP_NAME_LEN];
    int                           count_values;
    uint64_t                      *values;
    int                           count_enums;
    struct drm_mode_property_enum *enums;
    int                           count_blobs;
    uint32_t                      *blob_ids;
} drmModePropertyRes, *drmModePropertyPtr;

static inline uint32_t drmModeGetPropertyType(const drmModePropertyRes *prop)
{
    return prop->flags & (DRM_MODE_PROP_LEGACY_TYPE | DRM_MODE_PROP_EXTENDED_TYPE);
}

static inline int drm_property_type_is(const drmModePropertyPtr property, uint32_t type)
{
    return drmModeGetPropertyType(property) == type;
}

typedef struct _drmModeCrtc {
    uint32_t        crtc_id;
    uint32_t        buffer_id;
    uint32_t        x, y;
    uint32_t        width, height;
    int             mode_valid;
    drmModeModeInfo mode;
    int             gamma_size;
} drmModeCrtc, *drmModeCrtcPtr;

typedef struct _drmModeEncoder {
    uint32_t encoder_id;
    uint32_t encoder_type;
    uint32_t crtc_id;
    uint32_t possible_crtcs;
    uint32_t possible_clones;
} drmModeEncoder, *drmModeEncoderPtr;

typedef enum {
    DRM_MODE_CONNECTED         = 1,
    DRM_MODE_DISCONNECTED      = 2,
    DRM_MODE_UNKNOWNCONNECTION = 3
} drmModeConnection;

typedef enum {
    DRM_MODE_SUBPIXEL_UNKNOWN        = 1,
    DRM_MODE_SUBPIXEL_HORIZONTAL_RGB = 2,
    DRM_MODE_SUBPIXEL_HORIZONTAL_BGR = 3,
    DRM_MODE_SUBPIXEL_VERTICAL_RGB   = 4,
    DRM_MODE_SUBPIXEL_VERTICAL_BGR   = 5,
    DRM_MODE_SUBPIXEL_NONE           = 6
} drmModeSubPixel;

typedef struct _drmModeConnector {
    uint32_t           connector_id;
    uint32_t           encoder_id;
    uint32_t           connector_type;
    uint32_t           connector_type_id;
    drmModeConnection  connection;
    uint32_t           mmWidth, mmHeight;
    drmModeSubPixel    subpixel;

    int                count_modes;
    drmModeModeInfoPtr modes;

    int                count_props;
    uint32_t           *props;
    uint64_t           *prop_values;

    int                count_encoders;
    uint32_t           *encoders;
} drmModeConnector, *drmModeConnectorPtr;

#define DRM_PLANE_TYPE_OVERLAY                                  0
#define DRM_PLANE_TYPE_PRIMARY                                  1
#define DRM_PLANE_TYPE_CURSOR                                   2

typedef struct _drmModeObjectProperties {
    uint32_t count_props;
    uint32_t *props;
    uint64_t *prop_values;
} drmModeObjectProperties, *drmModeObjectPropertiesPtr;

typedef struct _drmModeFormatModifierIterator {
    uint32_t fmt_idx, mod_idx;
    uint32_t fmt;
    uint64_t mod;
} drmModeFormatModifierIterator;

typedef struct _drmModePlane {
    uint32_t count_formats;
    uint32_t *formats;
    uint32_t plane_id;

    uint32_t crtc_id;
    uint32_t fb_id;

    uint32_t crtc_x, crtc_y;
    uint32_t x, y;

    uint32_t possible_crtcs;
    uint32_t gamma_size;
} drmModePlane, *drmModePlanePtr;

typedef struct _drmModePlaneRes {
    uint32_t count_planes;
    uint32_t *planes;
} drmModePlaneRes, *drmModePlaneResPtr;

extern void drmModeFreeModeInfo(drmModeModeInfoPtr ptr);
extern void drmModeFreeResources(drmModeResPtr ptr);
extern void drmModeFreeFB(drmModeFBPtr ptr);
extern void drmModeFreeFB2(drmModeFB2Ptr ptr);
extern void drmModeFreeCrtc(drmModeCrtcPtr ptr);
extern void drmModeFreeConnector(drmModeConnectorPtr ptr);
extern void drmModeFreeEncoder(drmModeEncoderPtr ptr);
extern void drmModeFreePlane(drmModePlanePtr ptr);
extern void drmModeFreePlaneResources(drmModePlaneResPtr ptr);

extern int drmIsKMS(int fd);
extern drmModeResPtr drmModeGetResources(int fd);

extern drmModeFBPtr drmModeGetFB(int fd, uint32_t bufferId);
extern drmModeFB2Ptr drmModeGetFB2(int fd, uint32_t bufferId);

extern int drmModeAddFB(int fd, uint32_t width, uint32_t height, uint8_t depth, uint8_t bpp, uint32_t pitch, uint32_t bo_handle, uint32_t *buf_id);
extern int drmModeAddFB2(int fd, uint32_t width, uint32_t height, uint32_t pixel_format, const uint32_t bo_handles[4], const uint32_t pitches[4], const uint32_t offsets[4], uint32_t *buf_id, uint32_t flags);

int drmModeAddFB2WithModifiers(int fd, uint32_t width, uint32_t height, uint32_t pixel_format, const uint32_t bo_handles[4], const uint32_t pitches[4], const uint32_t offsets[4], const uint64_t modifier[4], uint32_t *buf_id, uint32_t flags);

extern int drmModeRmFB(int fd, uint32_t bufferId);
extern int drmModeDirtyFB(int fd, uint32_t bufferId, drmModeClipPtr clips, uint32_t num_clips);

extern drmModeCrtcPtr drmModeGetCrtc(int fd, uint32_t crtcId);
int drmModeSetCrtc(int fd, uint32_t crtcId, uint32_t bufferId, uint32_t x, uint32_t y, uint32_t *connectors, int count, drmModeModeInfoPtr mode);

int drmModeSetCursor(int fd, uint32_t crtcId, uint32_t bo_handle, uint32_t width, uint32_t height);
int drmModeSetCursor2(int fd, uint32_t crtcId, uint32_t bo_handle, uint32_t width, uint32_t height, int32_t hot_x, int32_t hot_y);

int drmModeMoveCursor(int fd, uint32_t crtcId, int x, int y);

drmModeEncoderPtr drmModeGetEncoder(int fd, uint32_t encoder_id);

extern drmModeConnectorPtr drmModeGetConnector(int fd, uint32_t connectorId);
extern drmModeConnectorPtr drmModeGetConnectorCurrent(int fd, uint32_t connector_id);

extern uint32_t drmModeConnectorGetPossibleCrtcs(int fd, const drmModeConnector *connector);

extern int drmModeAttachMode(int fd, uint32_t connectorId, drmModeModeInfoPtr mode_info);
extern int drmModeDetachMode(int fd, uint32_t connectorId, drmModeModeInfoPtr mode_info);

extern drmModePropertyPtr drmModeGetProperty(int fd, uint32_t propertyId);
extern void drmModeFreeProperty(drmModePropertyPtr ptr);

extern drmModePropertyBlobPtr drmModeGetPropertyBlob(int fd, uint32_t blob_id);
extern bool drmModeFormatModifierBlobIterNext(const drmModePropertyBlobRes *blob, drmModeFormatModifierIterator *iter);
extern void drmModeFreePropertyBlob(drmModePropertyBlobPtr ptr);
extern int drmModeConnectorSetProperty(int fd, uint32_t connector_id, uint32_t property_id, uint64_t value);
extern int drmCheckModesettingSupported(const char *busid);

extern int drmModeCrtcSetGamma(int fd, uint32_t crtc_id, uint32_t size, uint16_t *red, uint16_t *green, uint16_t *blue);
extern int drmModeCrtcGetGamma(int fd, uint32_t crtc_id, uint32_t size, uint16_t *red, uint16_t *green, uint16_t *blue);

extern int drmModePageFlip(int fd, uint32_t crtc_id, uint32_t fb_id, uint32_t flags, void *user_data);
extern int drmModePageFlipTarget(int fd, uint32_t crtc_id, uint32_t fb_id, uint32_t flags, void *user_data, uint32_t target_vblank);

extern drmModePlaneResPtr drmModeGetPlaneResources(int fd);
extern drmModePlanePtr drmModeGetPlane(int fd, uint32_t plane_id);
extern int drmModeSetPlane(int fd, uint32_t plane_id, uint32_t crtc_id, uint32_t fb_id, uint32_t flags, int32_t crtc_x, int32_t crtc_y, uint32_t crtc_w, uint32_t crtc_h, uint32_t src_x, uint32_t src_y, uint32_t src_w, uint32_t src_h);

extern drmModeObjectPropertiesPtr drmModeObjectGetProperties(int fd, uint32_t object_id, uint32_t object_type);
extern void drmModeFreeObjectProperties(drmModeObjectPropertiesPtr ptr);
extern int drmModeObjectSetProperty(int fd, uint32_t object_id, uint32_t object_type, uint32_t property_id, uint64_t value);

typedef struct _drmModeAtomicReq drmModeAtomicReq, *drmModeAtomicReqPtr;

extern drmModeAtomicReqPtr drmModeAtomicAlloc(void);
extern drmModeAtomicReqPtr drmModeAtomicDuplicate(const drmModeAtomicReqPtr req);
extern int drmModeAtomicMerge(drmModeAtomicReqPtr base, const drmModeAtomicReqPtr augment);
extern void drmModeAtomicFree(drmModeAtomicReqPtr req);
extern int drmModeAtomicGetCursor(const drmModeAtomicReqPtr req);
extern void drmModeAtomicSetCursor(drmModeAtomicReqPtr req, int cursor);
extern int drmModeAtomicAddProperty(drmModeAtomicReqPtr req, uint32_t object_id, uint32_t property_id, uint64_t value);
extern int drmModeAtomicCommit(int fd, const drmModeAtomicReqPtr req, uint32_t flags, void *user_data);

extern int drmModeCreatePropertyBlob(int fd, const void *data, size_t size, uint32_t *id);
extern int drmModeDestroyPropertyBlob(int fd, uint32_t id);

extern int drmModeCreateLease(int fd, const uint32_t *objects, int num_objects, int flags, uint32_t *lessee_id);

typedef struct drmModeLesseeList {
    uint32_t count;
    uint32_t lessees[];
} drmModeLesseeListRes, *drmModeLesseeListPtr;

extern drmModeLesseeListPtr drmModeListLessees(int fd);

typedef struct drmModeObjectList {
    uint32_t count;
    uint32_t objects[];
} drmModeObjectListRes, *drmModeObjectListPtr;

extern drmModeObjectListPtr drmModeGetLease(int fd);
extern int drmModeRevokeLease(int fd, uint32_t lessee_id);

extern const char *drmModeGetConnectorTypeName(uint32_t connector_type);

extern int drmModeCreateDumbBuffer(int fd, uint32_t width, uint32_t height, uint32_t bpp, uint32_t flags, uint32_t *handle, uint32_t *pitch, uint64_t *size);

extern int drmModeDestroyDumbBuffer(int fd, uint32_t handle);
extern int drmModeMapDumbBuffer(int fd, uint32_t handle, uint64_t *offset);

#if defined(__cplusplus)
}
#endif

#endif
