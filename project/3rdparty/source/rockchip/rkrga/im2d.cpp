#include <math.h>
#include <sstream>
#include <sys/ioctl.h>

#include <rockchip/rkrga/im2d.h>
#include <rockchip/rkrga/rgadbg.h>
#include <rockchip/rkrga/version.h>
#include <rockchip/rkrga/RgaUtils.h>
#include <rockchip/rkrga/NormalRga.h>
#include <rockchip/rkrga/RockchipRga.h>

#define ERR_MSG_LEN                 300

#define ALIGN(val, align)           (((val) + ((align) - 1)) & ~((align) - 1))
#define DOWN_ALIGN(val, align)      ((val) & ~((align) - 1))
#define RGA_UNUSED(...)             (void)(__VA_ARGS__)

extern struct rgaContext *rgaCtx;
__thread char rga_err_str[ERR_MSG_LEN] = "The current error message is empty!";

#define imSetErrorMsg(...)                                      \
    ({                                                          \
        int ret = 0;                                            \
        ret = snprintf(rga_err_str, ERR_MSG_LEN, __VA_ARGS__);  \
        ret;                                                    \
    })

const char *imStrError_t(IM_STATUS status)
{
    const char *error_type[] = {
        "No errors during operation",
        "Run successfully",
        "Unsupported function",
        "Memory overflow",
        "Invalid parameters",
        "Illegal parameters",
        "Fatal error",
        "unkown status"
    };

    const char *ptr = NULL;
    static __thread char error_str[ERR_MSG_LEN] = "The current error message is empty!";

    switch (status) {
        case IM_STATUS_NOERROR:
            return error_type[0];

        case IM_STATUS_SUCCESS:
            return error_type[1];

        case IM_STATUS_NOT_SUPPORTED:
            ptr = error_type[2];
            break;

        case IM_STATUS_OUT_OF_MEMORY:
            ptr = error_type[3];
            break;

        case IM_STATUS_INVALID_PARAM:
            ptr = error_type[4];
            break;

        case IM_STATUS_ILLEGAL_PARAM:
            ptr = error_type[5];
            break;

        case IM_STATUS_FAILED:
            ptr = error_type[6];
            break;

        default :
            return error_type[7];
    }

    snprintf(error_str, ERR_MSG_LEN, "%s: %s", ptr, rga_err_str);
    imSetErrorMsg("No error message, it has been cleared.");

    return error_str;
}

rga_buffer_t wrapbuffer_virtualaddr_t(void *vir_addr, int width, int height, int wstride, int hstride, int format)
{
    rga_buffer_t buffer;

    memset(&buffer, 0x00, sizeof(rga_buffer_t));
    buffer.vir_addr = vir_addr;
    buffer.width = width;
    buffer.height = height;
    buffer.wstride = wstride;
    buffer.hstride = hstride;
    buffer.format = format;

    return buffer;
}

rga_buffer_t wrapbuffer_physicaladdr_t(void *phy_addr, int width, int height, int wstride, int hstride, int format)
{
    rga_buffer_t buffer;

    memset(&buffer, 0x00, sizeof(rga_buffer_t));
    buffer.phy_addr = phy_addr;
    buffer.width = width;
    buffer.height = height;
    buffer.wstride = wstride;
    buffer.hstride = hstride;
    buffer.format = format;

    return buffer;
}

rga_buffer_t wrapbuffer_fd_t(int fd, int width, int height, int wstride, int hstride, int format)
{
    rga_buffer_t buffer;

    memset(&buffer, 0x00, sizeof(rga_buffer_t));
    buffer.fd = fd;
    buffer.width = width;
    buffer.height = height;
    buffer.wstride = wstride;
    buffer.hstride = hstride;
    buffer.format = format;

    return buffer;
}

static void empty_structure(rga_buffer_t *src, rga_buffer_t *dst, rga_buffer_t *pat, im_rect *srect, im_rect *drect, im_rect *prect)
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
}

static bool rga_is_buffer_valid(rga_buffer_t buf)
{
    return (buf.phy_addr != NULL || buf.fd > 0 || buf.vir_addr != NULL);
}

static bool rga_is_rect_valid(im_rect rect)
{
    return (rect.x > 0 || rect.y > 0 || (rect.width > 0 && rect.height > 0));
}

IM_STATUS rga_set_buffer_info(rga_buffer_t dst, rga_info_t *dstinfo)
{
    if (NULL == dstinfo) {
        rga_error("rga_im2d: invaild dstinfo");
        imSetErrorMsg("Dst structure address is NULL.");
        return IM_STATUS_INVALID_PARAM;
    }

    if (dst.phy_addr != NULL) {
        dstinfo->phyAddr= dst.phy_addr;
    } else if(dst.fd > 0) {
        dstinfo->fd = dst.fd;
        dstinfo->mmuFlag = 1;
    } else if(dst.vir_addr != NULL) {
        dstinfo->virAddr = dst.vir_addr;
        dstinfo->mmuFlag = 1;
    } else {
        rga_error("rga_im2d: invaild dst buffer");
        imSetErrorMsg("No address available in dst buffer, phy_addr:[%ld], fd:[%d], vir_addr:[%ld]", (unsigned long)dst.phy_addr, dst.fd, (unsigned long)dst.vir_addr);
        return IM_STATUS_INVALID_PARAM;
    }

    return IM_STATUS_SUCCESS;
}

IM_STATUS rga_set_buffer_info(const rga_buffer_t src, rga_buffer_t dst, rga_info_t *srcinfo, rga_info_t *dstinfo)
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

    if (src.phy_addr != NULL) {
        srcinfo->phyAddr = src.phy_addr;
    } else if(src.fd > 0) {
        srcinfo->fd = src.fd;
        srcinfo->mmuFlag = 1;
    } else if(src.vir_addr != NULL) {
        srcinfo->virAddr = src.vir_addr;
        srcinfo->mmuFlag = 1;
    } else {
        rga_error("rga_im2d: invaild src buffer");
        imSetErrorMsg("No address available in src buffer, phy_addr:[%ld], fd:[%d], vir_addr:[%ld]", (unsigned long)src.phy_addr, src.fd, (unsigned long)src.vir_addr);
        return IM_STATUS_INVALID_PARAM;
    }

    if (dst.phy_addr != NULL) {
        dstinfo->phyAddr= dst.phy_addr;
    } else if(dst.fd > 0) {
        dstinfo->fd = dst.fd;
        dstinfo->mmuFlag = 1;
    } else if(dst.vir_addr != NULL) {
        dstinfo->virAddr = dst.vir_addr;
        dstinfo->mmuFlag = 1;
    } else {
        rga_error("rga_im2d: invaild dst buffer");
        imSetErrorMsg("No address available in dst buffer, phy_addr:[%ld], fd:[%d], vir_addr:[%ld]", (unsigned long)dst.phy_addr, dst.fd, (unsigned long)dst.vir_addr);
        return IM_STATUS_INVALID_PARAM;
    }

    return IM_STATUS_SUCCESS;
}

static inline void rga_apply_rect(rga_buffer_t *image, im_rect *rect)
{
    if (rect->width > 0 && rect->height > 0) {
        image->width = rect->width;
        image->height = rect->height;
    }
}

