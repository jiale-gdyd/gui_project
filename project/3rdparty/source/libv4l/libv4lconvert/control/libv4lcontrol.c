#include <pwd.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <fnmatch.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <libv4l/videodev2.h>

#include "libv4lcontrol.h"
#include "libv4lcontrol-priv.h"
#include "../libv4lsyscall-priv.h"

#define ARRAY_SIZE(x)               ((int)sizeof(x) / (int)sizeof((x)[0]))

static const struct v4lcontrol_flags_info v4lcontrol_flags[] = {
    { 0x0471, 0x0325, 0, NULL, NULL, V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED },
    { 0x0471, 0x0326, 0, NULL, NULL, V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED },
    { 0x0471, 0x032d, 0, NULL, NULL, V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED },
    { 0x0471, 0x032e, 0, NULL, NULL, V4LCONTROL_VFLIPPED },
    { 0x093a, 0x2476, 0, NULL, NULL, V4LCONTROL_HFLIPPED|V4LCONTROL_VFLIPPED | V4LCONTROL_WANTS_WB, 1500 },
    { 0x0402, 0x5606, 0,
        "CLEVO CO.                       ",
        "M570TU                          ",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED },
    { 0x0402, 0x5606, 0, "AOpen", "i45GMt-HR R1.02 Dec.21.2009",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0,
        "RM plc", "RM ONE ECOQUIET 300" },
    { 0x0402, 0x5606, 0, "Intel Corporation", "DQ45CB",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0,
        "RM plc", "RM EXPERT 3040" },
    { 0x046d, 0x09b2, 0, "FUJITSU", "FJNB1C9",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0,
        "FUJITSU SIEMENS", "LIFEBOOK P7230" },
    { 0x046d, 0x09b2, 0, "FUJITSU", "FJNB1C9",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0,
        "FUJITSU", "LifeBook P7230" },
    { 0x04f2, 0xb012, 0, "Founder PC", "T14MF",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED },
    { 0x04f2, 0xb012, 0, "Packard Bell BV", "T32A      ",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED },
    { 0x04f2, 0xb012, 0, "PACKARD BELL BV              ", "EasyNote_BG45",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED },
    { 0x04f2, 0xb012, 0, "PACKARD BELL BV              ", "EasyNote_BG46",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED },
    { 0x04f2, 0xb012, 0, "PACKARD BELL BV              ", "EasyNote_BS45       ",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED },
    { 0x04f2, 0xb012, 0, "PACKARD BELL BV              ", "EasyNote_F0945",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED },
    { 0x04f2, 0xb012, 0, "ASUSTeK Computer Inc.        ", "F3Sc      ",
        0 },
    { 0x04f2, 0xb071, 0, "AXIOO", "PICO DJH Model",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED },
    { 0x04f2, 0xb071, 0, "PEGATRON CORPORATION", "H54",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED },
    { 0x04f2, 0xb071, 0, "PEGATRON CORP.", NULL,
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0,
        "Philco", "S\202rie PHN10050" },
    { 0x04f2, 0xb169, 0, "FUJITSU", "FJNB206",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0,
        "FUJITSU", "LifeBook T4410" },
    { 0x04f2, 0xb169, 0, "FUJITSU", "FJNB219",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0,
        "FUJITSU", "LIFEBOOK T730" },
    { 0x04f2, 0xb169, 0, "FUJITSU", "FJNB232",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0,
        "FUJITSU", "LIFEBOOK T731" },
    { 0x04f2, 0xb169, 0, "FUJITSU", "FJNB21A",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0,
        "FUJITSU", "LIFEBOOK TH700" },
    { 0x04f2, 0xb16b, 0, "ASUSTeK Computer Inc.        ", "U20A      ",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED },
    { 0x04f2, 0xb16b, 0, "To be filled by O.E.M.", "H34",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0,
        NULL, "H34" },
    { 0x04f2, 0xb186, 0, "FUJITSU", "FJNB206",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0,
        "FUJITSU SIEMENS", "LifeBook T4310" },
    { 0x04f2, 0xb186, 0, "FUJITSU", "FJNB206",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0,
        "FUJITSU", "LifeBook T4310" },
    { 0x04f2, 0xb213, 0, "FUJITSU", "FJNBB11",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0,
        "FUJITSU", "LIFEBOOK PH521" },
    { 0x04f2, 0xb213, 0, "FUJITSU", "FJNBB13",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0,
        "FUJITSU", "LIFEBOOK SH531" },
    { 0x04f2, 0xb213, 0, "FUJITSU", "FJNBB16",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0,
        "FUJITSU", "LIFEBOOK LH531" },
    { 0x04f2, 0xb213, 0, "FUJITSU", "FJNBB18",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0,
        "FUJITSU", "FMVN77ED" },
    { 0x04f2, 0xb213, 0, "FUJITSU", "FJNBB19",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0,
        "FUJITSU", "LIFEBOOK NH751" },
    { 0x04f2, 0xb217, 0, "LENOVO", "42982YG",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED },
    { 0x04f2, 0xb217, 0, "LENOVO", "42992QG",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED },
    { 0x04f2, 0xb27c, 0, "LENOVO", "12973MG",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0, NULL, NULL, NULL, 
        "ThinkPad Edge E325" },
    { 0x064e, 0xa111, 0, "Acer, Inc.", "Prespa1         ", 
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0,
        "Acer, inc.", "Aspire 5570     " },
    { 0x064e, 0xa116, 0, "To be filled by O.E.M.", "H34",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0,
        NULL, "H34" },
    { 0x064e, 0xa212, 0, "MEDIONAG", "WeTab ",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED },
    { 0x174f, 0x6a51, 0, NULL, "S96S",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0,
        "MicroLink", "S96S" },
    { 0x17ef, 0x480c, 0, "LENOVO", "7449C8G",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0, NULL, NULL, NULL,
        "ThinkPad X200 Tablet" },
    { 0x5986, 0x0200, 0, "LENOVO", "SPEEDY    ",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0, NULL, NULL, NULL,
        "Lenovo IdeaPad Y510" },
    { 0x5986, 0x0205, 0, "LENOVO", "Base Board Product Name",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0, NULL, NULL, NULL,
        "Lenovo IdeaPad U330" },
    { 0x5986, 0x0205, 0, "LENOVO", "Base Board Product Name",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED, 0, NULL, NULL, NULL,
        "Lenovo IdeaPad Y330" },
    { 0xeb1a, 0x2750, 0, "CLEVO", "D900K",
        V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED },
    { 0x0979, 0x0227, 0,    NULL, NULL, V4LCONTROL_WANTS_WB },
    { 0x0c45, 0x6011, 0,    NULL, NULL, 0, 1500 },
    { 0x0c45, 0x6019, 0,    NULL, NULL, 0, 1500 },
    { 0x0c45, 0x608f, 0,    NULL, NULL, 0, 1500 },
    { 0x0c45, 0x60b0, 0,    NULL, NULL, 0, 1500 },
    { 0x0c45, 0x6000, 0x1f, NULL, NULL, V4LCONTROL_WANTS_WB, 1500 },
    { 0x0c45, 0x6020, 0x0f, NULL, NULL, V4LCONTROL_WANTS_WB, 1500 },
    { 0x0c45, 0x60af, 0,    NULL, NULL, V4LCONTROL_WANTS_WB, 1500 },
    { 0x0c45, 0x60fe, 0,    NULL, NULL, V4LCONTROL_WANTS_WB },
    { 0x0c45, 0x610e, 0,    NULL, NULL, V4LCONTROL_WANTS_WB },
    { 0x0c45, 0x6128, 0,    NULL, NULL, V4LCONTROL_WANTS_WB },
    { 0x0c45, 0x612e, 0,    NULL, NULL, V4LCONTROL_WANTS_WB },
    { 0x0c45, 0x613e, 0,    NULL, NULL, V4LCONTROL_WANTS_WB },
    { 0x041e, 0x4028, 0,    NULL, NULL, V4LCONTROL_WANTS_WB, 1500 },
    { 0x093a, 0x2460, 0x1f, NULL, NULL, V4LCONTROL_WANTS_WB, 1500 },
    { 0x145f, 0x013a, 0,    NULL, NULL, V4LCONTROL_WANTS_WB, 1500 },
    { 0x2001, 0xf115, 0,    NULL, NULL, V4LCONTROL_WANTS_WB, 1500 },
    { 0x093a, 0x2620, 0x0f, NULL, NULL, V4LCONTROL_ROTATED_90_JPEG | V4LCONTROL_WANTS_WB, 1500 },
    { 0x06f8, 0x3009, 0,    NULL, NULL, V4LCONTROL_ROTATED_90_JPEG | V4LCONTROL_WANTS_WB, 1500 },
    { 0x06f8, 0x301b, 0,    NULL, NULL, V4LCONTROL_ROTATED_90_JPEG | V4LCONTROL_WANTS_WB, 1500 },
    { 0x145f, 0x013c, 0,    NULL, NULL, V4LCONTROL_ROTATED_90_JPEG | V4LCONTROL_WANTS_WB, 1500 },
    { 0x1ae7, 0x2001, 0,    NULL, NULL, V4LCONTROL_ROTATED_90_JPEG | V4LCONTROL_WANTS_WB, 1500 },
    { 0x093a, 0x2600, 0x0f, NULL, NULL, V4LCONTROL_WANTS_WB },
    { 0x2770, 0x9120, 0,    NULL, NULL, V4LCONTROL_WANTS_WB },
    { 0x041e, 0x403b, 0,    NULL, NULL, V4LCONTROL_WANTS_WB_AUTOGAIN },
    { 0x046d, 0x0928, 7,    NULL, NULL, V4LCONTROL_WANTS_WB_AUTOGAIN },
    { 0x046d, 0x0840, 0,    NULL, NULL, V4LCONTROL_WANTS_WB_AUTOGAIN },
    { 0x046d, 0x08f0, 0,    NULL, NULL, V4LCONTROL_WANTS_AUTOGAIN },
    { 0x046d, 0x08f5, 0,    NULL, NULL, V4LCONTROL_WANTS_AUTOGAIN },
    { 0x046d, 0x08f6, 0,    NULL, NULL, V4LCONTROL_WANTS_AUTOGAIN },
    { 0x046d, 0x08da, 0,    NULL, NULL, V4LCONTROL_WANTS_AUTOGAIN },
    { 0x08ca, 0x0111, 0,    NULL, NULL, V4LCONTROL_WANTS_WB_AUTOGAIN },
    { 0x093a, 0x010e, 1,    NULL, NULL, V4LCONTROL_WANTS_WB_AUTOGAIN },
    { 0x0553, 0x0202, 0,    NULL, NULL, V4LCONTROL_WANTS_WB },
    { 0x041e, 0x4007, 0,    NULL, NULL, V4LCONTROL_WANTS_WB },
    { 0x04c1, 0x009d, 0,    NULL, NULL, V4LCONTROL_WANTS_WB_AUTOGAIN, 1500 },
    { 0x0602, 0x1001, 0,    NULL, NULL, V4LCONTROL_WANTS_WB_AUTOGAIN, 1500 },
    { 0x03e8, 0x0004, 0,    NULL, NULL, V4LCONTROL_HFLIPPED | V4LCONTROL_WANTS_WB_AUTOGAIN },
    { 0x0471, 0x030b, 0,    NULL, NULL, V4LCONTROL_HFLIPPED | V4LCONTROL_WANTS_WB_AUTOGAIN },
    { 0x047d, 0x5001, 0,    NULL, NULL, V4LCONTROL_HFLIPPED | V4LCONTROL_WANTS_WB_AUTOGAIN },
    { 0x047d, 0x5002, 0,    NULL, NULL, V4LCONTROL_HFLIPPED | V4LCONTROL_WANTS_WB_AUTOGAIN },
    { 0x047d, 0x5003, 0,    NULL, NULL, V4LCONTROL_HFLIPPED | V4LCONTROL_WANTS_WB_AUTOGAIN },
    { 0x041e, 0x4003, 0,    NULL, NULL, V4LCONTROL_FORCE_TINYJPEG },
    { 0x1046, 0x9967, 0,    NULL, NULL, V4LCONTROL_FORCE_TINYJPEG },
};

