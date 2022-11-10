#ifndef LIBV4L_GET_MEDIA_DEVICES_H
#define LIBV4L_GET_MEDIA_DEVICES_H

#define GET_MEDIA_DEVICES_VERSION       0x0105

enum device_type {
    UNKNOWN         = 65535,
    NONE            = 65534,
    MEDIA_V4L_VIDEO = 0,
    MEDIA_V4L_VBI,
    MEDIA_V4L_RADIO,
    MEDIA_V4L_SWRADIO,
    MEDIA_V4L_TOUCH,
    MEDIA_V4L_SUBDEV,

    MEDIA_DVB_VIDEO = 100,
    MEDIA_DVB_AUDIO,
    MEDIA_DVB_SEC,
    MEDIA_DVB_FRONTEND,
    MEDIA_DVB_DEMUX,
    MEDIA_DVB_DVR,
    MEDIA_DVB_CA,
    MEDIA_DVB_NET,
    MEDIA_DVB_OSD,

    MEDIA_SND_CARD  = 200,
    MEDIA_SND_CAP,
    MEDIA_SND_OUT,
    MEDIA_SND_CONTROL,
    MEDIA_SND_HW,
    MEDIA_SND_TIMER,
    MEDIA_SND_SEQ,
};

enum bus_type {
    MEDIA_BUS_UNKNOWN,
    MEDIA_BUS_VIRTUAL,
    MEDIA_BUS_PCI,
    MEDIA_BUS_USB,
};

void *discover_media_devices(void);
void free_media_devices(void *opaque);

const char *media_device_type(const enum device_type type);
void display_media_devices(void *opaque);

const char *get_associated_device(void *opaque, const char *last_seek, const enum device_type desired_type, const char *seek_device, const enum device_type seek_type);
const char *fget_associated_device(void *opaque, const char *last_seek, const enum device_type desired_type, const int fd_seek_device, const enum device_type seek_type);

const char *get_not_associated_device(void *opaque, const char *last_seek, const enum device_type desired_type, const enum device_type not_desired_type);

#endif
