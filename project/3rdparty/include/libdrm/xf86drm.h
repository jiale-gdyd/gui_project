#ifndef LIBDRM_XF86DRM_H
#define LIBDRM_XF86DRM_H

#include <stdarg.h>
#include <stdint.h>
#include <sys/types.h>

#include "drm.h"

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef DRM_MAX_MINOR
#define DRM_MAX_MINOR                                       64
#endif

#define DRM_IOCTL_NR(n)                                     _IOC_NR(n)
#define DRM_IOC_VOID                                        _IOC_NONE
#define DRM_IOC_READ                                        _IOC_READ
#define DRM_IOC_WRITE                                       _IOC_WRITE
#define DRM_IOC_READWRITE                                   _IOC_READ|_IOC_WRITE
#define DRM_IOC(dir, group, nr, size)                       _IOC(dir, group, nr, size)


#define DRM_DEV_UID                                         0
#define DRM_DEV_GID                                         0

#define DRM_DEV_DIRMODE                                     (S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)
#define DRM_DEV_MODE                                        (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP)

#define DRM_DIR_NAME                                        "/dev/dri"
#define DRM_PRIMARY_MINOR_NAME                              "card"
#define DRM_CONTROL_MINOR_NAME                              "controlD"
#define DRM_RENDER_MINOR_NAME                               "renderD"
#define DRM_PROC_NAME                                       "/proc/dri/"

#define DRM_DEV_NAME                                        "%s/" DRM_PRIMARY_MINOR_NAME "%d"
#define DRM_CONTROL_DEV_NAME                                "%s/" DRM_CONTROL_MINOR_NAME "%d"
#define DRM_RENDER_DEV_NAME                                 "%s/" DRM_RENDER_MINOR_NAME "%d"

#define DRM_NODE_NAME_MAX \
    (sizeof(DRM_DIR_NAME) + 1 + MAX3(sizeof(DRM_PRIMARY_MINOR_NAME), sizeof(DRM_CONTROL_MINOR_NAME), sizeof(DRM_RENDER_MINOR_NAME)) + sizeof("1048575") + 1)

#define DRM_ERR_NO_DEVICE                                   (-1001)
#define DRM_ERR_NO_ACCESS                                   (-1002)
#define DRM_ERR_NOT_ROOT                                    (-1003)
#define DRM_ERR_INVALID                                     (-1004)
#define DRM_ERR_NO_FD                                       (-1005)

#define DRM_AGP_NO_HANDLE                                   0

typedef unsigned int drmSize, *drmSizePtr;
typedef void *drmAddress, **drmAddressPtr;

#if (__GNUC__ >= 3)
#define DRM_PRINTFLIKE(f, a)                                __attribute__ ((format(__printf__, f, a)))
#else
#define DRM_PRINTFLIKE(f, a)
#endif

typedef struct _drmServerInfo {
    int (*debug_print)(const char *format, va_list ap) DRM_PRINTFLIKE(1,0);
    int (*load_module)(const char *name);
    void (*get_perms)(gid_t *, mode_t *);
} drmServerInfo, *drmServerInfoPtr;

typedef struct drmHashEntry {
    int  fd;
    void (*f)(int, void *, void *);
    void *tagTable;
} drmHashEntry;

extern int drmIoctl(int fd, unsigned long request, void *arg);
extern void *drmGetHashTable(void);
extern drmHashEntry *drmGetEntry(int fd);

typedef struct _drmVersion {
    int  version_major;
    int  version_minor;
    int  version_patchlevel;
    int  name_len;
    char *name;
    int  date_len;
    char *date;
    int  desc_len;
    char *desc;
} drmVersion, *drmVersionPtr;

typedef struct _drmStats {
    unsigned long count;
    struct {
    unsigned long value;
    const char    *long_format;
    const char    *long_name;
    const char    *rate_format;
    const char    *rate_name;
    int           isvalue;
    const char    *mult_names;
    int           mult;
    int           verbose;
    } data[15];
} drmStatsT;