static const struct v4l2_queryctrl fake_controls[V4LCONTROL_COUNT];

static const char *asus_board_vendor[] = {
    "ASUSTeK Computer Inc.",
    "ASUSTeK Computer INC.",
    "ASUS CORPORATION",
    "PEGATRON CORPORATION",
    NULL
};

static const char *asus_board_name[] = {
    "A3[A-Z]*", "A6R*", "A7J", "A7M", "A7Sn",
    "B50[A-Z]*",
    "F[23579][A-Z]*", "F70[A-Z]*", "F[58]2[A-Z]*",
    "G[12][A-Z]*", "G[57]0[A-Z]*",
    "K[4567]0[A-Z]*", "K[56]1[A-Z]*", "K52[A-Z]*", "K[45]3[A-Z]*",
    "N[12579]0[A-Z]*", "N[56]1[A-Z]*", "N82[A-Z]*", "N[47]3[A-Z]*",
    "P5[02][A-Z]*", "P81[A-Z]*",
    "U6[A-Z]*", "U[28]0[A-Z]*", "U3[1356][A-Z]*", "U5[23][A-Z]*",
    "UL[35]0[A-Z]*",
    "X55[A-Z]*", "X58[A-Z]*", "X71[A-Z]*",
    "900AX",
    "N5051Tp", "NX90Jq",
    "T101MT", "T14C",
    "VX3",
    "W7S*",
    NULL
};

