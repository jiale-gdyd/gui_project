#include <time.h>
#include <math.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <linux/stddef.h>

#include <rockchip/rkrgax/rgadbg.h>
#include <rockchip/rkrgax/RgaUtils.h>
#include <rockchip/rkrgax/NormalRga.h>
#include <rockchip/rkrgax/RockchipRga.h>

struct format_table_entry {
    int        format;
    const char *name;
};

const struct format_table_entry format_table[] = {
    { RK_FORMAT_RGBA_8888,          "rgba8888" },
    { RK_FORMAT_RGBX_8888,          "rgbx8888" },
    { RK_FORMAT_RGB_888,            "rgb888" },
    { RK_FORMAT_RGB_565,            "rgb565" },
    { RK_FORMAT_RGBA_5551,          "rgba5551" },
    { RK_FORMAT_RGBA_4444,          "rgba4444" },
    { RK_FORMAT_BGRA_8888,          "bgra8888" },
    { RK_FORMAT_BGRX_8888,          "bgrx8888" },
    { RK_FORMAT_BGR_888,            "bgr888" },
    { RK_FORMAT_BGR_565,            "bgr565" },
    { RK_FORMAT_BGRA_5551,          "bgra5551" },
    { RK_FORMAT_BGRA_4444,          "bgra4444" },

    { RK_FORMAT_YCbCr_422_SP,       "cbcr422sp" },
    { RK_FORMAT_YCbCr_422_P,        "cbcr422p" },
    { RK_FORMAT_YCbCr_420_SP,       "nv12" },
    { RK_FORMAT_YCbCr_420_P,        "cbcr420p" },

    { RK_FORMAT_YCrCb_422_SP,       "crcb422sp" },
    { RK_FORMAT_YCrCb_422_P,        "crcb422p" },
    { RK_FORMAT_YCrCb_420_SP,       "crcb420sp" },
    { RK_FORMAT_YCrCb_420_P,        "crcb420p" },

    { RK_FORMAT_BPP1,               "bpp1" },
    { RK_FORMAT_BPP2,               "bpp2" },
    { RK_FORMAT_BPP4,               "bpp4" },
    { RK_FORMAT_BPP8,               "bpp8" },

    { RK_FORMAT_Y4,                 "y4" },
    { RK_FORMAT_YCbCr_400,          "cbcr400" },

    { RK_FORMAT_BGRX_8888,          "bgrx8888" },

    { RK_FORMAT_YVYU_422,           "yvyu422" },
    { RK_FORMAT_YVYU_420,           "yvyuv420" },
    { RK_FORMAT_VYUY_422,           "vyuy422" },
    { RK_FORMAT_VYUY_420,           "vyuy420" },

    { RK_FORMAT_YUYV_422,           "yuyv422" },
    { RK_FORMAT_YUYV_420,           "yuyv420" },
    { RK_FORMAT_UYVY_422,           "uyvy422" },
    { RK_FORMAT_UYVY_420,           "uyvy420" },

    { RK_FORMAT_YCbCr_420_SP_10B,   "nv12_10" },
    { RK_FORMAT_YCrCb_420_SP_10B,   "crcb420sp_10" },
    { RK_FORMAT_YCbCr_422_10b_SP,   "cbcr422_10b" },
    { RK_FORMAT_YCrCb_422_10b_SP,   "crcb422_10b" },

    { RK_FORMAT_BGR_565,            "bgr565" },
    { RK_FORMAT_BGRA_5551,          "bgra5551" },
    { RK_FORMAT_BGRA_4444,          "bgra4444" },

    { RK_FORMAT_ARGB_8888,          "argb8888" },
    { RK_FORMAT_XRGB_8888,          "xrgb8888" },
    { RK_FORMAT_ARGB_5551,          "argb5551" },
    { RK_FORMAT_ARGB_4444,          "argb4444" },
    { RK_FORMAT_ABGR_8888,          "abgr8888" },
    { RK_FORMAT_XBGR_8888,          "xbgr8888" },
    { RK_FORMAT_ABGR_5551,          "abgr5551" },
    { RK_FORMAT_ABGR_4444,          "abgr4444" },

    { RK_FORMAT_RGBA2BPP,           "rgba2bpp" },

    { RK_FORMAT_UNKNOWN,            "unknown" }
};

const char *translate_format_str(int format)
{
    format = RkRgaGetRgaFormat(format);

    for (size_t i = 0; i < (sizeof(format_table) / sizeof(format_table[0])); i++) {
        if (format_table[i].format == format) {
            return format_table[i].name;
        }
    }

    return "unknown";
}