typedef enum {
    DRM_FRAME_BUFFER   = 0,
    DRM_REGISTERS      = 1,
    DRM_SHM            = 2,
    DRM_AGP            = 3,
    DRM_SCATTER_GATHER = 4,
    DRM_CONSISTENT     = 5
} drmMapType;

typedef enum {
    DRM_RESTRICTED      = 0x0001,
    DRM_READ_ONLY       = 0x0002,
    DRM_LOCKED          = 0x0004,
    DRM_KERNEL          = 0x0008,
    DRM_WRITE_COMBINING = 0x0010,
    DRM_CONTAINS_LOCK   = 0x0020,
    DRM_REMOVABLE       = 0x0040
} drmMapFlags;

typedef enum {
    DRM_DMA_BLOCK        = 0x01,
    DRM_DMA_WHILE_LOCKED = 0x02,
    DRM_DMA_PRIORITY     = 0x04,
    DRM_DMA_WAIT         = 0x10,
    DRM_DMA_SMALLER_OK   = 0x20,
    DRM_DMA_LARGER_OK    = 0x40
} drmDMAFlags;

typedef enum {
    DRM_PAGE_ALIGN    = 0x01,
    DRM_AGP_BUFFER    = 0x02,
    DRM_SG_BUFFER     = 0x04,
    DRM_FB_BUFFER     = 0x08,
    DRM_PCI_BUFFER_RO = 0x10
} drmBufDescFlags;

typedef enum {
    DRM_LOCK_READY      = 0x01,
    DRM_LOCK_QUIESCENT  = 0x02,
    DRM_LOCK_FLUSH      = 0x04,
    DRM_LOCK_FLUSH_ALL  = 0x08,
    DRM_HALT_ALL_QUEUES = 0x10,
    DRM_HALT_CUR_QUEUES = 0x20
} drmLockFlags;

typedef enum {
    DRM_CONTEXT_PRESERVED = 0x01,
    DRM_CONTEXT_2DONLY    = 0x02
} drm_context_tFlags, *drm_context_tFlagsPtr;

typedef struct _drmBufDesc {
    int count;
    int size;
    int low_mark;
    int high_mark;
} drmBufDesc, *drmBufDescPtr;

typedef struct _drmBufInfo {
    int           count;
    drmBufDescPtr list;
} drmBufInfo, *drmBufInfoPtr;

typedef struct _drmBuf {
    int        idx;
    int        total;
    int        used;
    drmAddress address;
} drmBuf, *drmBufPtr;

typedef struct _drmBufMap {
    int       count;
    drmBufPtr list;
} drmBufMap, *drmBufMapPtr;

typedef struct _drmLock {
    volatile unsigned int lock;
    char                  padding[60];
} drmLock, *drmLockPtr;

typedef struct _drmDMAReq {
    drm_context_t context;
    int           send_count;
    int           *send_list;
    int           *send_sizes;
    drmDMAFlags   flags;
    int           request_count;
    int           request_size;
    int           *request_list;
    int           *request_sizes;
    int           granted_count;
} drmDMAReq, *drmDMAReqPtr;

typedef struct _drmRegion {
    drm_handle_t handle;
    unsigned int offset;
    drmSize      size;
    drmAddress   map;
} drmRegion, *drmRegionPtr;

typedef struct _drmTextureRegion {
    unsigned char next;
    unsigned char prev;
    unsigned char in_use;
    unsigned char padding;
    unsigned int  age;
} drmTextureRegion, *drmTextureRegionPtr;

typedef enum {
    DRM_VBLANK_ABSOLUTE       = 0x0,
    DRM_VBLANK_RELATIVE       = 0x1,
    DRM_VBLANK_HIGH_CRTC_MASK = 0x0000003e,
    DRM_VBLANK_EVENT          = 0x4000000,
    DRM_VBLANK_FLIP           = 0x8000000,
    DRM_VBLANK_NEXTONMISS     = 0x10000000,
    DRM_VBLANK_SECONDARY      = 0x20000000,
    DRM_VBLANK_SIGNAL         = 0x40000000
} drmVBlankSeqType;