void rga_check_perpare(rga_buffer_t *src, rga_buffer_t *dst, rga_buffer_t *pat, im_rect *src_rect, im_rect *dst_rect, im_rect *pat_rect, int mode_usage)
{
    if (mode_usage & IM_CROP) {
        dst_rect->width = src_rect->width;
        dst_rect->height = src_rect->height;
    }

    rga_apply_rect(src, src_rect);
    rga_apply_rect(dst, dst_rect);
    if (rga_is_buffer_valid(*pat)) {
        rga_apply_rect(pat, pat_rect);
    }
}

IM_STATUS rga_get_info(rga_info_table_entry *return_table)
{
    char buf[16];
    int rga_version = 0, rga_svn_version = 0;

    static rga_info_table_entry table[] = {
        { RGA_V_ERR     ,    0,     0,  0, 0,   0, 0, 0, {0} },
        { RGA_1         , 8192, 2048,   8, 1,   IM_RGA_SUPPORT_FORMAT_RGB |
                                                IM_RGA_SUPPORT_FORMAT_RGB_OTHER |
                                                IM_RGA_SUPPORT_FORMAT_BPP |
                                                IM_RGA_SUPPORT_FORMAT_YUV_8,
                                                IM_RGA_SUPPORT_FORMAT_RGB |
                                                IM_RGA_SUPPORT_FORMAT_RGB_OTHER |
                                                IM_RGA_SUPPORT_FORMAT_YUV_8,
                                                IM_RGA_SUPPORT_FEATURE_COLOR_FILL |
                                                IM_RGA_SUPPORT_FEATURE_COLOR_PALETTE |
                                                IM_RGA_SUPPORT_FEATURE_ROP,
                                                {0} },
        { RGA_1_PLUS    , 8192, 2048,   8, 1,   IM_RGA_SUPPORT_FORMAT_RGB |
                                                IM_RGA_SUPPORT_FORMAT_RGB_OTHER |
                                                IM_RGA_SUPPORT_FORMAT_BPP |
                                                IM_RGA_SUPPORT_FORMAT_YUV_8,
                                                IM_RGA_SUPPORT_FORMAT_RGB |
                                                IM_RGA_SUPPORT_FORMAT_RGB_OTHER |
                                                IM_RGA_SUPPORT_FORMAT_YUV_8,
                                                IM_RGA_SUPPORT_FEATURE_COLOR_FILL |
                                                IM_RGA_SUPPORT_FEATURE_COLOR_PALETTE,
                                                {0} },
        { RGA_2         , 8192, 4096, 16, 2,    IM_RGA_SUPPORT_FORMAT_RGB |
                                                IM_RGA_SUPPORT_FORMAT_RGB_OTHER |
                                                IM_RGA_SUPPORT_FORMAT_YUV_8,
                                                IM_RGA_SUPPORT_FORMAT_RGB |
                                                IM_RGA_SUPPORT_FORMAT_RGB_OTHER |
                                                IM_RGA_SUPPORT_FORMAT_YUV_8,
                                                IM_RGA_SUPPORT_FEATURE_COLOR_FILL |
                                                IM_RGA_SUPPORT_FEATURE_COLOR_PALETTE |
                                                IM_RGA_SUPPORT_FEATURE_ROP,
                                                {0} },
        { RGA_2_LITE0   , 8192, 4096,   8, 2,   IM_RGA_SUPPORT_FORMAT_RGB |
                                                IM_RGA_SUPPORT_FORMAT_RGB_OTHER |
                                                IM_RGA_SUPPORT_FORMAT_YUV_8,
                                                IM_RGA_SUPPORT_FORMAT_RGB |
                                                IM_RGA_SUPPORT_FORMAT_RGB_OTHER |
                                                IM_RGA_SUPPORT_FORMAT_YUV_8,
                                                IM_RGA_SUPPORT_FEATURE_COLOR_FILL |
                                                IM_RGA_SUPPORT_FEATURE_COLOR_PALETTE |
                                                IM_RGA_SUPPORT_FEATURE_ROP,
                                                {0} },
        { RGA_2_LITE1   , 8192, 4096,   8, 2,   IM_RGA_SUPPORT_FORMAT_RGB |
                                                IM_RGA_SUPPORT_FORMAT_RGB_OTHER |
                                                IM_RGA_SUPPORT_FORMAT_YUV_8 |
                                                IM_RGA_SUPPORT_FORMAT_YUV_10,
                                                IM_RGA_SUPPORT_FORMAT_RGB |
                                                IM_RGA_SUPPORT_FORMAT_RGB_OTHER |
                                                IM_RGA_SUPPORT_FORMAT_YUV_8,
                                                IM_RGA_SUPPORT_FEATURE_COLOR_FILL |
                                                IM_RGA_SUPPORT_FEATURE_COLOR_PALETTE,
                                                {0} },
        { RGA_2_ENHANCE , 8192, 4096, 16,  2,   IM_RGA_SUPPORT_FORMAT_RGB |
                                                IM_RGA_SUPPORT_FORMAT_RGB_OTHER |
                                                IM_RGA_SUPPORT_FORMAT_YUV_8 |
                                                IM_RGA_SUPPORT_FORMAT_YUV_10,
                                                IM_RGA_SUPPORT_FORMAT_RGB |
                                                IM_RGA_SUPPORT_FORMAT_RGB_OTHER |
                                                IM_RGA_SUPPORT_FORMAT_YUV_8 |
                                                IM_RGA_SUPPORT_FORMAT_YUV_10 |
                                                IM_RGA_SUPPORT_FORMAT_YUYV_420 |
                                                IM_RGA_SUPPORT_FORMAT_YUYV_422,
                                                IM_RGA_SUPPORT_FEATURE_COLOR_FILL |
                                                IM_RGA_SUPPORT_FEATURE_COLOR_PALETTE,
                                                {0} }
    };

    if (rgaCtx == NULL) {
        RockchipRga& rkRga(RockchipRga::get());
        if (rgaCtx == NULL) {
            memcpy(return_table, &table[RGA_V_ERR], sizeof(rga_info_table_entry));

            rga_error("rga_im2d: The current RockchipRga singleton is destroyed. Please check if RkRgaInit/RkRgaDeInit are called, if so, please disable them.");
            imSetErrorMsg("The current RockchipRga singleton is destroyed. Please check if RkRgaInit/RkRgaDeInit are called, if so, please disable them.");
            return IM_STATUS_FAILED;
        }
    }

    sprintf(buf, "%f", rgaCtx->mVersion);
    sscanf(rgaCtx->mVersion_str, "%*[^.].%*[^.].%x", &rga_svn_version);

    if (strncmp(buf, "2.0", 4) == 0) {
        if (rga_svn_version == 0) {
            rga_version = RGA_2;
            memcpy(return_table, &table[rga_version], sizeof(rga_info_table_entry));
        } else {
            goto TRY_TO_COMPATIBLE;
        }
    } else if (strncmp(buf, "3.0", 3) == 0) {
        switch (rga_svn_version) {
            case 0x16445:
                rga_version = RGA_2;
                memcpy(return_table, &table[rga_version], sizeof(rga_info_table_entry));
                break;

            case 0x22245:
                rga_version = RGA_2_ENHANCE;
                memcpy(return_table, &table[rga_version], sizeof(rga_info_table_entry));
                break;

            default:
                goto TRY_TO_COMPATIBLE;
        }
    } else if (strncmp(buf, "3.2", 3) == 0) {
        switch (rga_svn_version) {
            case 0x18218:
                rga_version = RGA_2_ENHANCE;
                memcpy(return_table, &table[rga_version], sizeof(rga_info_table_entry));
                return_table->feature |= IM_RGA_SUPPORT_FEATURE_ROP;
                break;

            case 0x56726:
            case 0x63318:
                rga_version = RGA_2_ENHANCE;
                memcpy(return_table, &table[rga_version], sizeof(rga_info_table_entry));
                return_table->input_format |= IM_RGA_SUPPORT_FORMAT_YUYV_422 | IM_RGA_SUPPORT_FORMAT_YUV_400;
                return_table->output_format |= IM_RGA_SUPPORT_FORMAT_YUV_400 | IM_RGA_SUPPORT_FORMAT_Y4;
                return_table->feature |= IM_RGA_SUPPORT_FEATURE_QUANTIZE | IM_RGA_SUPPORT_FEATURE_SRC1_R2Y_CSC | IM_RGA_SUPPORT_FEATURE_DST_FULL_CSC;
                break;

            default :
                goto TRY_TO_COMPATIBLE;
        }
    } else if (strncmp(buf, "4.0", 3) == 0) {
        switch (rga_svn_version) {
            case 0x18632:
                rga_version = RGA_2_LITE0;
                memcpy(return_table, &table[rga_version], sizeof(rga_info_table_entry));
                break;

            case 0x23998:
                rga_version = RGA_2_LITE1;
                memcpy(return_table, &table[rga_version], sizeof(rga_info_table_entry));
                return_table->feature |= IM_RGA_SUPPORT_FEATURE_SRC1_R2Y_CSC;
                break;

            default:
                goto TRY_TO_COMPATIBLE;
        }
    } else if (strncmp(buf, "42.0", 4) == 0) {
        if (rga_svn_version == 17760) {
            rga_version = RGA_2_LITE1;
            memcpy(return_table, &table[rga_version], sizeof(rga_info_table_entry));
        } else {
            goto TRY_TO_COMPATIBLE;
        }
    } else {
        goto TRY_TO_COMPATIBLE;
    }

    return IM_STATUS_SUCCESS;

TRY_TO_COMPATIBLE:
    if (strncmp(buf, "1.3", 3) == 0) {
        rga_version = RGA_1;
    } else if (strncmp(buf, "1.6", 3) == 0) {
        rga_version = RGA_1_PLUS;
    } else if (strncmp(buf, "2.00", 4) == 0) {
        rga_version = RGA_2;
    } else if (strncmp(buf, "3.00", 4) == 0) {
        rga_version = RGA_2;
    } else if (strncmp(buf, "3.02", 4) == 0) {
        rga_version = RGA_2_ENHANCE;
    } else if (strncmp(buf, "4.00", 4) == 0) {
        rga_version = RGA_2_LITE0;
    } else if (strncmp(buf, "4.00", 4) == 0) {
        rga_version = RGA_2_LITE1;
    } else {
        rga_version = RGA_V_ERR;
    }

    memcpy(return_table, &table[rga_version], sizeof(rga_info_table_entry));

    if (rga_version == RGA_V_ERR) {
        rga_error("rga_im2d: Can not get the correct RGA version, please check the driver, version:[%s]", rgaCtx->mVersion_str);
        imSetErrorMsg("Can not get the correct RGA version, please check the driver, version:[%s]", rgaCtx->mVersion_str);
        return IM_STATUS_FAILED;
    }

    return IM_STATUS_SUCCESS;
}

