#ifndef LIBV4L_V4L2_COMMON_H
#define LIBV4L_V4L2_COMMON_H

#include <linux/types.h>

#if 1
#include <linux/v4l2-common.h>
#else
#define V4L2_SEL_TGT_CROP                           0x0000
#define V4L2_SEL_TGT_CROP_DEFAULT                   0x0001
#define V4L2_SEL_TGT_CROP_BOUNDS                    0x0002
#define V4L2_SEL_TGT_NATIVE_SIZE                    0x0003
#define V4L2_SEL_TGT_COMPOSE                        0x0100
#define V4L2_SEL_TGT_COMPOSE_DEFAULT                0x0101
#define V4L2_SEL_TGT_COMPOSE_BOUNDS                 0x0102
#define V4L2_SEL_TGT_COMPOSE_PADDED                 0x0103

#define V4L2_SEL_FLAG_GE                            (1 << 0)
#define V4L2_SEL_FLAG_LE                            (1 << 1)
#define V4L2_SEL_FLAG_KEEP_CONFIG                   (1 << 2)

struct v4l2_edid {
    __u32 pad;
    __u32 start_block;
    __u32 blocks;
    __u32 reserved[5];
    __u8  *edid;
};

#define V4L2_SEL_TGT_CROP_ACTIVE                    V4L2_SEL_TGT_CROP
#define V4L2_SEL_TGT_COMPOSE_ACTIVE                 V4L2_SEL_TGT_COMPOSE
#define V4L2_SUBDEV_SEL_TGT_CROP_ACTUAL             V4L2_SEL_TGT_CROP
#define V4L2_SUBDEV_SEL_TGT_COMPOSE_ACTUAL          V4L2_SEL_TGT_COMPOSE
#define V4L2_SUBDEV_SEL_TGT_CROP_BOUNDS             V4L2_SEL_TGT_CROP_BOUNDS
#define V4L2_SUBDEV_SEL_TGT_COMPOSE_BOUNDS          V4L2_SEL_TGT_COMPOSE_BOUNDS

#define V4L2_SUBDEV_SEL_FLAG_SIZE_GE                V4L2_SEL_FLAG_GE
#define V4L2_SUBDEV_SEL_FLAG_SIZE_LE                V4L2_SEL_FLAG_LE
#define V4L2_SUBDEV_SEL_FLAG_KEEP_CONFIG            V4L2_SEL_FLAG_KEEP_CONFIG
#endif

#endif