#define DRM_VBLANK_HIGH_CRTC_SHIFT                          1

typedef struct _drmVBlankReq {
    drmVBlankSeqType type;
    unsigned int     sequence;
    unsigned long    signal;
} drmVBlankReq, *drmVBlankReqPtr;

typedef struct _drmVBlankReply {
    drmVBlankSeqType type;
    unsigned int     sequence;
    long             tval_sec;
    long             tval_usec;
} drmVBlankReply, *drmVBlankReplyPtr;

typedef union _drmVBlank {
    drmVBlankReq   request;
    drmVBlankReply reply;
} drmVBlank, *drmVBlankPtr;

typedef struct _drmSetVersion {
    int drm_di_major;
    int drm_di_minor;
    int drm_dd_major;
    int drm_dd_minor;
} drmSetVersion, *drmSetVersionPtr;

#define __drm_dummy_lock(lock)                              (*(__volatile__ unsigned int *)lock)

#define DRM_LOCK_HELD                                       0x80000000U
#define DRM_LOCK_CONT                                       0x40000000U

#if defined(__GNUC__) && (__GNUC__ >= 2)
#if defined(__i386) || defined(__AMD64__) || defined(__x86_64__) || defined(__amd64__)

#define DRM_CAS(lock, old, newt, __ret)                     \
    do {                                                    \
        int __dummy;                                        \
        __asm__ __volatile__(                               \
            "lock ; cmpxchg %4,%1\n\t"                      \
                        "setnz %0"                          \
            : "=d" (__ret),                                 \
            "=m" (__drm_dummy_lock(lock)),                  \
                        "=a" (__dummy)                      \
            : "2" (old),                                    \
            "r" (newt));                                    \
    } while (0)

#elif defined(__alpha__)

#define	DRM_CAS(lock, old, newt, ret)                       \
    do {                                                    \
        int tmp, old32;                                     \
        __asm__ __volatile__(                               \
        "   addl    $31, %5, %3\n"                          \
        "1: ldl_l   %0, %2\n"                               \
        "   cmpeq   %0, %3, %1\n"                           \
        "   beq	%1, 2f\n"                                   \
        "   mov	%4, %0\n"                                   \
        "   stl_c   %0, %2\n"                               \
        "   beq	%0, 3f\n"                                   \
        "   mb\n"                                           \
        "2: cmpeq   %1, 0, %1\n"                            \
        ".subsection 2\n"                                   \
        "3: br  1b\n"                                       \
        ".previous"                                         \
        : "=&r"(tmp), "=&r"(ret),                           \
        "=m"(__drm_dummy_lock(lock)),                       \
        "=&r"(old32)                                        \
        : "r"(newt), "r"(old)                               \
        : "memory");                                        \
    } while (0)

#elif defined(__sparc__)

#define DRM_CAS(lock, old, newt, __ret)                     \
    do {                                                    \
        register unsigned int __old __asm("o0");            \
        register unsigned int __new __asm("o1");            \
        register volatile unsigned int *__lock __asm("o2"); \
        __old = old;                                        \
        __new = newt;                                       \
        __lock = (volatile unsigned int *)lock;             \
        __asm__ __volatile__(                               \
            /*"cas [%2], %3, %0"*/                          \
            ".word 0xd3e29008\n\t"                          \
            /*"membar #StoreStore | #StoreLoad"*/           \
            ".word 0x8143e00a"                              \
            : "=&r" (__new)                                 \
            : "0" (__new),                                  \
            "r" (__lock),                                   \
            "r" (__old)                                     \
            : "memory");                                    \
        __ret = (__new != __old);                           \
    } while (0)

#elif defined(__ia64__)

#ifdef __INTEL_COMPILER
#include <ia64intrin.h>

#define DRM_CAS(lock, old, newt, __ret)                     \
    do {                                                    \
        unsigned long __result, __old = (old) & 0xffffffff; \
        __mf();                                             \
        __result = _InterlockedCompareExchange_acq(&__drm_dummy_lock(lock), (newt), __old);\
        __ret = (__result) != (__old);                      \
    } while (0)

