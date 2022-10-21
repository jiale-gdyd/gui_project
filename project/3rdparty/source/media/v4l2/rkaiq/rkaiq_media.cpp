#include <string.h>
#include <stdlib.h>
#include <media/utils/utils.h>

#include "rkaiq_media.h"

static media_entity *media_get_entity_by_name_rockchip(struct media_device *device, const char *name)
{
    return media_get_entity_by_name(device, name);
}

void RKAiqMedia::SetCameraOrder()
{
    if (link_cam.isp_link | link_cam.cif_link | link_cam.dvp_link) {
        cam_id2ispp_id[0] = 0;
    }

    if ((link_cam.cif_link & link_cam.dvp_link & !link_cam.isp_link)
        | (link_cam.isp_link & link_cam.dvp_link & !link_cam.cif_link)
        | (link_cam.cif_link & link_cam.isp_link & !link_cam.dvp_link))
    {
        cam_id2ispp_id[0] = 0;
        cam_id2ispp_id[1] = 1;
    }

    if (link_cam.isp_link & link_cam.cif_link & link_cam.dvp_link) {
        cam_id2ispp_id[0] = 0;
        cam_id2ispp_id[1] = 1;
        cam_id2ispp_id[2] = 2;
    }
}

void RKAiqMedia::FindLinkCam(int index)
{
    if (media_info[index].cif.linked_sensor) {
        link_cam.cif_link = 1;
    }

    if (media_info[index].cif_dvp.linked_sensor) {
        link_cam.dvp_link = 1;
    }

    if (media_info[index].isp.linked_sensor) {
        link_cam.isp_link = 1;
    }
}

void RKAiqMedia::BindCameraWithIsp()
{
    int free_cam_id2ispp_id;

    for (int i = 0; i < MAX_CAM_NUM; i++) {
        free_cam_id2ispp_id = ISP_ORDER_FREE;
        if ((media_info[i].isp.model_idx < 0) || (media_info[i].ispp.model_idx < 0)) {
            break;
        }

        for (int j = 0; j < MAX_CAM_NUM; j++) {
            if ((cam_id2ispp_id[j] == ISP_ORDER_OCCUPY) && (free_cam_id2ispp_id == ISP_ORDER_FREE)) {
                free_cam_id2ispp_id = j;
            }

            if (cam_id2ispp_id[j] == media_info[i].isp.model_idx) {
                free_cam_id2ispp_id = ISP_ORDER_FREE;
                break;
            }
        }

        if (free_cam_id2ispp_id != ISP_ORDER_FREE) {
            cam_id2ispp_id[free_cam_id2ispp_id] = media_info[i].isp.model_idx;
        }
    }
}

std::string RKAiqMedia::GetSensorName(struct media_device *device)
{
    std::string sensor_name;
    media_entity *entity = NULL;
    const struct media_entity_desc *entity_info;

    uint32_t nents = media_get_entities_count(device);
    for (uint32_t j = 0; j < nents; ++j) {
        entity = media_get_entity(device, j);
        entity_info = media_entity_get_info(entity);

        if ((NULL != entity_info) && (entity_info->type == MEDIA_ENT_T_V4L2_SUBDEV_SENSOR)) {
            sensor_name = entity_info->name;
            break;
        }
    }

    return sensor_name;
}

int RKAiqMedia::IsLinkSensor(struct media_device *device)
{
    int link_sensor = 0;
    media_entity *entity = NULL;
    const struct media_entity_desc *entity_info;
    uint32_t nents = media_get_entities_count(device);

    for (uint32_t j = 0; j < nents; ++j) {
        entity = media_get_entity(device, j);
        entity_info = media_entity_get_info(entity);

        if ((NULL != entity_info) && (entity_info->type == MEDIA_ENT_T_V4L2_SUBDEV_SENSOR)) {
            link_sensor = 1;
            break;
        }
    }

    return link_sensor;
}

