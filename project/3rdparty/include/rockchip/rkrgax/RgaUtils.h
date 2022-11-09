#ifndef ROCKCHIP_RKRGAX_RGA_UTILS_H
#define ROCKCHIP_RKRGAX_RGA_UTILS_H

float get_bpp_from_format(int format);
int get_perPixel_stride_from_format(int format);

int get_buf_from_file(void *buf, int f, int sw, int sh, int index);
int output_buf_data_to_file(void *buf, int f, int sw, int sh, int index);

const char *translate_format_str(int format);

int get_buf_from_file_FBC(void *buf, int f, int sw, int sh, int index);
int output_buf_data_to_file_FBC(void *buf, int f, int sw, int sh, int index);

#endif