static const struct v4lcontrol_usb_id asus_camera_id[] = {
    { 0x04f2, 0xb012 },
    { 0x04f2, 0xb034 },
    { 0x04f2, 0xb036 },
    { 0x04f2, 0xb071 },
    { 0x04f2, 0xb072 },
    { 0x04f2, 0xb106 },
    { 0x04f2, 0xb169 },
    { 0x04f2, 0xb16b },
    { 0x04f2, 0xb1b9 },
    { 0x04f2, 0xb1bb },
    { 0x04f2, 0xb1be },
    { 0x04f2, 0xb1e5 },
    { 0x064e, 0xa111 },
    { 0x064e, 0xa116 },
    { 0x064e, 0xa136 },
    { 0x090c, 0xe370 },
    { 0x13d3, 0x5094 },
    { 0x13d3, 0x5111 },
    { 0x13d3, 0x5120 },
    { 0x13d3, 0x5122 },
    { 0x13d3, 0x5126 },
    { 0x13d3, 0x5130 },
    { 0x13d3, 0x5702 },
    { 0x174f, 0x1120 },
    { 0x174f, 0x1408 },
    { 0x174f, 0x5a35 },
    { 0x174f, 0x8a31 },
    { 0x174f, 0xa311 },
    { 0x1d4d, 0x1002 },
    { 0x05e1, 0x0501 },
    { 0x0000, 0x0000 }
};

static const struct v4lcontrol_upside_down_table upside_down[] = {
    { asus_board_vendor, asus_board_name, asus_camera_id },
}; 

static void v4lcontrol_get_dmi_string(const char *sysfs_prefix, const char *string, char *buf, int size)
{
    FILE *f;
    char *s, sysfs_name[512];

    snprintf(sysfs_name, sizeof(sysfs_name), "%s/sys/class/dmi/id/%s", sysfs_prefix, string);
    f = fopen(sysfs_name, "r");
    if (!f) {
        snprintf(sysfs_name, sizeof(sysfs_name), "%s/sys/devices/virtual/dmi/id/%s", sysfs_prefix, string);
        f = fopen(sysfs_name, "r");
        if (!f) {
            buf[0] = 0;
            return;
        }
    }

    s = fgets(buf, size, f);
    if (s) {
        s[strlen(s) - 1] = 0;
    }

    fclose(f);
}

