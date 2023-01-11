#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/ioctl.h>

#include <rockchip/rkrgax/im2d.h>
#include <rockchip/rkrgax/rgadbg.h>
#include <rockchip/rkrgax/im2d_impl.h>
#include <rockchip/rkrgax/im2d_hardware.h>

#include <rockchip/rkrgax/RgaUtils.h>
#include <rockchip/rkrgax/NormalRga.h>
#include <rockchip/rkrgax/RockchipRga.h>

#define MAX(n1, n2)         ((n1) > (n2) ? (n1) : (n2))
#define GET_GCD(n1, n2)                                 \
    ({                                                  \
        int i;                                          \
        for(i = 1; i <= (n1) && i <= (n2); i++) {       \
            if (((n1) % i == 0) && ((n2) % i == 0)) {   \
                gcd = i;                                \
            }                                           \
        }                                               \
        gcd;                                            \
    })
#define GET_LCM(n1, n2, gcd) (((n1) * (n2)) / gcd)

extern struct rgaContext *rgaCtx;
extern __thread char rga_err_str[ERR_MSG_LEN];

__thread im_context_t g_im2d_context;
struct im2d_job_manager g_im2d_job_manager;
__thread char rga_err_str[ERR_MSG_LEN] = "The current error message is empty!";

static IM_STATUS rga_get_context(void)
{
    if (rgaCtx == NULL) {
        RockchipRga &rkRga(RockchipRga::get());
        if (rgaCtx == NULL) {
            rga_error("rga_im2d: The current RockchipRga singleton is destroyed. Please check if RkRgaInit/RkRgaDeInit are called, if so, please disable them.");
            imSetErrorMsg("The current RockchipRga singleton is destroyed. Please check if RkRgaInit/RkRgaDeInit are called, if so, please disable them.");
            return IM_STATUS_FAILED;
        }
    }

    return IM_STATUS_SUCCESS;
}

static IM_STATUS rga_support_info_merge_table(rga_info_table_entry *dst_table, rga_info_table_entry *merge_table)
{
    if ((dst_table == NULL) || (merge_table == NULL)) {
        rga_error("dst or merge table is NULL");
        return IM_STATUS_FAILED;
    }

    dst_table->version |= merge_table->version;
    dst_table->input_format |= merge_table->input_format;
    dst_table->output_format |= merge_table->output_format;
    dst_table->feature |= merge_table->feature;

    dst_table->input_resolution = MAX(dst_table->input_resolution, merge_table->input_resolution);
    dst_table->output_resolution = MAX(dst_table->output_resolution, merge_table->output_resolution);
    dst_table->byte_stride = MAX(dst_table->byte_stride, merge_table->byte_stride);
    dst_table->scale_limit = MAX(dst_table->scale_limit, merge_table->scale_limit);
    dst_table->performance = MAX(dst_table->performance, merge_table->performance);

    return IM_STATUS_SUCCESS;
}

static inline int rga_version_compare(struct rga_version_t version1, struct rga_version_t version2)
{
    if (version1.major > version2.major) {
        return 1;
    } else if ((version1.major == version2.major) && (version1.minor > version2.minor)) {
        return 1;
    } else if ((version1.major == version2.major) && (version1.minor == version2.minor) && (version1.revision > version2.revision)) {
        return 1;
    } else if ((version1.major == version2.major) && (version1.minor == version2.minor) && (version1.revision == version2.revision)) {
        return 0;
    }

    return -1;
}

static IM_STATUS rga_yuv_legality_check(const char *name, rga_buffer_t info, im_rect rect)
{
    if ((info.wstride % 2) || (info.hstride % 2) || (info.width % 2)  || (info.height % 2) || (rect.x % 2) || (rect.y % 2) || (rect.width % 2) || (rect.height % 2)) {
        imSetErrorMsg("%s, Error yuv not align to 2, rect[x,y,w,h] = [%d, %d, %d, %d], wstride = %d, hstride = %d, format = 0x%x(%s)", name, rect.x, rect.y, info.width, info.height, info.wstride, info.hstride, info.format, translate_format_str(info.format));
        return IM_STATUS_INVALID_PARAM;
    }

    return IM_STATUS_SUCCESS;
}

int imSetErrorMsg(const char *format, ...)
{
    va_list ap;
    int ret = 0;

    va_start(ap, format);
    ret = vsnprintf(rga_err_str, ERR_MSG_LEN, format, ap);
    va_end(ap);

    return ret;
}

bool rga_is_buffer_valid(rga_buffer_t buf)
{
    return ((buf.phy_addr != NULL) || (buf.vir_addr != NULL) || (buf.fd > 0) || (buf.handle > 0));
}

bool rga_is_rect_valid(im_rect rect)
{
    return ((rect.x > 0) || (rect.y > 0) || ((rect.width > 0) && (rect.height > 0)));
}

void empty_structure(rga_buffer_t *src, rga_buffer_t *dst, rga_buffer_t *pat, im_rect *srect, im_rect *drect, im_rect *prect, im_opt_t *opt)
{
    if (src != NULL) {
        memset(src, 0, sizeof(*src));
    }

    if (dst != NULL) {
        memset(dst, 0, sizeof(*dst));
    }

    if (pat != NULL) {
        memset(pat, 0, sizeof(*pat));
    }

    if (srect != NULL) {
        memset(srect, 0, sizeof(*srect));
    }

    if (drect != NULL) {
        memset(drect, 0, sizeof(*drect));
    }

    if (prect != NULL) {
        memset(prect, 0, sizeof(*prect));
    }

    if (opt != NULL) {
        memset(opt, 0, sizeof(*opt));
    }
}

IM_STATUS rga_set_buffer_info(rga_buffer_t dst, rga_info_t* dstinfo)
{
    if (NULL == dstinfo) {
        rga_error("rga_im2d: invaild dstinfo");
        imSetErrorMsg("Dst structure address is NULL.");
        return IM_STATUS_INVALID_PARAM;
    }

    if (dst.handle > 0) {
        dstinfo->handle = dst.handle;
    } else if (dst.phy_addr != NULL) {
        dstinfo->phyAddr= dst.phy_addr;
    } else if (dst.fd > 0) {
        dstinfo->fd = dst.fd;
        dstinfo->mmuFlag = 1;
    } else if (dst.vir_addr != NULL) {
        dstinfo->virAddr = dst.vir_addr;
        dstinfo->mmuFlag = 1;
    } else {
        rga_error("rga_im2d: invaild dst buffer");
        imSetErrorMsg("No address available in dst buffer, phy_addr = %ld, fd = %d, vir_addr = %ld, handle = %d", (unsigned long)dst.phy_addr, dst.fd, (unsigned long)dst.vir_addr, dst.handle);
        return IM_STATUS_INVALID_PARAM;
    }

    return IM_STATUS_SUCCESS;
}

IM_STATUS rga_set_buffer_info(const rga_buffer_t src, rga_buffer_t dst, rga_info_t* srcinfo, rga_info_t* dstinfo)
{
    if (NULL == srcinfo) {
        rga_error("rga_im2d: invaild srcinfo");
        imSetErrorMsg("Src structure address is NULL.");
        return IM_STATUS_INVALID_PARAM;
    }

    if (NULL == dstinfo) {
        rga_error("rga_im2d: invaild dstinfo");
        imSetErrorMsg("Dst structure address is NULL.");
        return IM_STATUS_INVALID_PARAM;
    }

    if (src.handle > 0) {
        srcinfo->handle = src.handle;
    } else if (src.phy_addr != NULL) {
        srcinfo->phyAddr = src.phy_addr;
    } else if (src.fd > 0) {
        srcinfo->fd = src.fd;
        srcinfo->mmuFlag = 1;
    } else if (src.vir_addr != NULL) {
        srcinfo->virAddr = src.vir_addr;
        srcinfo->mmuFlag = 1;
    } else {
        rga_error("rga_im2d: invaild src buffer");
        imSetErrorMsg("No address available in src buffer, phy_addr = %ld, fd = %d, vir_addr = %ld, handle = %d", (unsigned long)src.phy_addr, src.fd, (unsigned long)src.vir_addr, src.handle);
        return IM_STATUS_INVALID_PARAM;
    }

    if (dst.handle > 0) {
        dstinfo->handle = dst.handle;
    } else if (dst.phy_addr != NULL) {
        dstinfo->phyAddr= dst.phy_addr;
    } else if (dst.fd > 0) {
        dstinfo->fd = dst.fd;
        dstinfo->mmuFlag = 1;
    } else if (dst.vir_addr != NULL) {
        dstinfo->virAddr = dst.vir_addr;
        dstinfo->mmuFlag = 1;
    } else {
        rga_error("rga_im2d: invaild dst buffer");
        imSetErrorMsg("No address available in dst buffer, phy_addr = %ld, fd = %d, vir_addr = %ld, handle = %d", (unsigned long)dst.phy_addr, dst.fd, (unsigned long)dst.vir_addr, dst.handle);
        return IM_STATUS_INVALID_PARAM;
    }

    return IM_STATUS_SUCCESS;
}