int get_string_by_format(char *value, int format)
{
    const char *name = NULL;

    if (!value) {
        return -EINVAL;
    }

    name = translate_format_str(format);
    if (strcmp(name, "unknown") != 0) {
        memcpy(value, name, strlen(name) + 1);
    } else {
        rga_error("Is unsupport format now, please fix");
        return -EINVAL;
    }

    return 0;
}

float get_bpp_from_format(int format)
{
    float bpp = 0;

    if (!((format & 0xFF00) || (format == 0))) {
        format = RkRgaCompatibleFormat(format);
    }

    switch (format) {
        case RK_FORMAT_RGBA2BPP:
            return 0.25;

        case RK_FORMAT_Y4:
            bpp = 0.5;
            break;

        case RK_FORMAT_BPP1:
        case RK_FORMAT_BPP2:
        case RK_FORMAT_BPP4:
        case RK_FORMAT_BPP8:
        case RK_FORMAT_YCbCr_400:
            bpp = 1;
            break;

        case RK_FORMAT_YCbCr_420_SP:
        case RK_FORMAT_YCbCr_420_P:
        case RK_FORMAT_YCrCb_420_P:
        case RK_FORMAT_YCrCb_420_SP:
            bpp = 1.5;
            break;

        case RK_FORMAT_RGB_565:
        case RK_FORMAT_RGBA_5551:
        case RK_FORMAT_RGBA_4444:
        case RK_FORMAT_BGR_565:
        case RK_FORMAT_BGRA_5551:
        case RK_FORMAT_BGRA_4444:
        case RK_FORMAT_ARGB_5551:
        case RK_FORMAT_ARGB_4444:
        case RK_FORMAT_ABGR_5551:
        case RK_FORMAT_ABGR_4444:
        case RK_FORMAT_YCbCr_422_SP:
        case RK_FORMAT_YCbCr_422_P:
        case RK_FORMAT_YCrCb_422_SP:
        case RK_FORMAT_YCrCb_422_P:
        case RK_FORMAT_YVYU_422:
        case RK_FORMAT_VYUY_422:
        case RK_FORMAT_YUYV_422:
        case RK_FORMAT_UYVY_422:
        case RK_FORMAT_YVYU_420:
        case RK_FORMAT_VYUY_420:
        case RK_FORMAT_YUYV_420:
        case RK_FORMAT_UYVY_420:
            bpp = 2;
            break;

        case RK_FORMAT_YCbCr_420_SP_10B:
        case RK_FORMAT_YCrCb_420_SP_10B:
            bpp = 2;
            break;

        case RK_FORMAT_YCbCr_422_10b_SP:
        case RK_FORMAT_YCrCb_422_10b_SP:
            bpp = 2.5;
            break;

        case RK_FORMAT_BGR_888:
        case RK_FORMAT_RGB_888:
            bpp = 3;
            break;

        case RK_FORMAT_RGBA_8888:
        case RK_FORMAT_RGBX_8888:
        case RK_FORMAT_BGRA_8888:
        case RK_FORMAT_BGRX_8888:
        case RK_FORMAT_ARGB_8888:
        case RK_FORMAT_XRGB_8888:
        case RK_FORMAT_ABGR_8888:
        case RK_FORMAT_XBGR_8888:
            bpp = 4;
            break;

        default:
            rga_error("Is unsupport format now, please fix");
            return 0;
    }

    return bpp;
}

