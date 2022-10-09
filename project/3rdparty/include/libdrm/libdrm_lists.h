#ifndef _LIBDRM_LISTS_H_
#define _LIBDRM_LISTS_H_

#include <stddef.h>

typedef struct _drmMMListHead {
    struct _drmMMListHead *prev;
    struct _drmMMListHead *next;
} drmMMListHead;

#define DRMINITLISTHEAD(__item)                 \
    do{                                         \
        (__item)->prev = (__item);              \
        (__item)->next = (__item);              \
    } while (0)

#define DRMLISTADD(__item, __list)              \
    do {                                        \
        (__item)->prev = (__list);              \
        (__item)->next = (__list)->next;        \
        (__list)->next->prev = (__item);        \
        (__list)->next = (__item);              \
    } while (0)

#define DRMLISTADDTAIL(__item, __list)          \
    do {                                        \
        (__item)->next = (__list);              \
        (__item)->prev = (__list)->prev;        \
        (__list)->prev->next = (__item);        \
        (__list)->prev = (__item);              \
    } while (0)

#define DRMLISTDEL(__item)                      \
    do {                                        \
        (__item)->prev->next = (__item)->next;  \
        (__item)->next->prev = (__item)->prev;  \
    } while (0)

#define DRMLISTDELINIT(__item)                  \
    do {                                        \
        (__item)->prev->next = (__item)->next;  \
        (__item)->next->prev = (__item)->prev;  \
        (__item)->next = (__item);              \
        (__item)->prev = (__item);              \
    } while (0)

#define DRMLISTENTRY(__type, __item, __field)                                   \
    ((__type *)(((char *)(__item)) - offsetof(__type, __field)))

#define DRMLISTEMPTY(__item)                                                    \
    ((__item)->next == (__item))

#define DRMLISTSINGLE(__list)                                                   \
    (!DRMLISTEMPTY(__list) && ((__list)->next == (__list)->prev))

#define DRMLISTFOREACH(__item, __list)                                          \
    for ((__item) = (__list)->next; (__item) != (__list); (__item) = (__item)->next)

#define DRMLISTFOREACHSAFE(__item, __temp, __list)                              \
    for ((__item) = (__list)->next, (__temp) = (__item)->next; (__item) != (__list); (__item) = (__temp), (__temp) = (__item)->next)

#define DRMLISTFOREACHSAFEREVERSE(__item, __temp, __list)                       \
    for ((__item) = (__list)->prev, (__temp) = (__item)->prev; (__item) != (__list); (__item) = (__temp), (__temp) = (__item)->prev)

#define DRMLISTFOREACHENTRY(__item, __list, __head)                            \
    for ((__item) = DRMLISTENTRY(__typeof__(*__item), (__list)->next, __head); &(__item)->__head != (__list); (__item) = DRMLISTENTRY(__typeof__(*__item), (__item)->__head.next, __head))

#define DRMLISTFOREACHENTRYSAFE(__item, __temp, __list, __head)                \
    for ((__item) = DRMLISTENTRY(__typeof__(*__item), (__list)->next, __head), (__temp) = DRMLISTENTRY(__typeof__(*__item), (__item)->__head.next, __head); &(__item)->__head != (__list); (__item) = (__temp), (__temp) = DRMLISTENTRY(__typeof__(*__item), (__temp)->__head.next, __head))

#define DRMLISTJOIN(__list, __join)                                             \
    if (!DRMLISTEMPTY(__list)) {                                                \
        (__list)->next->prev = (__join);                                        \
        (__list)->prev->next = (__join)->next;                                  \
        (__join)->next->prev = (__list)->prev;                                  \
        (__join)->next = (__list)->next;                                        \
    }

#endif
