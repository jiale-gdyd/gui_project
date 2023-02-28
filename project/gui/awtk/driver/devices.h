#ifndef TK_DEVICES_H
#define TK_DEVICES_H

#include "tkc/types_def.h"

BEGIN_C_DECLS

typedef struct _device_info_t {
    char type[TK_NAME_LEN + 1];
    char path[MAX_PATH + 1];
} device_info_t;

typedef ret_t (*device_visit_t)(void *ctx, const device_info_t *info);

ret_t devices_load(void);
ret_t devices_unload(void);

ret_t devices_set(device_info_t *devices, uint32_t nr);
ret_t devices_foreach(device_visit_t visit, void *ctx);

END_C_DECLS

#endif
