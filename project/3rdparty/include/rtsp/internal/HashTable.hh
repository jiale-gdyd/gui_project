#ifndef RTSP_INTERNAL_HASH_TABLE_HH
#define RTSP_INTERNAL_HASH_TABLE_HH

namespace rtsp {
class HashTable {
public:
    virtual ~HashTable();

    static HashTable *create(int keyType);

    virtual void *Add(char const *key, void *value) = 0;
    virtual bool Remove(char const *key) = 0;
    virtual void *Lookup(char const *key) const = 0;

    virtual unsigned numEntries() const = 0;

    bool IsEmpty() const {
        return numEntries() == 0;
    }

    class Iterator {
    public:
        static Iterator *create(HashTable &hashTable);
        virtual ~Iterator();

        virtual void *next(char const *&key) = 0;

    protected:
        Iterator();
    };

    void *RemoveNext();

protected:
    HashTable();
};

int const STRING_HASH_KEYS = 0;
int const ONE_WORD_HASH_KEYS = 1;
}

#endif
