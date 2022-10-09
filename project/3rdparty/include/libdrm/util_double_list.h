#ifndef _U_DOUBLE_LIST_H_
#define _U_DOUBLE_LIST_H_

#include <stddef.h>

struct list_head {
    struct list_head *prev;
    struct list_head *next;
};

static inline void list_inithead(struct list_head *item)
{
    item->prev = item;
    item->next = item;
}

static inline void list_add(struct list_head *item, struct list_head *list)
{
    item->prev = list;
    item->next = list->next;
    list->next->prev = item;
    list->next = item;
}

static inline void list_addtail(struct list_head *item, struct list_head *list)
{
    item->next = list;
    item->prev = list->prev;
    list->prev->next = item;
    list->prev = item;
}

static inline void list_replace(struct list_head *from, struct list_head *to)
{
    to->prev = from->prev;
    to->next = from->next;
    from->next->prev = to;
    from->prev->next = to;
}

static inline void list_del(struct list_head *item)
{
    item->prev->next = item->next;
    item->next->prev = item->prev;
}

static inline void list_delinit(struct list_head *item)
{
    item->prev->next = item->next;
    item->next->prev = item->prev;
    item->next = item;
    item->prev = item;
}

#define LIST_INITHEAD(__item)               list_inithead(__item)
#define LIST_ADD(__item, __list)            list_add(__item, __list)
#define LIST_ADDTAIL(__item, __list)        list_addtail(__item, __list)
#define LIST_REPLACE(__from, __to)          list_replace(__from, __to)
#define LIST_DEL(__item)                    list_del(__item)
#define LIST_DELINIT(__item)                list_delinit(__item)

#define LIST_ENTRY(__type, __item, __field)                             \
    ((__type *)(((char *)(__item)) - offsetof(__type, __field)))

#define LIST_FIRST_ENTRY(__ptr, __type, __field)                        \
    LIST_ENTRY(__type, (__ptr)->next, __field)

#define LIST_LAST_ENTRY(__ptr, __type, __field)                         \
    LIST_ENTRY(__type, (__ptr)->prev, __field)

#define LIST_IS_EMPTY(__list)                                           \
    ((__list)->next == (__list))

#ifndef container_of
#define container_of(ptr, sample, member)                               \
    (void *)((char *)(ptr) - ((char *)&((__typeof__(sample))0)->member))
#endif

#define LIST_FOR_EACH_ENTRY(pos, head, member)                          \
   for (pos = container_of((head)->next, pos, member); &pos->member != (head); pos = container_of(pos->member.next, pos, member))

#define LIST_FOR_EACH_ENTRY_SAFE(pos, storage, head, member)            \
   for (pos = container_of((head)->next, pos, member), storage = container_of(pos->member.next, pos, member); &pos->member != (head); pos = storage, storage = container_of(storage->member.next, storage, member))

#define LIST_FOR_EACH_ENTRY_SAFE_REV(pos, storage, head, member)        \
   for (pos = container_of((head)->prev, pos, member), storage = container_of(pos->member.prev, pos, member); &pos->member != (head); pos = storage, storage = container_of(storage->member.prev, storage, member))

#define LIST_FOR_EACH_ENTRY_FROM(pos, start, head, member)              \
   for (pos = container_of((start), pos, member); &pos->member != (head); pos = container_of(pos->member.next, pos, member))

#define LIST_FOR_EACH_ENTRY_FROM_REV(pos, start, head, member)          \
   for (pos = container_of((start), pos, member); &pos->member != (head); pos = container_of(pos->member.prev, pos, member))

#endif