const char *querystring(int name)
{
    enum {
        RGA_API = 0,
        RGA_BUILT,
    };

    long usage = 0;
    const char *temp;
    int rga_version = 0;
    bool all_output = 0, all_output_prepared = 0;
    const char *output_vendor = "Rockchip Electronics Co.,Ltd.";
    const char *output_name[] = {
        "RGA vendor            : ",
        "RGA version           : ",
        "Max input             : ",
        "Max output            : ",
        "Scale limit           : ",
        "Input support format  : ",
        "output support format : ",
        "RGA feature           : ",
        "expected performance  : ",
    };

    const char *version_name[] = {
        "RGA_api version       : ",
        "RGA_built version     : "
    };

    const char *output_version[] = {
        "unknown",
        "RGA_1",
        "RGA_1_plus",
        "RGA_2",
        "RGA_2_lite0",
        "RGA_2_lite1",
        "RGA_2_Enhance"
    };

    const char *output_resolution[] = {
        "unknown",
        "2048x2048",
        "4096x4096",
        "8192x8192"
    };

    const char *output_scale_limit[] = {
        "unknown",
        "0.125 ~ 8",
        "0.0625 ~ 16"
    };

    const char *output_format[] = {
        "unknown",
        "RGBA_8888 RGB_888 RGB_565 ",
        "RGBA_4444 RGBA_5551 ",
        "BPP8 BPP4 BPP2 BPP1 ",
        "YUV420/YUV422 ",
        "YUV420_10bit/YUV422_10bit ",
        "YUYV420 ",
        "YUYV422 ",
        "YUV400/Y4 "
    };

    const char *feature[] = {
        "unknown ",
        "color_fill ",
        "color_palette ",
        "ROP ",
        "quantize ",
        "src1_r2y_csc ",
        "dst_full_csc ",
    };

    const char *performance[] = {
        "unknown",
        "300M pix/s ",
        "520M pix/s ",
        "600M pix/s ",
    };

    std::ostringstream out;
    static std::string info;
    rga_info_table_entry rga_info;

    usage = rga_get_info(&rga_info);
    if (IM_STATUS_FAILED == usage) {
        rga_error("rga im2d: rga2 get info failed");
        return "get info failed";
    }

    do {
        switch (name) {
            case RGA_VENDOR:
                out << output_name[name] << output_vendor << std::endl;
                break;

            case RGA_VERSION:
                out << version_name[RGA_API] << "v" << RGA_API_VERSION << std::endl;
                // out << version_name[RGA_BUILT] << RGA_API_GIT_BUILD_VERSION << std::endl;
                out << output_name[name] << output_version[rga_info.version] << std::endl;
                break;

            case RGA_MAX_INPUT:
                switch (rga_info.input_resolution) {
                    case 2048:
                        out << output_name[name] << output_resolution[1] << std::endl;
                        break;

                    case 4096:
                        out << output_name[name] << output_resolution[2] << std::endl;
                        break;

                    case 8192:
                        out << output_name[name] << output_resolution[3] << std::endl;
                        break;

                    default:
                        out << output_name[name] << output_resolution[RGA_V_ERR] << std::endl;
                        break;
                }
                break;

            case RGA_MAX_OUTPUT:
                switch (rga_info.output_resolution) {
                    case 2048:
                        out << output_name[name] << output_resolution[1] << std::endl;
                        break;

                    case 4096:
                        out << output_name[name] << output_resolution[2] << std::endl;
                        break;

                    case 8192:
                        out << output_name[name] << output_resolution[3] << std::endl;
                        break;

                    default:
                        out << output_name[name] << output_resolution[RGA_V_ERR] << std::endl;
                        break;
                }
                break;

            case RGA_SCALE_LIMIT:
                switch (rga_info.scale_limit) {
                    case 8:
                        out << output_name[name] << output_scale_limit[1] << std::endl;
                        break;

                    case 16:
                        out << output_name[name] << output_scale_limit[2] << std::endl;
                        break;

                    default:
                        out << output_name[name] << output_scale_limit[RGA_V_ERR] << std::endl;
                        break;
                }
                break;

            case RGA_INPUT_FORMAT:
                out << output_name[name];
                if (rga_info.input_format & IM_RGA_SUPPORT_FORMAT_RGB) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_RGB_INDEX];
                }
                if (rga_info.input_format & IM_RGA_SUPPORT_FORMAT_RGB_OTHER) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_RGB_OTHER_INDEX];
                }
                if (rga_info.input_format & IM_RGA_SUPPORT_FORMAT_BPP) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_BPP_INDEX];
                }
                if (rga_info.input_format & IM_RGA_SUPPORT_FORMAT_YUV_8) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUV_8_INDEX];
                }
                if (rga_info.input_format & IM_RGA_SUPPORT_FORMAT_YUV_10) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUV_10_INDEX];
                }
                if (rga_info.input_format & IM_RGA_SUPPORT_FORMAT_YUYV_420) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUYV_420_INDEX];
                }
                if (rga_info.input_format & IM_RGA_SUPPORT_FORMAT_YUYV_422) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUYV_422_INDEX];
                }
                if (rga_info.input_format & IM_RGA_SUPPORT_FORMAT_YUV_400) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUV_400_INDEX];
                }
                if (!(rga_info.input_format & IM_RGA_SUPPORT_FORMAT_MASK)) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_ERROR_INDEX];
                }
                out << std::endl;
                break;

            case RGA_OUTPUT_FORMAT:
                out << output_name[name];
                if (rga_info.output_format & IM_RGA_SUPPORT_FORMAT_RGB) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_RGB_INDEX];
                }
                if (rga_info.output_format & IM_RGA_SUPPORT_FORMAT_RGB_OTHER) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_RGB_OTHER_INDEX];
                }
                if (rga_info.output_format & IM_RGA_SUPPORT_FORMAT_BPP) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_BPP_INDEX];
                }
                if (rga_info.output_format & IM_RGA_SUPPORT_FORMAT_YUV_8) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUV_8_INDEX];
                }
                if (rga_info.output_format & IM_RGA_SUPPORT_FORMAT_YUV_10) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUV_10_INDEX];
                }
                if (rga_info.output_format & IM_RGA_SUPPORT_FORMAT_YUYV_420) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUYV_420_INDEX];
                }
                if (rga_info.output_format & IM_RGA_SUPPORT_FORMAT_YUYV_422) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUYV_422_INDEX];
                }
                if (rga_info.output_format & IM_RGA_SUPPORT_FORMAT_YUV_400) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUV_400_INDEX];
                }
                if (!(rga_info.output_format & IM_RGA_SUPPORT_FORMAT_MASK)) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_ERROR_INDEX];
                }
                out << std::endl;
                break;

            case RGA_FEATURE:
                out << output_name[name];
                if (rga_info.feature & IM_RGA_SUPPORT_FEATURE_COLOR_FILL) {
                    out << feature[IM_RGA_SUPPORT_FEATURE_COLOR_FILL_INDEX];
                }
                if (rga_info.feature & IM_RGA_SUPPORT_FEATURE_COLOR_PALETTE) {
                    out << feature[IM_RGA_SUPPORT_FEATURE_COLOR_PALETTE_INDEX];
                }
                if (rga_info.feature & IM_RGA_SUPPORT_FEATURE_ROP) {
                    out << feature[IM_RGA_SUPPORT_FEATURE_ROP_INDEX];
                }
                if (rga_info.feature & IM_RGA_SUPPORT_FEATURE_QUANTIZE) {
                    out << feature[IM_RGA_SUPPORT_FEATURE_QUANTIZE_INDEX];
                }
                if (rga_info.feature & IM_RGA_SUPPORT_FEATURE_SRC1_R2Y_CSC) {
                    out << feature[IM_RGA_SUPPORT_FEATURE_SRC1_R2Y_CSC_INDEX];
                }
                if (rga_info.feature & IM_RGA_SUPPORT_FEATURE_DST_FULL_CSC) {
                    out << feature[IM_RGA_SUPPORT_FEATURE_DST_FULL_CSC_INDEX];
                }
                out << std::endl;
                break;

            case RGA_EXPECTED:
                switch (rga_info.performance) {
                    case 1:
                        out << output_name[name] << performance[1] << std::endl;
                        break;

                    case 2:
                        if (rga_info.version == RGA_2_LITE0 || rga_info.version == RGA_2_LITE1) {
                            out << output_name[name] << performance[2] << std::endl;
                        } else {
                            out << output_name[name] << performance[3] << std::endl;
                        }
                        break;

                    default:
                        out << output_name[name] << performance[RGA_V_ERR] << std::endl;
                        break;
                }
                break;

            case RGA_ALL:
                if (!all_output) {
                    all_output = 1;
                    name = 0;
                } else {
                    all_output_prepared = 1;
                }
                break;

            default:
                return "Invalid instruction";
        }

        info = out.str();

        if (all_output_prepared) {
            break;
        } else if (all_output && strcmp(info.c_str(), "0") > 0) {
            name++;
        }
    } while(all_output);

    temp = info.c_str();
    return temp;
}

