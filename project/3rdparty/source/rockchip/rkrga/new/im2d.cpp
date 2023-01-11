#include <math.h>
#include <sstream>
#include <sys/ioctl.h>

#include <rockchip/rkrgax/im2d.hpp>
#include <rockchip/rkrgax/rgadbg.h>
#include <rockchip/rkrgax/RgaUtils.h>
#include <rockchip/rkrgax/rga_sync.h>
#include <rockchip/rkrgax/NormalRga.h>
#include <rockchip/rkrgax/im2d_impl.h>
#include <rockchip/rkrgax/RockchipRga.h>
#include <rockchip/rkrgax/im2d_hardware.h>

extern __thread im_context_t g_im2d_context;
extern __thread char rga_err_str[ERR_MSG_LEN];

const char *imStrError_t(IM_STATUS status) {
    const char *error_type[] = {
        "No errors during operation",
        "Run successfully",
        "Unsupported function",
        "Memory overflow",
        "Invalid parameters",
        "Illegal parameters",
        "verify librga and driver version",
        "Fatal error",
        "unkown status"
    };

    const char *ptr = NULL;
    static __thread char error_str[ERR_MSG_LEN] = "The current error message is empty!";

    switch(status) {
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

        case IM_STATUS_ERROR_VERSION:
            ptr = error_type[6];
            break;

        case IM_STATUS_FAILED:
            ptr = error_type[7];
            break;

        default :
            return error_type[8];
    }

    snprintf(error_str, ERR_MSG_LEN, "%s: %s", ptr, rga_err_str);
    imSetErrorMsg("No error message, it has been cleared.");

    return error_str;
}

rga_buffer_handle_t importbuffer_fd(int fd, int size)
{
    return rga_import_buffer((uint64_t)fd, RGA_DMA_BUFFER, (uint32_t)size);
}

rga_buffer_handle_t importbuffer_fd(int fd, im_handle_param_t *param)
{
    return rga_import_buffer((uint64_t)fd, RGA_DMA_BUFFER, param);
}

rga_buffer_handle_t importbuffer_fd(int fd, int width, int height, int format)
{
    im_handle_param_t param = {(uint32_t)width, (uint32_t)height, (uint32_t)format};
    return rga_import_buffer((uint64_t)fd, RGA_DMA_BUFFER, &param);
}

rga_buffer_handle_t importbuffer_virtualaddr(void *va, int size)
{
    return rga_import_buffer((uint64_t)va, RGA_VIRTUAL_ADDRESS, (uint32_t)size);
}

rga_buffer_handle_t importbuffer_virtualaddr(void *va, im_handle_param_t *param)
{
    return rga_import_buffer((uint64_t)va, RGA_VIRTUAL_ADDRESS, param);
}

rga_buffer_handle_t importbuffer_virtualaddr(void *va, int width, int height, int format)
{
    im_handle_param_t param = {(uint32_t)width, (uint32_t)height, (uint32_t)format};
    return rga_import_buffer((uint64_t)va, RGA_VIRTUAL_ADDRESS, &param);
}

rga_buffer_handle_t importbuffer_physicaladdr(uint64_t pa, int size)
{
    return rga_import_buffer(pa, RGA_PHYSICAL_ADDRESS, (uint32_t)size);
}

rga_buffer_handle_t importbuffer_physicaladdr(uint64_t pa, im_handle_param_t *param)
{
    return rga_import_buffer(pa, RGA_PHYSICAL_ADDRESS, param);
}

rga_buffer_handle_t importbuffer_physicaladdr(uint64_t pa, int width, int height, int format)
{
    im_handle_param_t param = {(uint32_t)width, (uint32_t)height, (uint32_t)format};
    return rga_import_buffer(pa, RGA_PHYSICAL_ADDRESS, &param);
}

IM_STATUS releasebuffer_handle(rga_buffer_handle_t handle)
{
    return rga_release_buffer(handle);
}

rga_buffer_t wrapbuffer_virtualaddr_t(void* vir_addr, int width, int height, int wstride, int hstride, int format)
{
    rga_buffer_t buffer;

    memset(&buffer, 0, sizeof(rga_buffer_t));
    buffer.vir_addr = vir_addr;
    buffer.width = width;
    buffer.height = height;
    buffer.wstride = wstride;
    buffer.hstride = hstride;
    buffer.format = format;

    return buffer;
}

