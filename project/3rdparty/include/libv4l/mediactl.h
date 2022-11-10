#ifndef LIBDRM_MEDIACTL_H
#define LIBDRM_MEDIACTL_H

#include <linux/media.h>

#ifdef __cplusplus
extern "C" {
#endif

struct media_link {
    struct media_pad  *source;
    struct media_pad  *sink;
    struct media_link *twin;
    __u32             flags;
    __u32             padding[3];
};

struct media_pad {
    struct media_entity *entity;
    __u32               index;
    __u32               flags;
    __u32               padding[3];
};

struct media_device;
struct media_entity;

struct media_device *media_device_new(const char *devnode);
struct media_device *media_device_new_emulated(struct media_device_info *info);

struct media_device *media_device_ref(struct media_device *media);
void media_device_unref(struct media_device *media);

int media_device_add_entity(struct media_device *media, const struct media_entity_desc *desc, const char *devnode);

void media_debug_set_handler(struct media_device *media, void (*debug_handler)(void *, ...), void *debug_priv);

int media_device_enumerate(struct media_device *media);
struct media_pad *media_entity_remote_source(struct media_pad *pad);

const struct media_entity_desc *media_entity_get_info(struct media_entity *entity);
const struct media_pad *media_entity_get_pad(struct media_entity *entity, unsigned int index);

unsigned int media_entity_get_links_count(struct media_entity *entity);
const struct media_link *media_entity_get_link(struct media_entity *entity, unsigned int index);

const char *media_entity_get_devname(struct media_entity *entity);

static inline unsigned int media_entity_type(struct media_entity *entity)
{
    return media_entity_get_info(entity)->type & MEDIA_ENT_TYPE_MASK;
}

struct media_entity *media_get_entity_by_name(struct media_device *media, const char *name);

struct media_entity *media_get_entity_by_id(struct media_device *media, __u32 id);

unsigned int media_get_entities_count(struct media_device *media);
struct media_entity *media_get_entity(struct media_device *media, unsigned int index);
struct media_entity *media_get_default_entity(struct media_device *media, unsigned int type);

const struct media_device_info *media_get_info(struct media_device *media);
const char *media_get_devnode(struct media_device *media);

int media_setup_link(struct media_device *media, struct media_pad *source, struct media_pad *sink, __u32 flags);
int media_reset_links(struct media_device *media);

struct media_entity *media_parse_entity(struct media_device *media, const char *p, char **endp);

struct media_pad *media_parse_pad(struct media_device *media, const char *p, char **endp);
struct media_link *media_parse_link(struct media_device *media, const char *p, char **endp);

int media_parse_setup_link(struct media_device *media, const char *p, char **endp);
int media_parse_setup_links(struct media_device *media, const char *p);

#ifdef __cplusplus
}
#endif

#endif