#else
#define DRM_CAS(lock, old, newt, __ret)                     \
    do {                                                    \
        unsigned int __result, __old = (old);               \
        __asm__ __volatile__(                               \
            "mf\n"                                          \
            "mov ar.ccv=%2\n"                               \
            ";;\n"                                          \
            "cmpxchg4.acq %0=%1,%3,ar.ccv"                  \
            : "=r" (__result), "=m" (__drm_dummy_lock(lock))\
            : "r" ((unsigned long)__old), "r" (newt)        \
            : "memory");                                    \
        __ret = (__result) != (__old);                      \
    } while (0)

#endif

#elif defined(__powerpc__)

#define DRM_CAS(lock, old, newt, __ret)                     \
    do {                                                    \
        __asm__ __volatile__(                               \
            "sync;"                                         \
            "0:    lwarx %0,0,%1;"                          \
            "      xor. %0,%3,%0;"                          \
            "      bne 1f;"                                 \
            "      stwcx. %2,0,%1;"                         \
            "      bne- 0b;"                                \
            "1:    "                                        \
            "sync;"                                         \
        : "=&r"(__ret)                                      \
        : "r"(lock), "r"(newt), "r"(old)                    \
        : "cr0", "memory");                                 \
    } while (0)

#elif defined (__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__)      \
    || defined (__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__)   \
    || defined (__ARM_ARCH_6K__) || defined(__ARM_ARCH_6T2__)   \
    || defined (__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__)     \
    || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__)     \
    || defined(__ARM_ARCH_7EM__) || defined(__arm__)
#undef DRM_DEV_MODE
#define DRM_DEV_MODE                                        (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

#define DRM_CAS(lock,old,new,__ret)                         \
    do {                                                    \
        __asm__ __volatile__ (                              \
                "1: ldrex %0, [%1]\n"                       \
                "   teq %0, %2\n"                           \
                "   ite eq\n"                               \
                "   strexeq %0, %3, [%1]\n"                 \
                "   movne   %0, #1\n"                       \
        : "=&r" (__ret)                                     \
        : "r" (lock), "r" (old), "r" (new)                  \
        : "cc","memory");                                   \
    } while (0)

#endif
#endif

#ifndef DRM_CAS
#define DRM_CAS(lock, old, newt, ret)                       do { ret = 1; } while (0)
#endif

#if defined(__alpha__)
#define DRM_CAS_RESULT(_result)                             long _result
#elif defined(__powerpc__)
#define DRM_CAS_RESULT(_result)                             int _result
#else
#define DRM_CAS_RESULT(_result)                             char _result
#endif

#define DRM_LIGHT_LOCK(fd, lock, context)                      \
    do {                                                       \
        DRM_CAS_RESULT(__ret);                                 \
        DRM_CAS(lock, context, DRM_LOCK_HELD|context, __ret);  \
        if (__ret) drmGetLock(fd, context, 0);                 \
    } while (0)

#define DRM_LIGHT_LOCK_COUNT(fd, lock, context, count)         \
    do {                                                       \
        DRM_CAS_RESULT(__ret);                                 \
        DRM_CAS(lock, context,DRM_LOCK_HELD|context, __ret);   \
        if (__ret) drmGetLock(fd, context, 0);                 \
        else ++count;                                          \
    } while (0)

#define DRM_LOCK(fd, lock, context, flags)                     \
    do {                                                       \
        if (flags) drmGetLock(fd, context, flags);             \
        else DRM_LIGHT_LOCK(fd, lock, context);                \
    } while (0)

#define DRM_UNLOCK(fd, lock, context)                          \
    do {                                                       \
        DRM_CAS_RESULT(__ret);                                 \
        DRM_CAS(lock, DRM_LOCK_HELD|context, context, __ret);  \
        if (__ret) drmUnlock(fd, context);                     \
    } while (0)