IM_STATUS rga_get_info(rga_info_table_entry *return_table)
{
    int ret;
    int  rga_version = 0;
    rga_info_table_entry merge_table;

    ret = rga_get_context();
    if (ret != IM_STATUS_SUCCESS) {
        return (IM_STATUS)ret;
    }

    memset(&merge_table, 0x0, sizeof(merge_table));

    for (uint32_t i = 0; i < rgaCtx->mHwVersions.size; i++) {
        if ((rgaCtx->mHwVersions.version[i].major == 2) && (rgaCtx->mHwVersions.version[i].minor == 0)) {
            if (rgaCtx->mHwVersions.version[i].revision == 0) {
                rga_version = IM_RGA_HW_VERSION_RGA_2_INDEX;
                memcpy(&merge_table, &hw_info_table[rga_version], sizeof(merge_table));
            } else {
                goto TRY_TO_COMPATIBLE;
            }
        } else if ((rgaCtx->mHwVersions.version[i].major == 3) && (rgaCtx->mHwVersions.version[i].minor == 0)) {
            switch (rgaCtx->mHwVersions.version[i].revision) {
                case 0x16445:
                    rga_version = IM_RGA_HW_VERSION_RGA_2_INDEX;
                    memcpy(&merge_table, &hw_info_table[rga_version], sizeof(merge_table));
                    break;

                case 0x22245:
                    rga_version = IM_RGA_HW_VERSION_RGA_2_ENHANCE_INDEX;
                    memcpy(&merge_table, &hw_info_table[rga_version], sizeof(merge_table));
                    break;

                case 0x76831:
                    rga_version = IM_RGA_HW_VERSION_RGA_3_INDEX;
                    memcpy(&merge_table, &hw_info_table[rga_version], sizeof(merge_table));
                    break;

                default:
                    goto TRY_TO_COMPATIBLE;
            }
        } else if ((rgaCtx->mHwVersions.version[i].major == 3) && (rgaCtx->mHwVersions.version[i].minor == 2)) {
            switch (rgaCtx->mHwVersions.version[i].revision) {
                case 0x18218:
                    rga_version = IM_RGA_HW_VERSION_RGA_2_ENHANCE_INDEX;
                    memcpy(&merge_table, &hw_info_table[rga_version], sizeof(merge_table));
                    merge_table.feature |= IM_RGA_SUPPORT_FEATURE_ROP;
                    break;

                case 0x56726:
                case 0x63318:
                    rga_version = IM_RGA_HW_VERSION_RGA_2_ENHANCE_INDEX;
                    memcpy(&merge_table, &hw_info_table[rga_version], sizeof(merge_table));

                    merge_table.input_format |= IM_RGA_SUPPORT_FORMAT_YUYV_422 | IM_RGA_SUPPORT_FORMAT_YUV_400;
                    merge_table.output_format |= IM_RGA_SUPPORT_FORMAT_YUV_400 | IM_RGA_SUPPORT_FORMAT_Y4;
                    merge_table.feature |= IM_RGA_SUPPORT_FEATURE_QUANTIZE | IM_RGA_SUPPORT_FEATURE_SRC1_R2Y_CSC | IM_RGA_SUPPORT_FEATURE_DST_FULL_CSC;
                    break;

                default :
                    goto TRY_TO_COMPATIBLE;
            }
        } else if ((rgaCtx->mHwVersions.version[i].major == 3) && (rgaCtx->mHwVersions.version[i].minor == 3)) {
            switch (rgaCtx->mHwVersions.version[i].revision) {
                case 0x87975:
                    rga_version = IM_RGA_HW_VERSION_RGA_2_ENHANCE_INDEX;
                    memcpy(&merge_table, &hw_info_table[rga_version], sizeof(merge_table));

                    merge_table.input_format |= IM_RGA_SUPPORT_FORMAT_YUYV_422 | IM_RGA_SUPPORT_FORMAT_YUV_400 | IM_RGA_SUPPORT_FORMAT_RGBA2BPP;
                    merge_table.output_format |= IM_RGA_SUPPORT_FORMAT_YUV_400 | IM_RGA_SUPPORT_FORMAT_Y4;
                    merge_table.feature |= IM_RGA_SUPPORT_FEATURE_QUANTIZE | IM_RGA_SUPPORT_FEATURE_SRC1_R2Y_CSC | IM_RGA_SUPPORT_FEATURE_DST_FULL_CSC | IM_RGA_SUPPORT_FEATURE_MOSAIC | IM_RGA_SUPPORT_FEATURE_OSD | IM_RGA_SUPPORT_FEATURE_PRE_INTR;
                    break;

                default:
                    goto TRY_TO_COMPATIBLE;
            }
        } else if ((rgaCtx->mHwVersions.version[i].major == 3) && (rgaCtx->mHwVersions.version[i].minor == 7)) {
            switch (rgaCtx->mHwVersions.version[i].revision) {
                case 0x93215:
                    rga_version = IM_RGA_HW_VERSION_RGA_2_ENHANCE_INDEX;
                    memcpy(&merge_table, &hw_info_table[rga_version], sizeof(merge_table));

                    merge_table.input_format |= IM_RGA_SUPPORT_FORMAT_YUYV_422 | IM_RGA_SUPPORT_FORMAT_YUV_400 | IM_RGA_SUPPORT_FORMAT_RGBA2BPP;
                    merge_table.output_format |= IM_RGA_SUPPORT_FORMAT_YUV_400 | IM_RGA_SUPPORT_FORMAT_Y4;
                    merge_table.feature |= IM_RGA_SUPPORT_FEATURE_QUANTIZE | IM_RGA_SUPPORT_FEATURE_SRC1_R2Y_CSC | IM_RGA_SUPPORT_FEATURE_DST_FULL_CSC | IM_RGA_SUPPORT_FEATURE_MOSAIC | IM_RGA_SUPPORT_FEATURE_OSD | IM_RGA_SUPPORT_FEATURE_PRE_INTR;
                    break;

                default:
                    goto TRY_TO_COMPATIBLE;
            }
        } else if ((rgaCtx->mHwVersions.version[i].major == 4) && (rgaCtx->mHwVersions.version[i].minor == 0)) {
            switch (rgaCtx->mHwVersions.version[i].revision) {
                case 0x18632:
                    rga_version = IM_RGA_HW_VERSION_RGA_2_LITE0_INDEX;
                    memcpy(&merge_table, &hw_info_table[rga_version], sizeof(merge_table));
                    break;

                case 0x23998:
                case 0x28610:
                    rga_version = IM_RGA_HW_VERSION_RGA_2_LITE1_INDEX;
                    memcpy(&merge_table, &hw_info_table[rga_version], sizeof(merge_table));
                    merge_table.feature |= IM_RGA_SUPPORT_FEATURE_SRC1_R2Y_CSC;
                    break;

                default:
                    goto TRY_TO_COMPATIBLE;
            }
        } else if ((rgaCtx->mHwVersions.version[i].major == 42) && (rgaCtx->mHwVersions.version[i].minor == 0)) {
            if (rgaCtx->mHwVersions.version[i].revision == 0x17760) {
                rga_version = IM_RGA_HW_VERSION_RGA_2_LITE1_INDEX;
                memcpy(&merge_table, &hw_info_table[rga_version], sizeof(merge_table));
            } else {
                goto TRY_TO_COMPATIBLE;
            }
        } else {
            goto TRY_TO_COMPATIBLE;
        }

        rga_support_info_merge_table(return_table, &merge_table);
    }

    return IM_STATUS_SUCCESS;

TRY_TO_COMPATIBLE:
    if (strncmp((char *)rgaCtx->mHwVersions.version[0].str, "1.3", 3) == 0) {
        rga_version = IM_RGA_HW_VERSION_RGA_1_INDEX;
    } else if (strncmp((char *)rgaCtx->mHwVersions.version[0].str, "1.6", 3) == 0) {
        rga_version = IM_RGA_HW_VERSION_RGA_1_PLUS_INDEX;
    } else if (strncmp((char *)rgaCtx->mHwVersions.version[0].str, "2.00", 4) == 0) {
        rga_version = IM_RGA_HW_VERSION_RGA_2_INDEX;
    } else if (strncmp((char *)rgaCtx->mHwVersions.version[0].str, "3.00", 4) == 0) {
        rga_version = IM_RGA_HW_VERSION_RGA_2_INDEX;
    } else if (strncmp((char *)rgaCtx->mHwVersions.version[0].str, "3.02", 4) == 0) {
        rga_version = IM_RGA_HW_VERSION_RGA_2_ENHANCE_INDEX;
    } else if (strncmp((char *)rgaCtx->mHwVersions.version[0].str, "4.00", 4) == 0) {
        rga_version = IM_RGA_HW_VERSION_RGA_2_LITE0_INDEX;
    } else if (strncmp((char *)rgaCtx->mHwVersions.version[0].str, "4.00", 4) == 0) {
        rga_version = IM_RGA_HW_VERSION_RGA_2_LITE1_INDEX;
    } else {
        rga_version = IM_RGA_HW_VERSION_RGA_V_ERR_INDEX;
    }

    memcpy(return_table, &hw_info_table[rga_version], sizeof(rga_info_table_entry));

    if (rga_version == IM_RGA_HW_VERSION_RGA_V_ERR_INDEX) {
        rga_error("rga_im2d: Can not get the correct RGA version, please check the driver, version=%s", rgaCtx->mHwVersions.version[0].str);
        imSetErrorMsg("Can not get the correct RGA version, please check the driver, version=%s", rgaCtx->mHwVersions.version[0].str);
        return IM_STATUS_FAILED;
    }

    return IM_STATUS_SUCCESS;
}