rga_buffer_t wrapbuffer_physicaladdr_t(void* phy_addr, int width, int height, int wstride, int hstride, int format)
{
    rga_buffer_t buffer;

    memset(&buffer, 0, sizeof(rga_buffer_t));
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

    memset(&buffer, 0, sizeof(rga_buffer_t));
    buffer.fd = fd;
    buffer.width = width;
    buffer.height = height;
    buffer.wstride = wstride;
    buffer.hstride = hstride;
    buffer.format = format;

    return buffer;
}

rga_buffer_t wrapbuffer_handle(rga_buffer_handle_t handle, int width, int height, int format, int wstride, int hstride)
{
    rga_buffer_t buffer;

    memset(&buffer, 0, sizeof(rga_buffer_t));
    buffer.handle = handle;
    buffer.width = width;
    buffer.height = height;
    buffer.wstride = wstride;
    buffer.hstride = hstride;
    buffer.format = format;

    return buffer;
}

rga_buffer_t wrapbuffer_handle(rga_buffer_handle_t handle, int width, int height, int format)
{
    return wrapbuffer_handle(handle, width, height, format, width, height);
}

rga_buffer_t wrapbuffer_handle_t(rga_buffer_handle_t handle, int width, int height, int wstride, int hstride, int format)
{
    return wrapbuffer_handle(handle, width, height, format, wstride, hstride);
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

const char *querystring(int name)
{
    long usage = 0;
    int rga_version = 0;
    bool all_output = 0, all_output_prepared = 0;

    enum {
        RGA_API = 0,
    };

    const char *temp;
    const char *output_vendor = "Rockchip Electronics Co.,Ltd.";
    const char *output_name[] = {
        "RGA vendor            : ",
        "RGA version           : ",
        "Max input             : ",
        "Max output            : ",
        "Byte stride           : ",
        "Scale limit           : ",
        "Input support format  : ",
        "output support format : ",
        "RGA feature           : ",
        "expected performance  : ",
    };

    const char *version_name[] = {
        "RGA_api version       : ",
    };

    const char *output_version[] = {
        "unknown ",
        "RGA_1 ",
        "RGA_1_plus ",
        "RGA_2 ",
        "RGA_2_lite0 ",
        "RGA_2_lite1 ",
        "RGA_2_Enhance ",
        "RGA_3 ",
    };

    const char *output_resolution[] = {
        "unknown",
        "2048x2048",
        "4096x4096",
        "8192x8192",
        "8128x8128",
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
        "YUV420_sp_8bit ",
        "YUV420_sp_10bit ",
        "YUV420_p_8bit ",
        "YUV420_p_10bit ",
        "YUV422_sp_8bit ",
        "YUV422_sp_10bit ",
        "YUV422_p_8bit ",
        "YUV422_p_10bit ",
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
        "FBC_mode ",
        "blend_in_YUV ",
        "BT.2020 ",
        "mosaic ",
        "OSD ",
        "early_interruption ",
    };

    const char *performance[] = {
        "unknown",
        "max 1 pixel/cycle ",
        "max 2 pixel/cycle ",
        "max 4 pixel/cycle ",
    };

    std::ostringstream out;
    static std::string info;
    rga_info_table_entry rga_info;

    memset(&rga_info, 0x0, sizeof(rga_info));
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

                out << output_name[name];
                if (rga_info.version == IM_RGA_HW_VERSION_RGA_V_ERR) {
                    out << output_version[IM_RGA_HW_VERSION_RGA_V_ERR_INDEX];
                } else {
                    if (rga_info.version & IM_RGA_HW_VERSION_RGA_1) {
                        out << output_version[IM_RGA_HW_VERSION_RGA_1_INDEX];
                    }

                    if (rga_info.version & IM_RGA_HW_VERSION_RGA_1_PLUS) {
                        out << output_version[IM_RGA_HW_VERSION_RGA_1_PLUS_INDEX];
                    }

                    if (rga_info.version & IM_RGA_HW_VERSION_RGA_2) {
                        out << output_version[IM_RGA_HW_VERSION_RGA_2_INDEX];
                    }

                    if (rga_info.version & IM_RGA_HW_VERSION_RGA_2_LITE0) {
                        out << output_version[IM_RGA_HW_VERSION_RGA_2_LITE0_INDEX];
                    }

                    if (rga_info.version & IM_RGA_HW_VERSION_RGA_2_LITE1) {
                        out << output_version[IM_RGA_HW_VERSION_RGA_2_LITE1_INDEX];
                    }

                    if (rga_info.version & IM_RGA_HW_VERSION_RGA_2_ENHANCE) {
                        out << output_version[IM_RGA_HW_VERSION_RGA_2_ENHANCE_INDEX];
                    }

                    if (rga_info.version & IM_RGA_HW_VERSION_RGA_3) {
                        out << output_version[IM_RGA_HW_VERSION_RGA_3_INDEX];
                    }
                }
                out << std::endl;
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

                    case 8128:
                        out << output_name[name] << output_resolution[4] << std::endl;
                        break;

                    default:
                        out << output_name[name] << output_resolution[IM_RGA_HW_VERSION_RGA_V_ERR_INDEX] << std::endl;
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

                    case 8128:
                        out << output_name[name] << output_resolution[4] << std::endl;
                        break;

                    default:
                        out << output_name[name] << output_resolution[IM_RGA_HW_VERSION_RGA_V_ERR_INDEX] << std::endl;
                        break;
                }
                break;

            case RGA_BYTE_STRIDE:
                if (rga_info.byte_stride > 0) {
                    out << output_name[name] << rga_info.byte_stride << " byte" << std::endl;
                } else {
                    out << output_name[name] << "unknown" << std::endl;
                }
                break;

            case RGA_SCALE_LIMIT :
                switch (rga_info.scale_limit) {
                    case 8:
                        out << output_name[name] << output_scale_limit[1] << std::endl;
                        break;

                    case 16:
                        out << output_name[name] << output_scale_limit[2] << std::endl;
                        break;

                    default:
                        out << output_name[name] << output_scale_limit[IM_RGA_HW_VERSION_RGA_V_ERR_INDEX] << std::endl;
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
                if (rga_info.input_format & IM_RGA_SUPPORT_FORMAT_YUV_420_SEMI_PLANNER_8_BIT) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUV_420_SEMI_PLANNER_8_BIT_INDEX];
                }
                if (rga_info.input_format & IM_RGA_SUPPORT_FORMAT_YUV_420_SEMI_PLANNER_10_BIT) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUV_420_SEMI_PLANNER_10_BIT_INDEX];
                }
                if (rga_info.input_format & IM_RGA_SUPPORT_FORMAT_YUV_420_PLANNER_8_BIT) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUV_420_PLANNER_8_BIT_INDEX];
                }
                if (rga_info.input_format & IM_RGA_SUPPORT_FORMAT_YUV_420_PLANNER_10_BIT) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUV_420_PLANNER_10_BIT_INDEX];
                }
                if (rga_info.input_format & IM_RGA_SUPPORT_FORMAT_YUV_422_SEMI_PLANNER_8_BIT) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUV_422_SEMI_PLANNER_8_BIT_INDEX];
                }
                if (rga_info.input_format & IM_RGA_SUPPORT_FORMAT_YUV_422_SEMI_PLANNER_10_BIT) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUV_422_SEMI_PLANNER_10_BIT_INDEX];
                }
                if (rga_info.input_format & IM_RGA_SUPPORT_FORMAT_YUV_422_PLANNER_8_BIT) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUV_422_PLANNER_8_BIT_INDEX];
                }
                if (rga_info.input_format & IM_RGA_SUPPORT_FORMAT_YUV_422_PLANNER_10_BIT) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUV_422_PLANNER_10_BIT_INDEX];
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

            case RGA_OUTPUT_FORMAT :
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
                if (rga_info.output_format & IM_RGA_SUPPORT_FORMAT_YUV_420_SEMI_PLANNER_8_BIT) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUV_420_SEMI_PLANNER_8_BIT_INDEX];
                }
                if (rga_info.output_format & IM_RGA_SUPPORT_FORMAT_YUV_420_SEMI_PLANNER_10_BIT) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUV_420_SEMI_PLANNER_10_BIT_INDEX];
                }
                if (rga_info.output_format & IM_RGA_SUPPORT_FORMAT_YUV_420_PLANNER_8_BIT) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUV_420_PLANNER_8_BIT_INDEX];
                }
                if (rga_info.output_format & IM_RGA_SUPPORT_FORMAT_YUV_420_PLANNER_10_BIT) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUV_420_PLANNER_10_BIT_INDEX];
                }
                if (rga_info.output_format & IM_RGA_SUPPORT_FORMAT_YUV_422_SEMI_PLANNER_8_BIT) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUV_422_SEMI_PLANNER_8_BIT_INDEX];
                }
                if (rga_info.output_format & IM_RGA_SUPPORT_FORMAT_YUV_422_SEMI_PLANNER_10_BIT) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUV_422_SEMI_PLANNER_10_BIT_INDEX];
                }
                if (rga_info.output_format & IM_RGA_SUPPORT_FORMAT_YUV_422_PLANNER_8_BIT) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUV_422_PLANNER_8_BIT_INDEX];
                }
                if (rga_info.output_format & IM_RGA_SUPPORT_FORMAT_YUV_422_PLANNER_10_BIT) {
                    out << output_format[IM_RGA_SUPPORT_FORMAT_YUV_422_PLANNER_10_BIT_INDEX];
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
                if (rga_info.feature & IM_RGA_SUPPORT_FEATURE_FBC) {
                    out << feature[IM_RGA_SUPPORT_FEATURE_FBC_INDEX];
                }
                if (rga_info.feature & IM_RGA_SUPPORT_FEATURE_BLEND_YUV) {
                    out << feature[IM_RGA_SUPPORT_FEATURE_BLEND_YUV_INDEX];
                }
                if (rga_info.feature & IM_RGA_SUPPORT_FEATURE_BT2020) {
                    out << feature[IM_RGA_SUPPORT_FEATURE_BT2020_INDEX];
                }
                if (rga_info.feature & IM_RGA_SUPPORT_FEATURE_MOSAIC) {
                    out << feature[IM_RGA_SUPPORT_FEATURE_MOSAIC_INDEX];
                }
                if (rga_info.feature & IM_RGA_SUPPORT_FEATURE_OSD) {
                    out << feature[IM_RGA_SUPPORT_FEATURE_OSD_INDEX];
                }
                if (rga_info.feature & IM_RGA_SUPPORT_FEATURE_PRE_INTR) {
                    out << feature[IM_RGA_SUPPORT_FEATURE_PRE_INTR_INDEX];
                }
                out << std::endl;
                break;

            case RGA_EXPECTED:
                switch(rga_info.performance) {
                    case 1:
                        out << output_name[name] << performance[1] << std::endl;
                        break;

                    case 2:
                        out << output_name[name] << performance[2] << std::endl;
                        break;

                    case 4:
                        out << output_name[name] << performance[3] << std::endl;
                        break;

                    default:
                        out << output_name[name] << performance[IM_RGA_HW_VERSION_RGA_V_ERR_INDEX] << std::endl;
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
        } else if (all_output && (strcmp(info.c_str(), "0") > 0)) {
            name++;
        }
    } while (all_output);

    temp = info.c_str();
    return temp;
}

