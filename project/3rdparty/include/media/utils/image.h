#ifndef LIBDRM_DISPLAY_IMAGE_H
#define LIBDRM_DISPLAY_IMAGE_H

typedef enum {
    DRM_PIX_FMT_NONE = -1,
    DRM_PIX_FMT_YUV420P,
    DRM_PIX_FMT_NV12,
    DRM_PIX_FMT_NV21,
    DRM_PIX_FMT_YUV422P,
    DRM_PIX_FMT_NV16,
    DRM_PIX_FMT_NV61,
    DRM_PIX_FMT_YUYV422,
    DRM_PIX_FMT_UYVY422,
    DRM_PIX_FMT_YUV444SP,
    DRM_PIX_FMT_RGB332,
    DRM_PIX_FMT_RGB565,
    DRM_PIX_FMT_BGR565,
    DRM_PIX_FMT_RGB888,
    DRM_PIX_FMT_BGR888,
    DRM_PIX_FMT_ARGB8888,
    DRM_PIX_FMT_ABGR8888,
    DRM_PIX_FMT_RGBA8888,
    DRM_PIX_FMT_BGRA8888,

    DRM_PIX_FMT_FBC0,
    DRM_PIX_FMT_FBC2,
    DRM_PIX_FMT_MJPEG,

    DRM_PIX_FMT_XRGB8888,
    DRM_PIX_FMT_NB
} DrmPixelFormat;

typedef struct {
    DrmPixelFormat pix_fmt;
    int            width;
    int            height;
    int            vir_width;
    int            vir_height;
} DrmImageInfo;

typedef struct {
    int x, y;
    int w, h;
} DrmImageRect;

typedef struct {
    int           priv;
    int           x;
    int           y;
    int           w;
    int           h;
    int           color;
    int           offset;
    unsigned char enable;
    int           id;
} DrmImageBorder;

typedef struct {
    int            priv;
    int            x;
    int            y;
    int            w;
    int            h;
    void           *data;
    DrmPixelFormat pix_fmt;
    unsigned char  enable;
    int            id;
} DrmImageOsd;

typedef struct {
    int            x;
    int            y;
    int            w;
    int            h;
    void           *data;
    DrmPixelFormat fmt;
    unsigned char  enable;
} DrmOsdInfo;

#define REGION_LUMA_MAX         64
typedef struct {
    int                priv;
    unsigned int       region_num;
    DrmImageRect       region[REGION_LUMA_MAX];
    unsigned long long luma_data[REGION_LUMA_MAX];
    int                ms;
    int                offset;
} DrmImageRegionLuma;

#include "utils.h"

void GetPixFmtNumDen(const DrmPixelFormat &fmt, int &num, int &den);
int CalPixFmtSize(const DrmPixelFormat &fmt, const int width, const int height, int align = 0);

inline int CalPixFmtSize(const DrmImageInfo &ii, int align = 0)
{
    return CalPixFmtSize(ii.pix_fmt, ii.vir_width, ii.vir_height, align);
}

const char *PixFmtToString(DrmPixelFormat fmt);
DrmPixelFormat StringToPixFmt(const char *type);

#include <map>
#include <string>
#include <vector>

namespace libdrm {
std::string to_param_string(const DrmImageInfo &ii, bool input = true);
bool ParseImageInfoFromMap(std::map<std::string, std::string> &params, DrmImageInfo &ii, bool input = true);

std::string TwoImageRectToString(const std::vector<DrmImageRect> &src_dst);
std::vector<DrmImageRect> StringToTwoImageRect(const std::string &str_rect);

std::string ImageRectToString(const DrmImageRect &src_dst);
std::vector<DrmImageRect> StringToImageRect(const std::string &str_rect);
}

#endif
