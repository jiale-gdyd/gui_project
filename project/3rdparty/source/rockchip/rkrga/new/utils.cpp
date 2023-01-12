#include <rockchip/rkrgax/rga.h>
#include <rockchip/rkrgax/utils.h>

static int get_compatible_format(int format)
{
    if (format == 0) {
        return format;
    }

    if ((format >> 8) != 0) {
        return format;
    } else {
        return format << 8;
    }

    return format;
}

int convert_to_rga_format(int ex_format)
{
    int fmt;

    if (is_drm_fourcc(ex_format)) {
        return get_format_from_drm_fourcc(ex_format);
    }

    ex_format = get_compatible_format(ex_format);
    if (is_rga_format(ex_format)) {
        return ex_format;
    }

    return RK_FORMAT_UNKNOWN;
}
