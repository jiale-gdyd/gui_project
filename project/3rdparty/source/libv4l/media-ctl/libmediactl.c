#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/sysmacros.h>

#include <linux/media.h>
#include <linux/videodev2.h>

#include <libv4l/mediactl.h>
#include <libv4l/v4l2subdev.h>
#include <libv4l/mediactl-private.h>

struct media_pad *media_entity_remote_source(struct media_pad *pad)
{
    unsigned int i;

    if (!(pad->flags & MEDIA_PAD_FL_SINK)) {
        return NULL;
    }

    for (i = 0; i < pad->entity->num_links; ++i) {
        struct media_link *link = &pad->entity->links[i];

        if (!(link->flags & MEDIA_LNK_FL_ENABLED)) {
            continue;
        }

        if (link->sink == pad) {
            return link->source;
        }
    }

    return NULL;
}

struct media_entity *media_get_entity_by_name(struct media_device *media, const char *name)
{
    unsigned int i;

    for (i = 0; i < media->entities_count; ++i) {
        struct media_entity *entity = &media->entities[i];

        if (strcmp(entity->info.name, name) == 0) {
            return entity;
        }
    }

    mediactl_error("media get entity by name:[%s] is null", name);
    return NULL;
}

struct media_entity *media_get_entity_by_id(struct media_device *media, __u32 id)
{
    unsigned int i;
    bool next = id & MEDIA_ENT_ID_FLAG_NEXT;

    id &= ~MEDIA_ENT_ID_FLAG_NEXT;

    for (i = 0; i < media->entities_count; ++i) {
        struct media_entity *entity = &media->entities[i];
        if (((entity->info.id == id) && !next) || ((entity->info.id > id) && next)) {
            return entity;
        }
    }

    return NULL;
}

unsigned int media_get_entities_count(struct media_device *media)
{
    return media->entities_count;
}

struct media_entity *media_get_entity(struct media_device *media, unsigned int index)
{
    if (index >= media->entities_count) {
        return NULL;
    }

    return &media->entities[index];
}

const struct media_pad *media_entity_get_pad(struct media_entity *entity, unsigned int index)
{
    if (index >= entity->info.pads) {
        return NULL;
    }

    return &entity->pads[index];
}

unsigned int media_entity_get_links_count(struct media_entity *entity)
{
    return entity->num_links;
}

const struct media_link *media_entity_get_link(struct media_entity *entity, unsigned int index)
{
    if (index >= entity->num_links) {
        return NULL;
    }

    return &entity->links[index];
}

const char *media_entity_get_devname(struct media_entity *entity)
{
    return entity->devname[0] ? entity->devname : NULL;
}

struct media_entity *media_get_default_entity(struct media_device *media, unsigned int type)
{
    switch (type) {
        case MEDIA_ENT_T_DEVNODE_V4L:
            return media->def.v4l;

        case MEDIA_ENT_T_DEVNODE_FB:
            return media->def.fb;

        case MEDIA_ENT_T_DEVNODE_ALSA:
            return media->def.alsa;

        case MEDIA_ENT_T_DEVNODE_DVB:
            return media->def.dvb;
    }

    return NULL;
}

const struct media_device_info *media_get_info(struct media_device *media)
{
    return &media->info;
}

const char *media_get_devnode(struct media_device *media)
{
    return media->devnode;
}

const struct media_entity_desc *media_entity_get_info(struct media_entity *entity)
{
    return &entity->info;
}

static int media_device_open(struct media_device *media)
{
    int ret;

    if (media->fd != -1) {
        return 0;
    }

    media_dbg(media, "Opening media device:[%s]\n", media->devnode);
    mediactl_debug("Opening media device:[%s]", media->devnode);

    media->fd = open(media->devnode, O_RDWR);
    if (media->fd < 0) {
        ret = -errno;
        media_dbg(media, "Can't open media device:[%s]\n", media->devnode);
        mediactl_error("Can't open media device:[%s]", media->devnode);
        return ret;
    }

    return 0;
}

static void media_device_close(struct media_device *media)
{
    if (media->fd != -1) {
        close(media->fd);
        media->fd = -1;
    }
}