int get_perPixel_stride_from_format(int format)
{
    if (!((format & 0xFF00) || (format == 0))) {
        format = RkRgaCompatibleFormat(format);
    }

    switch (format) {
        case RK_FORMAT_RGBA2BPP:
            return 2;

        case RK_FORMAT_Y4:
            return (0.5 * 8);

        case RK_FORMAT_BPP1:
        case RK_FORMAT_BPP2:
        case RK_FORMAT_BPP4:
        case RK_FORMAT_BPP8:
        case RK_FORMAT_YCbCr_400:
        case RK_FORMAT_YCbCr_420_SP:
        case RK_FORMAT_YCbCr_420_P:
        case RK_FORMAT_YCrCb_420_P:
        case RK_FORMAT_YCrCb_420_SP:
        case RK_FORMAT_YCbCr_422_SP:
        case RK_FORMAT_YCbCr_422_P:
        case RK_FORMAT_YCrCb_422_SP:
        case RK_FORMAT_YCrCb_422_P:
            return (1 * 8);

        case RK_FORMAT_YCbCr_420_SP_10B:
        case RK_FORMAT_YCrCb_420_SP_10B:
        case RK_FORMAT_YCbCr_422_10b_SP:
        case RK_FORMAT_YCrCb_422_10b_SP:
            return (1 * 10);

        case RK_FORMAT_RGB_565:
        case RK_FORMAT_RGBA_5551:
        case RK_FORMAT_RGBA_4444:
        case RK_FORMAT_BGR_565:
        case RK_FORMAT_BGRA_5551:
        case RK_FORMAT_BGRA_4444:
        case RK_FORMAT_ARGB_5551:
        case RK_FORMAT_ARGB_4444:
        case RK_FORMAT_ABGR_5551:
        case RK_FORMAT_ABGR_4444:
        case RK_FORMAT_YVYU_422:
        case RK_FORMAT_VYUY_422:
        case RK_FORMAT_YUYV_422:
        case RK_FORMAT_UYVY_422:
        case RK_FORMAT_YVYU_420:
        case RK_FORMAT_VYUY_420:
        case RK_FORMAT_YUYV_420:
        case RK_FORMAT_UYVY_420:
            return (2 * 8);

        case RK_FORMAT_BGR_888:
        case RK_FORMAT_RGB_888:
            return (3 * 8);

        case RK_FORMAT_RGBA_8888:
        case RK_FORMAT_RGBX_8888:
        case RK_FORMAT_BGRA_8888:
        case RK_FORMAT_BGRX_8888:
        case RK_FORMAT_ARGB_8888:
        case RK_FORMAT_XRGB_8888:
        case RK_FORMAT_ABGR_8888:
        case RK_FORMAT_XBGR_8888:
            return (4 * 8);

        default:
            rga_error("Is unsupport format now, please fix");
            return 0;
    }
}

int get_buf_size_by_w_h_f(int w, int h, int f)
{
    int size = 0;
    float bpp = get_bpp_from_format(f);

    size = (int)w * h * bpp;
    return size;
}

int get_buf_from_file(void *buf, int f, int sw, int sh, int index)
{
    int ret = 0;
    char filePath[100];
    const char *inputFilePath = "/userdata/in%dw%d-h%d-%s.bin";

    snprintf(filePath, 100, inputFilePath, index, sw, sh, translate_format_str(f));

    FILE *file = fopen(filePath, "rb");
    if (!file) {
        rga_error("Could not open %s", filePath);
        return -EINVAL;
    }

    fread(buf, get_buf_size_by_w_h_f(sw, sh, f), 1, file);
    fclose(file);

    return 0;
}

int get_buf_from_file_FBC(void *buf, int f, int sw, int sh, int index)
{
    int ret = 0;
    char fstring[30];
    char filePath[100];
    const char *inputFilePath = "/userdata/in%dw%d-h%d-%s-afbc.bin";

    ret = get_string_by_format(fstring, f);
    snprintf(filePath, 100, inputFilePath, index, sw, sh, fstring);

    FILE *file = fopen(filePath, "rb");
    if (!file) {
        rga_error("Could not open %s", filePath);
        return -EINVAL;
    }

    fread(buf, get_buf_size_by_w_h_f(sw, sh, f) * 1.5, 1, file);
    fclose(file);

    return 0;
}

int output_buf_data_to_file(void *buf, int f, int sw, int sh, int index)
{
    int ret = 0;
    char filePath[100];
    const char *outputFilePath = "/userdata/out%dw%d-h%d-%s.bin";

    snprintf(filePath, 100, outputFilePath, index, sw, sh, translate_format_str(f));

    FILE *file = fopen(filePath, "wb+");
    if (!file) {
        rga_error("Could not open %s", filePath);
        return false;
    } else {
        rga_info("open %s and write ok", filePath);
    }

    fwrite(buf, get_buf_size_by_w_h_f(sw, sh, f), 1, file);
    fclose(file);

    return 0;
}

int output_buf_data_to_file_FBC(void *buf, int f, int sw, int sh, int index)
{
    int ret = 0;
    char fstring[30];
    char filePath[100];
    const char *outputFilePath = "/userdata/out%dw%d-h%d-%s-afbc.bin";

    ret = get_string_by_format(fstring, f);
    snprintf(filePath, 100, outputFilePath, index, sw, sh, fstring);

    FILE *file = fopen(filePath, "wb+");
    if (!file) {
        rga_error("Could not open %s", filePath);
        return false;
    } else {
        rga_info("open %s and write ok", filePath);
    }

    fwrite(buf, get_buf_size_by_w_h_f(sw, sh, f) * 1.5, 1, file);
    fclose(file);

    return 0;
}