static int v4lcontrol_get_usb_info(struct v4lcontrol_data *data, const char *sysfs_prefix, unsigned short *vendor_id, unsigned short *product_id, int *speed)
{
    FILE *f;
    struct stat st;
    int i, minor_dev;
    char c, *s, buf[32];
    char sysfs_name[512];

    snprintf(sysfs_name, sizeof(sysfs_name), "%s/sys/class/video4linux", sysfs_prefix);
    if (stat(sysfs_name, &st) != 0) {
        return 0;
    }

    if (fstat(data->fd, &st) || !S_ISCHR(st.st_mode)) {
        return 0;
    }

    for (i = 0; i < 256; i++) {
        snprintf(sysfs_name, sizeof(sysfs_name), "%s/sys/class/video4linux/video%d/dev", sysfs_prefix, i);
        f = fopen(sysfs_name, "r");
        if (!f) {
            continue;
        }

        s = fgets(buf, sizeof(buf), f);
        fclose(f);

        if (s && (sscanf(buf, "%*d:%d%c", &minor_dev, &c) == 2) && (c == '\n') && (minor_dev == minor(st.st_rdev))) {
            break;
        }
    }

    if (i == 256) {
        return 0;
    }

    snprintf(sysfs_name, sizeof(sysfs_name), "%s/sys/class/video4linux/video%d/device/modalias", sysfs_prefix, i);
    f = fopen(sysfs_name, "r");
    if (f) {
        s = fgets(buf, sizeof(buf), f);
        fclose(f);

        if (!s || (sscanf(s, "usb:v%4hxp%4hx%c", vendor_id, product_id, &c) != 3) || (c != 'd')) {
            return 0;
        }

        snprintf(sysfs_name, sizeof(sysfs_name), "%s/sys/class/video4linux/video%d/device/../speed", sysfs_prefix, i);
    } else {
        snprintf(sysfs_name, sizeof(sysfs_name), "%s/sys/class/video4linux/video%d/device/idVendor", sysfs_prefix, i);
        f = fopen(sysfs_name, "r");
        if (!f) {
            return 0;
        }

        s = fgets(buf, sizeof(buf), f);
        fclose(f);

        if (!s || (sscanf(s, "%04hx%c", vendor_id, &c) != 2) || (c != '\n')) {
            return 0;
        }

        snprintf(sysfs_name, sizeof(sysfs_name), "%s/sys/class/video4linux/video%d/device/idProduct", sysfs_prefix, i);
        f = fopen(sysfs_name, "r");
        if (!f) {
            return 0;
        }

        s = fgets(buf, sizeof(buf), f);
        fclose(f);

        if (!s || (sscanf(s, "%04hx%c", product_id, &c) != 2) || (c != '\n')) {
            return 0;
        }

        snprintf(sysfs_name, sizeof(sysfs_name), "%s/sys/class/video4linux/video%d/device/speed", sysfs_prefix, i);
    }

    f = fopen(sysfs_name, "r");
    if (!f) {
        return 0;
    }

    s = fgets(buf, sizeof(buf), f);
    fclose(f);

    if (!s || (sscanf(s, "%d%c", speed, &c) != 2) || ((c != '\n') && (c != '.'))) {
        return 0;
    }

    return 1;
}

static int find_dmi_string(const char **table_entries, const char *dmi_value)
{
    size_t n;
    int found = 0;
    char *trimmed_dmi;
    const char **entry_ptr;
    const char *start = dmi_value;

    if (!start) {
        return 0;
    }

    while (isspace(*start)) {
        start++;
    }

    n = strlen(start);
    while ((n > 0) && isspace(start[n - 1])) {
        --n;
    }

    trimmed_dmi = strndup(start, n);

    for (entry_ptr = table_entries; *entry_ptr;  entry_ptr++) {
        found = fnmatch(*entry_ptr, trimmed_dmi, 0) == 0;
        if (found) {
            break;
        }
    }

    free(trimmed_dmi);
    return found;
}

static int find_usb_id(const struct v4lcontrol_usb_id *table_entries, unsigned short vendor_id, unsigned short product_id)
{
    const struct v4lcontrol_usb_id *entry_ptr;
    for (entry_ptr = table_entries; entry_ptr->vendor_id && entry_ptr->product_id; ++entry_ptr) {
        if ((entry_ptr->vendor_id == vendor_id) && (entry_ptr->product_id == product_id)) {
            return 1;
        }
    }

    return 0;
}