void RKAiqMedia::GetIsppSubDevs(int id, struct media_device *device, const char *devpath)
{
    media_entity *entity = NULL;
    ispp_info_t *ispp_info = NULL;
    const char *entity_name = NULL;

    if (id >= MAX_CAM_NUM || id < 0) {
        return;
    }

    for (int index = 0; index < MAX_CAM_NUM; index++) {
        ispp_info = &media_info[index].ispp;
        if (ispp_info->media_dev_path.empty()) {
            break;
        }

        if (ispp_info->media_dev_path.compare(devpath) == 0) {
            DRM_MEDIA_LOGE("RKAIQ: isp info of path:[%s] exists", devpath);
            return;
        }
    }

    ispp_info->model_idx = id;
    ispp_info = &media_info[id].ispp;
    ispp_info->media_dev_path = devpath;

    entity = media_get_entity_by_name_rockchip(device, "rkispp_input_image");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            ispp_info->pp_input_image_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkispp_m_bypass");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            ispp_info->pp_m_bypass_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkispp_scale0");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            ispp_info->pp_scale0_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkispp_scale1");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            ispp_info->pp_scale1_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkispp_scale2");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            ispp_info->pp_scale2_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkispp_input_params");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            ispp_info->pp_input_params_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkispp-stats");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            ispp_info->pp_stats_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkispp-subdev");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            ispp_info->pp_dev_path = entity_name;
        }
    }

    DRM_MEDIA_LOGI("model:[%s]: ispp_info:[%d]: ispp-subdev entity name:[%s]", device->info.model, id, ispp_info->pp_dev_path.c_str());
}

void RKAiqMedia::GetIspSubDevs(int id, struct media_device *device, const char *devpath)
{
    media_entity *entity = NULL;
    isp_info_t *isp_info = NULL;
    const char *entity_name = NULL;

    if (id >= MAX_CAM_NUM) {
        DRM_MEDIA_LOGW("path:[%s] set id:[%d] over max:[%d]", devpath, id, MAX_CAM_NUM - 1);
        return;
    }

    for (int index = 0; index < MAX_CAM_NUM; index++) {
        isp_info = &media_info[index].isp;
        if (isp_info->media_dev_path.compare(devpath) == 0) {
            DRM_MEDIA_LOGE("isp info of path:[%s] exists", devpath);
            return;
        }
    }

    isp_info = &media_info[id].isp;

    isp_info->linked_sensor = IsLinkSensor(device);
    if (isp_info->linked_sensor) {
        isp_info->sensor_name = GetSensorName(device);
    }

    isp_info->model_idx = id;
    isp_info->media_dev_path = devpath;

    entity = media_get_entity_by_name_rockchip(device, "rkisp-isp-subdev");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            isp_info->isp_dev_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkisp-csi-subdev");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            isp_info->csi_dev_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkisp-mpfbc-subdev");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            isp_info->mpfbc_dev_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkisp_mainpath");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            isp_info->main_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkisp_selfpath");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            isp_info->self_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkisp_rawwr0");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            isp_info->rawwr0_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkisp_rawwr1");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            isp_info->rawwr1_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkisp_rawwr2");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            isp_info->rawwr2_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkisp_rawwr3");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            isp_info->rawwr3_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkisp_dmapath");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            isp_info->dma_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkisp_rawrd0_m");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            isp_info->rawrd0_m_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkisp_rawrd1_l");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            isp_info->rawrd1_l_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkisp_rawrd2_s");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            isp_info->rawrd2_s_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkisp-statistics");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            isp_info->stats_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkisp-input-params");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            isp_info->input_params_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkisp-mipi-luma");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
        isp_info->mipi_luma_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rockchip-mipi-dphy-rx");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
        isp_info->mipi_dphy_rx_path = entity_name;
        }
    }

    DRM_MEDIA_LOGI("model:[%s]: isp_info:[%d]: isp-subdev entity name:[%s]", device->info.model, id, isp_info->isp_dev_path.c_str());
}

void RKAiqMedia::GetCifSubDevs(int id, struct media_device *device, const char *devpath)
{
    (void)id;

    int index = 0;
    media_entity *entity = NULL;
    const char *entity_name = NULL;
    cif_info_t *cif_info = nullptr;
    int link_sensor = IsLinkSensor(device);

    for (index = 0; index < MAX_CAM_NUM; index++) {
        cif_info = &media_info[index].cif;
        if (cif_info->media_dev_path.empty()) {
            break;
        }

        if (cif_info->media_dev_path.compare(devpath) == 0) {
            DRM_MEDIA_LOGE("isp info of path:[%s] exists", devpath);
            return;
        }
    }

    if (index >= MAX_CAM_NUM) {
        return;
    }

    cif_info->model_idx = id;
    cif_info->linked_sensor = link_sensor;
    if (cif_info->linked_sensor) {
        cif_info->sensor_name = GetSensorName(device);
    }

    cif_info->media_dev_path = devpath;

    entity = media_get_entity_by_name_rockchip(device, "stream_cif_mipi_id0");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            cif_info->mipi_id0 = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "stream_cif_mipi_id1");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            cif_info->mipi_id1 = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "stream_cif_mipi_id2");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            cif_info->mipi_id2 = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "stream_cif_mipi_id3");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            cif_info->mipi_id3 = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkcif-mipi-luma");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            cif_info->mipi_luma_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rockchip-mipi-csi2");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            cif_info->mipi_csi2_sd_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkcif-lvds-subdev");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            cif_info->lvds_sd_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkcif-lite-lvds-subdev");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            cif_info->lvds_sd_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rockchip-mipi-dphy-rx");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            cif_info->mipi_dphy_rx_path = entity_name;
        }
    }
}

