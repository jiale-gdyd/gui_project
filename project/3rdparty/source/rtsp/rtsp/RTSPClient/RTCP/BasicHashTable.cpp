#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <rtsp/internal/util.h>
#include <rtsp/internal/BasicHashTable.hh>

namespace rtsp {
#define REBUILD_MULTIPLIER      3

BasicHashTable::BasicHashTable(int keyType)
    : fBuckets(fStaticBuckets), fNumBuckets(SMALL_HASH_TABLE_SIZE),
    fNumEntries(0), fRebuildSize(SMALL_HASH_TABLE_SIZE*REBUILD_MULTIPLIER),
    fDownShift(28), fMask(0x3), fKeyType(keyType)
{
    for (unsigned i = 0; i < SMALL_HASH_TABLE_SIZE; ++i) {
        fStaticBuckets[i] = NULL;
    }
}

BasicHashTable::~BasicHashTable()
{
    for (unsigned i = 0; i < fNumBuckets; ++i) {
        TableEntry *entry;
        while ((entry = fBuckets[i]) != NULL) {
            deleteEntry(i, entry);
        }
    }

    if (fBuckets != fStaticBuckets) {
        delete[] fBuckets;
    }
}

void* BasicHashTable::Add(char const *key, void *value)
{
    void *oldValue;
    unsigned index;

    TableEntry *entry = lookupKey(key, index);
    if (entry != NULL) {
        oldValue = entry->value;
    } else {
        entry = insertNewEntry(index, key);
        oldValue = NULL;
    }

    entry->value = value;
    if (fNumEntries >= fRebuildSize) {
        rebuild();
    }

    return oldValue;
}

bool BasicHashTable::Remove(char const *key)
{
    unsigned index;
    TableEntry *entry = lookupKey(key, index);
    if (entry == NULL) {
        return false;
    }

    deleteEntry(index, entry);
    return true;
}

void *BasicHashTable::Lookup(char const *key) const
{
    unsigned index;
    TableEntry *entry = lookupKey(key, index);
    if (entry == NULL) {
        return NULL;
    }

    return entry->value;
}

unsigned BasicHashTable::numEntries() const
{
    return fNumEntries;
}

BasicHashTable::Iterator::Iterator(BasicHashTable &table)
    : fTable(table), fNextIndex(0), fNextEntry(NULL)
{

}

void *BasicHashTable::Iterator::next(char const *&key)
{
    while (fNextEntry == NULL) {
        if (fNextIndex >= fTable.fNumBuckets) {
            return NULL;
        }

        fNextEntry = fTable.fBuckets[fNextIndex++];
    }

    BasicHashTable::TableEntry *entry = fNextEntry;
    fNextEntry = entry->fNext;

    key = entry->key;
    return entry->value;
}

HashTable *HashTable::create(int keyType)
{
    return new BasicHashTable(keyType);
}

HashTable::Iterator *HashTable::Iterator::create(HashTable &hashTable)
{
    return new BasicHashTable::Iterator((BasicHashTable &)hashTable);
}

BasicHashTable::TableEntry *BasicHashTable::lookupKey(char const *key, unsigned &index) const
{
    TableEntry *entry;
    index = hashIndexFromKey(key);

    for (entry = fBuckets[index]; entry != NULL; entry = entry->fNext) {
        if (keyMatches(key, entry->key)) {
            break;
        }
    }

    return entry;
}

bool BasicHashTable::keyMatches(char const *key1, char const *key2) const
{
    if (fKeyType == STRING_HASH_KEYS) {
        return (strcmp(key1, key2) == 0);
    } else if (fKeyType == ONE_WORD_HASH_KEYS) {
        return (key1 == key2);
    } else {
        unsigned *k1 = (unsigned *)key1;
        unsigned *k2 = (unsigned *)key2;

        for (int i = 0; i < fKeyType; ++i) {
            if (k1[i] != k2[i]) {
                return false;
            }
        }

        return true;
    }
}

BasicHashTable::TableEntry *BasicHashTable::insertNewEntry(unsigned index, char const *key)
{
    TableEntry *entry = new TableEntry();
    entry->fNext = fBuckets[index];
    fBuckets[index] = entry;

    ++fNumEntries;
    assignKey(entry, key);

    return entry;
}

void BasicHashTable::assignKey(TableEntry *entry, char const *key)
{
    if (fKeyType == STRING_HASH_KEYS) {
        entry->key = strDup(key);
    } else if (fKeyType == ONE_WORD_HASH_KEYS) {
        entry->key = key;
    } else if (fKeyType > 0) {
        unsigned *keyFrom = (unsigned *)key;
        unsigned *keyTo = new unsigned[fKeyType];
        for (int i = 0; i < fKeyType; ++i) {
            keyTo[i] = keyFrom[i];
        }

        entry->key = (char const *)keyTo;
    }
}

void BasicHashTable::deleteEntry(unsigned index, TableEntry *entry)
{
    bool foundIt = false;
    TableEntry **ep = &fBuckets[index];

    while (*ep != NULL) {
        if (*ep == entry) {
            foundIt = true;
            *ep = entry->fNext;
            break;
        }

        ep = &((*ep)->fNext);
    }

    --fNumEntries;
    deleteKey(entry);
    delete entry;
}

void BasicHashTable::deleteKey(TableEntry *entry)
{
    if (fKeyType == ONE_WORD_HASH_KEYS) {
        entry->key = NULL;
    } else {
        delete[] (char *)entry->key;
        entry->key = NULL;
    }
}

void BasicHashTable::rebuild()
{
    unsigned oldSize = fNumBuckets;
    TableEntry **oldBuckets = fBuckets;

    fNumBuckets *= 4;
    fBuckets = new TableEntry*[fNumBuckets];
    for (unsigned i = 0; i < fNumBuckets; ++i) {
        fBuckets[i] = NULL;
    }

    fRebuildSize *= 4;
    fDownShift -= 2;
    fMask = (fMask << 2) | 0x3;

    for (TableEntry **oldChainPtr = oldBuckets; oldSize > 0; --oldSize, ++oldChainPtr) {
        for (TableEntry *hPtr = *oldChainPtr; hPtr != NULL; hPtr = *oldChainPtr) {
            *oldChainPtr = hPtr->fNext;

            unsigned index = hashIndexFromKey(hPtr->key);

            hPtr->fNext = fBuckets[index];
            fBuckets[index] = hPtr;
        }
    }

    if (oldBuckets != fStaticBuckets) {
        delete[] oldBuckets;
    }
}

unsigned BasicHashTable::hashIndexFromKey(char const *key) const
{
    unsigned result = 0;

    if (fKeyType == STRING_HASH_KEYS) {
        while (1) {
            char c = *key++;
            if (c == 0) {
                break;
            }

            result += (result << 3) + (unsigned)c;
        }

        result &= fMask;
    } else if (fKeyType == ONE_WORD_HASH_KEYS) {
        result = randomIndex((unsigned long)key);
    } else {
        unsigned long sum = 0;
        unsigned *k = (unsigned *)key;

        for (int i = 0; i < fKeyType; ++i) {
            sum += k[i];
        }

        result = randomIndex(sum);
    }

    return result;
}
}