#define DRM_SPINLOCK(spin, val)                                \
    do {                                                       \
        DRM_CAS_RESULT(__ret);                                 \
        do {                                                   \
            DRM_CAS(spin, 0, val, __ret);                      \
            if (__ret) while ((spin)->lock);                   \
        } while (__ret);                                       \
    } while (0)

#define DRM_SPINLOCK_TAKE(spin, val)                           \
    do {                                                       \
        DRM_CAS_RESULT(__ret);                                 \
        int cur;                                               \
        do {                                                   \
            cur = (*spin).lock;                                \
            DRM_CAS(spin, cur, val, __ret);                    \
        } while (__ret);                                       \
    } while (0)

#define DRM_SPINLOCK_COUNT(spin, val, count, __ret)                \
    do {                                                           \
        int  __i;                                                  \
        __ret = 1;                                                 \
        for (__i = 0; __ret && __i < count; __i++) {               \
            DRM_CAS(spin, 0, val, __ret);                          \
            if (__ret) for (;__i < count && (spin)->lock; __i++);  \
        }                                                          \
    } while(0)

#define DRM_SPINUNLOCK(spin, val)                                  \
    do {                                                           \
        DRM_CAS_RESULT(__ret);                                     \
        if ((*spin).lock == val) {                                 \
            do {                                                   \
                DRM_CAS(spin, val, 0, __ret);                      \
            } while (__ret);                                       \
        }                                                          \
    } while(0)

extern int drmAvailable(void);
extern int drmOpen(const char *name, const char *busid);

#define DRM_NODE_PRIMARY                                    0
#define DRM_NODE_CONTROL                                    1
#define DRM_NODE_RENDER                                     2
#define DRM_NODE_MAX                                        3

extern int drmOpenWithType(const char *name, const char *busid, int type);

extern int drmOpenControl(int minor);
extern int drmOpenRender(int minor);
extern int drmClose(int fd);
extern drmVersionPtr drmGetVersion(int fd);
extern drmVersionPtr drmGetLibVersion(int fd);
extern int drmGetCap(int fd, uint64_t capability, uint64_t *value);
extern void drmFreeVersion(drmVersionPtr);
extern int drmGetMagic(int fd, drm_magic_t * magic);
extern char *drmGetBusid(int fd);
extern int drmGetInterruptFromBusID(int fd, int busnum, int devnum, int funcnum);
extern int drmGetMap(int fd, int idx, drm_handle_t *offset, drmSize *size, drmMapType *type, drmMapFlags *flags, drm_handle_t *handle, int *mtrr);
extern int drmGetClient(int fd, int idx, int *auth, int *pid, int *uid, unsigned long *magic, unsigned long *iocs);
extern int drmGetStats(int fd, drmStatsT *stats);
extern int drmSetInterfaceVersion(int fd, drmSetVersion *version);
extern int drmCommandNone(int fd, unsigned long drmCommandIndex);
extern int drmCommandRead(int fd, unsigned long drmCommandIndex, void *data, unsigned long size);
extern int drmCommandWrite(int fd, unsigned long drmCommandIndex, void *data, unsigned long size);
extern int drmCommandWriteRead(int fd, unsigned long drmCommandIndex, void *data, unsigned long size);

extern void drmFreeBusid(const char *busid);
extern int drmSetBusid(int fd, const char *busid);
extern int drmAuthMagic(int fd, drm_magic_t magic);
extern int drmAddMap(int fd, drm_handle_t offset, drmSize size, drmMapType type, drmMapFlags flags, drm_handle_t * handle);
extern int drmRmMap(int fd, drm_handle_t handle);
extern int drmAddContextPrivateMapping(int fd, drm_context_t ctx_id, drm_handle_t handle);