IM_STATUS imcheck_t(const rga_buffer_t src, const rga_buffer_t dst, const rga_buffer_t pat, const im_rect src_rect, const im_rect dst_rect, const im_rect pat_rect, int mode_usage)
{
    bool pat_enable = 0;
    rga_info_table_entry rga_info;
    IM_STATUS ret = IM_STATUS_NOERROR;

    memset(&rga_info, 0x0, sizeof(rga_info));
    ret = rga_get_info(&rga_info);
    if (IM_STATUS_FAILED == ret) {
        rga_error("rga im2d: rga2 get info failed");
        return IM_STATUS_FAILED;
    }

    ret = rga_check_driver();
    if (ret == IM_STATUS_ERROR_VERSION) {
        return ret;
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

        ret = rga_check_align("src", src, rga_info.byte_stride);
        if (ret != IM_STATUS_NOERROR) {
            return ret;
        }
    }

    if (pat_enable) {
        if (rga_info.version & (IM_RGA_HW_VERSION_RGA_1 | IM_RGA_HW_VERSION_RGA_1_PLUS)) {
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

        ret = rga_check_align("pat", pat, rga_info.byte_stride);
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

    ret = rga_check_align("dst", dst, rga_info.byte_stride);
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

IM_STATUS imresize(const rga_buffer_t src, rga_buffer_t dst, double fx, double fy, int interpolation, int sync, int *release_fence_fd)
{
    im_opt_t opt;
    im_rect srect;
    im_rect drect;
    im_rect prect;
    int usage = 0;
    rga_buffer_t pat;
    int width = 0, height = 0;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(NULL, NULL, &pat, &srect, &drect, &prect, &opt);

    if ((fx > 0) || (fy > 0)) {
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
                rga_error("imresize error, factor[fx,fy]=[%lf,%lf], ALIGN[dw,dh]=[%d,%d][%d,%d]", fx, fy, width, height, dst.width, dst.height);
                return ret;
            }
        }
    }

    UNUSED(interpolation);

    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    ret = improcess(src, dst, pat, srect, drect, prect, -1, release_fence_fd, &opt, usage);
    return ret;
}

IM_STATUS imcrop(const rga_buffer_t src, rga_buffer_t dst, im_rect rect, int sync, int *release_fence_fd)
{
    im_opt_t opt;
    int usage = 0;
    im_rect drect;
    im_rect prect;
    rga_buffer_t pat;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(NULL, NULL, &pat, NULL, &drect, &prect, &opt);

    drect.width = rect.width;
    drect.height = rect.height;

    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    ret = improcess(src, dst, pat, rect, drect, prect, -1, release_fence_fd, &opt, usage);
    return ret;
}

IM_STATUS imrotate(const rga_buffer_t src, rga_buffer_t dst, int rotation, int sync, int *release_fence_fd)
{
    im_opt_t opt;
    int usage = 0;
    im_rect srect;
    im_rect drect;
    im_rect prect;
    rga_buffer_t pat;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(NULL, NULL, &pat, &srect, &drect, &prect, &opt);

    usage |= rotation;
    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    ret = improcess(src, dst, pat, srect, drect, prect, -1, release_fence_fd, &opt, usage);
    return ret;
}

IM_STATUS imflip(const rga_buffer_t src, rga_buffer_t dst, int mode, int sync, int *release_fence_fd)
{
    im_opt_t opt;
    int usage = 0;
    im_rect srect;
    im_rect drect;
    im_rect prect;
    rga_buffer_t pat;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(NULL, NULL, &pat, &srect, &drect, &prect, &opt);

    usage |= mode;
    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    ret = improcess(src, dst, pat, srect, drect, prect, -1, release_fence_fd, &opt, usage);
    return ret;
}

IM_STATUS imfill(rga_buffer_t dst, im_rect rect, int color, int sync, int *release_fence_fd)
{
    im_opt_t opt;
    int usage = 0;
    im_rect srect;
    im_rect prect;
    rga_buffer_t pat;
    rga_buffer_t src;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(&src, NULL, &pat, &srect, NULL, &prect, &opt);

    memset(&src, 0, sizeof(rga_buffer_t));
    usage |= IM_COLOR_FILL;

    opt.color = color;

    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    ret = improcess(src, dst, pat, srect, rect, prect, -1, release_fence_fd, &opt, usage);
    return ret;
}

IM_STATUS impalette(rga_buffer_t src, rga_buffer_t dst, rga_buffer_t lut, int sync, int *release_fence_fd)
{
    im_opt_t opt;
    int usage = 0;
    im_rect srect;
    im_rect drect;
    im_rect prect;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(NULL, NULL, NULL, &srect, &drect, &prect, &opt);

    if ((src.width != dst.width) || (src.height != dst.height)) {
        imSetErrorMsg("The width and height of src and dst need to be equal, src[w,h] = [%d, %d], dst[w,h] = [%d, %d]", src.width, src.height, dst.width, dst.height);
        return IM_STATUS_INVALID_PARAM;
    }

    usage |= IM_COLOR_PALETTE;

    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    ret = improcess(src, dst, lut, srect, drect, prect, -1, release_fence_fd, &opt, usage);
    return ret;
}

IM_STATUS imtranslate(const rga_buffer_t src, rga_buffer_t dst, int x, int y, int sync, int *release_fence_fd)
{
    im_opt_t opt;
    int usage = 0;
    im_rect srect;
    im_rect drect;
    im_rect prect;
    rga_buffer_t pat;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(NULL, NULL, &pat, &srect, &drect, &prect, &opt);

    if ((src.width != dst.width) || (src.height != dst.height)) {
        imSetErrorMsg("The width and height of src and dst need to be equal, src[w,h] = [%d, %d], dst[w,h] = [%d, %d]", src.width, src.height, dst.width, dst.height);
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

    ret = improcess(src, dst, pat, srect, drect, prect, -1, release_fence_fd, &opt, usage);
    return ret;
}

IM_STATUS imcopy(const rga_buffer_t src, rga_buffer_t dst, int sync, int *release_fence_fd)
{
    im_opt_t opt;
    int usage = 0;
    im_rect srect;
    im_rect drect;
    im_rect prect;
    rga_buffer_t pat;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(NULL, NULL, &pat, &srect, &drect, &prect, &opt);

    if ((src.width != dst.width) || (src.height != dst.height)) {
        imSetErrorMsg("imcopy cannot support scale, src[w,h] = [%d, %d], dst[w,h] = [%d, %d]", src.width, src.height, dst.width, dst.height);
        return IM_STATUS_INVALID_PARAM;
    }

    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    ret = improcess(src, dst, pat, srect, drect, prect, -1, release_fence_fd, &opt, usage);
    return ret;
}

IM_STATUS imcolorkey(const rga_buffer_t src, rga_buffer_t dst, im_colorkey_range range, int mode, int sync, int *release_fence_fd)
{
    im_opt_t opt;
    int usage = 0;
    im_rect srect;
    im_rect drect;
    im_rect prect;
    rga_buffer_t pat;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(NULL, NULL, &pat, &srect, &drect, &prect, &opt);

    usage |= mode;
    opt.colorkey_range = range;

    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    ret = improcess(src, dst, pat, srect, drect, prect, -1, release_fence_fd, &opt, usage);
    return ret;
}

IM_STATUS imcomposite(const rga_buffer_t srcA, const rga_buffer_t srcB, rga_buffer_t dst, int mode, int sync, int *release_fence_fd)
{
    im_opt_t opt;
    int usage = 0;
    im_rect srect;
    im_rect drect;
    im_rect prect;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(NULL, NULL, NULL, &srect, &drect, &prect, &opt);

    usage |= mode;

    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    ret = improcess(srcA, dst, srcB, srect, drect, prect, -1, release_fence_fd, &opt, usage);
    return ret;
}

IM_STATUS imblend(const rga_buffer_t src, rga_buffer_t dst, int mode, int sync, int *release_fence_fd)
{
    rga_buffer_t pat;

    memset(&pat, 0x0, sizeof(pat));
    return imcomposite(src, pat, dst, mode, sync, release_fence_fd);
}

IM_STATUS imosd(const rga_buffer_t osd,const rga_buffer_t dst, const im_rect osd_rect, im_osd_t *osd_info, int sync, int *release_fence_fd)
{
    im_opt_t opt;
    int usage = 0;
    im_rect tmp_rect;

    memset(&opt, 0x0, sizeof(opt));
    memset(&tmp_rect, 0x0, sizeof(tmp_rect));

    opt.version = RGA_SET_CURRENT_API_VERISON;
    memcpy(&opt.osd_config, osd_info, sizeof(im_osd_t));

    usage |= IM_ALPHA_BLEND_DST_OVER | IM_OSD;
    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    return improcess(dst, dst, osd, osd_rect, osd_rect, tmp_rect, -1, release_fence_fd, &opt, usage);
}

IM_STATUS imcvtcolor(rga_buffer_t src, rga_buffer_t dst, int sfmt, int dfmt, int mode, int sync, int *release_fence_fd)
{
    im_opt_t opt;
    int usage = 0;
    im_rect srect;
    im_rect drect;
    im_rect prect;
    rga_buffer_t pat;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(NULL, NULL, &pat, &srect, &drect, &prect, &opt);

    src.format = sfmt;
    dst.format = dfmt;
    dst.color_space_mode = mode;

    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    ret = improcess(src, dst, pat, srect, drect, prect, -1, release_fence_fd, &opt, usage);
    return ret;
}

IM_STATUS imquantize(const rga_buffer_t src, rga_buffer_t dst, im_nn_t nn_info, int sync, int *release_fence_fd)
{
    im_opt_t opt;
    int usage = 0;
    im_rect srect;
    im_rect drect;
    im_rect prect;
    rga_buffer_t pat;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(NULL, NULL, &pat, &srect, &drect, &prect, &opt);

    usage |= IM_NN_QUANTIZE;
    opt.nn = nn_info;

    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    ret = improcess(src, dst, pat, srect, drect, prect, -1, release_fence_fd, &opt, usage);
    return ret;
}

IM_STATUS imrop(const rga_buffer_t src, rga_buffer_t dst, int rop_code, int sync, int *release_fence_fd)
{
    im_opt_t opt;
    int usage = 0;
    im_rect srect;
    im_rect drect;
    im_rect prect;
    rga_buffer_t pat;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(NULL, NULL, &pat, &srect, &drect, &prect, &opt);

    usage |= IM_ROP;
    opt.rop_code = rop_code;

    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    ret = improcess(src, dst, pat, srect, drect, prect, -1, release_fence_fd, &opt, usage);
    return ret;
}

IM_STATUS immosaic(const rga_buffer_t image, im_rect rect, int mosaic_mode, int sync, int *release_fence_fd)
{
    im_opt_t opt;
    int usage = 0;
    im_rect tmp_rect;
    rga_buffer_t tmp_image;
    IM_STATUS ret = IM_STATUS_NOERROR;

    memset(&opt, 0x0, sizeof(opt));
    memset(&tmp_image, 0x0, sizeof(tmp_image));
    memset(&tmp_rect, 0x0, sizeof(tmp_rect));

    usage |= IM_MOSAIC;

    opt.version = RGA_SET_CURRENT_API_VERISON;
    opt.mosaic_mode = mosaic_mode;

    if (sync == 0) {
        usage |= IM_ASYNC;
    } else if (sync == 1) {
        usage |= IM_SYNC;
    }

    return improcess(image, image, tmp_image, rect, rect, tmp_rect, -1, release_fence_fd, &opt, usage);
}

IM_STATUS improcess(rga_buffer_t src, rga_buffer_t dst, rga_buffer_t pat, im_rect srect, im_rect drect, im_rect prect, int usage)
{
    im_opt_t opt;
    int release_fence_fd;
    IM_STATUS ret = IM_STATUS_NOERROR;

    memset(&opt, 0, sizeof(opt));
    ret = improcess(src, dst, pat, srect, drect, prect, -1, &release_fence_fd, &opt, usage);

    return ret;
}

IM_STATUS improcess(rga_buffer_t src, rga_buffer_t dst, rga_buffer_t pat, im_rect srect, im_rect drect, im_rect prect, int acquire_fence_fd, int *release_fence_fd, im_opt_t *opt_ptr, int usage)
{
    return rga_single_task_submit(src, dst, pat, srect, drect, prect, acquire_fence_fd, release_fence_fd, opt_ptr, usage);
}

IM_STATUS improcess(im_job_id_t job_id, rga_buffer_t src, rga_buffer_t dst, rga_buffer_t pat, im_rect srect, im_rect drect, im_rect prect, im_opt_t *opt_ptr, int usage)
{
    return rga_task_submit(job_id, src, dst, pat, srect, drect, prect, opt_ptr, usage);
}

IM_STATUS improcess(rga_buffer_t src, rga_buffer_t dst, rga_buffer_t pat, im_rect srect, im_rect drect, im_rect prect, int acquire_fence_fd, int *release_fence_fd, im_opt_t *opt_ptr, int usage, im_ctx_id_t ctx_id) 
{
    int ret;
    int sync_mode;

    UNUSED(acquire_fence_fd);
    UNUSED(release_fence_fd);

    ret = rga_task_submit((im_job_id_t)ctx_id, src, dst, pat, srect, drect, prect, opt_ptr, usage);
    if (ret != IM_STATUS_SUCCESS) {
        return (IM_STATUS)ret;
    }

    if (usage & IM_ASYNC) {
        sync_mode = IM_ASYNC;
    } else {
        sync_mode = IM_SYNC;
    }

    return rga_job_config((im_job_id_t)ctx_id, sync_mode, acquire_fence_fd, release_fence_fd);
}

IM_STATUS imfill(im_job_id_t id, rga_buffer_t dst, im_rect rect, int color)
{
    im_opt_t opt;
    int usage = 0;
    im_rect srect;
    im_rect prect;
    rga_buffer_t pat;
    rga_buffer_t src;
    IM_STATUS ret = IM_STATUS_NOERROR;

    empty_structure(&src, NULL, &pat, &srect, NULL, &prect, &opt);
    memset(&src, 0x00, sizeof(rga_buffer_t));

    opt.color = color;
    usage = IM_SYNC | IM_COLOR_FILL;

    ret = improcess(id, src, dst, pat, srect, rect, prect, &opt, usage);
    return ret;
}

IM_STATUS imsync(int fence_fd)
{
    int ret = 0;

    ret = rga_sync_wait(fence_fd, -1);
    if (ret) {
        rga_error("Failed to wait for out fence = %d, ret = %d", fence_fd, ret);
        return IM_STATUS_FAILED;
    }

    close(fence_fd);
    return IM_STATUS_SUCCESS;
}

IM_STATUS imconfig(IM_CONFIG_NAME name, uint64_t value)
{

    switch (name) {
        case IM_CONFIG_SCHEDULER_CORE:
            if (value & IM_SCHEDULER_MASK) {
                g_im2d_context.core = (IM_SCHEDULER_CORE)value;
            } else {
                rga_error("IM2D: It's not legal rga_core, it needs to be a 'IM_SCHEDULER_CORE'.");
                return IM_STATUS_ILLEGAL_PARAM;
            }
            break;

        case IM_CONFIG_PRIORITY:
            if ((value > 0) && (value <= 6)) {
                g_im2d_context.priority = (int)value;
            } else {
                rga_error("IM2D: It's not legal priority, it needs to be a 'int', and it should be in the range of 0~6.");
                return IM_STATUS_ILLEGAL_PARAM;
            }
            break;

        case IM_CONFIG_CHECK:
            if ((value == false) || (value == true)) {
                g_im2d_context.check_mode = (bool)value;
            } else {
                rga_error("IM2D: It's not legal check config, it needs to be a 'bool'.");
                return IM_STATUS_ILLEGAL_PARAM;
            }
            break;

        default:
            rga_error("IM2D: Unsupported config name!");
            return IM_STATUS_NOT_SUPPORTED;
    }

    return IM_STATUS_SUCCESS;
}

im_job_id_t imbegin(uint32_t flags)
{
    return rga_job_creat(flags);
}

IM_STATUS imcancel(im_job_id_t id)
{
    return rga_job_cancel(id);
}

IM_STATUS imend(im_job_id_t job_id, int sync_mode, int acquire_fence_fd, int *release_fence_fd)
{
    return rga_job_submit(job_id, sync_mode, acquire_fence_fd, release_fence_fd);
}

IM_STATUS imresize_t(const rga_buffer_t src, rga_buffer_t dst, double fx, double fy, int interpolation, int sync)
{
    return imresize(src, dst, fx, fy, interpolation, sync, NULL);
}

IM_STATUS imcrop_t(const rga_buffer_t src, rga_buffer_t dst, im_rect rect, int sync)
{
    return imcrop(src, dst, rect, sync, NULL);
}

IM_STATUS imrotate_t(const rga_buffer_t src, rga_buffer_t dst, int rotation, int sync)
{
    return imrotate(src, dst, rotation, sync, NULL);
}

IM_STATUS imflip_t(const rga_buffer_t src, rga_buffer_t dst, int mode, int sync)
{
    return imflip(src, dst, mode, sync, NULL);
}

IM_STATUS imfill_t(rga_buffer_t dst, im_rect rect, int color, int sync)
{
    return imfill(dst, rect, color, sync, NULL);
}

IM_STATUS impalette_t(rga_buffer_t src, rga_buffer_t dst, rga_buffer_t lut, int sync)
{
    return impalette(src, dst, lut, sync, NULL);
}

IM_STATUS imtranslate_t(const rga_buffer_t src, rga_buffer_t dst, int x, int y, int sync)
{
    return imtranslate(src, dst, x, y, sync, NULL);
}

IM_STATUS imcopy_t(const rga_buffer_t src, rga_buffer_t dst, int sync)
{
    return imcopy(src, dst, sync, NULL);
}

IM_STATUS imcolorkey_t(const rga_buffer_t src, rga_buffer_t dst, im_colorkey_range range, int mode, int sync)
{
    return imcolorkey(src, dst, range, mode, sync, NULL);
}

IM_STATUS imblend_t(const rga_buffer_t srcA, const rga_buffer_t srcB, rga_buffer_t dst, int mode, int sync)
{
    return imcomposite(srcA, srcB, dst, mode, sync, NULL);
}

IM_STATUS imcvtcolor_t(rga_buffer_t src, rga_buffer_t dst, int sfmt, int dfmt, int mode, int sync)
{
    return imcvtcolor(src, dst, sfmt, dfmt, mode, sync, NULL);
}

IM_STATUS imquantize_t(const rga_buffer_t src, rga_buffer_t dst, im_nn_t nn_info, int sync)
{
    return imquantize(src, dst, nn_info, sync, NULL);
}

IM_STATUS imrop_t(const rga_buffer_t src, rga_buffer_t dst, int rop_code, int sync)
{
    return imrop(src, dst, rop_code, sync, NULL);
}

IM_STATUS immosaic(const rga_buffer_t image, im_rect rect, int mosaic_mode, int sync)
{
    return immosaic(image, rect, mosaic_mode, sync, NULL);
}

IM_STATUS imosd(const rga_buffer_t osd,const rga_buffer_t dst, const im_rect osd_rect, im_osd_t *osd_info, int sync)
{
    return imosd(osd, dst, osd_rect, osd_info, sync, NULL);
}
