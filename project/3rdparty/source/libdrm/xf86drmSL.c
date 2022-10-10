#include <stdio.h>
#include <stdlib.h>

#include <libdrm/libdrm_macros.h>
#include <libdrm/xf86drm.h>

#define SL_LIST_MAGIC                           0xfacade00LU
#define SL_ENTRY_MAGIC                          0x00fab1edLU
#define SL_FREED_MAGIC                          0xdecea5edLU
#define SL_MAX_LEVEL                            16
#define SL_RANDOM_SEED                          0xc01055a1LU

#define SL_RANDOM_DECL                          static void *state = NULL
#define SL_RANDOM_INIT(seed)                    if (!state) state = drmRandomCreate(seed)
#define SL_RANDOM                               drmRandom(state)

typedef struct SLEntry {
    unsigned long  magic;
    unsigned long  key;
    void           *value;
    int            levels;
    struct SLEntry *forward[1];
} SLEntry, *SLEntryPtr;

typedef struct SkipList {
    unsigned long magic;
    int           level;
    int           count;
    SLEntryPtr    head;
    SLEntryPtr    p0;
} SkipList, *SkipListPtr;

static SLEntryPtr SLCreateEntry(int max_level, unsigned long key, void *value)
{
    SLEntryPtr entry;

    if (max_level < 0 || max_level > SL_MAX_LEVEL) {
        max_level = SL_MAX_LEVEL;
    }

    entry = (SLEntryPtr)drmMalloc(sizeof(*entry) + (max_level + 1) * sizeof(entry->forward[0]));
    if (!entry) {
        return NULL;
    }

    entry->magic = SL_ENTRY_MAGIC;
    entry->key = key;
    entry->value = value;
    entry->levels = max_level + 1;

    return entry;
}

static int SLRandomLevel(void)
{
    int level = 1;
    SL_RANDOM_DECL;

    SL_RANDOM_INIT(SL_RANDOM_SEED);
    
    while ((SL_RANDOM & 0x01) && level < SL_MAX_LEVEL) {
        ++level;
    }

    return level;
}

drm_public void *drmSLCreate(void)
{
    int i;
    SkipListPtr list;

    list = (SkipListPtr)drmMalloc(sizeof(*list));
    if (!list) {
        return NULL;
    }

    list->magic = SL_LIST_MAGIC;
    list->level = 0;
    list->head = SLCreateEntry(SL_MAX_LEVEL, 0, NULL);
    list->count = 0;

    for (i = 0; i <= SL_MAX_LEVEL; i++) {
        list->head->forward[i] = NULL;
    }

    return list;
}

drm_public int drmSLDestroy(void *l)
{
    SLEntryPtr next;
    SLEntryPtr entry;
    SkipListPtr list = (SkipListPtr)l;

    if (list->magic != SL_LIST_MAGIC) {
        return -1;
    }

    for (entry = list->head; entry; entry = next) {
        if (entry->magic != SL_ENTRY_MAGIC) {
            return -1;
        }

        next = entry->forward[0];
        entry->magic = SL_FREED_MAGIC;
        drmFree(entry);
    }

    list->magic = SL_FREED_MAGIC;
    drmFree(list);

    return 0;
}

static SLEntryPtr SLLocate(void *l, unsigned long key, SLEntryPtr *update)
{
    int i;
    SLEntryPtr entry;
    SkipListPtr list = (SkipListPtr)l;

    if (list->magic != SL_LIST_MAGIC) {
        return NULL;
    }

    for (i = list->level, entry = list->head; i >= 0; i--) {
        while (entry->forward[i] && entry->forward[i]->key < key) {
            entry = entry->forward[i];
        }

        update[i] = entry;
    }

    return entry->forward[0];
}

drm_public int drmSLInsert(void *l, unsigned long key, void *value)
{
    int i;
    int level;
    SLEntryPtr entry;
    SkipListPtr list = (SkipListPtr)l;
    SLEntryPtr update[SL_MAX_LEVEL + 1];

    if (list->magic != SL_LIST_MAGIC) {
        return -1;
    }

    entry = SLLocate(list, key, update);
    if (entry && entry->key == key) {
        return 1;
    }

    level = SLRandomLevel();
    if (level > list->level) {
        level = ++list->level;
        update[level] = list->head;
    }

    entry = SLCreateEntry(level, key, value);
    for (i = 0; i <= level; i++) {
        entry->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = entry;
    }

    ++list->count;
    return 0;
}