static void v4lcontrol_get_flags_from_db(struct v4lcontrol_data *data, const char *sysfs_prefix, unsigned short vendor_id, unsigned short product_id)
{
    int i;
    char dmi_board_vendor[512], dmi_board_name[512], dmi_board_version[512];
    char dmi_system_vendor[512], dmi_system_name[512], dmi_system_version[512];

    v4lcontrol_get_dmi_string(sysfs_prefix, "sys_vendor", dmi_system_vendor, sizeof(dmi_system_vendor));
    v4lcontrol_get_dmi_string(sysfs_prefix, "product_name", dmi_system_name, sizeof(dmi_system_name));
    v4lcontrol_get_dmi_string(sysfs_prefix, "product_version", dmi_system_version, sizeof(dmi_system_version));

    v4lcontrol_get_dmi_string(sysfs_prefix, "board_vendor", dmi_board_vendor, sizeof(dmi_board_vendor));
    v4lcontrol_get_dmi_string(sysfs_prefix, "board_name", dmi_board_name, sizeof(dmi_board_name));
    v4lcontrol_get_dmi_string(sysfs_prefix, "board_version", dmi_board_version, sizeof(dmi_board_version));

    for (i = 0; i < ARRAY_SIZE(v4lcontrol_flags); i++) {
        if (v4lcontrol_flags[i].vendor_id == vendor_id
            && v4lcontrol_flags[i].product_id == (product_id & ~v4lcontrol_flags[i].product_mask)
            && (v4lcontrol_flags[i].dmi_system_vendor == NULL
            || !strcmp(v4lcontrol_flags[i].dmi_system_vendor, dmi_system_vendor))
            && (v4lcontrol_flags[i].dmi_system_name == NULL
            || !strcmp(v4lcontrol_flags[i].dmi_system_name, dmi_system_name))
            && (v4lcontrol_flags[i].dmi_system_version == NULL
            || !strcmp(v4lcontrol_flags[i].dmi_system_version, dmi_system_version))
            && (v4lcontrol_flags[i].dmi_board_vendor == NULL
            || !strcmp(v4lcontrol_flags[i].dmi_board_vendor, dmi_board_vendor))
            && (v4lcontrol_flags[i].dmi_board_name == NULL
            || !strcmp(v4lcontrol_flags[i].dmi_board_name, dmi_board_name))
            && (v4lcontrol_flags[i].dmi_board_version == NULL
            || !strcmp(v4lcontrol_flags[i].dmi_board_version, dmi_board_version)))
        {
            data->flags |= v4lcontrol_flags[i].flags;
            data->flags_info = &v4lcontrol_flags[i];
            return;
        }
    }

    for (i = 0; i < ARRAY_SIZE(upside_down); i++) {
        if (find_dmi_string(upside_down[i].board_vendor, dmi_board_vendor)
            && find_dmi_string(upside_down[i].board_name, dmi_board_name)
            && find_usb_id(upside_down[i].camera_id, vendor_id, product_id))
        {
            data->flags |= V4LCONTROL_HFLIPPED | V4LCONTROL_VFLIPPED;
            break;
        }
    }
}

struct v4lcontrol_data *v4lcontrol_create(int fd, void *dev_ops_priv, const struct libv4l_dev_ops *dev_ops, int always_needs_conversion)
{
    int shm_fd;
    struct v4l2_input input;
    struct passwd pwd, *pwd_p;
    struct v4l2_capability cap;
    struct v4l2_queryctrl ctrl;
    unsigned short vendor_id = 0;
    unsigned short product_id = 0;
    char *s, shm_name[256], pwd_buf[1024];
    int i, rc, got_usb_info, speed, init = 0;
    struct v4lcontrol_data *data = calloc(1, sizeof(struct v4lcontrol_data));

    if (!data) {
        fprintf(stderr, "libv4lcontrol: error: out of memory!\n");
        return NULL;
    }

    data->fd = fd;
    data->dev_ops = dev_ops;
    data->dev_ops_priv = dev_ops_priv;

    if ((data->dev_ops->ioctl(data->dev_ops_priv, data->fd, VIDIOC_G_INPUT, &input.index) == 0)
        && (data->dev_ops->ioctl(data->dev_ops_priv, data->fd, VIDIOC_ENUMINPUT, &input) == 0))
    {
        if (input.status & V4L2_IN_ST_HFLIP) {
            data->flags |= V4LCONTROL_HFLIPPED;
        }

        if (input.status & V4L2_IN_ST_VFLIP) {
            data->flags |= V4LCONTROL_VFLIPPED;
        }
    }

    s = getenv("LIBV4LCONTROL_SYSFS_PREFIX");
    if (!s) {
        s = "";
    }

    got_usb_info = v4lcontrol_get_usb_info(data, s, &vendor_id, &product_id, &speed);
    if (got_usb_info) {
        v4lcontrol_get_flags_from_db(data, s, vendor_id, product_id);
        switch (speed) {
            case 12:
                data->bandwidth = 1023 * 1000;
                break;

            case 480:
                data->bandwidth = 3 * 1024 * 8000;
                break;

            case 5000:
                data->bandwidth = 48 * 1024 * 8000;
                break;

            default:
                data->bandwidth = speed / 20;
        }
    } else {
        data->bandwidth = 0;
    }

    s = getenv("LIBV4LCONTROL_FLAGS");
    if (s) {
        data->flags = strtol(s, NULL, 0);
    }

    ctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;
    if (data->dev_ops->ioctl(data->dev_ops_priv, data->fd, VIDIOC_QUERYCTRL, &ctrl) == 0) {
        data->priv_flags |= V4LCONTROL_SUPPORTS_NEXT_CTRL;
    }

    if (always_needs_conversion || v4lcontrol_needs_conversion(data)) {
        for (i = 0; i < V4LCONTROL_AUTO_ENABLE_COUNT; i++) {
            ctrl.id = fake_controls[i].id;
            rc = data->dev_ops->ioctl(data->dev_ops_priv, data->fd, VIDIOC_QUERYCTRL, &ctrl);
            if ((rc == -1) || ((rc == 0) && (ctrl.flags & V4L2_CTRL_FLAG_DISABLED))) {
                data->controls |= 1 << i;
            }
        }
    }

