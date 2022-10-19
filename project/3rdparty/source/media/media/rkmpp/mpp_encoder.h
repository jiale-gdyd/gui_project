#ifndef LIBDRM_MEDIA_MPP_ENCODER_H
#define LIBDRM_MEDIA_MPP_ENCODER_H

#include <linux/kconfig.h>
#include <media/utils/encoder.h>
#include <rockchip/rkmpp/mpp_rc_api.h>
#include <rockchip/rkmpp/rk_venc_cfg.h>

#include "mpp_inc.h"

namespace libdrm {
#if defined(CONFIG_RKMPP_RGA_OSD)
typedef struct RgaOsdData_ {
    std::shared_ptr<MediaBuffer> data;
    uint32_t                     pos_x;
    uint32_t                     pos_y;
    uint32_t                     width;
    uint32_t                     height;
    uint8_t                      enable;
    uint32_t                     inverse;
    drm_region_type_e            region_type;
    uint32_t                     cover_color;
} RgaOsdData;
#endif

class MPPEncoder : public VideoEncoder {
public:
    MPPEncoder();
    virtual ~MPPEncoder();

    virtual bool Init() override;
    virtual int Process(const std::shared_ptr<MediaBuffer> &input, std::shared_ptr<MediaBuffer> &output, std::shared_ptr<MediaBuffer> extra_output) override;

    virtual int SendInput(const std::shared_ptr<MediaBuffer> &) override;
    virtual std::shared_ptr<MediaBuffer> FetchOutput() override;

    void set_statistics_switch(bool value);
    int get_statistics_bps();
    int get_statistics_fps();

#if defined(CONFIG_RKMPP_HWOSD)
    int OsdPaletteSet(uint32_t *ptl_data);
    int OsdRegionSet(DrmOsdRegionData *region_data);
    int OsdRegionGet(DrmOsdRegionData *region_data);
#endif

#if defined(CONFIG_RKMPP_RGA_OSD)
    int RgaOsdRegionSet(DrmOsdRegionData *rdata, DrmVideoConfig &vfg);
    int RgaOsdRegionProcess(ImageBuffer *hw_buffer);
#endif

    int SetUserData(const char *data, uint16_t len);
    void ClearUserData();
    void RestartUserData();
    void EnableUserDataAllFrame(bool value);

    int RoiUpdateRegions(DrmEncROIRegion *regions, int region_cnt);

    virtual void QueryChange(uint32_t change, void *value, int32_t size);

protected:
    MppCodingType coding_type;
    uint32_t      output_mb_flags;
    std::string   rc_api_brief_name;

    void SetMppCodeingType(MppCodingType type);
    int EncodeControl(int cmd, void *param);

    virtual int PrepareMppFrame(const std::shared_ptr<MediaBuffer> &input, std::shared_ptr<MediaBuffer> &mdinfo, MppFrame &frame);
    virtual int PrepareMppPacket(std::shared_ptr<MediaBuffer> &output, MppPacket &packet);
    virtual int PrepareMppExtraBuffer(std::shared_ptr<MediaBuffer> extra_output, MppBuffer &buffer);
    virtual int PrepareThumbnail(const std::shared_ptr<MediaBuffer> &input, int thumbnail_num, char *buf, int *len);
    int Process(MppFrame frame, MppPacket &packet, MppBuffer &mv_buf);

private:
    std::shared_ptr<MPPContext> mpp_ctx;

    bool                        encoder_sta_en;
    uint32_t                    encoded_bps;
    float                       encoded_fps;

    size_t                      stream_size_1s;
    size_t                      frame_cnt_1s;
    int64_t                     last_ts;
    int64_t                     cur_ts;

#if defined(CONFIG_RKMPP_HWOSD)
    MppBufferGroup              osd_buf_grp;
    MppEncOSDData               osd_data;
    MppEncOSDData2              osd_data2;
#endif

#if defined(CONFIG_RKMPP_RGA_OSD)
    RgaOsdData                  rga_osd_data[OSD_REGIONS_CNT];
    int                         rga_osd_cnt;
#endif

    MppEncROICfg                roi_cfg;

#define MPP_ENCODER_USERDATA_MAX_SIZE       65535
    char                        userdata[MPP_ENCODER_USERDATA_MAX_SIZE];
    uint16_t                    userdata_len;
    uint16_t                    userdata_frame_id;
    uint8_t                     userdata_all_frame_en;
    MppEncUserDataSet           ud_set;
    MppEncUserDataFull          ud_datas[2];
    uint8_t                     thumbnail_cnt;
#define THUMBNAIL_MAX_CNT                   3
#define THUMBNAIL_TYPE_APP1                 1
#define THUMBNAIL_TYPE_APP2                 2
    uint16_t                    thumbnail_width[THUMBNAIL_MAX_CNT];
    uint16_t                    thumbnail_height[THUMBNAIL_MAX_CNT];
    uint8_t                     thumbnail_type[THUMBNAIL_MAX_CNT];
    uint16_t                    app2_len;
    char                        app2[512];

    friend class MPPMJPEGConfig;
    friend class MPPCommonConfig;
};
}

#endif
