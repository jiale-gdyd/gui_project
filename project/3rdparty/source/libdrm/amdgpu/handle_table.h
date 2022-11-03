#ifndef _HANDLE_TABLE_H_
#define _HANDLE_TABLE_H_

#include <stdint.h>
#include <libdrm/libdrm_macros.h>

struct handle_table {
    uint32_t max_key;
    void     **values;
};

drm_private int handle_table_insert(struct handle_table *table, uint32_t key, void *value);
drm_private void handle_table_remove(struct handle_table *table, uint32_t key);
drm_private void *handle_table_lookup(struct handle_table *table, uint32_t key);
drm_private void handle_table_fini(struct handle_table *table);

#endif