drm_public int drmSLDelete(void *l, unsigned long key)
{
    int i;
    SLEntryPtr entry;
    SkipListPtr list = (SkipListPtr)l;
    SLEntryPtr update[SL_MAX_LEVEL + 1];

    if (list->magic != SL_LIST_MAGIC) {
        return -1;
    }

    entry = SLLocate(list, key, update);
    if (!entry || entry->key != key) {
        return 1;
    }

    for (i = 0; i <= list->level; i++) {
        if (update[i]->forward[i] == entry) {
            update[i]->forward[i] = entry->forward[i];
        }
    }

    entry->magic = SL_FREED_MAGIC;
    drmFree(entry);

    while (list->level && !list->head->forward[list->level]) {
        --list->level;
    }
    --list->count;

    return 0;
}

drm_public int drmSLLookup(void *l, unsigned long key, void **value)
{
    SLEntryPtr entry;
    SkipListPtr list = (SkipListPtr)l;
    SLEntryPtr update[SL_MAX_LEVEL + 1];

    entry = SLLocate(list, key, update);
    if (entry && entry->key == key) {
        *value = entry;
        return 0;
    }
    *value = NULL;

    return -1;
}

drm_public int drmSLLookupNeighbors(void *l, unsigned long key, unsigned long *prev_key, void **prev_value, unsigned long *next_key, void **next_value)
{
    int retcode = 0;
    SkipListPtr list = (SkipListPtr)l;
    SLEntryPtr update[SL_MAX_LEVEL + 1] = {0};

    SLLocate(list, key, update);

    *prev_key   = *next_key   = key;
    *prev_value = *next_value = NULL;

    if (update[0]) {
        *prev_key = update[0]->key;
        *prev_value = update[0]->value;
        ++retcode;

        if (update[0]->forward[0]) {
            *next_key = update[0]->forward[0]->key;
            *next_value = update[0]->forward[0]->value;
            ++retcode;
        }
    }

    return retcode;
}

drm_public int drmSLNext(void *l, unsigned long *key, void **value)
{
    SLEntryPtr entry;
    SkipListPtr list = (SkipListPtr)l;

    if (list->magic != SL_LIST_MAGIC) {
        return -1;
    }

    entry = list->p0;
    if (entry) {
        list->p0 = entry->forward[0];
        *key = entry->key;
        *value = entry->value;
        return 1;
    }
    list->p0 = NULL;

    return 0;
}

drm_public int drmSLFirst(void *l, unsigned long *key, void **value)
{
    SkipListPtr list = (SkipListPtr)l;
    if (list->magic != SL_LIST_MAGIC) {
        return -1;
    }
    list->p0 = list->head->forward[0];

    return drmSLNext(list, key, value);
}

drm_public void drmSLDump(void *l)
{
    int i;
    SLEntryPtr entry;
    SkipListPtr list = (SkipListPtr)l;

    if (list->magic != SL_LIST_MAGIC) {
        printf("Bad magic: 0x%08lx (expected 0x%08lx)\n", list->magic, SL_LIST_MAGIC);
        return;
    }

    printf("Level = %d, count = %d\n", list->level, list->count);
    for (entry = list->head; entry; entry = entry->forward[0]) {
        if (entry->magic != SL_ENTRY_MAGIC) {
            printf("Bad magic: 0x%08lx (expected 0x%08lx)\n",
            list->magic, SL_ENTRY_MAGIC);
        }
        printf("\nEntry %p <0x%08lx, %p> has %2d levels\n", entry, entry->key, entry->value, entry->levels);

        for (i = 0; i < entry->levels; i++) {
            if (entry->forward[i]) {
                printf("   %2d: %p <0x%08lx, %p>\n", i, entry->forward[i], entry->forward[i]->key, entry->forward[i]->value);
            } else {
                printf("   %2d: %p\n", i, entry->forward[i]);
            }
        }
    }
}
