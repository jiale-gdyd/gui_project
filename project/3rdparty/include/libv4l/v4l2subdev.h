#ifndef LIBDRM_MEDIACTL_V4L2SUBDEV_H
#define LIBDRM_MEDIACTL_V4L2SUBDEV_H

#include "v4l2-subdev.h"

struct media_device;
struct media_entity;

int v4l2_subdev_open(struct media_entity *entity);
void v4l2_subdev_close(struct media_entity *entity);

int v4l2_subdev_get_format(struct media_entity *entity, struct v4l2_mbus_framefmt *format, unsigned int pad, enum v4l2_subdev_format_whence which);
int v4l2_subdev_set_format(struct media_entity *entity, struct v4l2_mbus_framefmt *format, unsigned int pad, enum v4l2_subdev_format_whence which);

int v4l2_subdev_get_selection(struct media_entity *entity, struct v4l2_rect *rect, unsigned int pad, unsigned int target, enum v4l2_subdev_format_whence which);
int v4l2_subdev_set_selection(struct media_entity *entity, struct v4l2_rect *rect, unsigned int pad, unsigned int target, enum v4l2_subdev_format_whence which);

int v4l2_subdev_query_dv_timings(struct media_entity *entity, struct v4l2_dv_timings *timings);
int v4l2_subdev_get_dv_timings_caps(struct media_entity *entity, struct v4l2_dv_timings_cap *caps);

int v4l2_subdev_get_dv_timings(struct media_entity *entity, struct v4l2_dv_timings *timings);
int v4l2_subdev_set_dv_timings(struct media_entity *entity, struct v4l2_dv_timings *timings);

int v4l2_subdev_set_frame_interval(struct media_entity *entity, struct v4l2_fract *interval, unsigned int pad);
int v4l2_subdev_get_frame_interval(struct media_entity *entity, struct v4l2_fract *interval, unsigned int pad);

int v4l2_subdev_parse_setup_formats(struct media_device *media, const char *p);

const char *v4l2_subdev_pixelcode_to_string(enum v4l2_mbus_pixelcode code);
enum v4l2_mbus_pixelcode v4l2_subdev_string_to_pixelcode(const char *string);

const char *v4l2_subdev_field_to_string(enum v4l2_field field);
enum v4l2_field v4l2_subdev_string_to_field(const char *string);

const char *v4l2_subdev_colorspace_to_string(enum v4l2_colorspace colorspace);
enum v4l2_colorspace v4l2_subdev_string_to_colorspace(const char *string);

const char *v4l2_subdev_xfer_func_to_string(enum v4l2_xfer_func xfer_func);
enum v4l2_xfer_func v4l2_subdev_string_to_xfer_func(const char *string);

const char *v4l2_subdev_ycbcr_encoding_to_string(enum v4l2_ycbcr_encoding ycbcr_enc);
enum v4l2_ycbcr_encoding v4l2_subdev_string_to_ycbcr_encoding(const char *string);

const char *v4l2_subdev_quantization_to_string(enum v4l2_quantization quantization);
enum v4l2_quantization v4l2_subdev_string_to_quantization(const char *string);

const enum v4l2_mbus_pixelcode *v4l2_subdev_pixelcode_list(unsigned int *length);

#endif
