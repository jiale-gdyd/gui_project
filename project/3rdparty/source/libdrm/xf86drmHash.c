#include <stdio.h>
#include <stdlib.h>

#include <libdrm/libdrm_macros.h>
#include <libdrm/xf86drm.h>
#include <libdrm/xf86drmHash.h>

#define HASH_MAGIC                      0xdeadbeef

static unsigned long HashHash(unsigned long key)
{
    int i;
    static int init = 0;
    unsigned long hash = 0;
    unsigned long tmp = key;
    static unsigned long scatter[256];

    if (!init) {
        void *state;

        state = drmRandomCreate(37);
        for (i = 0; i < 256; i++) {
            scatter[i] = drmRandom(state);
        }
        drmRandomDestroy(state);

        ++init;
    }

    while (tmp) {
        hash = (hash << 1) + scatter[tmp & 0xff];
        tmp >>= 8;
    }

    hash %= HASH_SIZE;
    return hash;
}

drm_public void *drmHashCreate(void)
{
    HashTablePtr table;

    table = (HashTablePtr)drmMalloc(sizeof(*table));
    if (!table) {
        return NULL;
    }
    table->magic = HASH_MAGIC;

    return table;
}

drm_public int drmHashDestroy(void *t)
{
    int i;
    HashBucketPtr next;
    HashBucketPtr bucket;
    HashTablePtr table = (HashTablePtr)t;

    if (table->magic != HASH_MAGIC) {
        return -1;
    }

    for (i = 0; i < HASH_SIZE; i++) {
        for (bucket = table->buckets[i]; bucket;) {
            next = bucket->next;
            drmFree(bucket);
            bucket = next;
        }
    }
    drmFree(table);

    return 0;
}

static HashBucketPtr HashFind(HashTablePtr table, unsigned long key, unsigned long *h)
{
    HashBucketPtr bucket;
    HashBucketPtr prev = NULL;
    unsigned long hash = HashHash(key);

    if (h) {
        *h = hash;
    }

    for (bucket = table->buckets[hash]; bucket; bucket = bucket->next) {
        if (bucket->key == key) {
            if (prev) {
                prev->next = bucket->next;
                bucket->next = table->buckets[hash];
                table->buckets[hash] = bucket;
                ++table->partials;
            } else {
                ++table->hits;
            }

            return bucket;
        }

        prev = bucket;
    }
    ++table->misses;

    return NULL;
}

drm_public int drmHashLookup(void *t, unsigned long key, void **value)
{
    HashBucketPtr bucket;
    HashTablePtr table = (HashTablePtr)t;

    if (!table || (table->magic != HASH_MAGIC)) {
        return -1;
    }

    bucket = HashFind(table, key, NULL);
    if (!bucket) {
        return 1;
    }

    *value = bucket->value;
    return 0;
}

drm_public int drmHashInsert(void *t, unsigned long key, void *value)
{
    unsigned long hash;
    HashBucketPtr bucket;
    HashTablePtr table = (HashTablePtr)t;

    if (table->magic != HASH_MAGIC) {
        return -1;
    }

    if (HashFind(table, key, &hash)) {
        return 1;
    }

    bucket = (HashBucketPtr)drmMalloc(sizeof(*bucket));
    if (!bucket) {
        return -1;
    }

    bucket->key = key;
    bucket->value = value;
    bucket->next = table->buckets[hash];
    table->buckets[hash] = bucket;

    return 0;
}

drm_public int drmHashDelete(void *t, unsigned long key)
{
    unsigned long hash;
    HashBucketPtr bucket;
    HashTablePtr table = (HashTablePtr)t;

    if (table->magic != HASH_MAGIC) {
        return -1;
    }

    bucket = HashFind(table, key, &hash);
    if (!bucket) {
        return 1;
    }

    table->buckets[hash] = bucket->next;
    drmFree(bucket);

    return 0;
}

drm_public int drmHashNext(void *t, unsigned long *key, void **value)
{
    HashTablePtr table = (HashTablePtr)t;

    while (table->p0 < HASH_SIZE) {
        if (table->p1) {
            *key = table->p1->key;
            *value = table->p1->value;
            table->p1 = table->p1->next;
            return 1;
        }

        table->p1 = table->buckets[table->p0];
        ++table->p0;
    }

    return 0;
}

drm_public int drmHashFirst(void *t, unsigned long *key, void **value)
{
    HashTablePtr table = (HashTablePtr)t;

    if (table->magic != HASH_MAGIC) {
        return -1;
    }

    table->p0 = 0;
    table->p1 = table->buckets[0];

    return drmHashNext(table, key, value);
}