int media_setup_link(struct media_device *media, struct media_pad *source, struct media_pad *sink, __u32 flags)
{
    int ret;
    unsigned int i;
    struct media_link *link;
    struct media_link_desc ulink = { { 0 } };

    ret = media_device_open(media);
    if (ret < 0) {
        goto done;
    }

    for (i = 0; i < source->entity->num_links; i++) {
        link = &source->entity->links[i];

        if ((link->source->entity == source->entity)
            && (link->source->index == source->index)
            && (link->sink->entity == sink->entity)
            && (link->sink->index == sink->index))
        {
            break;
        }
    }

    if (i == source->entity->num_links) {
        media_dbg(media, "Link not found\n");
        mediactl_error("Link not found");

        ret = -ENOENT;
        goto done;
    }

    ulink.source.entity = source->entity->info.id;
    ulink.source.index = source->index;
    ulink.source.flags = MEDIA_PAD_FL_SOURCE;

    ulink.sink.entity = sink->entity->info.id;
    ulink.sink.index = sink->index;
    ulink.sink.flags = MEDIA_PAD_FL_SINK;

    ulink.flags = flags | (link->flags & MEDIA_LNK_FL_IMMUTABLE);

    ret = ioctl(media->fd, MEDIA_IOC_SETUP_LINK, &ulink);
    if (ret == -1) {
        ret = -errno;
        media_dbg(media, "unable to setup link, errstr:[%s]\n", strerror(errno));
        mediactl_error("unable to setup link, errstr:[%s]", strerror(errno));

        goto done;
    }

    link->flags = ulink.flags;
    link->twin->flags = ulink.flags;
    ret = 0;

done:
    media_device_close(media);
    return ret;
}

int media_reset_links(struct media_device *media)
{
    int ret;
    unsigned int i, j;

    for (i = 0; i < media->entities_count; ++i) {
        struct media_entity *entity = &media->entities[i];

        for (j = 0; j < entity->num_links; j++) {
            struct media_link *link = &entity->links[j];

            if ((link->flags & MEDIA_LNK_FL_IMMUTABLE) || link->source->entity != entity) {
                continue;
            }

            ret = media_setup_link(media, link->source, link->sink, link->flags & ~MEDIA_LNK_FL_ENABLED);
            if (ret < 0) {
                return ret;
            }
        }
    }

    return 0;
}

static struct media_link *media_entity_add_link(struct media_entity *entity)
{
    if (entity->num_links >= entity->max_links) {
        unsigned int i;
        struct media_link *links = entity->links;
        unsigned int max_links = entity->max_links * 2;

        links = realloc(links, max_links * sizeof *links);
        if (links == NULL) {
            return NULL;
        }

        for (i = 0; i < entity->num_links; ++i) {
            links[i].twin->twin = &links[i];
        }

        entity->max_links = max_links;
        entity->links = links;
    }

    return &entity->links[entity->num_links++];
}

