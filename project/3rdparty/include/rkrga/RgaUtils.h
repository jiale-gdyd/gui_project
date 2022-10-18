#ifndef RKRGA_RGAUTILS_H
#define RKRGA_RGAUTILS_H

float get_bpp_from_format(int format);
const char *translate_format_str(int format);

int get_buf_from_file(void *buf, int f, int sw, int sh, int index);
int output_buf_data_to_file(void *buf, int f, int sw, int sh, int index);

#endif
