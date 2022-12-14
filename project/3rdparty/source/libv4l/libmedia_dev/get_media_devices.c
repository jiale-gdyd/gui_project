#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <libv4l/get_media_devices.h>

#define DEVICE_STR              "devices"
#define ARRAY_SIZE(arr)         (sizeof(arr) / sizeof((arr)[0]))

struct media_device_entry {
    char             *device;
    char             *node;
    enum device_type type;
    enum bus_type    bus;
    unsigned         major, minor;
};

struct media_devices {
    struct media_device_entry *md_entry;
    unsigned int              md_size;
};

typedef int (*fill_data_t)(struct media_device_entry *md);

static void get_uevent_info(struct media_device_entry *md_ptr, char *dname)
{
    FILE *fd;
    char s[1024];
    char file[PATH_MAX + 8], *name, *p;

    snprintf(file, sizeof(file), "%s/%s/uevent", dname, md_ptr->node);
    fd = fopen(file, "r");
    if (!fd) {
        return;
    }

    while (fgets(s, sizeof(s), fd)) {
        p = strtok(s, "=");
        if (!p) {
            continue;
        }

        name = p;
        p = strtok(NULL, "\n");
        if (!p) {
            continue;
        }

        if (!strcmp(name, "MAJOR")) {
            md_ptr->major = atol(p);
        } else if (!strcmp(name, "MINOR")) {
            md_ptr->minor = atol(p);
        }
    }

    fclose(fd);
}

static enum bus_type get_bus(char *device)
{
    FILE *f;
    char s[1024];
    char file[PATH_MAX + 9];

    if (!strcmp(device, "/sys/devices/virtual")) {
        return MEDIA_BUS_VIRTUAL;
    }

    snprintf(file, sizeof(file), "%s/modalias", device);
    f = fopen(file, "r");
    if (!f) {
        return MEDIA_BUS_UNKNOWN;
    }

    if (!fgets(s, sizeof(s), f)) {
        return MEDIA_BUS_UNKNOWN;
    }
    fclose(f);

    if (!strncmp(s, "pci", 3)) {
        return MEDIA_BUS_PCI;
    }

    if (!strncmp(s, "usb", 3)) {
        return MEDIA_BUS_USB;
    }

    return MEDIA_BUS_UNKNOWN;
}

static int get_class(char *class, struct media_device_entry **md, unsigned int *md_size, fill_data_t fill)
{
    DIR *dir;
    int err = -2;
    char *p, *device;
    char virt_dev[60];
    enum bus_type bus;
    char link[PATH_MAX];
    struct dirent *entry;
    char dname[PATH_MAX];
    static int virtual = 0;
    struct media_device_entry *md_ptr = NULL;
    char fname[PATH_MAX + sizeof(entry->d_name)];

    snprintf(dname, PATH_MAX, "/sys/class/%s", class);
    dir = opendir(dname);
    if (!dir) {
        return 0;
    }

    for (entry = readdir(dir); entry; entry = readdir(dir)) {
        if (entry->d_name[0] == '.') {
            continue;
        }

        snprintf(fname, sizeof(fname), "%s/%s", dname, entry->d_name);
        if (realpath(fname, link)) {
            device = link;

            p = strstr(device, class);
            if (!p) {
                continue;
            }
            *(p - 1) = '\0';

            bus = get_bus(device);
            device += 13;

            switch (bus) {
                case MEDIA_BUS_PCI:
                    p = strrchr(device, '.');
                    if (!p) {
                        continue;
                    }
                    *p = '\0';
                    break;

                case MEDIA_BUS_USB:
                    p = strrchr(device, '/');
                    if (!p) {
                        continue;
                    }

                    if (!strchr(p, ':')) {
                        break;
                    }
                    *p = '\0';
                    break;

                case MEDIA_BUS_VIRTUAL:
                    sprintf(virt_dev, "virtual%d", virtual++);
                    device = virt_dev;
                    break;

                case MEDIA_BUS_UNKNOWN:
                    break;
            }

            *md = realloc(*md, (*md_size + 1) * sizeof(*md_ptr));
            if (!*md) {
                goto error;
            }
            md_ptr = (*md) + *md_size;
            (*md_size)++;

            memset(md_ptr, 0, sizeof(*md_ptr));
            md_ptr->type = UNKNOWN;
            md_ptr->device = strdup(device);
            md_ptr->node = strdup(entry->d_name);

            get_uevent_info(md_ptr, dname);
            fill(md_ptr);
        }
    }

    err = 0;

error:
    closedir(dir);
    return err;
}