static int media_enum_links(struct media_device *media)
{
    __u32 id;
    int ret = 0;

    for (id = 1; id <= media->entities_count; id++) {
        unsigned int i;
        struct media_links_enum links = { 0 };
        struct media_entity *entity = &media->entities[id - 1];

        links.entity = entity->info.id;
        links.pads = calloc(entity->info.pads, sizeof(struct media_pad_desc));
        links.links = calloc(entity->info.links, sizeof(struct media_link_desc));

        if (ioctl(media->fd, MEDIA_IOC_ENUM_LINKS, &links) < 0) {
            ret = -errno;
            media_dbg(media, "unable to enumerate pads and links:[%s]\n", strerror(errno));
            mediactl_error("unable to enumerate pads and links:[%s]", strerror(errno));

            free(links.pads);
            free(links.links);
            return ret;
        }

        for (i = 0; i < entity->info.pads; ++i) {
            entity->pads[i].entity = entity;
            entity->pads[i].index = links.pads[i].index;
            entity->pads[i].flags = links.pads[i].flags;
        }

        for (i = 0; i < entity->info.links; ++i) {
            struct media_entity *sink;
            struct media_link *fwdlink;
            struct media_link *backlink;
            struct media_entity *source;
            struct media_link_desc *link = &links.links[i];

            source = media_get_entity_by_id(media, link->source.entity);
            sink = media_get_entity_by_id(media, link->sink.entity);

            if ((source == NULL) || (sink == NULL)) {
                media_dbg(media, "entity:[%u] link:[%u] from:[%u/%u] to:[%u/%u] is invalid\n", id, i, link->source.entity, link->source.index, link->sink.entity, link->sink.index);
                mediactl_warn("entity:[%u] link:[%u] from:[%u/%u] to:[%u/%u] is invalid", id, i, link->source.entity, link->source.index, link->sink.entity, link->sink.index);

                ret = -EINVAL;
            } else {
                fwdlink = media_entity_add_link(source);
                fwdlink->source = &source->pads[link->source.index];
                fwdlink->sink = &sink->pads[link->sink.index];
                fwdlink->flags = link->flags;

                backlink = media_entity_add_link(sink);
                backlink->source = &source->pads[link->source.index];
                backlink->sink = &sink->pads[link->sink.index];
                backlink->flags = link->flags;

                fwdlink->twin = backlink;
                backlink->twin = fwdlink;
            }
        }

        free(links.pads);
        free(links.links);
    }

    return ret;
}

#if defined(CONFIG_LIBUDEV)

#include <libudev.h>

static inline int media_udev_open(struct udev **udev)
{
    *udev = udev_new();
    if (*udev == NULL) {
        return -ENOMEM;
    }

    return 0;
}

static inline void media_udev_close(struct udev *udev)
{
    if (udev != NULL) {
        udev_unref(udev);
    }
}

static int media_get_devname_udev(struct udev *udev, struct media_entity *entity)
{
    dev_t devnum;
    const char *p;
    int ret = -ENODEV;
    struct udev_device *device;

    if (udev == NULL) {
        return -EINVAL;
    }

    devnum = makedev(entity->info.v4l.major, entity->info.v4l.minor);
    media_dbg(entity->media, "looking up device:[%u:%u]\n", major(devnum), minor(devnum));
    mediactl_debug("looking up device:[%u:%u]", major(devnum), minor(devnum));

    device = udev_device_new_from_devnum(udev, 'c', devnum);
    if (device) {
        p = udev_device_get_devnode(device);
        if (p) {
            strncpy(entity->devname, p, sizeof(entity->devname));
            entity->devname[sizeof(entity->devname) - 1] = '\0';
        }

        ret = 0;
    }

    udev_device_unref(device);
    return ret;
}

#else

struct udev;

static inline int media_udev_open(struct udev **udev)
{
    return 0;
}

static inline void media_udev_close(struct udev *udev)
{

}

static inline int media_get_devname_udev(struct udev *udev, struct media_entity *entity)
{
    return -ENOTSUP;
}
#endif

static int media_get_devname_sysfs(struct media_entity *entity)
{
    char *p;
    int ret;
    char devname[32];
    char sysname[32];
    char target[1024];
    struct stat devstat;

    sprintf(sysname, "/sys/dev/char/%u:%u", entity->info.v4l.major, entity->info.v4l.minor);
    ret = readlink(sysname, target, sizeof(target));
    if (ret < 0) {
        return -errno;
    }

    target[ret] = '\0';
    p = strrchr(target, '/');
    if (p == NULL) {
        return -EINVAL;
    }

    sprintf(devname, "/dev/%s", p + 1);
    if (strstr(p + 1, "dvb")) {
        char *s = p + 1;

        if (strncmp(s, "dvb", 3)) {
            return -EINVAL;
        }

        s += 3;
        p = strchr(s, '.');
        if (!p) {
            return -EINVAL;
        }

        *p = '/';
        sprintf(devname, "/dev/dvb/adapter%s", s);
    } else {
        sprintf(devname, "/dev/%s", p + 1);
    }

    ret = stat(devname, &devstat);
    if (ret < 0) {
        return -errno;
    }

    if ((major(devstat.st_rdev) == entity->info.v4l.major) && (minor(devstat.st_rdev) == entity->info.v4l.minor)) {
        strcpy(entity->devname, devname);
    }

    return 0;
}

