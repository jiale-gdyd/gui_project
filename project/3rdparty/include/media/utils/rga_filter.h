#ifndef LIBDRM_DISPLAY_RGA_FILTER_H
#define LIBDRM_DISPLAY_RGA_FILTER_H

#include <linux/kconfig.h>

#include "image.h"
#include "buffer.h"
#include "filter.h"

#if defined(CONFIG_RKRGA)
#if defined(CONFIG_RKRGA_OLD)
#include <rockchip/rkrga/rga.h>
#include <rockchip/rkrga/im2d.h>
#include <rockchip/rkrga/RockchipRga.h>
#else
#include <rockchip/rkrgax/rga.h>
#include <rockchip/rkrgax/im2d.h>
#include <rockchip/rkrgax/RockchipRga.h>
#endif
#endif

namespace libdrm {
class ImageBuffer;

typedef enum {
    FLIP_NULL,
    FLIP_H,
    FLIP_V,
    FLIP_HV
} FlipEnum;

class RgaFilter : public Filter {
public:
    RgaFilter(const char *param);
    virtual ~RgaFilter();

    static const char *GetFilterName() {
        return "rkrga";
    }

    virtual int Process(std::shared_ptr<MediaBuffer> input, std::shared_ptr<MediaBuffer> &output) override;

    void SetRects(std::vector<DrmImageRect> vec_rect);

    static RockchipRga gRkRga;
    virtual int IoCtrl(unsigned long int request _DRM_UNUSED, ...) override;

private:
    unsigned int                src_max_width;
    unsigned int                src_max_height;
    unsigned int                dst_max_width;
    unsigned int                dst_max_height;
    ConditionLockMutex          param_mtx;
    std::vector<DrmImageRect>   vec_rect;
    int                         rotate;
    FlipEnum                    flip;
    std::vector<DrmImageBorder> lines;
    int                         hide;
    std::map<int, DrmOsdInfo>   osds;
    std::mutex                  luma_mutex;
    std::condition_variable     luma_cond;
    DrmImageRegionLuma          region_luma;
};

int rga_blit(std::shared_ptr<ImageBuffer> src, std::shared_ptr<ImageBuffer> dst, std::vector<DrmImageBorder> &lines, std::map<int, DrmOsdInfo> osds, DrmImageRegionLuma *region_luma, DrmImageRect *src_rect = nullptr, DrmImageRect *dst_rect = nullptr, int rotate = 0, FlipEnum flip = FLIP_NULL, int hide = 0);
int get_rga_format(DrmPixelFormat f);
}

#endif