static int add_v4l_class(struct media_device_entry *md)
{
    if (strstr(md->node, "video")) {
        md->type = MEDIA_V4L_VIDEO;
    } else if (strstr(md->node, "vbi")) {
        md->type = MEDIA_V4L_VBI;
    } else if (strstr(md->node, "swradio")) {
        md->type = MEDIA_V4L_SWRADIO;
    } else if (strstr(md->node, "radio")) {
        md->type = MEDIA_V4L_RADIO;
    } else if (strstr(md->node, "v4l-touch")) {
        md->type = MEDIA_V4L_TOUCH;
    } else if (strstr(md->node, "v4l-subdev")) {
        md->type = MEDIA_V4L_SUBDEV;
    }

    return 0;
}

static int add_snd_class(struct media_device_entry *md)
{
    char node[64];
    unsigned c = 65535, d = 65535;

    if (strstr(md->node, "timer")) {
        md->type = MEDIA_SND_TIMER;
        return 0;
    }

    if (strstr(md->node, "seq")) {
        md->type = MEDIA_SND_SEQ;
        return 0;
    }

    if (strstr(md->node, "card")) {
        sscanf(md->node, "card%u", &c);
        md->type = MEDIA_SND_CARD;
    } else if (strstr(md->node, "hw")) {
        sscanf(md->node, "hwC%uD%u", &c, &d);
        md->type = MEDIA_SND_HW;
    } else if (strstr(md->node, "control")) {
        sscanf(md->node, "controlC%u", &c);
        md->type = MEDIA_SND_CONTROL;
    } else if (strstr(md->node, "pcm")) {
        sscanf(md->node, "pcmC%uD%u", &c, &d);
        if (md->node[strlen(md->node) - 1] == 'p') {
            md->type = MEDIA_SND_OUT;
        } else if (md->node[strlen(md->node) - 1] == 'c') {
            md->type = MEDIA_SND_CAP;
        }
    }

    if (c == 65535) {
        return 0;
    }

    if (d == 65535) {
        snprintf(node, sizeof(node), "hw:%u", c);
    } else {
        snprintf(node, sizeof(node), "hw:%u,%u", c, d);
    }

    free(md->node);
    md->node = strdup(node);

    return 0;
}

static int add_dvb_class(struct media_device_entry *md)
{
    if (strstr(md->node, "video")) {
        md->type = MEDIA_DVB_VIDEO;
    }

    if (strstr(md->node, "audio")) {
        md->type = MEDIA_DVB_AUDIO;
    }

    if (strstr(md->node, "sec")) {
        md->type = MEDIA_DVB_SEC;
    }

    if (strstr(md->node, "frontend")) {
        md->type = MEDIA_DVB_FRONTEND;
    } else if (strstr(md->node, "demux")) {
        md->type = MEDIA_DVB_DEMUX;
    } else if (strstr(md->node, "dvr")) {
        md->type = MEDIA_DVB_DVR;
    } else if (strstr(md->node, "net")) {
        md->type = MEDIA_DVB_NET;
    } else if (strstr(md->node, "ca")) {
        md->type = MEDIA_DVB_CA;
    } else if (strstr(md->node, "osd")) {
        md->type = MEDIA_DVB_OSD;
    }

    return 0;
}

static int sort_media_device_entry(const void *a, const void *b)
{
    int cmp;
    const struct media_device_entry *md_a = a;
    const struct media_device_entry *md_b = b;

    cmp = strcmp(md_a->device, md_b->device);
    if (cmp) {
        return cmp;
    }

    cmp = (int)md_a->type - (int)md_b->type;
    if (cmp) {
        return cmp;
    }

    return strcmp(md_a->node, md_b->node);
}