void RKAiqMedia::GetCifDvpSubDevs(int id, struct media_device *device, const char *devpath)
{
    (void)id;

    int index = 0;
    media_entity *entity = NULL;
    const char *entity_name = NULL;
    cif_dvp_info_t *cif_dvp_info = nullptr;
    int link_sensor = IsLinkSensor(device);

    for (index = 0; index < MAX_CAM_NUM; index++) {
        cif_dvp_info = &media_info[index].cif_dvp;
        if (cif_dvp_info->media_dev_path.empty()) {
            break;
        }

        if (cif_dvp_info->media_dev_path.compare(devpath) == 0) {
            DRM_MEDIA_LOGE("isp info of path:[%s] exists", devpath);
            return;
        }
    }

    if (index >= MAX_CAM_NUM) {
        return;
    }

    cif_dvp_info->model_idx = id;
    cif_dvp_info->linked_sensor = link_sensor;
    if (cif_dvp_info->linked_sensor) {
        cif_dvp_info->sensor_name = GetSensorName(device);
    }

    cif_dvp_info->media_dev_path = devpath;

    entity = media_get_entity_by_name_rockchip(device, "stream_cif_dvp_id0");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            cif_dvp_info->dvp_id0 = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "stream_cif_dvp_id1");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            cif_dvp_info->dvp_id1 = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "stream_cif_dvp_id2");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            cif_dvp_info->dvp_id2 = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "stream_cif_dvp_id3");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            cif_dvp_info->dvp_id3 = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkcif-mipi-luma");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            cif_dvp_info->mipi_luma_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkcif-dvp");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            cif_dvp_info->dvp_sd_path = entity_name;
        }
    }

    entity = media_get_entity_by_name_rockchip(device, "rkcif-dvp-sof");
    if (entity) {
        entity_name = media_entity_get_devname(entity);
        if (entity_name) {
            cif_dvp_info->dvp_sof_path = entity_name;
        }
    }
}

int RKAiqMedia::GetMediaInfo()
{
    int ret;
    char sys_path[64];
    unsigned int index = 0;
    struct media_device *device = NULL;

    while (index < MAX_MEDIA_NUM) {
        snprintf(sys_path, 64, "/dev/media%d", index++);
        if (access(sys_path, F_OK)) {
            continue;
        }

        DRM_MEDIA_LOGI("parsing sys_path:[%s]", sys_path);

        device = media_device_new(sys_path);
        if (!device) {
            return -ENOMEM;
        }

        ret = media_device_enumerate(device);
        if (ret) {
            media_device_unref(device);
            return ret;
        }

        if (strcmp(device->info.model, "rkispp0") == 0 || strcmp(device->info.model, "rkispp") == 0) {
            GetIsppSubDevs(0, device, sys_path);
        } else if (strcmp(device->info.model, "rkispp1") == 0) {
            GetIsppSubDevs(1, device, sys_path);
        } else if (strcmp(device->info.model, "rkispp2") == 0) {
            GetIsppSubDevs(2, device, sys_path);
        } else if (strcmp(device->info.model, "rkisp0") == 0 || strcmp(device->info.model, "rkisp") == 0) {
            GetIspSubDevs(0, device, sys_path);
        } else if (strcmp(device->info.model, "rkisp1") == 0) {
            GetIspSubDevs(1, device, sys_path);
        } else if (strcmp(device->info.model, "rkisp2") == 0) {
            GetIspSubDevs(2, device, sys_path);
        } else if (strcmp(device->info.model, "rkcif") == 0 || strcmp(device->info.model, "rkcif_mipi_lvds") == 0 || strcmp(device->info.model, "rkcif_lite_mipi_lvds") == 0) {
            GetCifSubDevs(index, device, sys_path);
        } else if (strcmp(device->info.model, "rkcif_dvp") == 0) {
            GetCifDvpSubDevs(index, device, sys_path);
        }

        FindLinkCam(index - 1);
        media_device_unref(device);
    }

    SetCameraOrder();
    BindCameraWithIsp();

    return ret;
}