    while (data->flags & V4LCONTROL_WANTS_AUTOGAIN) {
        ctrl.id = V4L2_CID_AUTOGAIN;
        rc = data->dev_ops->ioctl(data->dev_ops_priv, data->fd, VIDIOC_QUERYCTRL, &ctrl);
        if ((rc == 0) && !(ctrl.flags & V4L2_CTRL_FLAG_DISABLED)) {
            break;
        }

        ctrl.id = V4L2_CID_EXPOSURE;
        rc = data->dev_ops->ioctl(data->dev_ops_priv, data->fd, VIDIOC_QUERYCTRL, &ctrl);
        if ((rc != 0) || (ctrl.flags & V4L2_CTRL_FLAG_DISABLED)) {
            break;
        }

        ctrl.id = V4L2_CID_GAIN;
        rc = data->dev_ops->ioctl(data->dev_ops_priv, data->fd, VIDIOC_QUERYCTRL, &ctrl);
        if ((rc != 0) || (ctrl.flags & V4L2_CTRL_FLAG_DISABLED)) {
            break;
        }

        data->controls |= 1 << V4LCONTROL_AUTOGAIN | 1 << V4LCONTROL_AUTOGAIN_TARGET;
        break;
    }

    s = getenv("LIBV4LCONTROL_CONTROLS");
    if (s) {
        data->controls = strtol(s, NULL, 0);
    }

    if (data->controls == 0) {
        return data;
    }

    if (data->dev_ops->ioctl(data->dev_ops_priv, fd, VIDIOC_QUERYCAP, &cap)) {
        perror("libv4lcontrol: error querying device capabilities");
        goto error;
    }

    if (getpwuid_r(geteuid(), &pwd, pwd_buf, sizeof(pwd_buf), &pwd_p) == 0) {
        if (got_usb_info) {
            snprintf(shm_name, 256, "/libv4l-%s:%s:%04x:%04x:%s", pwd.pw_name, cap.bus_info, (int)vendor_id, (int)product_id, cap.card);
        } else {
            snprintf(shm_name, 256, "/libv4l-%s:%s:%s", pwd.pw_name, cap.bus_info, cap.card);
        }
    } else {
        perror("libv4lcontrol: error getting username using uid instead");
        if (got_usb_info) {
            snprintf(shm_name, 256, "/libv4l-%lu:%s:%04x:%04x:%s", (unsigned long)geteuid(), cap.bus_info, (int)vendor_id, (int)product_id, cap.card);
        } else {
            snprintf(shm_name, 256, "/libv4l-%lu:%s:%s", (unsigned long)geteuid(), cap.bus_info, cap.card);
        }
    }

    for (i = 1; shm_name[i]; i++) {
        if (shm_name[i] == '/') {
            shm_name[i] = '-';
        }
    }

    shm_fd = shm_open(shm_name, (O_CREAT | O_EXCL | O_RDWR), (S_IREAD | S_IWRITE));
    if (shm_fd >= 0) {
        init = 1;
    } else {
        shm_fd = shm_open(shm_name, O_RDWR, (S_IREAD | S_IWRITE));
    }