static int media_enum_entities(struct media_device *media)
{
    int ret;
    __u32 id;
    struct udev *udev;
    unsigned int size;
    struct media_entity *entity;

    ret = media_udev_open(&udev);
    if (ret < 0) {
        media_dbg(media, "Can't get udev context\n");
        mediactl_debug("Can't get udev context");
    }

    for (id = 0, ret = 0; ; id = entity->info.id) {
        size = (media->entities_count + 1) * sizeof(*media->entities);
        media->entities = realloc(media->entities, size);

        entity = &media->entities[media->entities_count];
        memset(entity, 0, sizeof(*entity));
        entity->fd = -1;
        entity->info.id = id | MEDIA_ENT_ID_FLAG_NEXT;
        entity->media = media;

        ret = ioctl(media->fd, MEDIA_IOC_ENUM_ENTITIES, &entity->info);
        if (ret < 0) {
            ret = errno != EINVAL ? -errno : 0;
            break;
        }

        entity->max_links = entity->info.pads + entity->info.links;

        entity->pads = malloc(entity->info.pads * sizeof(*entity->pads));
        entity->links = malloc(entity->max_links * sizeof(*entity->links));
        if ((entity->pads == NULL) || (entity->links == NULL)) {
            ret = -ENOMEM;
            break;
        }

        media->entities_count++;

        if (entity->info.flags & MEDIA_ENT_FL_DEFAULT) {
            switch (entity->info.type) {
                case MEDIA_ENT_T_DEVNODE_V4L:
                    media->def.v4l = entity;
                    break;

                case MEDIA_ENT_T_DEVNODE_FB:
                    media->def.fb = entity;
                    break;

                case MEDIA_ENT_T_DEVNODE_ALSA:
                    media->def.alsa = entity;
                    break;

                case MEDIA_ENT_T_DEVNODE_DVB:
                    media->def.dvb = entity;
                    break;
            }
        }

        if ((media_entity_type(entity) != MEDIA_ENT_T_DEVNODE) && (media_entity_type(entity) != MEDIA_ENT_T_V4L2_SUBDEV)) {
            continue;
        }

        if (!entity->info.dev.major && !entity->info.dev.minor) {
            continue;
        }

        if (!media_get_devname_udev(udev, entity)) {
            continue;
        }

        media_get_devname_sysfs(entity);
    }

    media_udev_close(udev);
    return ret;
}

int media_device_enumerate(struct media_device *media)
{
    int ret;

    if (media->entities) {
        return 0;
    }

    ret = media_device_open(media);
    if (ret < 0) {
        return ret;
    }

    memset(&media->info, 0x00, sizeof(media->info));

    ret = ioctl(media->fd, MEDIA_IOC_DEVICE_INFO, &media->info);
    if (ret < 0) {
        ret = -errno;
        media_dbg(media, "unable to retrieve media device information for device %s (%s)\n", media->devnode, strerror(errno));
        goto done;
    }

    media_dbg(media, "Enumerating entities\n");

    ret = media_enum_entities(media);
    if (ret < 0) {
        media_dbg(media, "unable to enumerate entities for device %s (%s)\n", media->devnode, strerror(-ret));
        goto done;
    }

    media_dbg(media, "Found %u entities\n", media->entities_count);
    media_dbg(media, "Enumerating pads and links\n");

    ret = media_enum_links(media);
    if (ret < 0) {
        media_dbg(media, "unable to enumerate pads and linksfor device %s\n", media->devnode);
        goto done;
    }

    ret = 0;

done:
    media_device_close(media);
    return ret;
}

static void media_debug_default(void *ptr, ...)
{

}

void media_debug_set_handler(struct media_device *media, void (*debug_handler)(void *, ...), void *debug_priv)
{
    if (debug_handler) {
        media->debug_handler = debug_handler;
        media->debug_priv = debug_priv;
    } else {
        media->debug_handler = media_debug_default;
        media->debug_priv = NULL;
    }
}

static struct media_device *__media_device_new(void)
{
    struct media_device *media;