int RKAiqMedia::DumpMediaInfo()
{
    DRM_MEDIA_LOGI("RKAIQ: DumpMediaInfo:");

    for (int i = 0; i < MAX_CAM_NUM; i++) {
        cif_info_t *cif = &media_info[i].cif;
        isp_info_t *isp = &media_info[i].isp;
        ispp_info_t *ispp = &media_info[i].ispp;
        cif_dvp_info_t *cif_dvp = &media_info[0].cif_dvp;

        DRM_MEDIA_LOGI("RKAIQ: ##### Camera index: [%d]", i);
        if (isp->linked_sensor) {
            DRM_MEDIA_LOGI("RKAIQ: \t link_isp_sensor_name     :    [%s]", isp->sensor_name.c_str());
        } else if (cif->linked_sensor) {
            DRM_MEDIA_LOGI("RKAIQ: \t link_cif_sensor_name     :    [%s]", cif->sensor_name.c_str());
        } else if (cif_dvp->linked_sensor && i == 2) {
            DRM_MEDIA_LOGI("RKAIQ: \t link_cif_dvp_sensor_name :    [%s]", cif_dvp->sensor_name.c_str());
        }

        if (cif->model_idx >= 0) {
            DRM_MEDIA_LOGI("RKAIQ: #### cif:");
            DRM_MEDIA_LOGI("RKAIQ: \t model_idx         : [%d]", cif->model_idx);
            DRM_MEDIA_LOGI("RKAIQ: \t linked_sensor     : [%d]", cif->linked_sensor);
            DRM_MEDIA_LOGI("RKAIQ: \t media_dev_path    : [%s]", cif->media_dev_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t mipi_id0          : [%s]", cif->mipi_id0.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t mipi_id1          : [%s]", cif->mipi_id1.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t mipi_id2          : [%s]", cif->mipi_id2.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t mipi_id3          : [%s]", cif->mipi_id3.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t mipi_dphy_rx_path : [%s]", cif->mipi_dphy_rx_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t mipi_csi2_sd_path : [%s]", cif->mipi_csi2_sd_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t lvds_sd_path      : [%s]", cif->lvds_sd_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t mipi_luma_path    : [%s]", cif->mipi_luma_path.c_str());
        }

        if (i == 2) {
            if (cif_dvp->model_idx >= 0) {
                DRM_MEDIA_LOGI("RKAIQ: #### cif_dvp:");
                DRM_MEDIA_LOGI("RKAIQ: \t model_idx      : [%d]", cif_dvp->model_idx);
                DRM_MEDIA_LOGI("RKAIQ: \t linked_sensor  : [%d]", cif_dvp->linked_sensor);
                DRM_MEDIA_LOGI("RKAIQ: \t media_dev_path : [%s]", cif_dvp->media_dev_path.c_str());
                DRM_MEDIA_LOGI("RKAIQ: \t dvp_id0        : [%s]", cif_dvp->dvp_id0.c_str());
                DRM_MEDIA_LOGI("RKAIQ: \t dvp_id1        : [%s]", cif_dvp->dvp_id1.c_str());
                DRM_MEDIA_LOGI("RKAIQ: \t dvp_id2        : [%s]", cif_dvp->dvp_id2.c_str());
                DRM_MEDIA_LOGI("RKAIQ: \t dvp_id3        : [%s]", cif_dvp->dvp_id3.c_str());
                DRM_MEDIA_LOGI("RKAIQ: \t dvp_sd_path    : [%s]", cif_dvp->dvp_sd_path.c_str());
                DRM_MEDIA_LOGI("RKAIQ: \t mipi_luma_path : [%s]", cif_dvp->mipi_luma_path.c_str());
                DRM_MEDIA_LOGI("RKAIQ: \t dvp_sof_path   : [%s]", cif_dvp->dvp_sof_path.c_str());
            }
        }

        if (isp->model_idx >= 0) {
            DRM_MEDIA_LOGI("RKAIQ: #### isp:");
            DRM_MEDIA_LOGI("RKAIQ: \t model_idx         : [%d]", isp->model_idx);
            DRM_MEDIA_LOGI("RKAIQ: \t linked_sensor     : [%d]", isp->linked_sensor);
            DRM_MEDIA_LOGI("RKAIQ: \t media_dev_path    : [%s]", isp->media_dev_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t isp_dev_path      : [%s]", isp->isp_dev_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t csi_dev_path      : [%s]", isp->csi_dev_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t mpfbc_dev_path    : [%s]", isp->mpfbc_dev_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t main_path         : [%s]", isp->main_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t self_path         : [%s]", isp->self_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t rawwr0_path       : [%s]", isp->rawwr0_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t rawwr1_path       : [%s]", isp->rawwr1_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t rawwr2_path       : [%s]", isp->rawwr2_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t rawwr3_path       : [%s]", isp->rawwr3_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t dma_path          : [%s]", isp->dma_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t rawrd0_m_path     : [%s]", isp->rawrd0_m_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t rawrd1_l_path     : [%s]", isp->rawrd1_l_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t rawrd2_s_path     : [%s]", isp->rawrd2_s_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t stats_path        : [%s]", isp->stats_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t input_params_path : [%s]", isp->input_params_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t mipi_luma_path    : [%s]", isp->mipi_luma_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t mipi_dphy_rx_path : [%s]", isp->mipi_dphy_rx_path.c_str());
        }

        if (ispp->model_idx >= 0) {
            DRM_MEDIA_LOGI("RKAIQ: #### ispp:");
            DRM_MEDIA_LOGI("RKAIQ: \t model_idx            : [%d]", ispp->model_idx);
            DRM_MEDIA_LOGI("RKAIQ: \t media_dev_path       : [%s]", ispp->media_dev_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t pp_input_image_path  : [%s]", ispp->pp_input_image_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t pp_m_bypass_path     : [%s]", ispp->pp_m_bypass_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t pp_scale0_path       : [%s]", ispp->pp_scale0_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t pp_scale1_path       : [%s]", ispp->pp_scale1_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t pp_scale2_path       : [%s]", ispp->pp_scale2_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t pp_input_params_path : [%s]", ispp->pp_input_params_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t pp_stats_path        : [%s]", ispp->pp_stats_path.c_str());
            DRM_MEDIA_LOGI("RKAIQ: \t pp_dev_path          : [%s]", ispp->pp_dev_path.c_str());
        }
    }

    return 0;
}

std::string RKAiqMedia::GetVideoNode(int camera_id, const char *chn_name)
{
    DRM_MEDIA_LOGW("camera_id:[%d], channel name:[%s]", camera_id, chn_name);

    if (camera_id >= MAX_CAM_NUM || camera_id < 0) {
        return "";
    }

    if (camera_id >= 3) {
        DRM_MEDIA_LOGW("video node may only support 3 sensor");
    }

    int idx = cam_id2ispp_id[camera_id];
    if (idx >= MAX_CAM_NUM || idx < 0) {
        return "";
    }

    DRM_MEDIA_LOGW("camera_id:[%d], chanenl name:[%s], idx:[%d]", camera_id, chn_name, idx);

    if (strstr(chn_name, "bypass")) {
        return media_info[idx].ispp.pp_m_bypass_path;
    } else if (strstr(chn_name, "scale0")) {
        return media_info[idx].ispp.pp_scale0_path;
    } else if (strstr(chn_name, "scale1")) {
        return media_info[idx].ispp.pp_scale1_path;
    } else if (strstr(chn_name, "scale2")) {
        return media_info[idx].ispp.pp_scale2_path;
    }

    return "";
}

RKAiqMedia::RKAiqMedia()
{
    for (int i = 0; i < MAX_CAM_NUM; i++) {
        media_info[i].cif.model_idx = -1;
        media_info[i].cif_dvp.model_idx = -1;
        media_info[i].isp.model_idx = -1;
        media_info[i].ispp.model_idx = -1;

        media_info[i].cif_dvp.linked_sensor = 0;
        media_info[i].cif.linked_sensor = 0;
        media_info[i].isp.linked_sensor = 0;
        cam_id2ispp_id[i] = ISP_ORDER_FREE;
    }

    GetMediaInfo();
    // DumpMediaInfo();
}
