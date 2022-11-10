#ifndef LIBV4L_LIBV4LCONTROL_PRIV_H
#define LIBV4L_LIBV4LCONTROL_PRIV_H

#include <libv4l/libv4l-plugin.h>

#define V4LCONTROL_SHM_SIZE                     4096
#define V4LCONTROL_SUPPORTS_NEXT_CTRL           0x01
#define V4LCONTROL_MEMORY_IS_MALLOCED           0x02

struct v4lcontrol_flags_info;

struct v4lcontrol_data {
    int                                fd;
    int                                bandwidth;
    int                                flags;
    int                                priv_flags;
    int                                controls;
    unsigned int                       *shm_values;
    unsigned int                       old_values[V4LCONTROL_COUNT];
    const struct v4lcontrol_flags_info *flags_info;
    void                               *dev_ops_priv;
    const struct libv4l_dev_ops        *dev_ops;
};

struct v4lcontrol_flags_info {
    unsigned short vendor_id;
    unsigned short product_id;
    unsigned short product_mask;
    const char     *dmi_board_vendor;
    const char     *dmi_board_name;
    int            flags;
    int            default_gamma;
    const char     *dmi_system_vendor;
    const char     *dmi_system_name;
    const char     *dmi_board_version;
    const char     *dmi_system_version;
};

struct v4lcontrol_usb_id {
    unsigned short vendor_id;
    unsigned short product_id;
};

struct v4lcontrol_upside_down_table {
    const char                     **board_vendor;
    const char                     **board_name;
    const struct v4lcontrol_usb_id *camera_id;
};

#endif
