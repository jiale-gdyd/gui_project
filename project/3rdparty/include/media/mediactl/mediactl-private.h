#ifndef LIBDRM_MEDIACTL_PRIVATE_H
#define LIBDRM_MEDIACTL_PRIVATE_H

#include "media.h"
#include "mediactl.h"

struct media_entity {
    struct media_device      *media;
    struct media_entity_desc info;
    struct media_pad         *pads;
    struct media_link        *links;
    unsigned int             max_links;
    unsigned int             num_links;
    char                     devname[32];
    int                      fd;
};

struct media_device {
    int                      fd;
    int                      refcount;
    char                     *devnode;

    struct media_device_info info;
    struct media_entity      *entities;
    unsigned int             entities_count;

    void (*debug_handler)(void *, ...);
    void *debug_priv;

    struct {
        struct media_entity *v4l;
        struct media_entity *fb;
        struct media_entity *alsa;
        struct media_entity *dvb;
    } def;
};

#define media_dbg(media, ...)           (media)->debug_handler((media)->debug_priv, __VA_ARGS__)

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array)               (sizeof(array) / sizeof((array)[0]))
#endif

#define FIELD_SIZEOF(t, f)              (sizeof(((t *)0)->f))

#ifndef mediactl_print
#define mediactl_print(msg, ...)        fprintf(stderr, msg, ##__VA_ARGS__);
#endif

#ifndef mediactl_error
#define mediactl_error(msg, ...)        mediactl_print("\033[1;31m[MEDIACTL][E]: " msg "\033[0m\n", ##__VA_ARGS__)
#endif

#ifndef mediactl_warn
#define mediactl_warn(msg, ...)         mediactl_print("\033[1;33m[MEDIACTL][W]: " msg "\033[0m\n", ##__VA_ARGS__)
#endif

#ifndef mediactl_info
#define mediactl_info(msg, ...)         mediactl_print("\033[1;32m[MEDIACTL][I]: " msg "\033[0m\n", ##__VA_ARGS__)
#endif

#ifndef mediactl_debug
#define mediactl_debug(msg, ...)        mediactl_print("\033[1;34m[MEDIACTL][D]: " msg "\033[0m\n", ##__VA_ARGS__)
#endif

void media_print_streampos(struct media_device *media, const char *p, const char *end);

#endif