IM_STATUS rga_check_info(const char *name, const rga_buffer_t info, const im_rect rect, int resolution_usage)
{
    if (info.width <= 0 || info.height <= 0 || info.format < 0) {
        imSetErrorMsg("Illegal name:[%s], the parameter cannot be negative or 0, width:[%d], height:[%d], format:[0x%X(%s)]", name, info.width, info.height, info.format, translate_format_str(info.format));
        return IM_STATUS_ILLEGAL_PARAM;
    }

    if (info.width < 2 || info.height < 2) {
        imSetErrorMsg("Hardware limitation name:[%s], unsupported operation of images smaller than 2 pixels, width:[%d], height:[%d]", name, info.width, info.height);
        return IM_STATUS_ILLEGAL_PARAM;
    }

    if (info.wstride < info.width || info.hstride < info.height) {
        imSetErrorMsg("Invaild name:[%s], Virtual width or height is less than actual width and height, wstride:[%d], width:[%d], hstride:[%d], height:[%d]", name, info.wstride, info.width, info.hstride, info.height);
        return IM_STATUS_INVALID_PARAM;
    }

    if (rect.width < 0 || rect.height < 0 || rect.x < 0 || rect.y < 0) {
        imSetErrorMsg("Illegal name:[%s] rect, the parameter cannot be negative, rect:[x,y,w,h] = [%d, %d, %d, %d]", name, rect.x, rect.y, rect.width, rect.height);
        return IM_STATUS_ILLEGAL_PARAM;
    }

    if ((rect.width > 0  && rect.width < 2) || (rect.height > 0 && rect.height < 2) || (rect.x > 0 && rect.x < 2) || (rect.y > 0 && rect.y < 2)) {
        imSetErrorMsg("Hardware limitation name:[%s] rect, unsupported operation of images smaller than 2 pixels, rect:[x,y,w,h] = [%d, %d, %d, %d]", name, rect.x, rect.y, rect.width, rect.height);
        return IM_STATUS_INVALID_PARAM;
    }

    if ((rect.width + rect.x > info.wstride) || (rect.height + rect.y > info.hstride)) {
        imSetErrorMsg("Invaild name:[%s] rect, the sum of width and height of rect needs to be less than wstride or hstride, rect:[x,y,w,h] = [%d, %d, %d, %d], wstride:[%d], hstride:[%d]", name, rect.x, rect.y, rect.width, rect.height, info.wstride, info.hstride);
        return IM_STATUS_INVALID_PARAM;
    }

    if (info.width > resolution_usage || info.height > resolution_usage) {
        imSetErrorMsg("Unsupported name:[%s] to input resolution more than %d, width:[%d], height:[%d]", name, resolution_usage, info.width, info.height);
        return IM_STATUS_NOT_SUPPORTED;
    } else if ((rect.width > 0 && rect.width > resolution_usage) || (rect.height > 0 && rect.height > resolution_usage)) {
        imSetErrorMsg("Unsupported name:[%s] rect to output resolution more than %d, rect:[x,y,w,h] = [%d, %d, %d, %d]", name, resolution_usage, rect.x, rect.y, rect.width, rect.height);
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

    if (mode_usage & IM_HAL_TRANSFORM_ROT_270 || mode_usage & IM_HAL_TRANSFORM_ROT_90) {
        dst_width = dst.height;
        dst_height = dst.width;
    } else {
        dst_width = dst.width;
        dst_height = dst.height;
    }

    if (((src_width >> (int)(log(scale_usage)/log(2))) > dst_width) || ((src_height >> (int)(log(scale_usage)/log(2))) > dst_height)) {
        imSetErrorMsg("Unsupported to scaling less than 1/%d ~ %d times, src:[w,h] = [%d, %d], dst:[w,h] = [%d, %d]", scale_usage, scale_usage, src.width, src.height, dst.width, dst.height);
        return IM_STATUS_NOT_SUPPORTED;
    }

    if (((dst_width >> (int)(log(scale_usage)/log(2))) > src_width) || ((dst_height >> (int)(log(scale_usage)/log(2))) > src_height)) {
        imSetErrorMsg("Unsupported to scaling more than 1/%d ~ %d times, src:[w,h] = [%d, %d], dst:[w,h] = [%d, %d]", scale_usage, scale_usage, src.width, src.height, dst.width, dst.height);
        return IM_STATUS_NOT_SUPPORTED;
    }

    return IM_STATUS_NOERROR;
}

IM_STATUS rga_check_format(const char *name, rga_buffer_t info, im_rect rect, int format_usage, int mode_usgae)
{
    int format = -1;

    format = RkRgaGetRgaFormat(RkRgaCompatibleFormat(info.format));
    if (-1 == format) {
        imSetErrorMsg("illegal name:[%s] format, please query and fix, format:[0x%X(%s)]\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
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
            imSetErrorMsg("name:[%s] unsupported RGB format, format:[0x%X(%s)]\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
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
            imSetErrorMsg("name:[%s] unsupported RGBA 4444/5551 format, format:[0x%X(%s)]\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
            return IM_STATUS_NOT_SUPPORTED;
        }
    } else if (format == RK_FORMAT_BPP1
        || format == RK_FORMAT_BPP2
        || format == RK_FORMAT_BPP4
        || format == RK_FORMAT_BPP8)
    {
        if ((~format_usage & IM_RGA_SUPPORT_FORMAT_BPP) && !(mode_usgae & IM_COLOR_PALETTE)) {
            imSetErrorMsg("name:[%s] unsupported BPP format, format:[0x%X(%s)]\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
            return IM_STATUS_NOT_SUPPORTED;
        }
    } else if (format == RK_FORMAT_YCrCb_420_SP
        || format == RK_FORMAT_YCbCr_420_SP
        || format == RK_FORMAT_YCrCb_420_P
        || format == RK_FORMAT_YCbCr_420_P
        || format == RK_FORMAT_YCrCb_422_SP
        || format == RK_FORMAT_YCbCr_422_SP
        || format == RK_FORMAT_YCrCb_422_P
        || format == RK_FORMAT_YCbCr_422_P)
    {
        if (~format_usage & IM_RGA_SUPPORT_FORMAT_YUV_8) {
            imSetErrorMsg("name:[%s] unsupported YUV 8bit format, format:[0x%X(%s)]\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
            return IM_STATUS_NOT_SUPPORTED;
        }

        if ((info.wstride % 4) || (info.hstride % 2) || (info.width % 2)  || (info.height % 2) || (rect.x % 2) || (rect.y % 2) || (rect.width % 2) || (rect.height % 2)) {
            imSetErrorMsg("name:[%s], Error yuv not align to 2 or width stride not align to 4, "
                          "rect:[x,y,w,h] = [%d, %d, %d, %d], "
                          "wstride:[%d], hstride:[%d], format:[0x%X(%s)]\n%s",
                          name, rect.x, rect.y, info.width, info.height, info.wstride, info.hstride,
                          info.format, translate_format_str(info.format),
                          querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
            return IM_STATUS_INVALID_PARAM;
        }
    } else if (format == RK_FORMAT_YCbCr_420_SP_10B || format == RK_FORMAT_YCrCb_420_SP_10B) {
        if (~format_usage & IM_RGA_SUPPORT_FORMAT_YUV_10) {
            imSetErrorMsg("name:[%s] unsupported YUV 10bit format, format:[0x%X(%s)]\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
            return IM_STATUS_NOT_SUPPORTED;
        }

        if ((info.wstride % 16) || (info.hstride % 2) || (info.width % 2)  || (info.height % 2) || (rect.x % 2) || (rect.y % 2) || (rect.width % 2) || (rect.height % 2)) {
            imSetErrorMsg("name:[%s], Err src wstride is not align to 16 or yuv not align to 2, "
                          "rect:[x,y,w,h] = [%d, %d, %d, %d], "
                          "wstride:[%d], hstride:[%d], format:[0x%X(%s)]\n%s",
                          name, rect.x, rect.y, info.width, info.height, info.wstride, info.hstride,
                          info.format, translate_format_str(info.format),
                          querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
            return IM_STATUS_INVALID_PARAM;
        }

        rga_error("If it is an RK encoder output, it needs to be aligned with an odd multiple of 256");
    } else if (format == RK_FORMAT_YUYV_420 || format == RK_FORMAT_YVYU_420 || format == RK_FORMAT_UYVY_420 || format == RK_FORMAT_VYUY_420) {
        if (~format_usage & IM_RGA_SUPPORT_FORMAT_YUYV_420) {
            imSetErrorMsg("name:[%s] unsupported YUYV format, format:[0x%X(%s)]\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
            return IM_STATUS_NOT_SUPPORTED;
        }
    } else if (format == RK_FORMAT_YUYV_422 || format == RK_FORMAT_YVYU_422 || format == RK_FORMAT_UYVY_422 || format == RK_FORMAT_VYUY_422) {
        if (~format_usage & IM_RGA_SUPPORT_FORMAT_YUYV_422) {
            imSetErrorMsg("name:[%s] unsupported YUYV format, format:[0x%X(%s)]\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
            return IM_STATUS_NOT_SUPPORTED;
        }
    } else if (format == RK_FORMAT_YCbCr_400) {
        if (~format_usage & IM_RGA_SUPPORT_FORMAT_YUV_400) {
            imSetErrorMsg("name:[%s] unsupported YUV400 format, format:[0x%X(%s)]\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
            return IM_STATUS_NOT_SUPPORTED;
        }
    } else if (format == RK_FORMAT_Y4) {
        if (~format_usage & IM_RGA_SUPPORT_FORMAT_Y4) {
            imSetErrorMsg("name:[%s] unsupported Y4/Y1 format, format:[0x%X(%s)]\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
            return IM_STATUS_NOT_SUPPORTED;
        }
    } else {
        imSetErrorMsg("name:[%s] unsupported this format, format:[0x%X(%s)]\n%s", name, info.format, translate_format_str(info.format), querystring((strcmp("dst", name) == 0) ? RGA_OUTPUT_FORMAT : RGA_INPUT_FORMAT));
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
                imSetErrorMsg("Blend mode 'src_over' unsupported src format without alpha, "
                              "format:[src,src1,dst] = [0x%X(%s), 0x%X(%s), 0x%X(%s)]",
                              src_fmt, translate_format_str(src_fmt),
                              pat_fmt, translate_format_str(pat_fmt),
                              dst_fmt, translate_format_str(dst_fmt));
                return IM_STATUS_NOT_SUPPORTED;
            }
            break;

        case IM_ALPHA_BLEND_DST_OVER:
            if (pat_enable) {
                if (!NormalRgaFormatHasAlpha(pat_fmt)) {
                    imSetErrorMsg("Blend mode 'dst_over' unsupported pat format without alpha, "
                                  "format:[src,src1,dst] = [0x%X(%s), 0x%X(%s), 0x%X(%s)]",
                                  src_fmt, translate_format_str(src_fmt),
                                  pat_fmt, translate_format_str(pat_fmt),
                                  dst_fmt, translate_format_str(dst_fmt));
                    return IM_STATUS_NOT_SUPPORTED;
                }
            } else {
                if (!NormalRgaFormatHasAlpha(dst_fmt)) {
                    imSetErrorMsg("Blend mode 'dst_over' unsupported dst format without alpha, "
                                  "format:[src,src1,dst] = [0x%X(%s), 0x%X(%s), 0x%X(%s)]",
                                  src_fmt, translate_format_str(src_fmt),
                                  pat_fmt, translate_format_str(pat_fmt),
                                  dst_fmt, translate_format_str(dst_fmt));
                    return IM_STATUS_NOT_SUPPORTED;
                }
            }
            break;

        default:
            if (!(NormalRgaFormatHasAlpha(src_fmt) || NormalRgaFormatHasAlpha(dst_fmt))) {
                imSetErrorMsg("Blend mode unsupported format without alpha, "
                              "format:[src,src1,dst] = [0x%X(%s), 0x%X(%s), 0x%X(%s)]",
                              src_fmt, translate_format_str(src_fmt),
                              pat_fmt, translate_format_str(pat_fmt),
                              dst_fmt, translate_format_str(dst_fmt));
                return IM_STATUS_NOT_SUPPORTED;
            }
            break;
    }

    if (pat_enable && (pat.width != dst.width || pat.height != dst.height)) {
        imSetErrorMsg("In the three-channel mode Alapha blend, the width and height of the src1 channel "
                      "must be equal to the dst channel, src1:[w,h] = [%d, %d], dst:[w,h] = [%d, %d]",
                      pat.width, pat.height, dst.width, dst.height);
        return IM_STATUS_NOT_SUPPORTED;
    }

    return IM_STATUS_NOERROR;
}

IM_STATUS rga_check_rotate(int mode_usage, rga_info_table_entry &table)
{
    if (table.version == RGA_1 || table.version == RGA_1_PLUS) {
        if (mode_usage & IM_HAL_TRANSFORM_FLIP_H_V) {
            imSetErrorMsg("RGA1/RGA1_PLUS cannot support H_V mirror");
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
    if (mode_usage == IM_COLOR_FILL && (~feature_usage & IM_RGA_SUPPORT_FEATURE_COLOR_FILL)) {
        imSetErrorMsg("The platform does not support color fill featrue. \n%s", querystring(RGA_FEATURE));
        return IM_STATUS_NOT_SUPPORTED;
    }

    if (mode_usage == IM_COLOR_PALETTE && (~feature_usage & IM_RGA_SUPPORT_FEATURE_COLOR_PALETTE)) {
        imSetErrorMsg("The platform does not support color palette featrue. \n%s", querystring(RGA_FEATURE));
        return IM_STATUS_NOT_SUPPORTED;
    }

    if (mode_usage == IM_ROP && (~feature_usage & IM_RGA_SUPPORT_FEATURE_ROP)) {
        imSetErrorMsg("The platform does not support ROP featrue. \n%s", querystring(RGA_FEATURE));
        return IM_STATUS_NOT_SUPPORTED;
    }

    if (mode_usage == IM_NN_QUANTIZE && (~feature_usage & IM_RGA_SUPPORT_FEATURE_QUANTIZE)) {
        imSetErrorMsg("The platform does not support quantize featrue. \n%s", querystring(RGA_FEATURE));
        return IM_STATUS_NOT_SUPPORTED;
    }

    if ((pat_enable ? (pat.color_space_mode & IM_RGB_TO_YUV_MASK) : 0) && (~feature_usage & IM_RGA_SUPPORT_FEATURE_SRC1_R2Y_CSC)) {
        imSetErrorMsg("The platform does not support src1 channel RGB2YUV color space convert featrue. \n%s", querystring(RGA_FEATURE));
        return IM_STATUS_NOT_SUPPORTED;
    }

    if ((src.color_space_mode & IM_FULL_CSC_MASK
        || dst.color_space_mode & IM_FULL_CSC_MASK ||
        (pat_enable ? (pat.color_space_mode & IM_FULL_CSC_MASK) : 0))
        && (~feature_usage & IM_RGA_SUPPORT_FEATURE_DST_FULL_CSC))
    {
        imSetErrorMsg("The platform does not support dst channel full color space convert(Y2Y/Y2R) featrue. \n%s", querystring(RGA_FEATURE));
        return IM_STATUS_NOT_SUPPORTED;
    }

    return IM_STATUS_NOERROR;
}

IM_STATUS imcheck_t(const rga_buffer_t src, const rga_buffer_t dst, const rga_buffer_t pat, const im_rect src_rect, const im_rect dst_rect, const im_rect pat_rect, int mode_usage)
{
    bool pat_enable = 0;
    rga_info_table_entry rga_info;
    IM_STATUS ret = IM_STATUS_NOERROR;

    ret = rga_get_info(&rga_info);
    if (IM_STATUS_FAILED == ret) {
        rga_error("rga im2d: rga2 get info failed");
        return IM_STATUS_FAILED;
    }

    if (mode_usage & IM_ALPHA_BLEND_MASK) {
        if (rga_is_buffer_valid(pat)) {
            pat_enable = 1;
        }
    }

    ret = rga_check_feature(src, pat, dst, pat_enable, mode_usage, rga_info.feature);
    if (ret != IM_STATUS_NOERROR) {
        return ret;
    }

    if (~mode_usage & IM_COLOR_FILL) {
        ret = rga_check_info("src", src, src_rect, rga_info.input_resolution);
        if (ret != IM_STATUS_NOERROR) {
            return ret;
        }

        ret = rga_check_format("src", src, src_rect, rga_info.input_format, mode_usage);
        if (ret != IM_STATUS_NOERROR) {
            return ret;
        }
    }

    if (pat_enable) {
        if (rga_info.version == RGA_1 || rga_info.version == RGA_1_PLUS) {
            imSetErrorMsg("RGA1/RGA1_PLUS cannot support src1.");
            return IM_STATUS_NOT_SUPPORTED;
        }

        ret = rga_check_info("pat", pat, pat_rect, rga_info.input_resolution);
        if (ret != IM_STATUS_NOERROR) {
            return ret;
        }

        ret = rga_check_format("pat", pat, pat_rect, rga_info.input_format, mode_usage);
        if (ret != IM_STATUS_NOERROR) {
            return ret;
        }
    }

    ret = rga_check_info("dst", dst, dst_rect, rga_info.output_resolution);
    if (ret != IM_STATUS_NOERROR) {
        return ret;
    }

    ret = rga_check_format("dst", dst, dst_rect, rga_info.output_format, mode_usage);
    if (ret != IM_STATUS_NOERROR) {
        return ret;
    }

    if ((~mode_usage & IM_COLOR_FILL)) {
        ret = rga_check_limit(src, dst, rga_info.scale_limit, mode_usage);
        if (ret != IM_STATUS_NOERROR) {
            return ret;
        }
    }

    if (mode_usage & IM_ALPHA_BLEND_MASK) {
        ret = rga_check_blend(src, pat, dst, pat_enable, mode_usage);
        if (ret != IM_STATUS_NOERROR) {
            return ret;
        }
    }

    ret = rga_check_rotate(mode_usage, rga_info);
    if (ret != IM_STATUS_NOERROR) {
        return ret;
    }

    return IM_STATUS_NOERROR;
}

IM_STATUS imresize_t(const rga_buffer_t src, rga_buffer_t dst, double fx, double fy, int interpolation, int sync)
{
    int usage = 0;
    im_rect srect;
    im_rect drect;
    im_rect prect;
    rga_buffer_t pat;
    int width = 0, height = 0;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(NULL, NULL, &pat, &srect, &drect, &prect);

    if (fx > 0 || fy > 0) {
        if (fx == 0) {
            fx = 1;
        }

        if (fy == 0) {
            fy = 1;
        }

        dst.width = (int)(src.width * fx);
        dst.height = (int)(src.height * fy);

        if (NormalRgaIsYuvFormat(RkRgaGetRgaFormat(src.format))) {
            width = dst.width;
            height = dst.height;
            dst.width = DOWN_ALIGN(dst.width, 2);
            dst.height = DOWN_ALIGN(dst.height, 2);

            ret = imcheck(src, dst, srect, drect, usage);
            if (ret != IM_STATUS_NOERROR) {
                rga_error("imresize error, factor:[fx,fy] = [%lf,%lf], ALIGN:[dw,dh] = [%d,%d][%d,%d]", fx, fy, width, height, dst.width, dst.height);
                return ret;
            }
        }
    }

    RGA_UNUSED(interpolation);

    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    ret = improcess(src, dst, pat, srect, drect, prect, usage);
    return ret;
}

IM_STATUS imcrop_t(const rga_buffer_t src, rga_buffer_t dst, im_rect rect, int sync)
{
    int usage = 0;
    im_rect drect;
    im_rect prect;
    rga_buffer_t pat;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(NULL, NULL, &pat, NULL, &drect, &prect);

    drect.width = rect.width;
    drect.height = rect.height;

    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    ret = improcess(src, dst, pat, rect, drect, prect, usage);
    return ret;
}

IM_STATUS imrotate_t(const rga_buffer_t src, rga_buffer_t dst, int rotation, int sync)
{
    int usage = 0;
    im_rect srect;
    im_rect drect;
    im_rect prect;
    rga_buffer_t pat;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(NULL, NULL, &pat, &srect, &drect, &prect);

    usage |= rotation;
    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    ret = improcess(src, dst, pat, srect, drect, prect, usage);
    return ret;
}

IM_STATUS imflip_t (const rga_buffer_t src, rga_buffer_t dst, int mode, int sync)
{
    int usage = 0;
    im_rect srect;
    im_rect drect;
    im_rect prect;
    rga_buffer_t pat;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(NULL, NULL, &pat, &srect, &drect, &prect);

    usage |= mode;
    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    ret = improcess(src, dst, pat, srect, drect, prect, usage);
    return ret;
}

IM_STATUS imfill_t(rga_buffer_t dst, im_rect rect, int color, int sync)
{
    int usage = 0;
    im_rect srect;
    im_rect prect;
    rga_buffer_t pat;
    rga_buffer_t src;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(&src, NULL, &pat, &srect, NULL, &prect);

    memset(&src, 0, sizeof(rga_buffer_t));
    usage |= IM_COLOR_FILL;
    dst.color = color;

    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    ret = improcess(src, dst, pat, srect, rect, prect, usage);
    return ret;
}

IM_STATUS impalette_t(rga_buffer_t src, rga_buffer_t dst, rga_buffer_t lut, int sync)
{
    int usage = 0;
    im_rect srect;
    im_rect drect;
    im_rect prect;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(NULL, NULL, NULL, &srect, &drect, &prect);

    if ((src.width != dst.width) || (src.height != dst.height)) {
        return IM_STATUS_INVALID_PARAM;
    }

    usage |= IM_COLOR_PALETTE;
    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    ret = improcess(src, dst, lut, srect, drect, prect, usage);
    return ret;
}

IM_STATUS imtranslate_t(const rga_buffer_t src, rga_buffer_t dst, int x, int y, int sync)
{
    int usage = 0;
    im_rect srect;
    im_rect drect;
    im_rect prect;
    rga_buffer_t pat;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(NULL, NULL, &pat, &srect, &drect, &prect);

    if ((src.width != dst.width) || (src.height != dst.height)) {
        return IM_STATUS_INVALID_PARAM;
    }

    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    srect.width = src.width - x;
    srect.height = src.height - y;
    drect.x = x;
    drect.y = y;
    drect.width = src.width - x;
    drect.height = src.height - y;

    ret = improcess(src, dst, pat, srect, drect, prect, usage);
    return ret;
}

IM_STATUS imcopy_t(const rga_buffer_t src, rga_buffer_t dst, int sync)
{
    int usage = 0;
    im_rect srect;
    im_rect drect;
    im_rect prect;
    rga_buffer_t pat;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(NULL, NULL, &pat, &srect, &drect, &prect);

    if ((src.width != dst.width) || (src.height != dst.height)) {
        imSetErrorMsg("imcopy cannot support scale, src:[w,h] = [%d, %d], dst:[w,h] = [%d, %d]", src.width, src.height, dst.width, dst.height);
        return IM_STATUS_INVALID_PARAM;
    }

    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    ret = improcess(src, dst, pat, srect, drect, prect, usage);
    return ret;
}

IM_STATUS imcolorkey_t(const rga_buffer_t src, rga_buffer_t dst, im_colorkey_range range, int mode, int sync)
{
    int usage = 0;
    im_rect srect;
    im_rect drect;
    im_rect prect;
    rga_buffer_t pat;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(NULL, NULL, &pat, &srect, &drect, &prect);

    usage |= mode;
    dst.colorkey_range = range;

    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    ret = improcess(src, dst, pat, srect, drect, prect, usage);
    return ret;
}

IM_STATUS imblend_t(const rga_buffer_t srcA, const rga_buffer_t srcB, rga_buffer_t dst, int mode, int sync)
{
    int usage = 0;
    im_rect srect;
    im_rect drect;
    im_rect prect;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(NULL, NULL, NULL, &srect, &drect, &prect);

    usage |= mode;
    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    ret = improcess(srcA, dst, srcB, srect, drect, prect, usage);
    return ret;
}

IM_STATUS imcvtcolor_t(rga_buffer_t src, rga_buffer_t dst, int sfmt, int dfmt, int mode, int sync)
{
    int usage = 0;
    im_rect srect;
    im_rect drect;
    im_rect prect;
    rga_buffer_t pat;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(NULL, NULL, &pat, &srect, &drect, &prect);

    src.format = sfmt;
    dst.format = dfmt;
    dst.color_space_mode = mode;

    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    ret = improcess(src, dst, pat, srect, drect, prect, usage);
    return ret;
}

IM_STATUS imquantize_t(const rga_buffer_t src, rga_buffer_t dst, im_nn_t nn_info, int sync)
{
    int usage = 0;
    im_rect srect;
    im_rect drect;
    im_rect prect;
    rga_buffer_t pat;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(NULL, NULL, &pat, &srect, &drect, &prect);

    usage |= IM_NN_QUANTIZE;
    dst.nn = nn_info;

    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    ret = improcess(src, dst, pat, srect, drect, prect, usage);
    return ret;
}

IM_STATUS imrop_t(const rga_buffer_t src, rga_buffer_t dst, int rop_code, int sync)
{
    int usage = 0;
    im_rect srect;
    im_rect drect;
    im_rect prect;
    rga_buffer_t pat;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(NULL, NULL, &pat, &srect, &drect, &prect);

    usage |= IM_ROP;
    dst.rop_code = rop_code;

    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    ret = improcess(src, dst, pat, srect, drect, prect, usage);
    return ret;
}

IM_STATUS improcess(rga_buffer_t src, rga_buffer_t dst, rga_buffer_t pat, im_rect srect, im_rect drect, im_rect prect, int usage)
{
    int ret;
    rga_info_t srcinfo;
    rga_info_t dstinfo;
    rga_info_t patinfo;

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

    if ((usage & IM_ALPHA_BLEND_MASK) && rga_is_buffer_valid(pat)) {
        ret = imcheck_composite(src, dst, pat, srect, drect, prect, usage);
    } else {
        ret = imcheck(src, dst, srect, drect, usage);
    }

    if(ret != IM_STATUS_NOERROR) {
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

        if (srcinfo.rotation ==0) {
            rga_error("rga_im2d: Could not find rotate/flip usage:[0x%X]", usage);
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
            rga_error("rga_im2d: Could not find blend usage:[0x%x]", usage);
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
        srcinfo.colorkey_min = dst.colorkey_range.min;
        srcinfo.colorkey_max = dst.colorkey_range.max;
        switch (usage & IM_ALPHA_COLORKEY_MASK) {
            case IM_ALPHA_COLORKEY_NORMAL:
                srcinfo.colorkey_mode = 0;
                break;

            case IM_ALPHA_COLORKEY_INVERTED:
                srcinfo.colorkey_mode = 1;
                break;
        }
    }

    if (usage & IM_NN_QUANTIZE) {
        dstinfo.nn.nn_flag = 1;
        dstinfo.nn.scale_r = dst.nn.scale_r;
        dstinfo.nn.scale_g = dst.nn.scale_g;
        dstinfo.nn.scale_b = dst.nn.scale_b;
        dstinfo.nn.offset_r = dst.nn.offset_r;
        dstinfo.nn.offset_g = dst.nn.offset_g;
        dstinfo.nn.offset_b = dst.nn.offset_b;
    }

    if (usage & IM_ROP) {
        srcinfo.rop_code = dst.rop_code;
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
                          "src_fromat:[0x%X(%s)], src1_format:[0x%X(%s)], dst_format:[0x%X(%s)]",
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
                          "src_fromat:[0x%X(%s)], src1_format:[0x%X(%s)], dst_format:[0x%X(%s)]",
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
                          "src_fromat:[0x%X(%s)], src1_format:[0x%X(%s)], dst_format:[0x%X(%s)]",
                          src.format, translate_format_str(src.format),
                          pat.format, rga_is_buffer_valid(pat) ? translate_format_str(pat.format) : "none",
                          dst.format, translate_format_str(dst.format));
            return IM_STATUS_ILLEGAL_PARAM;
        }
    } else if (src.color_space_mode & IM_FULL_CSC_MASK || dst.color_space_mode & IM_FULL_CSC_MASK) {
        if (src.color_space_mode == IM_COLOR_SPACE_DEFAULT) {
            if (NormalRgaIsRgbFormat(RkRgaGetRgaFormat(src.format))) {
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

        if (src.color_space_mode == IM_RGB_FULL && dst.color_space_mode == IM_YUV_BT709_FULL_RANGE) {
            dstinfo.color_space_mode = rgb2yuv_709_full;
        } else if (src.color_space_mode == IM_YUV_BT601_FULL_RANGE && dst.color_space_mode == IM_YUV_BT709_LIMIT_RANGE) {
            dstinfo.color_space_mode = yuv2yuv_601_full_2_709_limit;
        } else if (src.color_space_mode == IM_YUV_BT709_LIMIT_RANGE && dst.color_space_mode == IM_YUV_BT601_LIMIT_RANGE) {
            dstinfo.color_space_mode = yuv2yuv_709_limit_2_601_limit;
        } else if (src.color_space_mode == IM_YUV_BT709_FULL_RANGE && dst.color_space_mode == IM_YUV_BT601_LIMIT_RANGE) {
            dstinfo.color_space_mode = yuv2yuv_709_full_2_601_limit;
        } else if (src.color_space_mode == IM_YUV_BT709_FULL_RANGE && dst.color_space_mode == IM_YUV_BT601_FULL_RANGE) {
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

    RockchipRga &rkRga(RockchipRga::get());

    if (usage & IM_ASYNC) {
        dstinfo.sync_mode = RGA_BLIT_ASYNC;
    } else if (usage & IM_SYNC) {
        dstinfo.sync_mode = RGA_BLIT_SYNC;
    }

    if (usage & IM_COLOR_FILL) {
        dstinfo.color = dst.color;
        ret = rkRga.RkRgaCollorFill(&dstinfo);
    } else if (usage & IM_COLOR_PALETTE) {
        ret = rkRga.RkRgaCollorPalette(&srcinfo, &dstinfo, &patinfo);
    } else if ((usage & IM_ALPHA_BLEND_MASK) && rga_is_buffer_valid(pat)) {
        ret = rkRga.RkRgaBlit(&srcinfo, &dstinfo, &patinfo);
    }else {
        ret = rkRga.RkRgaBlit(&srcinfo, &dstinfo, NULL);
    }

    if (ret) {
        imSetErrorMsg("Failed to call RockChipRga interface, query log to find the cause of failure.");
        rga_error("srect:[x,y,w,h] = [%d, %d, %d, %d], src:[w,h,ws,hs] = [%d, %d, %d, %d]", srect.x, srect.y, srect.width, srect.height, src.width, src.height, src.wstride, src.hstride);
        if (rga_is_buffer_valid(pat)) {
           rga_error("s1/prect:[x,y,w,h] = [%d, %d, %d, %d], src1/pat:[w,h,ws,hs] = [%d, %d, %d, %d]", prect.x, prect.y, prect.width, prect.height, pat.width, pat.height, pat.wstride, pat.hstride);
        }

        rga_error("drect:[x,y,w,h] = [%d, %d, %d, %d], dst:[w,h,ws,hs] = [%d, %d, %d, %d]", drect.x, drect.y, drect.width, drect.height, dst.width, dst.height, dst.wstride, dst.hstride);
        rga_error("usage:[0x%X]", usage);

        return IM_STATUS_FAILED;
    }

    return IM_STATUS_SUCCESS;
}

IM_STATUS imsync(void)
{
    int ret = 0;

    RockchipRga &rkRga(RockchipRga::get());

    ret = rkRga.RkRgaFlush();
    if (ret) {
        return IM_STATUS_FAILED;
    }

    return IM_STATUS_SUCCESS;
}
