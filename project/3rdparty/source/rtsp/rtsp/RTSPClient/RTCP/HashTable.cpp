#include <rtsp/internal/HashTable.hh>

namespace rtsp {
HashTable::HashTable() {

}

HashTable::~HashTable() {

}

HashTable::Iterator::Iterator() {

}

HashTable::Iterator::~Iterator()
{

}

void *HashTable::RemoveNext()
{
    char const *key;
    Iterator *iter = Iterator::create(*this);

    void *removedValue = iter->next(key);
    if (removedValue != 0) {
        Remove(key);
    }

    delete iter;
    return removedValue;
}
}