void free_media_devices(void *opaque)
{
    int i;
    struct media_devices *md = opaque;
    struct media_device_entry *md_ptr = md->md_entry;

    for (i = 0; i < md->md_size; i++) {
        free(md_ptr->node);
        free(md_ptr->device);
        md_ptr++;
    }

    free(md->md_entry);
    free(md);
}

void *discover_media_devices(void)
{
    struct media_devices *md = NULL;
    struct media_device_entry *md_entry = NULL;

    md = calloc(1, sizeof(*md));
    if (!md) {
        return NULL;
    }

    md->md_size = 0;
    if (get_class("video4linux", &md_entry, &md->md_size, add_v4l_class)) {
        goto error;
    }

    if (get_class("sound", &md_entry, &md->md_size, add_snd_class)) {
        goto error;
    }

    if (get_class("dvb", &md_entry, &md->md_size, add_dvb_class)) {
        goto error;
    }

    if (!md_entry) {
        goto error;
    }

    qsort(md_entry, md->md_size, sizeof(*md_entry), sort_media_device_entry);
    md->md_entry = md_entry;

    return md;

error:
    free_media_devices(md);
    return NULL;
}

const char *media_device_type(enum device_type type)
{
    switch(type) {
        case MEDIA_V4L_VIDEO:
            return "video";

        case MEDIA_V4L_VBI:
            return "vbi";

        case MEDIA_V4L_RADIO:
            return "radio";

        case MEDIA_V4L_SWRADIO:
            return "swradio";

        case MEDIA_V4L_TOUCH:
            return "v4l-touch";

        case MEDIA_V4L_SUBDEV:
            return "v4l subdevice";

        case MEDIA_DVB_VIDEO:
            return "dvb video";

        case MEDIA_DVB_AUDIO:
            return "dvb audio";

        case MEDIA_DVB_SEC:
            return "dvb sec";

        case MEDIA_DVB_FRONTEND:
            return "dvb frontend";

        case MEDIA_DVB_DEMUX:
            return "dvb demux";

        case MEDIA_DVB_DVR:
            return "dvb dvr";

        case MEDIA_DVB_NET:
            return "dvb net";

        case MEDIA_DVB_CA:
            return "dvb conditional access";

        case MEDIA_DVB_OSD:
            return "dvb OSD";

        case MEDIA_SND_CARD:
            return "sound card";

        case MEDIA_SND_CAP:
            return "pcm capture";

        case MEDIA_SND_OUT:
            return "pcm output";

        case MEDIA_SND_CONTROL:
            return "mixer";

        case MEDIA_SND_HW:
            return "sound hardware";

        case MEDIA_SND_TIMER:
            return "sound timer";

        case MEDIA_SND_SEQ:
            return "sound sequencer";

        default:
            return "unknown";
    };
}

void display_media_devices(void *opaque)
{
    int i;
    char *prev = "";
    struct media_devices *md = opaque;
    struct media_device_entry *md_ptr = md->md_entry;

    for (i = 0; i < md->md_size; i++) {
        if (strcmp(prev, md_ptr->device)) {
            printf("\nDevice %s:\n\t", md_ptr->device);
            prev = md_ptr->device;
        }

        printf("%s(%s, dev %i:%i) ", md_ptr->node, media_device_type(md_ptr->type), md_ptr->major, md_ptr->minor);
        md_ptr++;
    }
    printf("\n");
}

