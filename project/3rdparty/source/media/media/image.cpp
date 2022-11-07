#include <assert.h>
#include <string.h>

#include <media/utils/utils.h>
#include <media/utils/image.h>
#include <media/utils/key_string.h>
#include <media/utils/media_type.h>

void GetPixFmtNumDen(const DrmPixelFormat &fmt, int &num, int &den)
{
    num = 0;
    den = 1;

    switch (fmt) {
        case DRM_PIX_FMT_RGB332:
            num = 1;
            break;

        case DRM_PIX_FMT_YUV420P:
        case DRM_PIX_FMT_NV12:
        case DRM_PIX_FMT_NV21:
        case DRM_PIX_FMT_FBC0:
            num = 3;
            den = 2;
            break;

        case DRM_PIX_FMT_YUV422P:
        case DRM_PIX_FMT_NV16:
        case DRM_PIX_FMT_NV61:
        case DRM_PIX_FMT_YUYV422:
        case DRM_PIX_FMT_UYVY422:
        case DRM_PIX_FMT_RGB565:
        case DRM_PIX_FMT_BGR565:
        case DRM_PIX_FMT_FBC2:
            num = 2;
            break;

        case DRM_PIX_FMT_RGB888:
        case DRM_PIX_FMT_BGR888:
        case DRM_PIX_FMT_YUV444SP:
            num = 3;
            break;

        case DRM_PIX_FMT_ARGB8888:
        case DRM_PIX_FMT_ABGR8888:
        case DRM_PIX_FMT_RGBA8888:
        case DRM_PIX_FMT_BGRA8888:
            num = 4;
            break;

        case DRM_PIX_FMT_XRGB8888:
            num = 4;
            break;

        case DRM_PIX_FMT_MJPEG:
            num = 2;
            break;

        default:
            DRM_MEDIA_LOGE("unsupport get num/den for pixel fmt: %d", fmt);
    }
}

int CalPixFmtSize(const DrmPixelFormat &fmt, const int width, const int height, int align)
{
    int num = 0;
    int den = 0;
    int pix_fmt_size = 0;
    int extra_hdr_size = 0;

    GetPixFmtNumDen(fmt, num, den);

    if (fmt == DRM_PIX_FMT_FBC0 || fmt == DRM_PIX_FMT_FBC2) {
        align = 16;
        extra_hdr_size = DRM_UPALIGNTO(width, align) * DRM_UPALIGNTO(height, align) / 16;
        extra_hdr_size = DRM_UPALIGNTO(extra_hdr_size, 4096);
    }

    if (align > 0) {
        pix_fmt_size = DRM_UPALIGNTO(width, align) * DRM_UPALIGNTO(height, align) * num / den;
    } else {
        pix_fmt_size = width * height * num / den;
    }

    return (extra_hdr_size + pix_fmt_size);
}

static const struct PixFmtStringEntry {
    DrmPixelFormat fmt;
    const char     *type_str;
} pix_fmt_string_map[] = {
    {DRM_PIX_FMT_YUV420P,  DRM_IMAGE_YUV420P},  {DRM_PIX_FMT_NV12,     DRM_IMAGE_NV12},
    {DRM_PIX_FMT_NV21,     DRM_IMAGE_NV21},     {DRM_PIX_FMT_YUV422P,  DRM_IMAGE_YUV422P},
    {DRM_PIX_FMT_NV16,     DRM_IMAGE_NV16},     {DRM_PIX_FMT_NV61,     DRM_IMAGE_NV61},
    {DRM_PIX_FMT_YUYV422,  DRM_IMAGE_YUYV422},  {DRM_PIX_FMT_UYVY422,  DRM_IMAGE_UYVY422},
    {DRM_PIX_FMT_RGB332,   DRM_IMAGE_RGB332},   {DRM_PIX_FMT_RGB565,   DRM_IMAGE_RGB565},
    {DRM_PIX_FMT_BGR565,   DRM_IMAGE_BGR565},   {DRM_PIX_FMT_RGB888,   DRM_IMAGE_RGB888},
    {DRM_PIX_FMT_BGR888,   DRM_IMAGE_BGR888},   {DRM_PIX_FMT_ARGB8888, DRM_IMAGE_ARGB8888},
    {DRM_PIX_FMT_ABGR8888, DRM_IMAGE_ABGR8888}, {DRM_PIX_FMT_RGBA8888, DRM_IMAGE_RGBA8888},
    {DRM_PIX_FMT_BGRA8888, DRM_IMAGE_BGRA8888}, {DRM_PIX_FMT_FBC0,     DRM_IMAGE_FBC0},
    {DRM_PIX_FMT_FBC0,     DRM_IMAGE_FBC2},     {DRM_PIX_FMT_YUV444SP, DRM_IMAGE_YUV444SP},
    {DRM_PIX_FMT_MJPEG,    DRM_IMAGE_JPEG},     {DRM_PIX_FMT_XRGB8888, DRM_IMAGE_XRGB8888}
};

DrmPixelFormat StringToPixFmt(const char *type)
{
    if (!type) {
        return DRM_PIX_FMT_NONE;
    }

    FIND_ENTRY_TARGET_BY_STRCMP(type, pix_fmt_string_map, type_str, fmt)
    return DRM_PIX_FMT_NONE;
}

const char *PixFmtToString(DrmPixelFormat fmt)
{
    FIND_ENTRY_TARGET(fmt, pix_fmt_string_map, fmt, type_str)
    return nullptr;
}

