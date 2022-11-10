#ifndef LIBV4L_LIBV4LPROCESSING_PRIV_H
#define LIBV4L_LIBV4LPROCESSING_PRIV_H

#include "../libv4lsyscall-priv.h"
#include "../control/libv4lcontrol.h"

#define V4L2PROCESSING_UPDATE_RATE      10

struct v4lprocessing_data {
    struct v4lcontrol_data *control;
    int                    fd;
    int                    do_process;
    int                    controls_changed;
    int                    lookup_table_active;
    int                    lookup_table_update_counter;
    unsigned char          comp1[256];
    unsigned char          green[256];
    unsigned char          comp2[256];
    int                    green_avg;
    int                    comp1_avg;
    int                    comp2_avg;
    int                    last_gamma;
    unsigned char          gamma_table[256];
    int                    last_gain_correction;
};

struct v4lprocessing_filter {
    int (*active)(struct v4lprocessing_data *data);
    int (*calculate_lookup_tables)(struct v4lprocessing_data *data, unsigned char *buf, const struct v4l2_format *fmt);
};

extern const struct v4lprocessing_filter gamma_filter;
extern const struct v4lprocessing_filter autogain_filter;
extern const struct v4lprocessing_filter whitebalance_filter;

#endif