extern int drmAddBufs(int fd, int count, int size, drmBufDescFlags flags, int agp_offset);
extern int drmMarkBufs(int fd, double low, double high);
extern int drmCreateContext(int fd, drm_context_t * handle);
extern int drmSetContextFlags(int fd, drm_context_t context, drm_context_tFlags flags);
extern int drmGetContextFlags(int fd, drm_context_t context, drm_context_tFlagsPtr flags);
extern int drmAddContextTag(int fd, drm_context_t context, void *tag);
extern int drmDelContextTag(int fd, drm_context_t context);
extern void *drmGetContextTag(int fd, drm_context_t context);
extern drm_context_t *drmGetReservedContextList(int fd, int *count);
extern void drmFreeReservedContextList(drm_context_t *);
extern int drmSwitchToContext(int fd, drm_context_t context);
extern int drmDestroyContext(int fd, drm_context_t handle);
extern int drmCreateDrawable(int fd, drm_drawable_t * handle);
extern int drmDestroyDrawable(int fd, drm_drawable_t handle);
extern int drmUpdateDrawableInfo(int fd, drm_drawable_t handle, drm_drawable_info_type_t type, unsigned int num, void *data);
extern int drmCtlInstHandler(int fd, int irq);
extern int drmCtlUninstHandler(int fd);
extern int drmSetClientCap(int fd, uint64_t capability, uint64_t value);

extern int drmCrtcGetSequence(int fd, uint32_t crtcId, uint64_t *sequence, uint64_t *ns);
extern int drmCrtcQueueSequence(int fd, uint32_t crtcId, uint32_t flags, uint64_t sequence, uint64_t *sequence_queued, uint64_t user_data);

extern int drmMap(int fd, drm_handle_t handle, drmSize size, drmAddressPtr address);
extern int drmUnmap(drmAddress address, drmSize size);
extern drmBufInfoPtr drmGetBufInfo(int fd);
extern drmBufMapPtr drmMapBufs(int fd);
extern int drmUnmapBufs(drmBufMapPtr bufs);
extern int drmDMA(int fd, drmDMAReqPtr request);
extern int drmFreeBufs(int fd, int count, int *list);
extern int drmGetLock(int fd, drm_context_t context, drmLockFlags flags);
extern int drmUnlock(int fd, drm_context_t context);
extern int drmFinish(int fd, int context, drmLockFlags flags);
extern int drmGetContextPrivateMapping(int fd, drm_context_t ctx_id,  drm_handle_t * handle);

extern int drmAgpAcquire(int fd);
extern int drmAgpRelease(int fd);
extern int drmAgpEnable(int fd, unsigned long mode);
extern int drmAgpAlloc(int fd, unsigned long size, unsigned long type, unsigned long *address, drm_handle_t *handle);
extern int drmAgpFree(int fd, drm_handle_t handle);
extern int drmAgpBind(int fd, drm_handle_t handle, unsigned long offset);
extern int drmAgpUnbind(int fd, drm_handle_t handle);

extern int drmAgpVersionMajor(int fd);
extern int drmAgpVersionMinor(int fd);
extern unsigned long drmAgpGetMode(int fd);
extern unsigned long drmAgpBase(int fd);
extern unsigned long drmAgpSize(int fd);
extern unsigned long drmAgpMemoryUsed(int fd);
extern unsigned long drmAgpMemoryAvail(int fd);
extern unsigned int drmAgpVendorId(int fd);
extern unsigned int drmAgpDeviceId(int fd);

extern int drmScatterGatherAlloc(int fd, unsigned long size, drm_handle_t *handle);
extern int drmScatterGatherFree(int fd, drm_handle_t handle);

extern int drmWaitVBlank(int fd, drmVBlankPtr vbl);

extern void drmSetServerInfo(drmServerInfoPtr info);
extern int drmError(int err, const char *label);
extern void *drmMalloc(int size);
extern void drmFree(void *pt);

extern void *drmHashCreate(void);
extern int drmHashDestroy(void *t);
extern int drmHashLookup(void *t, unsigned long key, void **value);
extern int drmHashInsert(void *t, unsigned long key, void *value);
extern int drmHashDelete(void *t, unsigned long key);
extern int drmHashFirst(void *t, unsigned long *key, void **value);
extern int drmHashNext(void *t, unsigned long *key, void **value);

