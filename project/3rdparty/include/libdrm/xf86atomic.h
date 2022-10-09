#ifndef LIBDRM_ATOMICS_H
#define LIBDRM_ATOMICS_H

#define HAS_ATOMIC_OPS                  1

typedef struct {
    int atomic;
} atomic_t;

#define atomic_read(x)                  ((x)->atomic)
#define atomic_set(x, val)              ((x)->atomic = (val))
#define atomic_inc(x)                   ((void)__sync_fetch_and_add(&(x)->atomic, 1))
#define atomic_inc_return(x)            (__sync_add_and_fetch(&(x)->atomic, 1))
#define atomic_dec_and_test(x)          (__sync_add_and_fetch(&(x)->atomic, -1) == 0)
#define atomic_add(x, v)                ((void)__sync_add_and_fetch(&(x)->atomic, (v)))
#define atomic_dec(x, v)                ((void)__sync_sub_and_fetch(&(x)->atomic, (v)))
#define atomic_cmpxchg(x, oldv, newv)   __sync_val_compare_and_swap(&(x)->atomic, oldv, newv)

static inline int atomic_add_unless(atomic_t *v, int add, int unless)
{
    int c, old;

    c = atomic_read(v);
    while (c != unless && (old = atomic_cmpxchg(v, c, c + add)) != c) {
        c = old;
    }

    return c != unless;
}

#endif