    if (shm_fd >= 0) {
        int ret = ftruncate(shm_fd, V4LCONTROL_SHM_SIZE);
        if (ret) {
            perror("libv4lcontrol: shm ftruncate failed");
            close(shm_fd);
        } else {
            data->shm_values = mmap(NULL, V4LCONTROL_SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
            close(shm_fd);

            if (data->shm_values == MAP_FAILED) {
                perror("libv4lcontrol: shm mmap failed");
                data->shm_values = NULL;
            }
        }
    } else {
        perror("libv4lcontrol: error creating shm segment failed");
    }

    if (data->shm_values == NULL) {
        fprintf(stderr, "libv4lcontrol: falling back to malloc-ed memory for controls\n");
        data->shm_values = malloc(V4LCONTROL_SHM_SIZE);
        if (!data->shm_values) {
            fprintf(stderr, "libv4lcontrol: error: out of memory!\n");
            goto error;
        }

        init = 1;
        data->priv_flags |= V4LCONTROL_MEMORY_IS_MALLOCED;
    }

    if (init) {
        memset(data->shm_values, 0, V4LCONTROL_SHM_SIZE);

        for (i = 0; i < V4LCONTROL_COUNT; i++) {
            data->shm_values[i] = fake_controls[i].default_value;
        }

        if (data->flags & V4LCONTROL_WANTS_WB) {
            data->shm_values[V4LCONTROL_WHITEBALANCE] = 1;
        }

        if (data->flags_info && data->flags_info->default_gamma) {
            data->shm_values[V4LCONTROL_GAMMA] = data->flags_info->default_gamma;
        }
    }

    return data;

error:
    free(data);
    return NULL;
}

void v4lcontrol_destroy(struct v4lcontrol_data *data)
{
    if (data->controls) {
        if (data->priv_flags & V4LCONTROL_MEMORY_IS_MALLOCED) {
            free(data->shm_values);
        } else {
            munmap(data->shm_values, V4LCONTROL_SHM_SIZE);
        }
    }

    free(data);
}

static const struct v4l2_queryctrl fake_controls[V4LCONTROL_COUNT] = {
    {
        .id = V4L2_CID_AUTO_WHITE_BALANCE,
        .type = V4L2_CTRL_TYPE_BOOLEAN,
        .name =  "White Balance, Automatic",
        .minimum = 0,
        .maximum = 1,
        .step = 1,
        .default_value = 0,
        .flags = 0
    }, {
        .id = V4L2_CID_HFLIP,
        .type = V4L2_CTRL_TYPE_BOOLEAN,
        .name =  "Horizontal Flip",
        .minimum = 0,
        .maximum = 1,
        .step = 1,
        .default_value = 0,
        .flags = 0
    }, {
        .id = V4L2_CID_VFLIP,
        .type = V4L2_CTRL_TYPE_BOOLEAN,
        .name =  "Vertical Flip",
        .minimum = 0,
        .maximum = 1,
        .step = 1,
        .default_value = 0,
        .flags = 0
    }, {
        .id = V4L2_CID_GAMMA,
        .type = V4L2_CTRL_TYPE_INTEGER,
        .name =  "Gamma",
        .minimum = 500,
        .maximum = 3000,
        .step = 1,
        .default_value = 1000,
        .flags = V4L2_CTRL_FLAG_SLIDER
    }, {
    }, {
        .id = V4L2_CID_AUTOGAIN,
        .type = V4L2_CTRL_TYPE_BOOLEAN,
        .name =  "Gain, Automatic",
        .minimum = 0,
        .maximum = 1,
        .step = 1,
        .default_value = 1,
        .flags = 0
    }, {
        .id = V4L2_CTRL_CLASS_USER + 0x2000, /* FIXME */
        .type = V4L2_CTRL_TYPE_INTEGER,
        .name =  "Auto Gain Target",
        .minimum = 0,
        .maximum = 255,
        .step = 1,
        .default_value = 100,
        .flags = V4L2_CTRL_FLAG_SLIDER
    },
};

static void v4lcontrol_copy_queryctrl(struct v4lcontrol_data *data, struct v4l2_queryctrl *ctrl, int i)
{
    memcpy(ctrl, &fake_controls[i], sizeof(struct v4l2_queryctrl));

    if ((ctrl->id == V4L2_CID_AUTO_WHITE_BALANCE) && (data->flags & V4LCONTROL_WANTS_WB)) {
        ctrl->default_value = 1;
    }

    if ((ctrl->id == V4L2_CID_GAMMA) && data->flags_info && data->flags_info->default_gamma) {
        ctrl->default_value = data->flags_info->default_gamma;
    }
}

int v4lcontrol_vidioc_queryctrl(struct v4lcontrol_data *data, void *arg)
{
    int i;
    int retval;
    struct v4l2_queryctrl *ctrl = arg;
    uint32_t orig_id = ctrl->id;

    for (i = 0; i < V4LCONTROL_COUNT; i++) {
        if ((data->controls & (1 << i)) && (ctrl->id == fake_controls[i].id)) {
            v4lcontrol_copy_queryctrl(data, ctrl, i);
            return 0;
        }
    }

    retval = data->dev_ops->ioctl(data->dev_ops_priv, data->fd, VIDIOC_QUERYCTRL, arg);

    if ((data->priv_flags & V4LCONTROL_SUPPORTS_NEXT_CTRL) && (orig_id & V4L2_CTRL_FLAG_NEXT_CTRL)) {
        if (retval) {
            ctrl->id = V4L2_CTRL_ID_MASK;
        }

        for (i = 0; i < V4LCONTROL_COUNT; i++) {
            if ((data->controls & (1 << i)) && (fake_controls[i].id > (orig_id & ~V4L2_CTRL_FLAG_NEXT_CTRL)) && (fake_controls[i].id <= ctrl->id)) {
                v4lcontrol_copy_queryctrl(data, ctrl, i);
                retval = 0;
            }
        }
    }

    return retval;
}

int v4lcontrol_vidioc_g_ctrl(struct v4lcontrol_data *data, void *arg)
{
    int i;
    struct v4l2_control *ctrl = arg;

    for (i = 0; i < V4LCONTROL_COUNT; i++) {
        if ((data->controls & (1 << i)) && ctrl->id == fake_controls[i].id) {
            ctrl->value = data->shm_values[i];
            return 0;
        }
    }

    return data->dev_ops->ioctl(data->dev_ops_priv, data->fd, VIDIOC_G_CTRL, arg);
}

static void v4lcontrol_alloc_valid_controls(struct v4lcontrol_data *data, const struct v4l2_ext_controls *src, struct v4l2_ext_controls *dst)
{
    unsigned i, j;
    struct v4l2_ext_control *ctrl;

    *dst = *src;
    if (data->controls == 0) {
        return;
    }

    ctrl = malloc(src->count * sizeof(*ctrl));
    if (ctrl == NULL) {
        return;
    }

    dst->controls = ctrl;
    dst->count = 0;

    for (i = 0; i < src->count; i++) {
        for (j = 0; j < V4LCONTROL_COUNT; j++)
            if ((data->controls & (1 << j)) && (src->controls[i].id == fake_controls[j].id)) {
                break;
            }

        if (j == V4LCONTROL_COUNT) {
            ctrl[dst->count++] = src->controls[i];
        }
    }
}

static void v4lcontrol_free_valid_controls(struct v4lcontrol_data *data, struct v4l2_ext_controls *src, struct v4l2_ext_controls *dst)
{
    int inc_idx;
    unsigned i, j, k = 0;

    src->error_idx = dst->error_idx;
    if (dst->controls == src->controls) {
        return;
    }

    inc_idx = dst->error_idx < dst->count;
    for (i = 0; i < src->count; i++) {
        for (j = 0; j < V4LCONTROL_COUNT; j++) {
            if ((data->controls & (1 << j)) && (src->controls[i].id == fake_controls[j].id)) {
                break;
            }
        }

        if (j == V4LCONTROL_COUNT) {
            src->controls[i] = dst->controls[k++];
        } else if (inc_idx) {
            src->error_idx++;
        }
    }

    free(dst->controls);
}

int v4lcontrol_vidioc_g_ext_ctrls(struct v4lcontrol_data *data, void *arg)
{
    int res;
    int i, j;
    struct v4l2_ext_controls dst;
    struct v4l2_ext_controls *ctrls = arg;

    v4lcontrol_alloc_valid_controls(data, ctrls, &dst);
    res = data->dev_ops->ioctl(data->dev_ops_priv, data->fd, VIDIOC_G_EXT_CTRLS, &dst);
    v4lcontrol_free_valid_controls(data, ctrls, &dst);
    if (res) {
        return res;
    }

    for (i = 0; i < ctrls->count; i++) {
        for (j = 0; j < V4LCONTROL_COUNT; j++) {
            if ((data->controls & (1 << j)) && (ctrls->controls[i].id == fake_controls[j].id)) {
                ctrls->controls[i].value = data->shm_values[j];
                break;
            }
        }
    }

    return 0;
}

int v4lcontrol_vidioc_s_ctrl(struct v4lcontrol_data *data, void *arg)
{
    int i;
    struct v4l2_control *ctrl = arg;

    for (i = 0; i < V4LCONTROL_COUNT; i++) {
        if ((data->controls & (1 << i)) && (ctrl->id == fake_controls[i].id)) {
            if ((ctrl->value > fake_controls[i].maximum) || (ctrl->value < fake_controls[i].minimum)) {
                errno = EINVAL;
                return -1;
            }

            data->shm_values[i] = ctrl->value;
            return 0;
        }
    }

    return data->dev_ops->ioctl(data->dev_ops_priv, data->fd, VIDIOC_S_CTRL, arg);
}

static int v4lcontrol_validate_ext_ctrls(struct v4lcontrol_data *data, struct v4l2_ext_controls *ctrls)
{
    int i, j;

    if (data->controls == 0) {
        return 0;
    }

    for (i = 0; i < ctrls->count; i++) {
        for (j = 0; j < V4LCONTROL_COUNT; j++) {
            if ((data->controls & (1 << j)) && (ctrls->controls[i].id == fake_controls[j].id)) {
                if ((ctrls->controls[i].value > fake_controls[j].maximum) || (ctrls->controls[i].value < fake_controls[j].minimum)) {
                    ctrls->error_idx = i;
                    errno = EINVAL;
                    return -1;
                }
            }
        }
    }

    return 0;
}

int v4lcontrol_vidioc_try_ext_ctrls(struct v4lcontrol_data *data, void *arg)
{
    struct v4l2_ext_controls dst;
    struct v4l2_ext_controls *ctrls = arg;
    int res = v4lcontrol_validate_ext_ctrls(data, ctrls);

    if (res) {
        return res;
    }

    v4lcontrol_alloc_valid_controls(data, ctrls, &dst);
    res = data->dev_ops->ioctl(data->dev_ops_priv, data->fd, VIDIOC_TRY_EXT_CTRLS, &dst);
    v4lcontrol_free_valid_controls(data, ctrls, &dst);

    return res;
}

int v4lcontrol_vidioc_s_ext_ctrls(struct v4lcontrol_data *data, void *arg)
{
    int i, j;
    struct v4l2_ext_controls dst;
    struct v4l2_ext_controls *ctrls = arg;
    int res = v4lcontrol_validate_ext_ctrls(data, ctrls);

    if (res) {
        return res;
    }

    v4lcontrol_alloc_valid_controls(data, ctrls, &dst);
    res = data->dev_ops->ioctl(data->dev_ops_priv, data->fd, VIDIOC_S_EXT_CTRLS, &dst);
    v4lcontrol_free_valid_controls(data, ctrls, &dst);

    if (res) {
        return res;
    }

    for (i = 0; i < ctrls->count; i++) {
        for (j = 0; j < V4LCONTROL_COUNT; j++) {
            if ((data->controls & (1 << j)) && (ctrls->controls[i].id == fake_controls[j].id)) {
                data->shm_values[j] = ctrls->controls[i].value;
                break;
            }
        }
    }

    return 0;
}

int v4lcontrol_get_bandwidth(struct v4lcontrol_data *data)
{
    return data->bandwidth;
}

int v4lcontrol_get_flags(struct v4lcontrol_data *data)
{
    return data->flags;
}

int v4lcontrol_get_ctrl(struct v4lcontrol_data *data, int ctrl)
{
    if (data->controls & (1 << ctrl)) {
        if (((ctrl == V4LCONTROL_HFLIP) && (data->flags & V4LCONTROL_HFLIPPED)) || ((ctrl == V4LCONTROL_VFLIP) && (data->flags & V4LCONTROL_VFLIPPED))) {
            return !data->shm_values[ctrl];
        }

        return data->shm_values[ctrl];
    }

    return 0;
}

int v4lcontrol_controls_changed(struct v4lcontrol_data *data)
{
    int res;

    if (!data->controls) {
        return 0;
    }

    res = memcmp(data->shm_values, data->old_values, V4LCONTROL_COUNT * sizeof(unsigned int));
    memcpy(data->old_values, data->shm_values, V4LCONTROL_COUNT * sizeof(unsigned int));

    return res;
}

int v4lcontrol_needs_conversion(struct v4lcontrol_data *data)
{
    return data->flags || data->controls;
}
