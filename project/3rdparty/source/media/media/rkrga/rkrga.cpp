#include <vector>
#include <assert.h>

#include <media/utils/buffer.h>
#include <media/utils/filter.h>
#include <media/utils/rga_filter.h>
#include <media/utils/media_config.h>

namespace libdrm {
RockchipRga RgaFilter::gRkRga;

struct RgaRotationMap {
    int rkRotation;
    int rgaRotation;
};

struct RgaMirrorMap {
    int rkMirror;
    int rgaMirror;
};

static const struct RgaRotationMap sRgaRotationMap[] = {
    {0,   0},
    {90,  HAL_TRANSFORM_ROT_90},
    {180, HAL_TRANSFORM_ROT_180},
    {270, HAL_TRANSFORM_ROT_270},
};

static const struct RgaMirrorMap sRgaMirrorMap[] = {
    {FLIP_NULL, 0},
    {FLIP_H,    HAL_TRANSFORM_FLIP_H},
    {FLIP_V,    HAL_TRANSFORM_FLIP_V},
    {FLIP_HV,   HAL_TRANSFORM_FLIP_H_V},
};

static int rotate_rk_to_rga(int rotation)
{
    for (int i = 0; i < (int)ARRAY_ELEMS(sRgaRotationMap); i++) {
        if (sRgaRotationMap[i].rkRotation == rotation) {
            return sRgaRotationMap[i].rgaRotation;
        }
    }

    return 0;
}

static int mirror_rk_to_rga(int mirror)
{
    for (int i = 0; i < (int)ARRAY_ELEMS(sRgaMirrorMap); i++) {
        if (sRgaMirrorMap[i].rkMirror == mirror) {
            return sRgaMirrorMap[i].rgaMirror;
        }
    }

    return 0;
}

static int rga_rect_check(DrmImageRect *rect, int max_w, int max_h)
{
    if (!rect) {
        return -1;
    }

    if ((rect->x < 0) || (rect->w < 0) || ((max_w > 0) && ((rect->x + rect->w) > max_w))) {
        return -1;
    }

    if ((rect->y < 0) || (rect->h < 0) || ((max_h > 0) && ((rect->y + rect->h) > max_h))) {
        return -1;
    }

    return 0;
}

RgaFilter::RgaFilter(const char *param) : rotate(0), flip(FLIP_NULL), hide(0)
{
    std::map<std::string, std::string> params;
    if (!parse_media_param_map(param, params)) {
        SetError(-EINVAL);
        return;
    }

    const std::string &value = params[DRM_KEY_BUFFER_RECT];
    auto &&rects = StringToTwoImageRect(value);
    if (rects.empty()) {
        DRM_MEDIA_LOGE("Missing src and dst rects");
        SetError(-EINVAL);
        return;
    }

    vec_rect = std::move(rects);

    if (rga_rect_check(&vec_rect[0], 0, 0) || rga_rect_check(&vec_rect[1], 0, 0)) {
        DRM_MEDIA_LOGE("Invalid src rect:<%d,%d,%d,%d> or dst rect:<%d,%d,%d,%d>", vec_rect[0].x, vec_rect[0].y, vec_rect[0].w, vec_rect[0].h, vec_rect[1].x, vec_rect[1].y, vec_rect[1].w, vec_rect[1].h);
        SetError(-EINVAL);
        return;
    }

    src_max_width = vec_rect[0].w;
    src_max_height = vec_rect[0].h;
    dst_max_width = vec_rect[1].w;
    dst_max_height = vec_rect[1].h;

    const std::string &v = params[DRM_KEY_BUFFER_ROTATE];
    if (!v.empty()) {
        rotate = std::stoi(v);
    }

    const std::string &f = params[DRM_KEY_BUFFER_FLIP];
    if (!f.empty()) {
        flip = (FlipEnum)std::stoi(f);
    }

    memset(&region_luma, 0, sizeof(DrmImageRegionLuma));
}

void RgaFilter::SetRects(std::vector<DrmImageRect> rects)
{
    vec_rect = std::move(rects);
}

int RgaFilter::Process(std::shared_ptr<MediaBuffer> input, std::shared_ptr<MediaBuffer> &output)
{
    if (vec_rect.size() < 2) {
        return -EINVAL;
    }

    if (!input || input->GetType() != DrmType::Image) {
        return -EINVAL;
    }

    if (!output || output->GetType() != DrmType::Image) {
        return -EINVAL;
    }

    auto src = std::static_pointer_cast<libdrm::ImageBuffer>(input);
    auto dst = std::static_pointer_cast<libdrm::ImageBuffer>(output);
    if (!src->IsValid() || !dst->IsValid()) {
        DRM_MEDIA_LOGE("Src:(%zuBytes) or Dst:(%zuBytes) Buffer is invalid", src->GetValidSize(), dst->GetValidSize());
        return -EINVAL;
    }

    int cur_hide = 0;
    int cur_rotate = 0;
    DrmImageRect src_rect;
    DrmImageRect dst_rect;
    FlipEnum cur_flip = FLIP_NULL;

    memset(&src_rect, 0, sizeof(DrmImageRect));
    memset(&dst_rect, 0, sizeof(DrmImageRect));

    param_mtx.lock();
    src_rect = vec_rect[0];
    dst_rect = vec_rect[1];
    cur_rotate = rotate;
    cur_flip = flip;
    std::vector<DrmImageBorder> cur_lines = lines;
    cur_hide = hide;
    std::map<int, DrmOsdInfo> cur_osds = osds;
    DrmImageRegionLuma cur_region_luma;
    memcpy(&cur_region_luma, &region_luma, sizeof(DrmImageRegionLuma));
    param_mtx.unlock();

    if ((!dst_rect.w || !dst_rect.h) && !dst->IsValid()) {
        DrmImageInfo info = src->GetImageInfo();
        info.pix_fmt = dst->GetPixelFormat();
        size_t size = CalPixFmtSize(info.pix_fmt, info.vir_width, info.vir_height, 16);
        if (size == 0) {
            return -EINVAL;
        }

        auto &&mb = MediaBuffer::Alloc2(size, MediaBuffer::MemType::MEM_HARD_WARE);
        ImageBuffer ib(mb, info);
        if (ib.GetSize() >= size) {
            ib.SetValidSize(size);
            *dst.get() = ib;
        }

        assert(dst->IsValid());
    }

    int ret = rga_blit(src, dst, cur_lines, cur_osds, &cur_region_luma, &src_rect, &dst_rect, cur_rotate, cur_flip, cur_hide);
    if (cur_region_luma.region_num > 0) {
        param_mtx.lock();
        memcpy(&region_luma, &cur_region_luma, sizeof(DrmImageRegionLuma));
        param_mtx.unlock();

        luma_mutex.lock();
        luma_cond.notify_all();
        luma_mutex.unlock();
    }

    return ret;
}

RgaFilter::~RgaFilter()
{

}

int get_rga_format(DrmPixelFormat f)
{
    static std::map<DrmPixelFormat, int> rga_format_map = {
        {DRM_PIX_FMT_YUV420P,  RK_FORMAT_YCbCr_420_P},
        {DRM_PIX_FMT_NV12,     RK_FORMAT_YCbCr_420_SP},
        {DRM_PIX_FMT_NV21,     RK_FORMAT_YCrCb_420_SP},
        {DRM_PIX_FMT_YUV422P,  RK_FORMAT_YCbCr_422_P},
        {DRM_PIX_FMT_NV16,     RK_FORMAT_YCbCr_422_SP},
        {DRM_PIX_FMT_NV61,     RK_FORMAT_YCrCb_422_SP},
        {DRM_PIX_FMT_YUYV422,  RK_FORMAT_YUYV_422},
        {DRM_PIX_FMT_UYVY422,  RK_FORMAT_UYVY_422},
        {DRM_PIX_FMT_RGB565,   RK_FORMAT_RGB_565},
        {DRM_PIX_FMT_BGR565,   -1},
        {DRM_PIX_FMT_RGB888,   RK_FORMAT_BGR_888},
        {DRM_PIX_FMT_BGR888,   RK_FORMAT_RGB_888},
        {DRM_PIX_FMT_ARGB8888, RK_FORMAT_BGRA_8888},
        {DRM_PIX_FMT_ABGR8888, RK_FORMAT_RGBA_8888},
        {DRM_PIX_FMT_XRGB8888, RK_FORMAT_XRGB_8888}
    };

    auto it = rga_format_map.find(f);
    if (it != rga_format_map.end()) {
        return it->second;
    }

    return -1;
}

int RgaFilter::IoCtrl(unsigned long int request, ...)
{
    va_list vl;
    int ret = 0;

    va_start(vl, request);
    void *arg = va_arg(vl, void *);
    va_end(vl);

    if (!arg) {
        DRM_MEDIA_LOGE("Invalid IoCtrl args(request:[%ld], args:NULL)", request);
        return -1;
    }

    switch (request) {
        case S_RGA_CFG: {
            DrmRgaConfig *new_rga_cfg = (DrmRgaConfig *)arg;
            DrmImageRect *new_src_rect = &(new_rga_cfg->src_rect);
            DrmImageRect *new_dst_rect = &(new_rga_cfg->dst_rect);
            int new_rotation = new_rga_cfg->rotation;
            FlipEnum new_flip = (FlipEnum)new_rga_cfg->flip;

            if (rga_rect_check(new_src_rect, src_max_width, src_max_height) || rga_rect_check(new_dst_rect, dst_max_width, dst_max_height)) {
                DRM_MEDIA_LOGE("IoCtrl: Invalid srcRect:<%d,%d,%d,%d> or dstRect:<%d,%d,%d,%d>, max "
                    "srcRect:<0,0,%d,%d>, max dstRect:<0,0,%d,%d>",
                    new_src_rect->x, new_src_rect->y, new_src_rect->w, new_src_rect->h,
                    new_dst_rect->x, new_dst_rect->y, new_dst_rect->w, new_dst_rect->h,
                    src_max_width, src_max_height, dst_max_width, dst_max_height);
                ret = -1;
                break;
            }

            if ((new_rotation != 0) && (new_rotation != 90) && (new_rotation != 180) && (new_rotation != 270)) {
                DRM_MEDIA_LOGE("IoCtrl: Invalid rotation:[%d]", new_rotation);
                ret = -1;
                break;
            }

            param_mtx.lock();
            vec_rect[0].x = new_src_rect->x;
            vec_rect[0].y = new_src_rect->y;
            vec_rect[0].w = new_src_rect->w;
            vec_rect[0].h = new_src_rect->h;
            vec_rect[1].x = new_dst_rect->x;
            vec_rect[1].y = new_dst_rect->y;
            vec_rect[1].w = new_dst_rect->w;
            vec_rect[1].h = new_dst_rect->h;
            rotate = new_rotation;
            flip = new_flip;
            param_mtx.unlock();

            break;
        }

        case G_RGA_CFG: {
            DrmRgaConfig *rd_rga_cfg = (DrmRgaConfig *)arg;
            DrmImageRect *rd_src_rect = &(rd_rga_cfg->src_rect);
            DrmImageRect *rd_dst_rect = &(rd_rga_cfg->dst_rect);
            int *rd_rotation = &(rd_rga_cfg->rotation);
            FlipEnum *rd_flip = (FlipEnum *)&(rd_rga_cfg->flip);

            param_mtx.lock();
            rd_src_rect->x = vec_rect[0].x;
            rd_src_rect->y = vec_rect[0].y;
            rd_src_rect->w = vec_rect[0].w;
            rd_src_rect->h = vec_rect[0].h;
            rd_dst_rect->x = vec_rect[1].x;
            rd_dst_rect->y = vec_rect[1].y;
            rd_dst_rect->w = vec_rect[1].w;
            rd_dst_rect->h = vec_rect[1].h;
            *rd_rotation = rotate;
            *rd_flip = flip;
            param_mtx.unlock();
            break;
        }

        case S_RGA_LINE_INFO: {
            DrmImageBorder *line = (DrmImageBorder *)arg;

            param_mtx.lock();
            int exist = 0;
            for (auto it = lines.begin(); it != lines.end(); ++it) {
                DrmImageBorder &l = *it;
                if (l.id == line->id) {
                    exist = 1;
                    memcpy(&l, line, sizeof(DrmImageBorder));
                    break;
                }
            }

            if (!exist) {
                lines.push_back(*line);
            }
            param_mtx.unlock();
            break;
        }

        case G_RGA_LINE_INFO:
            break;

        case S_RGA_SHOW:
            param_mtx.lock();
            hide = 0;
            param_mtx.unlock();
            break;

        case S_RGA_HIDE:
            param_mtx.lock();
            hide = 1;
            param_mtx.unlock();
            break;

        case S_RGA_OSD_INFO: {
            DrmImageOsd *osd = (DrmImageOsd *)arg;
            DrmOsdInfo info = {osd->x, osd->y, osd->w, osd->h, osd->data, osd->pix_fmt, osd->enable};
            param_mtx.lock();
            osds[osd->id] = info;
            param_mtx.unlock();
            break;
        }

        case G_RGA_OSD_INFO:
            break;

        case G_RGA_REGION_LUMA: {
            DrmImageRegionLuma *rl = (DrmImageRegionLuma *)arg;

            param_mtx.lock();
            memcpy(&region_luma, rl, sizeof(DrmImageRegionLuma));
            param_mtx.unlock();

            std::unique_lock<std::mutex> lck(luma_mutex);
            if (rl->ms <= 0) {
                luma_cond.wait(lck);
                memcpy(rl, &region_luma, sizeof(DrmImageRegionLuma));
            } else if (rl->ms > 0) {
                if (luma_cond.wait_for(lck, std::chrono::milliseconds(rl->ms)) ==
                    std::cv_status::timeout) {
                    DRM_MEDIA_LOGI("G_RGA_REGION_LUMA timeout");
                    memset(rl->luma_data, 0, sizeof(rl->luma_data));
                } else {
                    memcpy(rl, &region_luma, sizeof(DrmImageRegionLuma));
                }
            }

            param_mtx.lock();
            memset(&region_luma, 0, sizeof(DrmImageRegionLuma));
            param_mtx.unlock();
            break;
        }

        default:
            ret = -1;
            DRM_MEDIA_LOGE("Not support IoCtrl cmd(%ld)", request);
            break;
    }

    return ret;
}

static void dummp_rga_info(rga_info_t info, std::string name)
{
    DRM_MEDIA_LOGD("### %s dummp info:", name.c_str());
    DRM_MEDIA_LOGD("\t info.fd:[%d]", info.fd);
    DRM_MEDIA_LOGD("\t info.mmuFlag:[%d]", info.mmuFlag);
    DRM_MEDIA_LOGD("\t info.rotation:[%d]", info.rotation);
    DRM_MEDIA_LOGD("\t info.blend:[%d]", info.blend);
    DRM_MEDIA_LOGD("\t info.virAddr:[%p]", info.virAddr);
    DRM_MEDIA_LOGD("\t info.phyAddr:[%p]", info.phyAddr);
    DRM_MEDIA_LOGD("\t info.rect.xoffset:[%d]", info.rect.xoffset);
    DRM_MEDIA_LOGD("\t info.rect.yoffset:[%d]", info.rect.yoffset);
    DRM_MEDIA_LOGD("\t info.rect.width:[%d]", info.rect.width);
    DRM_MEDIA_LOGD("\t info.rect.height:[%d]", info.rect.height);
    DRM_MEDIA_LOGD("\t info.rect.wstride:[%d]", info.rect.wstride);
    DRM_MEDIA_LOGD("\t info.rect.hstride:[%d]", info.rect.hstride);
    DRM_MEDIA_LOGD("\t info.rect.format:[%d]", info.rect.format);
    DRM_MEDIA_LOGD("\t info.rect.size:[%d]", info.rect.size);
}

int rga_blit(std::shared_ptr<ImageBuffer> src, std::shared_ptr<ImageBuffer> dst, std::vector<DrmImageBorder> &lines, std::map<int, DrmOsdInfo> osds, DrmImageRegionLuma *region_luma, DrmImageRect *src_rect, DrmImageRect *dst_rect, int rotate, FlipEnum flip, int hide)
{
    if (!src || !src->IsValid()) {
        return -EINVAL;
    }

    if (!dst || !dst->IsValid()) {
        return -EINVAL;
    }

    rga_info_t src_info, dst_info, pat_info;
    memset(&src_info, 0, sizeof(src_info));
    src_info.fd = src->GetFD();
    if (src_info.fd < 0) {
        src_info.virAddr = src->GetPtr();
    }
    src_info.mmuFlag = 1;

    if (rotate && flip) {
        src_info.rotation = rotate_rk_to_rga(rotate);
        src_info.rotation |= (mirror_rk_to_rga(flip) << 4);
    } else if (rotate) {
        src_info.rotation = rotate_rk_to_rga(rotate);
    } else {
        src_info.rotation = mirror_rk_to_rga(flip);
    }

    if (src_rect) {
        rga_set_rect(&src_info.rect, src_rect->x, src_rect->y, src_rect->w, src_rect->h, src->GetVirWidth(), src->GetVirHeight(), get_rga_format(src->GetPixelFormat()));
    } else {
        rga_set_rect(&src_info.rect, 0, 0, src->GetWidth(), src->GetHeight(), src->GetVirWidth(), src->GetVirHeight(), get_rga_format(src->GetPixelFormat()));
    }

    memset(&dst_info, 0, sizeof(dst_info));
    dst_info.fd = dst->GetFD();
    if (dst_info.fd < 0) {
        dst_info.virAddr = dst->GetPtr();
    }
    dst_info.mmuFlag = 1;

    if (dst_rect) {
        rga_set_rect(&dst_info.rect, dst_rect->x, dst_rect->y, dst_rect->w, dst_rect->h, dst->GetVirWidth(), dst->GetVirHeight(), get_rga_format(dst->GetPixelFormat()));
    } else {
        rga_set_rect(&dst_info.rect, 0, 0, dst->GetWidth(), dst->GetHeight(), dst->GetVirWidth(), dst->GetVirHeight(), get_rga_format(dst->GetPixelFormat()));
    }

    // dummp_rga_info(src_info, "SrcInfo");
    // dummp_rga_info(dst_info, "DstInfo");

    // flush cache,  2688x1520 NV12 cost 1399us, 1080P cost 905us
    // src->BeginCPUAccess(false);
    // src->EndCPUAccess(false);

    int ret;
    if (hide) {
        ret = RgaFilter::gRkRga.RkRgaCollorFill(&dst_info);
    } else {
        ret = RgaFilter::gRkRga.RkRgaBlit(&src_info, &dst_info, NULL);
    }

    if (ret) {
        dst->SetValidSize(0);
        DRM_MEDIA_LOGI("Fail to RkRgaBlit, return:[%d]", ret);
    } else {
        size_t valid_size = CalPixFmtSize(dst->GetPixelFormat(), dst->GetVirWidth(), dst->GetVirHeight(), 0);
        dst->SetValidSize(valid_size);

        if (src->GetUSTimeStamp() > dst->GetUSTimeStamp()) {
            dst->SetUSTimeStamp(src->GetUSTimeStamp());
        }

        dst->SetAtomicClock(src->GetAtomicClock());
    }

    for (auto &line : lines) {
        if (!line.enable) {
            continue;
        }

        int x, y, w, h;
        if (dst_rect && line.offset) {
            x = dst_rect->x + line.x;
            y = dst_rect->y + line.y;
            w = line.w;
            h = line.h;
        } else {
            x = line.x;
            y = line.y;
            w = line.w;
            h = line.h;
        }

        if (x + w > dst->GetWidth() || y + h > dst->GetHeight()) {
            continue;
        }

        dst_info.color = line.color;
        rga_set_rect(&dst_info.rect, x, y, w, h, dst->GetVirWidth(), dst->GetVirHeight(), get_rga_format(dst->GetPixelFormat()));
        RgaFilter::gRkRga.RkRgaCollorFill(&dst_info);
    }

    for (auto &osd : osds) {
        if (!osd.second.enable) {
            continue;
        }

        memcpy(&src_info, &dst_info, sizeof(rga_info_t));
        src_info.blend = 0xff0501;
        rga_set_rect(&src_info.rect, osd.second.x, osd.second.y, osd.second.w, osd.second.h, dst->GetVirWidth(), dst->GetVirHeight(), get_rga_format(dst->GetPixelFormat()));

        memset(&pat_info, 0, sizeof(rga_info_t));
        pat_info.virAddr = osd.second.data;
        pat_info.mmuFlag = 1;
        rga_set_rect(&pat_info.rect, 0, 0, osd.second.w, osd.second.h, osd.second.w, osd.second.h, get_rga_format(osd.second.fmt));
        RgaFilter::gRkRga.RkRgaBlit(&src_info, &src_info, &pat_info);
    }

    // invalidate cache, 2688x1520 NV12 cost  1072us, 1080P cost 779us
    // dst->BeginCPUAccess(true);
    // dst->EndCPUAccess(true);

    if (region_luma && region_luma->region_num > 0) {
        int x, y, w, h;
        if (dst_rect && region_luma->offset) {
            x = dst_rect->x;
            y = dst_rect->y;
            w = dst_rect->w;
            h = dst_rect->h;
        } else {
            x = 0;
            y = 0;
            w = dst->GetWidth();
            h = dst->GetHeight();
        }

        memset(region_luma->luma_data, 0, sizeof(region_luma->luma_data));
        for (unsigned int i = 0; i < region_luma->region_num; i++) {
            if (region_luma->region[i].x + region_luma->region[i].w > w || region_luma->region[i].y + region_luma->region[i].h > h) {
                continue;
            }

            int line_size = dst->GetVirWidth();
            int rx = x + region_luma->region[i].x;
            int ry = y + region_luma->region[i].y;
            int rw = region_luma->region[i].w;
            int rh = region_luma->region[i].h;
            char *rect_start = (char *)dst->GetPtr() + ry * line_size + rx;

            for (int k = 0; k < rh; k++) {
                char *line_start = rect_start + k * line_size;
                for (int j = 0; j < rw; j++) {
                    region_luma->luma_data[i] += *(line_start + j);
                }
            }
        }
    }

    return ret;
}

class _PRIVATE_SUPPORT_FMTS : public SupportMediaTypes {
public:
    _PRIVATE_SUPPORT_FMTS() {
        types.append(DRM_TYPENEAR(DRM_IMAGE_YUV420P));
        types.append(DRM_TYPENEAR(DRM_IMAGE_NV12));
        types.append(DRM_TYPENEAR(DRM_IMAGE_NV21));
        types.append(DRM_TYPENEAR(DRM_IMAGE_YUV422P));
        types.append(DRM_TYPENEAR(DRM_IMAGE_NV16));
        types.append(DRM_TYPENEAR(DRM_IMAGE_NV61));
        types.append(DRM_TYPENEAR(DRM_IMAGE_YUYV422));
        types.append(DRM_TYPENEAR(DRM_IMAGE_UYVY422));
        types.append(DRM_TYPENEAR(DRM_IMAGE_RGB565));
        types.append(DRM_TYPENEAR(DRM_IMAGE_BGR565));
        types.append(DRM_TYPENEAR(DRM_IMAGE_RGB888));
        types.append(DRM_TYPENEAR(DRM_IMAGE_BGR888));
        types.append(DRM_TYPENEAR(DRM_IMAGE_ARGB8888));
        types.append(DRM_TYPENEAR(DRM_IMAGE_ABGR8888));
        types.append(DRM_TYPENEAR(DRM_IMAGE_XRGB8888));
    }
};
static _PRIVATE_SUPPORT_FMTS priv_fmts;

DEFINE_COMMON_FILTER_FACTORY(RgaFilter)

const char *FACTORY(RgaFilter)::ExpectedInputDataType()
{
    return priv_fmts.types.c_str();
}

const char *FACTORY(RgaFilter)::OutPutDataType()
{
    return priv_fmts.types.c_str();
}
}