    media = calloc(1, sizeof(*media));
    if (media == NULL) {
        return NULL;
    }

    media->fd = -1;
    media->refcount = 1;

    media_debug_set_handler(media, NULL, NULL);
    return media;
}

struct media_device *media_device_new(const char *devnode)
{
    struct media_device *media;

    media = __media_device_new();
    if (media == NULL) {
        return NULL;
    }

    media->devnode = strdup(devnode);
    if (media->devnode == NULL) {
        media_device_unref(media);
        return NULL;
    }

    return media;
}

struct media_device *media_device_new_emulated(struct media_device_info *info)
{
    struct media_device *media;

    media = __media_device_new();
    if (media == NULL) {
        return NULL;
    }
    media->info = *info;

    return media;
}

struct media_device *media_device_ref(struct media_device *media)
{
    media->refcount++;
    return media;
}

void media_device_unref(struct media_device *media)
{
    unsigned int i;

    media->refcount--;
    if (media->refcount > 0) {
        return;
    }

    for (i = 0; i < media->entities_count; ++i) {
        struct media_entity *entity = &media->entities[i];

        free(entity->pads);
        free(entity->links);
        if (entity->fd != -1) {
            close(entity->fd);
        }
    }

    free(media->entities);
    free(media->devnode);
    free(media);
}

int media_device_add_entity(struct media_device *media, const struct media_entity_desc *desc, const char *devnode)
{
    unsigned int size;
    struct media_entity *entity;
    struct media_entity **defent = NULL;

    size = (media->entities_count + 1) * sizeof(*media->entities);
    entity = realloc(media->entities, size);
    if (entity == NULL) {
        return -ENOMEM;
    }

    media->entities = entity;
    media->entities_count++;

    entity = &media->entities[media->entities_count - 1];
    memset(entity, 0x00, sizeof *entity);

    entity->fd = -1;
    entity->media = media;
    strncpy(entity->devname, devnode, sizeof entity->devname);
    entity->devname[sizeof entity->devname - 1] = '\0';

    entity->info.id = 0;
    entity->info.type = desc->type;
    entity->info.flags = 0;
    memcpy(entity->info.name, desc->name, sizeof entity->info.name);

    switch (entity->info.type) {
        case MEDIA_ENT_T_DEVNODE_V4L:
            defent = &media->def.v4l;
            entity->info.v4l = desc->v4l;
            break;

        case MEDIA_ENT_T_DEVNODE_FB:
            defent = &media->def.fb;
            entity->info.fb = desc->fb;
            break;

        case MEDIA_ENT_T_DEVNODE_ALSA:
            defent = &media->def.alsa;
            entity->info.alsa = desc->alsa;
            break;

        case MEDIA_ENT_T_DEVNODE_DVB:
            defent = &media->def.dvb;
            entity->info.dvb = desc->dvb;
            break;
    }

    if (desc->flags & MEDIA_ENT_FL_DEFAULT) {
        entity->info.flags |= MEDIA_ENT_FL_DEFAULT;
        if (defent) {
            *defent = entity;
        }
    }

    return 0;
}

struct media_entity *media_parse_entity(struct media_device *media, const char *p, char **endp)
{
    char *end;
    unsigned int entity_id;
    struct media_entity *entity;

    if (endp == NULL) {
        endp = &end;
    }

    for (; isspace(*p); ++p);

    if (*p == '"' || *p == '\'') {
        char *name;

        for (end = (char *)p + 1; *end && *end != '"' && *end != '\''; ++end);
        if (*end != '"' && *end != '\'') {
            media_dbg(media, "missing matching '\"'\n");
            *endp = end;
            return NULL;
        }

        name = strndup(p + 1, end - p - 1);
        if (!name) {
            return NULL;
        }

        entity = media_get_entity_by_name(media, name);
        free(name);
        if (entity == NULL) {
            media_dbg(media, "no such entity \"%.*s\"\n", end - p - 1, p + 1);
            *endp = (char *)p + 1;
            return NULL;
        }

        ++end;
    } else {
        entity_id = strtoul(p, &end, 10);
        entity = media_get_entity_by_id(media, entity_id);
        if (entity == NULL) {
            media_dbg(media, "no such entity %d\n", entity_id);
            *endp = (char *)p;
            return NULL;
        }
    }