const char *get_associated_device(void *opaque, const char *last_seek, const enum device_type desired_type, const char *seek_device, const enum device_type seek_type)
{
    char *prev, *p;
    int i, found = 0;
    struct media_devices *md = opaque;
    struct media_device_entry *md_ptr = md->md_entry;

    if ((seek_type != NONE) && seek_device[0]) {
        p = strrchr(seek_device, '/');
        if (p) {
            seek_device = p + 1;
        }

        for (i = 0; i < md->md_size; i++, md_ptr++) {
            if (last_seek && (md_ptr->type == seek_type) && !strcmp(md_ptr->node, last_seek)) {
                found = 1;
                continue;
            }

            if (last_seek && !found) {
                continue;
            }

            if ((md_ptr->type == seek_type) && !strcmp(seek_device, md_ptr->node)) {
                break;
            }
        }

        if (i == md->md_size) {
            return NULL;
        }

        i++;
        prev = md_ptr->device;
        md_ptr++;

        for (; i < md->md_size && !strcmp(prev, md_ptr->device); i++, md_ptr++) {
            if (last_seek && (md_ptr->type == seek_type) && !strcmp(md_ptr->node, last_seek)) {
                found = 1;
                continue;
            }

            if (last_seek && !found) {
                continue;
            }

            if (md_ptr->type == desired_type) {
                return md_ptr->node;
            }
        }
    } else {
        for (i = 0; i < md->md_size; i++, md_ptr++) {
            if (last_seek && !strcmp(md_ptr->node, last_seek)) {
                found = 1;
                continue;
            }

            if (last_seek && !found) {
                continue;
            }

            if (md_ptr->type == desired_type) {
                return md_ptr->node;
            }
        }
    }

    return NULL;
}

const char *fget_associated_device(void *opaque, const char *last_seek, const enum device_type desired_type, const int fd_seek_device, const enum device_type seek_type)
{
    char *prev;
    int i, found = 0;
    struct stat f_status;
    unsigned int dev_major, dev_minor;
    struct media_devices *md = opaque;
    struct media_device_entry *md_ptr = md->md_entry;

    if (fstat(fd_seek_device, &f_status)) {
        perror("Can't get file status");
        return NULL;
    }

    if (!S_ISCHR(f_status.st_mode)) {
        fprintf(stderr, "File descriptor is not a char device\n");
        return NULL;
    }

    dev_major = major(f_status.st_rdev);
    dev_minor = minor(f_status.st_rdev);

    for (i = 0; i < md->md_size; i++, md_ptr++) {
        if (last_seek
            && md_ptr->type == seek_type
            && md_ptr->major == dev_major
            && md_ptr->minor == dev_minor)
        {
            found = 1;
            continue;
        }

        if (last_seek && !found) {
            continue;
        }

        if (md_ptr->type == seek_type
            && md_ptr->major == dev_major
            && md_ptr->minor == dev_minor)
        {
            break;
        }
    }

    if (i == md->md_size) {
        return NULL;
    }

    i++;
    prev = md_ptr->device;
    md_ptr++;

    for (; i < md->md_size && !strcmp(prev, md_ptr->device); i++, md_ptr++) {
        if (last_seek && md_ptr->type == seek_type
            && md_ptr->major == dev_major
            && md_ptr->minor == dev_minor)
        {
            found = 1;
            continue;
        }

        if (last_seek && !found) {
            continue;
        }

        if (md_ptr->type == desired_type) {
            return md_ptr->node;
        }
    }

    return NULL;
}

const char *get_not_associated_device(void *opaque, const char *last_seek, const enum device_type desired_type, const enum device_type not_desired_type)
{
    int i, skip = 0, found = 0;
    char *prev = "", *result = NULL;
    struct media_devices *md = opaque;
    struct media_device_entry *md_ptr = md->md_entry;

    for (i = 0; i < md->md_size; i++, md_ptr++) {
        if (last_seek && !strcmp(md_ptr->node, last_seek)) {
            found = 1;
            continue;
        }

        if (last_seek && !found) {
            continue;
        }

        if (strcmp(prev, md_ptr->device)) {
            if (!skip && result) {
                break;
            }

            prev = md_ptr->device;
            skip = 0;
            result = NULL;
        }

        if (md_ptr->type == not_desired_type) {
            skip = 1;
        } else if (!skip && !result && (md_ptr->type == desired_type)) {
            result = md_ptr->node;
        }
    }

    if (skip) {
        result = NULL;
    }

    return result;
}
