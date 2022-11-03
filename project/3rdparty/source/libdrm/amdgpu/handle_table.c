#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libdrm/util_math.h>
#include "handle_table.h"

drm_private int handle_table_insert(struct handle_table *table, uint32_t key, void *value)
{
    if (key >= table->max_key) {
        void **values;
        uint32_t alignment = sysconf(_SC_PAGESIZE) / sizeof(void*);
        uint32_t max_key = ALIGN(key + 1, alignment);

        values = realloc(table->values, max_key * sizeof(void *));
        if (!values) {
            return -ENOMEM;
        }

        memset(values + table->max_key, 0, (max_key - table->max_key) * sizeof(void *));

        table->max_key = max_key;
        table->values = values;
    }

    table->values[key] = value;
    return 0;
}

drm_private void handle_table_remove(struct handle_table *table, uint32_t key)
{
    if (key < table->max_key) {
        table->values[key] = NULL;
    }
}

drm_private void *handle_table_lookup(struct handle_table *table, uint32_t key)
{
    if (key < table->max_key) {
        return table->values[key];
    } else {
        return NULL;
    }
}

drm_private void handle_table_fini(struct handle_table *table)
{
    free(table->values);
    table->max_key = 0;
    table->values = NULL;
}