extern void *drmRandomCreate(unsigned long seed);
extern int drmRandomDestroy(void *state);
extern unsigned long drmRandom(void *state);
extern double drmRandomDouble(void *state);
extern void *drmSLCreate(void);
extern int drmSLDestroy(void *l);
extern int drmSLLookup(void *l, unsigned long key, void **value);
extern int drmSLInsert(void *l, unsigned long key, void *value);
extern int drmSLDelete(void *l, unsigned long key);
extern int drmSLNext(void *l, unsigned long *key, void **value);
extern int drmSLFirst(void *l, unsigned long *key, void **value);
extern void drmSLDump(void *l);
extern int drmSLLookupNeighbors(void *l, unsigned long key, unsigned long *prev_key, void **prev_value, unsigned long *next_key, void **next_value);

extern int drmOpenOnce(void *unused, const char *BusID, int *newlyopened);
extern int drmOpenOnceWithType(const char *BusID, int *newlyopened, int type);
extern void drmCloseOnce(int fd);
extern void drmMsg(const char *format, ...) DRM_PRINTFLIKE(1, 2);

extern int drmSetMaster(int fd);
extern int drmDropMaster(int fd);
extern int drmIsMaster(int fd);

#define DRM_EVENT_CONTEXT_VERSION                           4

typedef struct _drmEventContext {
    int version;

    void (*vblank_handler)(int fd, unsigned int sequence,  unsigned int tv_sec, unsigned int tv_usec, void *user_data);
    void (*page_flip_handler)(int fd, unsigned int sequence, unsigned int tv_sec, unsigned int tv_usec, void *user_data);
    void (*page_flip_handler2)(int fd, unsigned int sequence, unsigned int tv_sec, unsigned int tv_usec, unsigned int crtc_id, void *user_data);
    void (*sequence_handler)(int fd, uint64_t sequence, uint64_t ns, uint64_t user_data);
} drmEventContext, *drmEventContextPtr;

extern int drmHandleEvent(int fd, drmEventContextPtr evctx);

extern char *drmGetDeviceNameFromFd(int fd);

extern char *drmGetDeviceNameFromFd2(int fd);
extern int drmGetNodeTypeFromFd(int fd);

extern int drmPrimeHandleToFD(int fd, uint32_t handle, uint32_t flags, int *prime_fd);
extern int drmPrimeFDToHandle(int fd, int prime_fd, uint32_t *handle);

extern int drmCloseBufferHandle(int fd, uint32_t handle);

extern char *drmGetPrimaryDeviceNameFromFd(int fd);
extern char *drmGetRenderDeviceNameFromFd(int fd);

#define DRM_BUS_PCI                                         0
#define DRM_BUS_USB                                         1
#define DRM_BUS_PLATFORM                                    2
#define DRM_BUS_HOST1X                                      3

typedef struct _drmPciBusInfo {
    uint16_t domain;
    uint8_t  bus;
    uint8_t  dev;
    uint8_t  func;
} drmPciBusInfo, *drmPciBusInfoPtr;

typedef struct _drmPciDeviceInfo {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t subvendor_id;
    uint16_t subdevice_id;
    uint8_t  revision_id;
} drmPciDeviceInfo, *drmPciDeviceInfoPtr;

typedef struct _drmUsbBusInfo {
    uint8_t bus;
    uint8_t dev;
} drmUsbBusInfo, *drmUsbBusInfoPtr;

typedef struct _drmUsbDeviceInfo {
    uint16_t vendor;
    uint16_t product;
} drmUsbDeviceInfo, *drmUsbDeviceInfoPtr;

#define DRM_PLATFORM_DEVICE_NAME_LEN                        512

typedef struct _drmPlatformBusInfo {
    char fullname[DRM_PLATFORM_DEVICE_NAME_LEN];
} drmPlatformBusInfo, *drmPlatformBusInfoPtr;

typedef struct _drmPlatformDeviceInfo {
    char **compatible;
} drmPlatformDeviceInfo, *drmPlatformDeviceInfoPtr;