    for (p = end; isspace(*p); ++p);
    *endp = (char *)p;

    return entity;
}

struct media_pad *media_parse_pad(struct media_device *media, const char *p, char **endp)
{
    char *end;
    unsigned int pad;
    struct media_entity *entity;

    if (endp == NULL) {
        endp = &end;
    }

    entity = media_parse_entity(media, p, &end);
    if (!entity) {
        *endp = end;
        return NULL;
    }

    if (*end != ':') {
        media_dbg(media, "Expected ':'\n", *end);
        *endp = end;
        return NULL;
    }

    for (p = end + 1; isspace(*p); ++p);

    pad = strtoul(p, &end, 10);
    if (pad >= entity->info.pads) {
        media_dbg(media, "No pad '%d' on entity \"%s\". Maximum pad number is %d\n", pad, entity->info.name, entity->info.pads - 1);
        *endp = (char *)p;
        return NULL;
    }

    for (p = end; isspace(*p); ++p);
    *endp = (char *)p;

    return &entity->pads[pad];
}

struct media_link *media_parse_link(struct media_device *media, const char *p, char **endp)
{
    char *end;
    unsigned int i;
    struct media_pad *sink;
    struct media_link *link;
    struct media_pad *source;

    source = media_parse_pad(media, p, &end);
    if (source == NULL) {
        *endp = end;
        return NULL;
    }

    if ((end[0] != '-') || (end[1] != '>')) {
        *endp = end;
        media_dbg(media, "Expected '->'\n");
        return NULL;
    }

    p = end + 2;

    sink = media_parse_pad(media, p, &end);
    if (sink == NULL) {
        *endp = end;
        return NULL;
    }

    *endp = end;

    for (i = 0; i < source->entity->num_links; i++) {
        link = &source->entity->links[i];
        if ((link->source == source) && (link->sink == sink)) {
            return link;
        }
    }

    media_dbg(media, "No link between \"%s\":%d and \"%s\":%d\n", source->entity->info.name, source->index, sink->entity->info.name, sink->index);
    return NULL;
}

int media_parse_setup_link(struct media_device *media, const char *p, char **endp)
{
    char *end;
    __u32 flags;
    struct media_link *link;

    link = media_parse_link(media, p, &end);
    if (link == NULL) {
        media_dbg(media, "Unable to parse link\n");
        *endp = end;
        return -EINVAL;
    }

    p = end;
    if (*p++ != '[') {
        media_dbg(media, "Unable to parse link flags: expected '['.\n");
        *endp = (char *)p - 1;
        return -EINVAL;
    }

    flags = strtoul(p, &end, 10);
    for (p = end; isspace(*p); p++);

    if (*p++ != ']') {
        media_dbg(media, "Unable to parse link flags: expected ']'.\n");
        *endp = (char *)p - 1;
        return -EINVAL;
    }

    for (; isspace(*p); p++);
    *endp = (char *)p;

    media_dbg(media, "Setting up link %u:%u -> %u:%u [%u]\n", link->source->entity->info.id, link->source->index, link->sink->entity->info.id, link->sink->index, flags);

    return media_setup_link(media, link->source, link->sink, flags);
}

void media_print_streampos(struct media_device *media, const char *p, const char *end)
{
    size_t pos;

    pos = end - p + 1;
    if (pos < 0) {
        pos = 0;
    }

    if (pos > strlen(p)) {
        pos = strlen(p);
    }

    media_dbg(media, "\n");
    media_dbg(media, " %s\n", p);
    media_dbg(media, " %*s\n", pos, "^");
}

int media_parse_setup_links(struct media_device *media, const char *p)
{
    int ret;
    char *end;

    do {
        ret = media_parse_setup_link(media, p, &end);
        if (ret < 0) {
            media_print_streampos(media, p, end);
            return ret;
        }

        p = end + 1;
    } while (*end == ',');

    return *end ? -EINVAL : 0;
}