IM_STATUS rga_check_driver(void)
{
    bool user_bind = false;
    int table_size, bind_index, least_index;

    if (rgaCtx == NULL) {
        rga_error("rga context is NULL!");
        imSetErrorMsg("rga context is NULL!");
        return IM_STATUS_FAILED;
    }

    table_size = sizeof(driver_bind_table) / sizeof(rga_dirver_bind_table_entry);
    for (int i = (table_size - 1); i >= 0; i--) {
        if (rga_version_compare((struct rga_version_t) { RGA_API_MAJOR_VERSION, RGA_API_MINOR_VERSION, RGA_API_REVISION_VERSION, RGA_API_VERSION }, driver_bind_table[i].user) >= 0) {
            if (i == (table_size - 1)) {
                user_bind = true;
                bind_index = i;
                break;
            } else if (rga_version_compare(driver_bind_table[i + 1].user, (struct rga_version_t) { RGA_API_MAJOR_VERSION, RGA_API_MINOR_VERSION, RGA_API_REVISION_VERSION, RGA_API_VERSION }) > 0) {
                user_bind = true;
                bind_index = i;
                break;
            }
        }
    }

    if (user_bind) {
        if (rga_version_compare(rgaCtx->mDriverVersion, driver_bind_table[bind_index].driver) >= 0) {
            if (bind_index == table_size - 1) {
                return IM_STATUS_SUCCESS;
            } else if (rga_version_compare(driver_bind_table[bind_index + 1].driver, rgaCtx->mDriverVersion) > 0) {
                return IM_STATUS_SUCCESS;
            } else {
                least_index = table_size - 1;
                for (int i = (table_size - 1); i >= 0; i--) {
                    if (rga_version_compare(rgaCtx->mDriverVersion, driver_bind_table[i].driver) >= 0) {
                        if (i == (table_size - 1)) {
                            least_index = i;
                            break;
                        } else if (rga_version_compare(driver_bind_table[i + 1].driver, rgaCtx->mDriverVersion) > 0) {
                            least_index = i;
                            break;
                        }
                    }
                }

                rga_error("The librga needs to be updated to version %s at least. current version: librga %s, driver %s.", driver_bind_table[least_index].user.str, RGA_API_VERSION, rgaCtx->mDriverVersion.str);
                imSetErrorMsg("The librga needs to be updated to version %s at least. current version: librga %s, driver %s.", driver_bind_table[least_index].user.str, RGA_API_VERSION, rgaCtx->mDriverVersion.str);

                return IM_STATUS_ERROR_VERSION;
            }
        } else {
            rga_error("The driver may be compatible, but it is best to update the driver to version %s. current version: librga %s, driver %s.", driver_bind_table[bind_index].driver.str, RGA_API_VERSION, rgaCtx->mDriverVersion.str);
            imSetErrorMsg("The driver may be compatible, but it is best to update the driver to version %s. current version: librga %s, driver %s.", driver_bind_table[bind_index].driver.str, RGA_API_VERSION, rgaCtx->mDriverVersion.str);

            return IM_STATUS_NOERROR;
        }
    } else {
        rga_error("Failed to get the version binding table of librga, current version: librga: %s, driver: %s", RGA_API_VERSION, rgaCtx->mDriverVersion.str);
        imSetErrorMsg("Failed to get the version binding table of librga, current version: librga: %s, driver: %s", RGA_API_VERSION, rgaCtx->mDriverVersion.str);

        return IM_STATUS_ERROR_VERSION;
    }
}

IM_STATUS rga_check_info(const char *name, const rga_buffer_t info, const im_rect rect, int resolution_usage)
{
    if ((info.width <= 0) || (info.height <= 0) || (info.format < 0)) {
        imSetErrorMsg("Illegal %s, the parameter cannot be negative or 0, width = %d, height = %d, format = 0x%x(%s)", name, info.width, info.height, info.format, translate_format_str(info.format));
        return IM_STATUS_ILLEGAL_PARAM;
    }

    if ((info.width < 2) || (info.height < 2)) {
        imSetErrorMsg("Hardware limitation %s, unsupported operation of images smaller than 2 pixels, width = %d, height = %d", name, info.width, info.height);
        return IM_STATUS_ILLEGAL_PARAM;
    }

    if ((info.wstride < info.width) || (info.hstride < info.height)) {
        imSetErrorMsg("Invaild %s, Virtual width or height is less than actual width and height, wstride = %d, width = %d, hstride = %d, height = %d", name, info.wstride, info.width, info.hstride, info.height);
        return IM_STATUS_INVALID_PARAM;
    }

    if ((rect.width < 0) || (rect.height < 0) || (rect.x < 0) || (rect.y < 0)) {
        imSetErrorMsg("Illegal %s rect, the parameter cannot be negative, rect[x,y,w,h] = [%d, %d, %d, %d]", name, rect.x, rect.y, rect.width, rect.height);
        return IM_STATUS_ILLEGAL_PARAM;
    }

    if (((rect.width > 0) && (rect.width < 2)) || ((rect.height > 0) && (rect.height < 2)) || ((rect.x > 0) && (rect.x < 2)) || ((rect.y > 0) && (rect.y < 2))) {
        imSetErrorMsg("Hardware limitation %s rect, unsupported operation of images smaller than 2 pixels, rect[x,y,w,h] = [%d, %d, %d, %d]", name, rect.x, rect.y, rect.width, rect.height);
        return IM_STATUS_INVALID_PARAM;
    }

    if (((rect.width + rect.x) > info.wstride) || ((rect.height + rect.y) > info.hstride)) {
        imSetErrorMsg("Invaild %s rect, the sum of width and height of rect needs to be less than wstride or hstride, rect[x,y,w,h] = [%d, %d, %d, %d], wstride = %d, hstride = %d", name, rect.x, rect.y, rect.width, rect.height, info.wstride, info.hstride);
        return IM_STATUS_INVALID_PARAM;
    }

    if ((info.width > resolution_usage) || (info.height > resolution_usage)) {
        imSetErrorMsg("Unsupported %s to input resolution more than %d, width = %d, height = %d", name, resolution_usage, info.width, info.height);
        return IM_STATUS_NOT_SUPPORTED;
    } else if (((rect.width > 0) && (rect.width > resolution_usage)) || ((rect.height > 0) && (rect.height > resolution_usage))) {
        imSetErrorMsg("Unsupported %s rect to output resolution more than %d, rect[x,y,w,h] = [%d, %d, %d, %d]", name, resolution_usage, rect.x, rect.y, rect.width, rect.height);
        return IM_STATUS_NOT_SUPPORTED;
    }

    return IM_STATUS_NOERROR;
}

IM_STATUS rga_check_limit(rga_buffer_t src, rga_buffer_t dst, int scale_usage, int mode_usage)
{
    int src_width = 0, src_height = 0;
    int dst_width = 0, dst_height = 0;

    src_width = src.width;
    src_height = src.height;

    if ((mode_usage & IM_HAL_TRANSFORM_ROT_270) || (mode_usage & IM_HAL_TRANSFORM_ROT_90)) {
        dst_width = dst.height;
        dst_height = dst.width;
    } else {
        dst_width = dst.width;
        dst_height = dst.height;
    }

    if (((src_width >> (int)(log(scale_usage)/log(2))) > dst_width)
        || ((src_height >> (int)(log(scale_usage)/log(2))) > dst_height))
    {
        imSetErrorMsg("Unsupported to scaling less than 1/%d ~ %d times, src[w,h] = [%d, %d], dst[w,h] = [%d, %d]", scale_usage, scale_usage, src.width, src.height, dst.width, dst.height);
        return IM_STATUS_NOT_SUPPORTED;
    }

    if (((dst_width >> (int)(log(scale_usage)/log(2))) > src_width)
        || ((dst_height >> (int)(log(scale_usage)/log(2))) > src_height))
    {
        imSetErrorMsg("Unsupported to scaling more than 1/%d ~ %d times, src[w,h] = [%d, %d], dst[w,h] = [%d, %d]", scale_usage, scale_usage, src.width, src.height, dst.width, dst.height);
        return IM_STATUS_NOT_SUPPORTED;
    }

    return IM_STATUS_NOERROR;
}