#define DRM_HOST1X_DEVICE_NAME_LEN                          512

typedef struct _drmHost1xBusInfo {
    char fullname[DRM_HOST1X_DEVICE_NAME_LEN];
} drmHost1xBusInfo, *drmHost1xBusInfoPtr;

typedef struct _drmHost1xDeviceInfo {
    char **compatible;
} drmHost1xDeviceInfo, *drmHost1xDeviceInfoPtr;

typedef struct _drmDevice {
    char **nodes;
    int  available_nodes;
    int  bustype;

    union {
        drmPciBusInfoPtr      pci;
        drmUsbBusInfoPtr      usb;
        drmPlatformBusInfoPtr platform;
        drmHost1xBusInfoPtr   host1x;
    } businfo;

    union {
        drmPciDeviceInfoPtr      pci;
        drmUsbDeviceInfoPtr      usb;
        drmPlatformDeviceInfoPtr platform;
        drmHost1xDeviceInfoPtr   host1x;
    } deviceinfo;
} drmDevice, *drmDevicePtr;

extern int drmGetDevice(int fd, drmDevicePtr *device);
extern void drmFreeDevice(drmDevicePtr *device);

extern int drmGetDevices(drmDevicePtr devices[], int max_devices);
extern void drmFreeDevices(drmDevicePtr devices[], int count);

#define DRM_DEVICE_GET_PCI_REVISION (1 << 0)
extern int drmGetDevice2(int fd, uint32_t flags, drmDevicePtr *device);
extern int drmGetDevices2(uint32_t flags, drmDevicePtr devices[], int max_devices);

extern int drmGetDeviceFromDevId(dev_t dev_id, uint32_t flags, drmDevicePtr *device);

extern int drmDevicesEqual(drmDevicePtr a, drmDevicePtr b);

extern int drmSyncobjCreate(int fd, uint32_t flags, uint32_t *handle);
extern int drmSyncobjDestroy(int fd, uint32_t handle);
extern int drmSyncobjHandleToFD(int fd, uint32_t handle, int *obj_fd);
extern int drmSyncobjFDToHandle(int fd, int obj_fd, uint32_t *handle);

extern int drmSyncobjImportSyncFile(int fd, uint32_t handle, int sync_file_fd);
extern int drmSyncobjExportSyncFile(int fd, uint32_t handle, int *sync_file_fd);
extern int drmSyncobjWait(int fd, uint32_t *handles, unsigned num_handles, int64_t timeout_nsec, unsigned flags, uint32_t *first_signaled);
extern int drmSyncobjReset(int fd, const uint32_t *handles, uint32_t handle_count);
extern int drmSyncobjSignal(int fd, const uint32_t *handles, uint32_t handle_count);
extern int drmSyncobjTimelineSignal(int fd, const uint32_t *handles, uint64_t *points, uint32_t handle_count);
extern int drmSyncobjTimelineWait(int fd, uint32_t *handles, uint64_t *points, unsigned num_handles, int64_t timeout_nsec, unsigned flags, uint32_t *first_signaled);
extern int drmSyncobjQuery(int fd, uint32_t *handles, uint64_t *points, uint32_t handle_count);
extern int drmSyncobjQuery2(int fd, uint32_t *handles, uint64_t *points, uint32_t handle_count, uint32_t flags);
extern int drmSyncobjTransfer(int fd, uint32_t dst_handle, uint64_t dst_point, uint32_t src_handle, uint64_t src_point, uint32_t flags);
extern int drmSyncobjEventfd(int fd, uint32_t handle, uint64_t point, int ev_fd, uint32_t flags);

extern char *drmGetFormatModifierVendor(uint64_t modifier);
extern char *drmGetFormatModifierName(uint64_t modifier);

extern char *drmGetFormatName(uint32_t format);

#ifndef fourcc_mod_get_vendor
#define fourcc_mod_get_vendor(modifier)                     (((modifier) >> 56) & 0xff)
#endif

#if defined(__cplusplus)
}
#endif

#endif
