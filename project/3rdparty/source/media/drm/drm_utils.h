#ifndef LIBDRM_DISPLAY_DRM_UTILS_H
#define LIBDRM_DISPLAY_DRM_UTILS_H

#include <libdrm/xf86drm.h>
#include <libdrm/xf86drmMode.h>
#include <libdrm/drm/drm_fourcc.h>

#include <string>
#include <vector>

namespace libdrm {
#define DRM_ID_ISVALID(id)          ((id != 0) && (id != ((uint32_t)-1)))

enum csi_path_mode {
    VOP_PATH    = 0,
    BYPASS_PATH = 1,
};

enum vop_pdaf_mode {
    VOP_HOLD_MODE            = 0,
    VOP_NORMAL_MODE          = 1,
    VOP_PINGPONG_MODE        = 2,
    VOP_BYPASS_MODE          = 3,
    VOP_BACKGROUND_MODE      = 4,
    VOP_ONEFRAME_MODE        = 5,
    VOP_ONEFRAME_NOSEND_MODE = 6
};

enum vop_pdaf_type {
    VOP_PDAF_TYPE_DEFAULT = 0,
    VOP_PDAF_TYPE_HBLANK  = 1,
    VOP_PDAF_TYPE_VBLANK  = 2,
};

struct crtc {
    drmModeCrtc             *crtc;
    drmModeObjectProperties *props;
    drmModePropertyRes      **props_info;
    drmModeModeInfo         *mode;
};

struct encoder {
    drmModeEncoder *encoder;
};

struct connector {
    drmModeConnector        *connector;
    drmModeObjectProperties *props;
    drmModePropertyRes      **props_info;
    char                    *name;

    drmModeCrtc             *saved_crtc;
};

struct fb {
    drmModeFB *fb;
};

struct plane {
    drmModePlane            *plane;
    drmModeObjectProperties *props;
    drmModePropertyRes      **props_info;
};

struct drm_ids {
    uint32_t count_fbs;
    uint32_t *fb_ids;
    uint32_t count_connectors;
    uint32_t *connector_ids;
    uint32_t count_encoders;
    uint32_t *encoder_ids;
    uint32_t count_crtcs;
    uint32_t *crtc_ids;
    uint32_t count_planes;
    uint32_t *plane_ids;
};

struct resources {
    int              drm_fd;
    drmModeRes       *res;
    drmModePlaneRes  *plane_res;
    struct crtc      *crtcs;
    struct encoder   *encoders;
    struct connector *connectors;
    struct fb        *fbs;
    struct plane     *planes;
    struct drm_ids   ids;
};

void dump_suitable_ids(struct resources *res);

int get_connector_index(struct resources *res, uint32_t id);
drmModeConnector *get_connector_by_id(struct resources *res, uint32_t id);
drmModeEncoder *get_encoder_by_id(struct resources *res, uint32_t id);

int get_crtc_index(struct resources *res, uint32_t id);
int get_crtc_index_by_connector(struct resources *res, drmModeConnector *conn);

drmModeCrtc *get_crtc_by_id(struct resources *res, uint32_t id);
int get_plane_index(struct resources *res, uint32_t id);
drmModePlane *get_plane_by_id(struct resources *res, uint32_t id);
const char *GetPlaneTypeStrByPlaneID(struct resources *res, uint32_t id);

bool reserve_ids_by_connector(struct resources *res, uint32_t conn_id);
bool reserve_ids_by_encoder(struct resources *res, uint32_t enc_id);
bool reserve_ids_by_crtc(struct resources *res, uint32_t crtc_id);
bool reserve_ids_by_plane(struct resources *res, uint32_t plane_id);

bool filter_ids_if_connector_notready(struct resources *res);
bool filter_ids_by_fps(struct resources *res, uint32_t vrefresh);
bool comp_width_height(drmModeConnector *dmc, int w, int h);
bool find_connector_ids_by_wh(struct resources *res, int w, int h);
bool filter_ids_by_wh(struct resources *res, int w, int h);
bool filter_ids_by_data_type(struct resources *res, const std::string &data_type);
bool filter_ids_by_plane_type(struct resources *res, const std::string &plane_type);
bool filter_ids_by_skip_plane_ids(struct resources *res, const std::vector<uint32_t> &skip_plane_ids);

int filter_modeinfo_by_wh(int w, int h, int &size, drmModeModeInfoPtr *modes);
int filter_modeinfo_by_fps(int fps, int &size, drmModeModeInfoPtr *modes);
int filter_modeinfo_by_type(uint32_t type, int &size, drmModeModeInfoPtr *modes);

uint32_t get_property_id(struct resources *res, uint32_t object_type, uint32_t obj_id, const char *property_name, uint64_t *value = nullptr);
int set_property(struct resources *res, uint32_t object_type, uint32_t obj_id, const char *property_name, uint64_t value);

uint32_t GetDRMFmtByString(const char *type);
const std::string &GetStringOfDRMFmts();

class DRMDevice {
public:
    DRMDevice(const std::string &drm_path);
    ~DRMDevice();

    int GetDeviceFd() {
        return fd;
    }

    struct resources *get_resources();
    void free_resources(struct resources *res);

private:
    int         fd;
    std::string path;
};
}

#endif