IM_STATUS rga_check_format(const char *name, rga_buffer_t info, im_rect rect, int format_usage, int mode_usgae)
{
    IM_STATUS ret;
    int format = -1;

    format = RkRgaGetRgaFormat(RkRgaCompatibleFormat(info.format));
    if (-1 == format) {
        imSetErrorMsg("illegal %s format, please query and fix, format = 0x%x(%s)\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
        return IM_STATUS_NOT_SUPPORTED;
    }

    if (format == RK_FORMAT_RGBA_8888
        || format == RK_FORMAT_BGRA_8888
        || format == RK_FORMAT_RGBX_8888
        || format == RK_FORMAT_BGRX_8888
        || format == RK_FORMAT_ARGB_8888
        || format == RK_FORMAT_ABGR_8888
        || format == RK_FORMAT_XRGB_8888
        || format == RK_FORMAT_XBGR_8888
        || format == RK_FORMAT_RGB_888
        || format == RK_FORMAT_BGR_888
        || format == RK_FORMAT_RGB_565
        || format == RK_FORMAT_BGR_565)
    {
        if (~format_usage & IM_RGA_SUPPORT_FORMAT_RGB) {
            imSetErrorMsg("%s unsupported RGB format, format = 0x%x(%s)\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
            return IM_STATUS_NOT_SUPPORTED;
        }
    } else if (format == RK_FORMAT_RGBA_4444
        || format == RK_FORMAT_BGRA_4444
        || format == RK_FORMAT_RGBA_5551
        || format == RK_FORMAT_BGRA_5551
        || format == RK_FORMAT_ARGB_4444
        || format == RK_FORMAT_ABGR_4444
        || format == RK_FORMAT_ARGB_5551
        || format == RK_FORMAT_ABGR_5551)
    {
        if (~format_usage & IM_RGA_SUPPORT_FORMAT_RGB_OTHER) {
            imSetErrorMsg("%s unsupported RGBA 4444/5551 format, format = 0x%x(%s)\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
            return IM_STATUS_NOT_SUPPORTED;
        }
    } else if (format == RK_FORMAT_BPP1
        || format == RK_FORMAT_BPP2
        || format == RK_FORMAT_BPP4
        || format == RK_FORMAT_BPP8)
    {
        if ((~format_usage & IM_RGA_SUPPORT_FORMAT_BPP) && !(mode_usgae & IM_COLOR_PALETTE)) {
            imSetErrorMsg("%s unsupported BPP format, format = 0x%x(%s)\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
            return IM_STATUS_NOT_SUPPORTED;
        }
    } else if ((format == RK_FORMAT_YCrCb_420_SP )|| (format == RK_FORMAT_YCbCr_420_SP)) {
        if (~format_usage & IM_RGA_SUPPORT_FORMAT_YUV_420_SEMI_PLANNER_8_BIT) {
            imSetErrorMsg("%s unsupported YUV420 semi-planner 8bit format, format = 0x%x(%s)\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
            return IM_STATUS_NOT_SUPPORTED;
        }

        ret = rga_yuv_legality_check(name, info, rect);
        if (ret != IM_STATUS_SUCCESS) {
            return ret;
        }
    } else if ((format == RK_FORMAT_YCrCb_420_P) || (format == RK_FORMAT_YCbCr_420_P)) {
        if (~format_usage & IM_RGA_SUPPORT_FORMAT_YUV_420_PLANNER_8_BIT) {
            imSetErrorMsg("%s unsupported YUV420 planner 8bit format, format = 0x%x(%s)\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
            return IM_STATUS_NOT_SUPPORTED;
        }

        ret = rga_yuv_legality_check(name, info, rect);
        if (ret != IM_STATUS_SUCCESS) {
            return ret;
        }
    } else if ((format == RK_FORMAT_YCrCb_422_SP) || (format == RK_FORMAT_YCbCr_422_SP)) {
        if (~format_usage & IM_RGA_SUPPORT_FORMAT_YUV_422_SEMI_PLANNER_8_BIT) {
            imSetErrorMsg("%s unsupported YUV422 semi-planner 8bit format, format = 0x%x(%s)\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
            return IM_STATUS_NOT_SUPPORTED;
        }

        ret = rga_yuv_legality_check(name, info, rect);
        if (ret != IM_STATUS_SUCCESS) {
            return ret;
        }
    } else if ((format == RK_FORMAT_YCrCb_422_P) || (format == RK_FORMAT_YCbCr_422_P)) {
        if (~format_usage & IM_RGA_SUPPORT_FORMAT_YUV_422_PLANNER_8_BIT) {
            imSetErrorMsg("%s unsupported YUV422 planner 8bit format, format = 0x%x(%s)\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
            return IM_STATUS_NOT_SUPPORTED;
        }

        ret = rga_yuv_legality_check(name, info, rect);
        if (ret != IM_STATUS_SUCCESS) {
            return ret;
        }
    } else if ((format == RK_FORMAT_YCrCb_420_SP_10B) || (format == RK_FORMAT_YCbCr_420_SP_10B)) {
        if (~format_usage & IM_RGA_SUPPORT_FORMAT_YUV_420_SEMI_PLANNER_10_BIT) {
            imSetErrorMsg("%s unsupported YUV420 semi-planner 10bit format, format = 0x%x(%s)\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
            return IM_STATUS_NOT_SUPPORTED;
        }

        ret = rga_yuv_legality_check(name, info, rect);
        if (ret != IM_STATUS_SUCCESS) {
            return ret;
        }

        rga_warn("If it is an RK encoder output, it needs to be aligned with an odd multiple of 256");
    } else if ((format == RK_FORMAT_YCrCb_422_10b_SP) || (format == RK_FORMAT_YCbCr_422_10b_SP)) {
        if (~format_usage & IM_RGA_SUPPORT_FORMAT_YUV_422_SEMI_PLANNER_10_BIT) {
            imSetErrorMsg("%s unsupported YUV422 semi-planner 10bit format, format = 0x%x(%s)\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
            return IM_STATUS_NOT_SUPPORTED;
        }

        ret = rga_yuv_legality_check(name, info, rect);
        if (ret != IM_STATUS_SUCCESS) {
            return ret;
        }

        rga_warn("If it is an RK encoder output, it needs to be aligned with an odd multiple of 256");
    } else if (format == RK_FORMAT_YUYV_420
        || format == RK_FORMAT_YVYU_420
        || format == RK_FORMAT_UYVY_420
        || format == RK_FORMAT_VYUY_420)
    {
        if (~format_usage & IM_RGA_SUPPORT_FORMAT_YUYV_420) {
            imSetErrorMsg("%s unsupported YUYV format, format = 0x%x(%s)\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
            return IM_STATUS_NOT_SUPPORTED;
        }

        ret = rga_yuv_legality_check(name, info, rect);
        if (ret != IM_STATUS_SUCCESS) {
            return ret;
        }
    } else if (format == RK_FORMAT_YUYV_422
        || format == RK_FORMAT_YVYU_422
        || format == RK_FORMAT_UYVY_422
        || format == RK_FORMAT_VYUY_422)
    {
        if (~format_usage & IM_RGA_SUPPORT_FORMAT_YUYV_422) {
            imSetErrorMsg("%s unsupported YUYV format, format = 0x%x(%s)\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
            return IM_STATUS_NOT_SUPPORTED;
        }

        ret = rga_yuv_legality_check(name, info, rect);
        if (ret != IM_STATUS_SUCCESS) {
            return ret;
        }
    } else if (format == RK_FORMAT_YCbCr_400) {
        if (~format_usage & IM_RGA_SUPPORT_FORMAT_YUV_400) {
            imSetErrorMsg("%s unsupported YUV400 format, format = 0x%x(%s)\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
            return IM_STATUS_NOT_SUPPORTED;
        }

        ret = rga_yuv_legality_check(name, info, rect);
        if (ret != IM_STATUS_SUCCESS) {
            return ret;
        }
    } else if (format == RK_FORMAT_Y4) {
        if (~format_usage & IM_RGA_SUPPORT_FORMAT_Y4) {
            imSetErrorMsg("%s unsupported Y4/Y1 format, format = 0x%x(%s)\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
            return IM_STATUS_NOT_SUPPORTED;
        }

        ret = rga_yuv_legality_check(name, info, rect);
        if (ret != IM_STATUS_SUCCESS) {
            return ret;
        }
    } else if (format == RK_FORMAT_RGBA2BPP) {
        if (~format_usage & IM_RGA_SUPPORT_FORMAT_RGBA2BPP) {
            imSetErrorMsg("%s unsupported rgba2bpp format, format = 0x%x(%s)\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
            return IM_STATUS_NOT_SUPPORTED;
        }
    } else {
        imSetErrorMsg("%s unsupported this format, format = 0x%x(%s)\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
        return IM_STATUS_NOT_SUPPORTED;
    }

    return IM_STATUS_NOERROR;
}

IM_STATUS rga_check_align(const char *name, rga_buffer_t info, int byte_stride)
{
    int bpp = 0;
    int bit_stride, pixel_stride, align, gcd;

    pixel_stride = get_perPixel_stride_from_format(info.format);

    bit_stride = pixel_stride * info.wstride;
    if (bit_stride % (byte_stride * 8) == 0) {
        return IM_STATUS_NOERROR;
    } else {
        gcd = GET_GCD(pixel_stride, byte_stride * 8);
        align = GET_LCM(pixel_stride, byte_stride * 8, gcd) / pixel_stride;

        imSetErrorMsg("%s unsupport width stride %d, %s width stride should be %d aligned!", name, info.wstride, translate_format_str(info.format), align);
        return IM_STATUS_NOT_SUPPORTED;
    }

    return IM_STATUS_NOERROR;
}

IM_STATUS rga_check_blend(rga_buffer_t src, rga_buffer_t pat, rga_buffer_t dst, int pat_enable, int mode_usage)
{
    int src_fmt, pat_fmt, dst_fmt;
    bool src_isRGB, pat_isRGB, dst_isRGB;

    src_fmt = RkRgaGetRgaFormat(RkRgaCompatibleFormat(src.format));
    pat_fmt = RkRgaGetRgaFormat(RkRgaCompatibleFormat(pat.format));
    dst_fmt = RkRgaGetRgaFormat(RkRgaCompatibleFormat(dst.format));

    src_isRGB = NormalRgaIsRgbFormat(src_fmt);
    pat_isRGB = NormalRgaIsRgbFormat(pat_fmt);
    dst_isRGB = NormalRgaIsRgbFormat(dst_fmt);

    switch (mode_usage & IM_ALPHA_BLEND_MASK) {
        case IM_ALPHA_BLEND_SRC:
        case IM_ALPHA_BLEND_DST:
            break;

        case IM_ALPHA_BLEND_SRC_OVER:
            if (!NormalRgaFormatHasAlpha(src_fmt)) {
                imSetErrorMsg("Blend mode 'src_over' unsupported src format without alpha, format[src,src1,dst] = [0x%x(%s), 0x%x(%s), 0x%x(%s)]", src_fmt, translate_format_str(src_fmt), pat_fmt, translate_format_str(pat_fmt), dst_fmt, translate_format_str(dst_fmt));
                return IM_STATUS_NOT_SUPPORTED;
            }
            break;

        case IM_ALPHA_BLEND_DST_OVER:
            if (pat_enable) {
                if (!NormalRgaFormatHasAlpha(pat_fmt)) {
                    imSetErrorMsg("Blend mode 'dst_over' unsupported pat format without alpha, format[src,src1,dst] = [0x%x(%s), 0x%x(%s), 0x%x(%s)]", src_fmt, translate_format_str(src_fmt), pat_fmt, translate_format_str(pat_fmt), dst_fmt, translate_format_str(dst_fmt));
                    return IM_STATUS_NOT_SUPPORTED;
                }
            } else {
                if (!NormalRgaFormatHasAlpha(dst_fmt)) {
                    imSetErrorMsg("Blend mode 'dst_over' unsupported dst format without alpha, format[src,src1,dst] = [0x%x(%s), 0x%x(%s), 0x%x(%s)]", src_fmt, translate_format_str(src_fmt), pat_fmt, translate_format_str(pat_fmt), dst_fmt, translate_format_str(dst_fmt));
                    return IM_STATUS_NOT_SUPPORTED;
                }
            }
            break;

        default:
            if (!(NormalRgaFormatHasAlpha(src_fmt) || NormalRgaFormatHasAlpha(dst_fmt))) {
                imSetErrorMsg("Blend mode unsupported format without alpha, format[src,src1,dst] = [0x%x(%s), 0x%x(%s), 0x%x(%s)]", src_fmt, translate_format_str(src_fmt), pat_fmt, translate_format_str(pat_fmt), dst_fmt, translate_format_str(dst_fmt));
                return IM_STATUS_NOT_SUPPORTED;
            }
            break;
    }

    if (pat_enable && (pat.width != dst.width || pat.height != dst.height)) {
        imSetErrorMsg("In the three-channel mode Alapha blend, the width and height of the src1 channel must be equal to the dst channel, src1[w,h] = [%d, %d], dst[w,h] = [%d, %d]", pat.width, pat.height, dst.width, dst.height);
        return IM_STATUS_NOT_SUPPORTED;
    }

    return IM_STATUS_NOERROR;
}

IM_STATUS rga_check_rotate(int mode_usage, rga_info_table_entry &table)
{
    if (table.version & (IM_RGA_HW_VERSION_RGA_1 | IM_RGA_HW_VERSION_RGA_1_PLUS)) {
        if (mode_usage & IM_HAL_TRANSFORM_FLIP_H_V) {
            imSetErrorMsg("RGA1/RGA1_PLUS cannot support H_V mirror.");
            return IM_STATUS_NOT_SUPPORTED;
        }

        if ((mode_usage & (IM_HAL_TRANSFORM_ROT_90 + IM_HAL_TRANSFORM_ROT_180 + IM_HAL_TRANSFORM_ROT_270))
            && (mode_usage & (IM_HAL_TRANSFORM_FLIP_H + IM_HAL_TRANSFORM_FLIP_V + IM_HAL_TRANSFORM_FLIP_H_V)))
        {
            imSetErrorMsg("RGA1/RGA1_PLUS cannot support rotate with mirror.");
            return IM_STATUS_NOT_SUPPORTED;
        }
    }

    return IM_STATUS_NOERROR;
}

IM_STATUS rga_check_feature(rga_buffer_t src, rga_buffer_t pat, rga_buffer_t dst, int pat_enable, int mode_usage, int feature_usage)
{
    if ((mode_usage & IM_COLOR_FILL) && (~feature_usage & IM_RGA_SUPPORT_FEATURE_COLOR_FILL)) {
        imSetErrorMsg("The platform does not support color fill featrue. \n%s", querystring(RGA_FEATURE));
        return IM_STATUS_NOT_SUPPORTED;
    }

    if ((mode_usage & IM_COLOR_PALETTE) && (~feature_usage & IM_RGA_SUPPORT_FEATURE_COLOR_PALETTE)) {
        imSetErrorMsg("The platform does not support color palette featrue. \n%s", querystring(RGA_FEATURE));
        return IM_STATUS_NOT_SUPPORTED;
    }

    if ((mode_usage & IM_ROP) && (~feature_usage & IM_RGA_SUPPORT_FEATURE_ROP)) {
        imSetErrorMsg("The platform does not support ROP featrue. \n%s", querystring(RGA_FEATURE));
        return IM_STATUS_NOT_SUPPORTED;
    }

    if ((mode_usage & IM_NN_QUANTIZE) && (~feature_usage & IM_RGA_SUPPORT_FEATURE_QUANTIZE)) {
        imSetErrorMsg("The platform does not support quantize featrue. \n%s", querystring(RGA_FEATURE));
        return IM_STATUS_NOT_SUPPORTED;
    }

    if ((pat_enable ? (pat.color_space_mode & IM_RGB_TO_YUV_MASK) : 0) && (~feature_usage & IM_RGA_SUPPORT_FEATURE_SRC1_R2Y_CSC)) {
        imSetErrorMsg("The platform does not support src1 channel RGB2YUV color space convert featrue. \n%s", querystring(RGA_FEATURE));
        return IM_STATUS_NOT_SUPPORTED;
    }

    if ((src.color_space_mode & IM_FULL_CSC_MASK
        || dst.color_space_mode & IM_FULL_CSC_MASK
        || (pat_enable ? (pat.color_space_mode & IM_FULL_CSC_MASK) : 0))
        && (~feature_usage & IM_RGA_SUPPORT_FEATURE_DST_FULL_CSC))
    {
        imSetErrorMsg("The platform does not support dst channel full color space convert(Y2Y/Y2R) featrue. \n%s", querystring(RGA_FEATURE));
        return IM_STATUS_NOT_SUPPORTED;
    }

    if ((mode_usage & IM_MOSAIC) && (~feature_usage & IM_RGA_SUPPORT_FEATURE_MOSAIC)) {
        imSetErrorMsg("The platform does not support mosaic featrue. \n%s", querystring(RGA_FEATURE));
        return IM_STATUS_NOT_SUPPORTED;
    }

    if ((mode_usage & IM_OSD) && (~feature_usage & IM_RGA_SUPPORT_FEATURE_OSD)) {
        imSetErrorMsg("The platform does not support osd featrue. \n%s", querystring(RGA_FEATURE));
        return IM_STATUS_NOT_SUPPORTED;
    }

    if ((mode_usage & IM_PRE_INTR) && (~feature_usage & IM_RGA_SUPPORT_FEATURE_PRE_INTR)) {
        imSetErrorMsg("The platform does not support pre_intr featrue. \n%s", querystring(RGA_FEATURE));
        return IM_STATUS_NOT_SUPPORTED;
    }

    return IM_STATUS_NOERROR;
}

IM_STATUS rga_import_buffers(struct rga_buffer_pool *buffer_pool)
{
    int ret = 0;

    ret = rga_get_context();
    if (ret != IM_STATUS_SUCCESS) {
        return (IM_STATUS)ret;
    }

    if (buffer_pool == NULL) {
        imSetErrorMsg("buffer pool is null!");
        return IM_STATUS_FAILED;
    }

    ret = ioctl(rgaCtx->rgaFd, RGA_IOC_IMPORT_BUFFER, buffer_pool);
    if (ret < 0) {
        imSetErrorMsg("RGA_IOC_IMPORT_BUFFER fail! %s", strerror(errno));
        return IM_STATUS_FAILED;
    }

    return IM_STATUS_SUCCESS;
}

rga_buffer_handle_t rga_import_buffer(uint64_t memory, int type, uint32_t size)
{
    struct rga_buffer_pool buffer_pool;
    struct rga_external_buffer buffers[1];

    memset(&buffer_pool, 0x0, sizeof(buffer_pool));
    memset(buffers, 0x0, sizeof(buffers));

    buffers[0].type = type;
    buffers[0].memory = memory;
    buffers[0].memory_info.size = size;

    buffer_pool.buffers = (uint64_t)buffers;
    buffer_pool.size = 1;

    if (rga_import_buffers(&buffer_pool) != IM_STATUS_SUCCESS) {
        return -1;
    }

    return buffers[0].handle;
}

rga_buffer_handle_t rga_import_buffer(uint64_t memory, int type, im_handle_param_t *param)
{
    struct rga_buffer_pool buffer_pool;
    struct rga_external_buffer buffers[1];

    memset(&buffer_pool, 0x0, sizeof(buffer_pool));
    memset(buffers, 0x0, sizeof(buffers));

    buffers[0].type = type;
    buffers[0].memory = memory;
    memcpy(&buffers[0].memory_info, param, sizeof(*param));
    buffers[0].memory_info.format = RkRgaGetRgaFormat(buffers[0].memory_info.format) >> 8;

    buffer_pool.buffers = (uint64_t)buffers;
    buffer_pool.size = 1;

    if (rga_import_buffers(&buffer_pool) != IM_STATUS_SUCCESS) {
        return -1;
    }

    return buffers[0].handle;
}

IM_STATUS rga_release_buffers(struct rga_buffer_pool *buffer_pool)
{
    int ret = 0;

    ret = rga_get_context();
    if (ret != IM_STATUS_SUCCESS) {
        return (IM_STATUS)ret;
    }

    if (buffer_pool == NULL) {
        imSetErrorMsg("buffer pool is null!");
        return IM_STATUS_FAILED;
    }

    ret = ioctl(rgaCtx->rgaFd, RGA_IOC_RELEASE_BUFFER, buffer_pool);
    if (ret < 0) {
        imSetErrorMsg("RGA_IOC_RELEASE_BUFFER fail! %s", strerror(errno));
        return IM_STATUS_FAILED;
    }

    return IM_STATUS_SUCCESS;
}

IM_STATUS rga_release_buffer(int handle)
{
    struct rga_buffer_pool buffer_pool;
    struct rga_external_buffer buffers[1];

    memset(&buffer_pool, 0x0, sizeof(buffer_pool));
    memset(buffers, 0x0, sizeof(buffers));

    buffers[0].handle = handle;

    buffer_pool.buffers = (uint64_t)buffers;
    buffer_pool.size = 1;

    return rga_release_buffers(&buffer_pool);
}

IM_STATUS rga_get_opt(im_opt_t *opt, void *ptr)
{
    if ((opt == NULL) || (ptr == NULL)) {
        return IM_STATUS_FAILED;
    }

    if (rga_version_compare(RGA_GET_API_VERSION(*(im_api_version_t *)ptr), (struct rga_version_t){ 2, 0, 0, {0}}) > 0) {
        return IM_STATUS_FAILED;
    }

    if (rga_version_compare(RGA_GET_API_VERSION(*(im_api_version_t *)ptr), (struct rga_version_t){ 1, 7, 2, {0}}) <= 0) {
        opt->color = ((im_opt_t *)ptr)->color;
        memcpy(&opt->colorkey_range, &((im_opt_t *)ptr)->colorkey_range, sizeof(im_colorkey_range));
        memcpy(&opt->nn, &((im_opt_t *)ptr)->nn, sizeof(im_nn_t));
        opt->rop_code = ((im_opt_t *)ptr)->rop_code;
        opt->priority = ((im_opt_t *)ptr)->priority;
        opt->core = ((im_opt_t *)ptr)->core;
    } else {
        memcpy(opt, ptr, sizeof(im_opt_t));
    }

    return IM_STATUS_SUCCESS;
}

static IM_STATUS rga_task_submit(im_job_id_t job_id, rga_buffer_t src, rga_buffer_t dst, rga_buffer_t pat, im_rect srect, im_rect drect, im_rect prect, int acquire_fence_fd, int *release_fence_fd, im_opt_t *opt_ptr, int usage)
{
    int ret;
    im_opt_t opt;
    rga_info_t srcinfo;
    rga_info_t dstinfo;
    rga_info_t patinfo;

    if (rga_get_opt(&opt, opt_ptr) == IM_STATUS_FAILED) {
        memset(&opt, 0x0, sizeof(opt));
    }

    src.format = RkRgaCompatibleFormat(src.format);
    dst.format = RkRgaCompatibleFormat(dst.format);
    pat.format = RkRgaCompatibleFormat(pat.format);

    memset(&srcinfo, 0, sizeof(rga_info_t));
    memset(&dstinfo, 0, sizeof(rga_info_t));
    memset(&patinfo, 0, sizeof(rga_info_t));

    if (usage & IM_COLOR_FILL) {
        ret = rga_set_buffer_info(dst, &dstinfo);
    } else {
        ret = rga_set_buffer_info(src, dst, &srcinfo, &dstinfo);
    }

    if (ret <= 0) {
        return (IM_STATUS)ret;
    }

    rga_apply_rect(&src, &srect);
    rga_apply_rect(&dst, &drect);

    rga_set_rect(&srcinfo.rect, srect.x, srect.y, src.width, src.height, src.wstride, src.hstride, src.format);
    rga_set_rect(&dstinfo.rect, drect.x, drect.y, dst.width, dst.height, dst.wstride, dst.hstride, dst.format);

    if (((usage & IM_COLOR_PALETTE) || (usage & IM_ALPHA_BLEND_MASK)) && rga_is_buffer_valid(pat)) {
        ret = rga_set_buffer_info(pat, &patinfo);
        if (ret <= 0) {
            return (IM_STATUS)ret;
        }

        rga_apply_rect(&pat, &prect);
        rga_set_rect(&patinfo.rect, prect.x, prect.y, pat.width, pat.height, pat.wstride, pat.hstride, pat.format);
    }

    if ((usage & IM_ALPHA_BLEND_MASK) && rga_is_buffer_valid(pat))  {
        ret = imcheck_composite(src, dst, pat, srect, drect, prect, usage);
    } else {
        ret = imcheck(src, dst, srect, drect, usage);
    }

    if (ret != IM_STATUS_NOERROR) {
        return (IM_STATUS)ret;
    }

    if (usage & IM_HAL_TRANSFORM_MASK) {
        switch (usage & (IM_HAL_TRANSFORM_ROT_90 + IM_HAL_TRANSFORM_ROT_180 + IM_HAL_TRANSFORM_ROT_270)) {
            case IM_HAL_TRANSFORM_ROT_90:
                srcinfo.rotation = HAL_TRANSFORM_ROT_90;
                break;

            case IM_HAL_TRANSFORM_ROT_180:
                srcinfo.rotation = HAL_TRANSFORM_ROT_180;
                break;

            case IM_HAL_TRANSFORM_ROT_270:
                srcinfo.rotation = HAL_TRANSFORM_ROT_270;
                break;
        }

        switch (usage & (IM_HAL_TRANSFORM_FLIP_V + IM_HAL_TRANSFORM_FLIP_H + IM_HAL_TRANSFORM_FLIP_H_V)) {
            case IM_HAL_TRANSFORM_FLIP_V:
                srcinfo.rotation |= srcinfo.rotation ? HAL_TRANSFORM_FLIP_V << 4 : HAL_TRANSFORM_FLIP_V;
                break;

            case IM_HAL_TRANSFORM_FLIP_H:
                srcinfo.rotation |= srcinfo.rotation ? HAL_TRANSFORM_FLIP_H << 4 : HAL_TRANSFORM_FLIP_H;
                break;

            case IM_HAL_TRANSFORM_FLIP_H_V:
                srcinfo.rotation |= srcinfo.rotation ? HAL_TRANSFORM_FLIP_H_V << 4 : HAL_TRANSFORM_FLIP_H_V;
                break;
        }

        if (srcinfo.rotation == 0) {
            rga_error("Could not find rotate/flip usage:[0x%X]", usage);
        }
    }

    if (usage & IM_ALPHA_BLEND_MASK) {
        switch (usage & IM_ALPHA_BLEND_MASK) {
            case IM_ALPHA_BLEND_SRC:
                srcinfo.blend = 0x1;
                break;

            case IM_ALPHA_BLEND_DST:
                srcinfo.blend = 0x2;
                break;

            case IM_ALPHA_BLEND_SRC_OVER:
                srcinfo.blend = (usage & IM_ALPHA_BLEND_PRE_MUL) ? 0x405 : 0x105;
                break;

            case IM_ALPHA_BLEND_SRC_IN:
                break;

            case IM_ALPHA_BLEND_DST_IN:
                break;

            case IM_ALPHA_BLEND_SRC_OUT:
                break;

            case IM_ALPHA_BLEND_DST_OVER:
                srcinfo.blend = (usage & IM_ALPHA_BLEND_PRE_MUL) ? 0x504 : 0x501;
                break;

            case IM_ALPHA_BLEND_SRC_ATOP:
                break;

            case IM_ALPHA_BLEND_DST_OUT:
                break;

            case IM_ALPHA_BLEND_XOR:
                break;
        }

        if (srcinfo.blend == 0) {
            rga_error("Could not find blend usage:[0x%X]", usage);
        }

        if (src.global_alpha > 0) {
            srcinfo.blend ^= src.global_alpha << 16;
        } else {
            srcinfo.blend ^= 0xFF << 16;
        }
    }

    if (usage & IM_ALPHA_COLORKEY_MASK) {
        srcinfo.blend = 0xff0105;

        srcinfo.colorkey_en = 1;
        srcinfo.colorkey_min = opt.colorkey_range.min;
        srcinfo.colorkey_max = opt.colorkey_range.max;

        switch (usage & IM_ALPHA_COLORKEY_MASK) {
            case IM_ALPHA_COLORKEY_NORMAL:
                srcinfo.colorkey_mode = 0;
                break;

            case IM_ALPHA_COLORKEY_INVERTED:
                srcinfo.colorkey_mode = 1;
                break;
        }
    }

    if (usage & IM_OSD) {
        srcinfo.osd_info.enable = true;

        srcinfo.osd_info.mode_ctrl.mode = opt.osd_config.osd_mode;

        srcinfo.osd_info.mode_ctrl.width_mode = opt.osd_config.block_parm.width_mode;
        if (opt.osd_config.block_parm.width_mode == IM_OSD_BLOCK_MODE_NORMAL) {
            srcinfo.osd_info.mode_ctrl.block_fix_width = opt.osd_config.block_parm.width;
        } else if (opt.osd_config.block_parm.width_mode == IM_OSD_BLOCK_MODE_DIFFERENT) {
            srcinfo.osd_info.mode_ctrl.unfix_index = opt.osd_config.block_parm.width_index;
        }
        srcinfo.osd_info.mode_ctrl.block_num = opt.osd_config.block_parm.block_count;
        srcinfo.osd_info.mode_ctrl.default_color_sel = opt.osd_config.block_parm.background_config;
        srcinfo.osd_info.mode_ctrl.direction_mode = opt.osd_config.block_parm.direction;
        srcinfo.osd_info.mode_ctrl.color_mode = opt.osd_config.block_parm.color_mode;

        if (pat.format == RK_FORMAT_RGBA2BPP) {
            srcinfo.osd_info.bpp2_info.ac_swap = opt.osd_config.bpp2_info.ac_swap;
            srcinfo.osd_info.bpp2_info.endian_swap = opt.osd_config.bpp2_info.endian_swap;
            srcinfo.osd_info.bpp2_info.color0.value = opt.osd_config.bpp2_info.color0.value;
            srcinfo.osd_info.bpp2_info.color1.value = opt.osd_config.bpp2_info.color1.value;
        } else {
            srcinfo.osd_info.bpp2_info.color0.value = opt.osd_config.block_parm.normal_color.value;
            srcinfo.osd_info.bpp2_info.color1.value = opt.osd_config.block_parm.invert_color.value;
        }

        switch (opt.osd_config.invert_config.invert_channel) {
            case IM_OSD_INVERT_CHANNEL_NONE:
                srcinfo.osd_info.mode_ctrl.invert_enable = (0x1 << 1) | (0x1 << 2);
                break;

            case IM_OSD_INVERT_CHANNEL_Y_G:
                srcinfo.osd_info.mode_ctrl.invert_enable = 0x1 << 2;
                break;

            case IM_OSD_INVERT_CHANNEL_C_RB:
                srcinfo.osd_info.mode_ctrl.invert_enable = 0x1 << 1;
                break;

            case IM_OSD_INVERT_CHANNEL_ALPHA:
                srcinfo.osd_info.mode_ctrl.invert_enable = (0x1 << 0) | (0x1 << 1) | (0x1 << 2);
                break;

            case IM_OSD_INVERT_CHANNEL_COLOR:
                srcinfo.osd_info.mode_ctrl.invert_enable = 0;
                break;

            case IM_OSD_INVERT_CHANNEL_BOTH:
                srcinfo.osd_info.mode_ctrl.invert_enable = 0x1 << 0;
        }
        srcinfo.osd_info.mode_ctrl.invert_flags_mode = opt.osd_config.invert_config.flags_mode;
        srcinfo.osd_info.mode_ctrl.flags_index = opt.osd_config.invert_config.flags_index;

        srcinfo.osd_info.last_flags = opt.osd_config.invert_config.invert_flags;
        srcinfo.osd_info.cur_flags = opt.osd_config.invert_config.current_flags;

        srcinfo.osd_info.mode_ctrl.invert_mode = opt.osd_config.invert_config.invert_mode;
        if (opt.osd_config.invert_config.invert_mode == IM_OSD_INVERT_USE_FACTOR) {
            srcinfo.osd_info.cal_factor.alpha_max = opt.osd_config.invert_config.factor.alpha_max;
            srcinfo.osd_info.cal_factor.alpha_min = opt.osd_config.invert_config.factor.alpha_min;
            srcinfo.osd_info.cal_factor.crb_max = opt.osd_config.invert_config.factor.crb_max;
            srcinfo.osd_info.cal_factor.crb_min = opt.osd_config.invert_config.factor.crb_min;
            srcinfo.osd_info.cal_factor.yg_max = opt.osd_config.invert_config.factor.yg_max;
            srcinfo.osd_info.cal_factor.yg_min = opt.osd_config.invert_config.factor.yg_min;
        }
        srcinfo.osd_info.mode_ctrl.invert_thresh = opt.osd_config.invert_config.threash;
    }

    if (usage & IM_NN_QUANTIZE) {
        dstinfo.nn.nn_flag = 1;
        dstinfo.nn.scale_r  = opt.nn.scale_r;
        dstinfo.nn.scale_g  = opt.nn.scale_g;
        dstinfo.nn.scale_b  = opt.nn.scale_b;
        dstinfo.nn.offset_r = opt.nn.offset_r;
        dstinfo.nn.offset_g = opt.nn.offset_g;
        dstinfo.nn.offset_b = opt.nn.offset_b;
    }

    if (usage & IM_ROP) {
        srcinfo.rop_code = opt.rop_code;
    }

    if (usage & IM_MOSAIC) {
        srcinfo.mosaic_info.enable = true;
        srcinfo.mosaic_info.mode = opt.mosaic_mode;
    }

    if (usage & IM_PRE_INTR) {
        srcinfo.pre_intr.enable = true;

        srcinfo.pre_intr.read_intr_en = opt.intr_config.flags & IM_INTR_READ_INTR ? true : false;
        if (srcinfo.pre_intr.read_intr_en) {
            srcinfo.pre_intr.read_intr_en = true;
            srcinfo.pre_intr.read_hold_en = opt.intr_config.flags & IM_INTR_READ_HOLD ? true : false;
            srcinfo.pre_intr.read_threshold = opt.intr_config.read_threshold;
        }

        srcinfo.pre_intr.write_intr_en = opt.intr_config.flags & IM_INTR_WRITE_INTR ? true : false;
        if (srcinfo.pre_intr.write_intr_en > 0) {
            srcinfo.pre_intr.write_start = opt.intr_config.write_start;
            srcinfo.pre_intr.write_step = opt.intr_config.write_step;
        }
    }

    if ((dst.color_space_mode & IM_YUV_TO_RGB_MASK) && (dst.color_space_mode & IM_RGB_TO_YUV_MASK)) {
        if (rga_is_buffer_valid(pat)
            && NormalRgaIsYuvFormat(RkRgaGetRgaFormat(src.format))
            && NormalRgaIsRgbFormat(RkRgaGetRgaFormat(pat.format))
            && NormalRgaIsYuvFormat(RkRgaGetRgaFormat(dst.format)))
        {
            dstinfo.color_space_mode = dst.color_space_mode;
        } else {
            imSetErrorMsg("Not yuv + rgb -> yuv does not need for color_sapce_mode R2Y & Y2R, please fix, "
                          "src_fromat = 0x%x(%s), src1_format = 0x%x(%s), dst_format = 0x%x(%s)",
                          src.format, translate_format_str(src.format),
                          pat.format, translate_format_str(pat.format),
                          dst.format, translate_format_str(dst.format));
            return IM_STATUS_ILLEGAL_PARAM;
        }
    } else if (dst.color_space_mode & (IM_YUV_TO_RGB_MASK)) {
        if (rga_is_buffer_valid(pat)
            && NormalRgaIsYuvFormat(RkRgaGetRgaFormat(src.format))
            && NormalRgaIsRgbFormat(RkRgaGetRgaFormat(pat.format))
            && NormalRgaIsRgbFormat(RkRgaGetRgaFormat(dst.format)))
        {
            dstinfo.color_space_mode = dst.color_space_mode;
        } else if (NormalRgaIsYuvFormat(RkRgaGetRgaFormat(src.format)) && NormalRgaIsRgbFormat(RkRgaGetRgaFormat(dst.format))) {
            dstinfo.color_space_mode = dst.color_space_mode;
        } else {
            imSetErrorMsg("Not yuv to rgb does not need for color_sapce_mode, please fix, "
                          "src_fromat = 0x%x(%s), src1_format = 0x%x(%s), dst_format = 0x%x(%s)",
                          src.format, translate_format_str(src.format),
                          pat.format, rga_is_buffer_valid(pat) ? translate_format_str(pat.format) : "none",
                          dst.format, translate_format_str(dst.format));
            return IM_STATUS_ILLEGAL_PARAM;
        }
    } else if (dst.color_space_mode & (IM_RGB_TO_YUV_MASK)) {
        if (rga_is_buffer_valid(pat)
            && NormalRgaIsRgbFormat(RkRgaGetRgaFormat(src.format))
            && NormalRgaIsRgbFormat(RkRgaGetRgaFormat(pat.format))
            && NormalRgaIsYuvFormat(RkRgaGetRgaFormat(dst.format)))
        {
            dstinfo.color_space_mode = dst.color_space_mode;
        } else if (NormalRgaIsRgbFormat(RkRgaGetRgaFormat(src.format)) && NormalRgaIsYuvFormat(RkRgaGetRgaFormat(dst.format))) {
            dstinfo.color_space_mode = dst.color_space_mode;
        } else {
            imSetErrorMsg("Not rgb to yuv does not need for color_sapce_mode, please fix, "
                          "src_fromat = 0x%x(%s), src1_format = 0x%x(%s), dst_format = 0x%x(%s)",
                          src.format, translate_format_str(src.format),
                          pat.format, rga_is_buffer_valid(pat) ? translate_format_str(pat.format) : "none",
                          dst.format, translate_format_str(dst.format));
            return IM_STATUS_ILLEGAL_PARAM;
        }
    } else if ((src.color_space_mode & IM_FULL_CSC_MASK) || (dst.color_space_mode & IM_FULL_CSC_MASK)) {
        if (src.color_space_mode == IM_COLOR_SPACE_DEFAULT) {
            if  (NormalRgaIsRgbFormat(RkRgaGetRgaFormat(src.format))) {
                src.color_space_mode = IM_RGB_FULL;
            } else if (NormalRgaIsYuvFormat(RkRgaGetRgaFormat(src.format))) {
                src.color_space_mode = IM_YUV_BT601_LIMIT_RANGE;
            }
        }

        if (dst.color_space_mode == IM_COLOR_SPACE_DEFAULT) {
            if (NormalRgaIsRgbFormat(RkRgaGetRgaFormat(dst.format))) {
                src.color_space_mode = IM_RGB_FULL;
            } else if (NormalRgaIsYuvFormat(RkRgaGetRgaFormat(dst.format))) {
                src.color_space_mode = IM_YUV_BT601_LIMIT_RANGE;
            }
        }

        if ((src.color_space_mode == IM_RGB_FULL) && (dst.color_space_mode == IM_YUV_BT709_FULL_RANGE)) {
            dstinfo.color_space_mode = rgb2yuv_709_full;
        } else if ((src.color_space_mode == IM_YUV_BT601_FULL_RANGE) && (dst.color_space_mode == IM_YUV_BT709_LIMIT_RANGE)) {
            dstinfo.color_space_mode = yuv2yuv_601_full_2_709_limit;
        } else if ((src.color_space_mode == IM_YUV_BT709_LIMIT_RANGE) && (dst.color_space_mode == IM_YUV_BT601_LIMIT_RANGE)) {
            dstinfo.color_space_mode = yuv2yuv_709_limit_2_601_limit;
        } else if ((src.color_space_mode == IM_YUV_BT709_FULL_RANGE) && (dst.color_space_mode == IM_YUV_BT601_LIMIT_RANGE)) {
            dstinfo.color_space_mode = yuv2yuv_709_full_2_601_limit;
        } else if ((src.color_space_mode == IM_YUV_BT709_FULL_RANGE) && (dst.color_space_mode == IM_YUV_BT601_FULL_RANGE)) {
            dstinfo.color_space_mode = yuv2yuv_709_full_2_601_full;
        } else {
            imSetErrorMsg("Unsupported full csc mode! src_csm:[0x%X], dst_csm:[0x%X]", src.color_space_mode, dst.color_space_mode);
            return IM_STATUS_NOT_SUPPORTED;
        }
    }

    if (dst.format == RK_FORMAT_Y4) {
        switch (dst.color_space_mode) {
            case IM_RGB_TO_Y4:
                dstinfo.dither.enable = 0;
                dstinfo.dither.mode = 0;
                break;

            case IM_RGB_TO_Y4_DITHER:
                dstinfo.dither.enable = 1;
                dstinfo.dither.mode = 0;
                break;

            case IM_RGB_TO_Y1_DITHER:
                dstinfo.dither.enable = 1;
                dstinfo.dither.mode = 1;
                break;

            default:
                dstinfo.dither.enable = 1;
                dstinfo.dither.mode = 0;
                break;
        }

        dstinfo.dither.lut0_l = 0x3210;
        dstinfo.dither.lut0_h = 0x7654;
        dstinfo.dither.lut1_l = 0xba98;
        dstinfo.dither.lut1_h = 0xfedc;
    }

    srcinfo.rd_mode = src.rd_mode;
    dstinfo.rd_mode = dst.rd_mode;
    if (rga_is_buffer_valid(pat)) {
        patinfo.rd_mode = pat.rd_mode;
    }

    RockchipRga &rkRga(RockchipRga::get());

    if (usage & IM_ASYNC) {
        if (release_fence_fd == NULL) {
            imSetErrorMsg("Async mode release_fence_fd cannot be NULL!");
            return IM_STATUS_ILLEGAL_PARAM;
        }

        dstinfo.sync_mode = RGA_BLIT_ASYNC;
    } else if (usage & IM_SYNC) {
        dstinfo.sync_mode = RGA_BLIT_SYNC;
    }

    dstinfo.in_fence_fd = acquire_fence_fd;
    dstinfo.core = opt.core ? opt.core : g_im2d_context.core;
    dstinfo.priority = opt.priority ? opt.priority : g_im2d_context.priority;

    dstinfo.job_id = job_id;

    if (usage & IM_COLOR_FILL) {
        dstinfo.color = opt.color;
        ret = rkRga.RkRgaCollorFill(&dstinfo);
    } else if (usage & IM_COLOR_PALETTE) {
        ret = rkRga.RkRgaCollorPalette(&srcinfo, &dstinfo, &patinfo);
    } else if ((usage & IM_ALPHA_BLEND_MASK) && rga_is_buffer_valid(pat)) {
        ret = rkRga.RkRgaBlit(&srcinfo, &dstinfo, &patinfo);
    } else {
        ret = rkRga.RkRgaBlit(&srcinfo, &dstinfo, NULL);
    }

    if (ret) {
        imSetErrorMsg("Failed to call RockChipRga interface, query log to find the cause of failure.");
        rga_error("srect[x,y,w,h] = [%d, %d, %d, %d], src[w,h,ws,hs] = [%d, %d, %d, %d]", srect.x, srect.y, srect.width, srect.height, src.width, src.height, src.wstride, src.hstride);

        if (rga_is_buffer_valid(pat)) {
           rga_error("s1/prect[x,y,w,h] = [%d, %d, %d, %d], src1/pat[w,h,ws,hs] = [%d, %d, %d, %d]", prect.x, prect.y, prect.width, prect.height, pat.width, pat.height, pat.wstride, pat.hstride);
        }
        rga_info("drect[x,y,w,h] = [%d, %d, %d, %d], dst[w,h,ws,hs] = [%d, %d, %d, %d], usage[0x%x]", drect.x, drect.y, drect.width, drect.height, dst.width, dst.height, dst.wstride, dst.hstride, usage);

        return IM_STATUS_FAILED;
    }

    if (usage & IM_ASYNC) {
        *release_fence_fd = dstinfo.out_fence_fd;
    }

    return IM_STATUS_SUCCESS;
}

IM_STATUS rga_single_task_submit(rga_buffer_t src, rga_buffer_t dst, rga_buffer_t pat, im_rect srect, im_rect drect, im_rect prect, int acquire_fence_fd, int *release_fence_fd, im_opt_t *opt_ptr, int usage)
{
    return rga_task_submit(0, src, dst, pat, srect, drect, prect, acquire_fence_fd, release_fence_fd, opt_ptr, usage);
}

IM_STATUS rga_task_submit(im_job_id_t job_id, rga_buffer_t src, rga_buffer_t dst, rga_buffer_t pat, im_rect srect, im_rect drect, im_rect prect, im_opt_t *opt_ptr, int usage)
{
    return rga_task_submit(job_id, src, dst, pat, srect, drect, prect, 0, NULL, opt_ptr, usage);
}

im_job_id_t rga_job_creat(uint32_t flags)
{
    int ret;
    im_job_id_t job_id;
    im_rga_job_t *job = NULL;

    if (rga_get_context() != IM_STATUS_SUCCESS) {
        return IM_STATUS_FAILED;
    }

    if (ioctl(rgaCtx->rgaFd, RGA_IOC_REQUEST_CREATE, &flags) < 0) {
        rga_error("start config failed:[%s]", strerror(errno));
        return IM_STATUS_FAILED;
    }

    job_id = flags;

    g_im2d_job_manager.mutex.lock();
    if (g_im2d_job_manager.job_map.count(job_id) != 0) {
        rga_error("job_map error");
        ret = IM_STATUS_FAILED;
        goto error_cancel_job;
    }

    job = (im_rga_job_t *)malloc(sizeof(*job));
    if (job == NULL) {
        rga_error("rga job alloc error");
        ret = IM_STATUS_FAILED;
        goto error_cancel_job;
    }

    memset(job, 0x0, sizeof(*job));

    job->id = job_id;
    g_im2d_job_manager.job_map[job_id] = job;
    g_im2d_job_manager.job_count++;
    g_im2d_job_manager.mutex.unlock();

    return job_id;

error_cancel_job:
    g_im2d_job_manager.mutex.unlock();
    rga_job_cancel(job_id);
    return ret;
}

IM_STATUS rga_job_cancel(im_job_id_t job_id)
{
    im_rga_job_t *job = NULL;

    if (rga_get_context() != IM_STATUS_SUCCESS) {
        return IM_STATUS_FAILED;
    }

    g_im2d_job_manager.mutex.lock();
    if (g_im2d_job_manager.job_map.count(job_id) > 0) {
        job = g_im2d_job_manager.job_map[job_id];
        if (job != NULL) {
            free(job);
        }

        g_im2d_job_manager.job_map.erase(job_id);
    }

    g_im2d_job_manager.job_count--;
    g_im2d_job_manager.mutex.unlock();

    if (ioctl(rgaCtx->rgaFd, RGA_IOC_REQUEST_CANCEL, &job_id) < 0) {
        rga_error("start config failed:[%s]", strerror(errno));
        return IM_STATUS_FAILED;
    }

    return IM_STATUS_SUCCESS;
}

IM_STATUS rga_job_submit(im_job_id_t job_id, int sync_mode, int acquire_fence_fd, int *release_fence_fd)
{
    int ret;
    im_rga_job_t *job = NULL;
    struct rga_user_request submit_request;

    if (rga_get_context() != IM_STATUS_SUCCESS) {
        return IM_STATUS_FAILED;
    }

    g_im2d_job_manager.mutex.lock();
    if (g_im2d_job_manager.job_map.count(job_id) == 0) {
        rga_error("job_id:[%d] is illegal", job_id);
        g_im2d_job_manager.mutex.unlock();

        return IM_STATUS_ILLEGAL_PARAM;
    }

    job = g_im2d_job_manager.job_map[job_id];
    if (job == NULL) {
        rga_error("job is NULL");
        g_im2d_job_manager.mutex.unlock();

        return IM_STATUS_FAILED;
    }

    memset(&submit_request, 0x0, sizeof(submit_request));

    submit_request.task_ptr = (uint64_t)&job->req;
    submit_request.task_num = job->task_count;
    submit_request.id = job->id;
    g_im2d_job_manager.mutex.unlock();

    switch (sync_mode) {
        case IM_SYNC:
            submit_request.sync_mode = RGA_BLIT_SYNC;
            break;

        case IM_ASYNC:
            submit_request.sync_mode = RGA_BLIT_ASYNC;
            break;

        default:
            rga_error("illegal sync mode");
            return IM_STATUS_ILLEGAL_PARAM;
    }

    submit_request.acquire_fence_fd = acquire_fence_fd;

    ret = ioctl(rgaCtx->rgaFd, RGA_IOC_REQUEST_SUBMIT, &submit_request);
    if (ret < 0) {
        rga_error("start config failed:[%s]", strerror(errno));
        return IM_STATUS_FAILED;
    }

    if ((sync_mode == IM_ASYNC) && release_fence_fd) {
        *release_fence_fd = submit_request.release_fence_fd;
    }

    return IM_STATUS_SUCCESS;
}

IM_STATUS rga_job_config(im_job_id_t job_id, int sync_mode, int acquire_fence_fd, int *release_fence_fd)
{
    int ret;
    im_rga_job_t *job = NULL;
    struct rga_user_request config_request;

    if (rga_get_context() != IM_STATUS_SUCCESS) {
        return IM_STATUS_FAILED;
    }

    g_im2d_job_manager.mutex.lock();
    if (g_im2d_job_manager.job_map.count(job_id) == 0) {
        rga_error("job_id:[%d] is illegal", job_id);
        g_im2d_job_manager.mutex.unlock();

        return IM_STATUS_ILLEGAL_PARAM;
    }

    job = g_im2d_job_manager.job_map[job_id];
    if (job == NULL) {
        rga_error("job is NULL");
        g_im2d_job_manager.mutex.unlock();

        return IM_STATUS_FAILED;
    }

    memset(&config_request, 0x0, sizeof(config_request));

    config_request.task_ptr = (uint64_t)&job->req;
    config_request.task_num = job->task_count;
    config_request.id = job->id;
    g_im2d_job_manager.mutex.unlock();

    switch (sync_mode) {
        case IM_SYNC:
            config_request.sync_mode = RGA_BLIT_SYNC;
            break;

        case IM_ASYNC:
            config_request.sync_mode = RGA_BLIT_ASYNC;
            break;

        default:
            rga_error("illegal sync mode");
            return IM_STATUS_ILLEGAL_PARAM;
    }

    config_request.acquire_fence_fd = acquire_fence_fd;

    ret = ioctl(rgaCtx->rgaFd, RGA_IOC_REQUEST_CONFIG, &config_request);
    if (ret < 0) {
        rga_error("start config failed:[%s]", strerror(errno));
        return IM_STATUS_FAILED;
    }

    if ((sync_mode == IM_ASYNC) && release_fence_fd) {
        *release_fence_fd = config_request.release_fence_fd;
    }

    return IM_STATUS_SUCCESS;
}
