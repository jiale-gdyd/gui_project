#ifndef LIBV4L_V4L2UTIL_H
#define LIBV4L_V4L2UTIL_H

struct v4l2_channel_list {
    const char *const name;
    unsigned          freq;
};

struct v4l2_channel_lists {
    const char *const                     name;
    const struct v4l2_channel_list *const list;
    unsigned                              count;
};

struct v4l2_country_std_map {
    const char *const iso_code;
    v4l2_std_id       std;
};

extern const struct v4l2_channel_lists v4l2_channel_lists[];
extern const struct v4l2_country_std_map v4l2_country_std_map[];

#endif