namespace libdrm {
bool ParseImageInfoFromMap(std::map<std::string, std::string> &params, DrmImageInfo &info, bool input)
{
    std::string value;
    const char *type = input ? DRM_KEY_INPUTDATATYPE : DRM_KEY_OUTPUTDATATYPE;
    CHECK_EMPTY(value, params, type)

    info.pix_fmt = StringToPixFmt(value.c_str());
    if (info.pix_fmt == DRM_PIX_FMT_NONE) {
        DRM_MEDIA_LOGE("unsupport pix fmt %s", value.c_str());
        return false;
    }

    CHECK_EMPTY(value, params, DRM_KEY_BUFFER_WIDTH)
    info.width = std::stoi(value);

    CHECK_EMPTY(value, params, DRM_KEY_BUFFER_HEIGHT)
    info.height = std::stoi(value);

    CHECK_EMPTY(value, params, DRM_KEY_BUFFER_VIR_WIDTH)
    info.vir_width = std::stoi(value);

    CHECK_EMPTY(value, params, DRM_KEY_BUFFER_VIR_HEIGHT)
    info.vir_height = std::stoi(value);

    return true;
}

std::string to_param_string(const DrmImageInfo &info, bool input)
{
    std::string s;
    const char *fmt = PixFmtToString(info.pix_fmt);
    if (!fmt) {
        return s;
    }

    if (input) {
        PARAM_STRING_APPEND(s, DRM_KEY_INPUTDATATYPE, fmt);
    } else {
        PARAM_STRING_APPEND(s, DRM_KEY_OUTPUTDATATYPE, fmt);
    }

    PARAM_STRING_APPEND_TO(s, DRM_KEY_BUFFER_WIDTH, info.width);
    PARAM_STRING_APPEND_TO(s, DRM_KEY_BUFFER_HEIGHT, info.height);
    PARAM_STRING_APPEND_TO(s, DRM_KEY_BUFFER_VIR_WIDTH, info.vir_width);
    PARAM_STRING_APPEND_TO(s, DRM_KEY_BUFFER_VIR_HEIGHT, info.vir_height);

    return s;
}

std::string TwoImageRectToString(const std::vector<DrmImageRect> &src_dst)
{
    char r[128] = {0};

    assert(src_dst[0].x < 10000 && src_dst[0].y < 10000);
    assert(src_dst[0].w < 10000 && src_dst[0].h < 10000);
    assert(src_dst[1].x < 10000 && src_dst[1].y < 10000);
    assert(src_dst[1].w < 10000 && src_dst[1].h < 10000);

    snprintf(r, sizeof(r), "(%d,%d,%d,%d)%s(%d,%d,%d,%d)", src_dst[0].x, src_dst[0].y, src_dst[0].w, src_dst[0].h, DRM_KEY_RIGHT_DIRECTION, src_dst[1].x, src_dst[1].y, src_dst[1].w, src_dst[1].h);
    return r;
}

std::vector<DrmImageRect> StringToTwoImageRect(const std::string &str_rect)
{
    const char *s = nullptr;
    std::vector<DrmImageRect> ret;

    if (str_rect.empty() || !(s = strstr(str_rect.c_str(), DRM_KEY_RIGHT_DIRECTION))) {
        return ret;
    }

    const char *args[2] = {str_rect.c_str(), s + sizeof(DRM_KEY_RIGHT_DIRECTION) - 1};
    for (int i = 0; i < 2; i++) {
        DrmImageRect rect = {0, 0, 0, 0};
        int r = sscanf(args[i], "(%d,%d,%d,%d)", &rect.x, &rect.y, &rect.w, &rect.h);
        if (r != 4) {
            DRM_MEDIA_LOGE("Fail to sscanf(ret=%d) : %m", r);
            ret.clear();
            return ret;
        }

        ret.push_back(std::move(rect));
    }

    return ret;
}

std::string ImageRectToString(const DrmImageRect &src_dst)
{
    char r[64] = {0};

    assert(src_dst.x >= 0 && src_dst.y >= 0);
    assert(src_dst.x < 10000 && src_dst.y < 10000);
    assert(src_dst.w < 10000 && src_dst.h < 10000);

    snprintf(r, sizeof(r), "(%d,%d,%d,%d)", src_dst.x, src_dst.y, src_dst.w, src_dst.h);
    return r;
}

std::vector<DrmImageRect> StringToImageRect(const std::string &str_rect)
{
    const char *start = nullptr;
    std::vector<DrmImageRect> ret;
    const char *delimiter = nullptr;

    if (str_rect.empty()) {
        return ret;
    }

    start = str_rect.c_str();
    while (start) {
        delimiter = start;

        for (int i = 0; i < 4; i++) {
            if (i == 3) {
                delimiter = strstr(delimiter, ")");
            } else {
                delimiter = strstr(delimiter, ",");
            }

            if (!delimiter) {
                return ret;
            }

            delimiter += 1;
        }

        DrmImageRect rect = {0, 0, 0, 0};
        int r = sscanf(start, "(%d,%d,%d,%d)", &rect.x, &rect.y, &rect.w, &rect.h);
        if (r != 4) {
            DRM_MEDIA_LOGE("Fail to sscanf(ret=%d) : %m", r);
            return ret;
        }

        ret.push_back(std::move(rect));
        start = strstr(delimiter, "(");
    }

    return ret;
}
}
