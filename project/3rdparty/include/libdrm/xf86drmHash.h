#ifndef LIBDRM_XF86DRMHASH_H
#define LIBDRM_XF86DRMHASH_H

#define HASH_SIZE       512

typedef struct HashBucket {
    unsigned long     key;
    void              *value;
    struct HashBucket *next;
} HashBucket, *HashBucketPtr;

typedef struct HashTable {
    unsigned long magic;
    unsigned long entries;
    unsigned long hits;
    unsigned long partials;
    unsigned long misses;
    HashBucketPtr buckets[HASH_SIZE];
    int           p0;
    HashBucketPtr p1;
} HashTable, *HashTablePtr;

#endif
