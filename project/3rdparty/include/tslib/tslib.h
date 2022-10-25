#ifndef _TSLIB_H_
#define _TSLIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <sys/time.h>

#define TSIMPORT
#ifdef GCC_HASCLASSVISIBILITY
#define TSEXPORT                    __attribute__ ((visibility("default")))
#define TSLOCAL                     __attribute__ ((visibility("hidden")))
#else
#define TSEXPORT
#define TSLOCAL
#endif

#ifdef TSLIB_INTERNAL
#define TSAPI                       TSEXPORT
#else
#define TSAPI                       TSIMPORT
#endif

struct tsdev;

struct ts_sample {
    int            x;
    int            y;
    unsigned int   pressure;
    struct timeval tv;
};

struct ts_sample_mt {
    int            x;
    int            y;
    unsigned int   pressure;
    int            slot;
    int            tracking_id;

    int            tool_type;
    int            tool_x;
    int            tool_y;
    unsigned int   touch_major;
    unsigned int   width_major;
    unsigned int   touch_minor;
    unsigned int   width_minor;
    int            orientation;
    int            distance;
    int            blob_id;

    struct timeval tv;
    short          pen_down;
    short          valid;
};

#define TSLIB_MT_VALID                  (1 << 0)
#define TSLIB_MT_VALID_TOOL             (1 << 1)

struct ts_lib_version_data {
    const char   *package_version;
    int          version_num;
    unsigned int features;
};

#define TSLIB_VERSION_MT                (1 << 0)
#define TSLIB_VERSION_OPEN_RESTRICTED   (1 << 1)
#define TSLIB_VERSION_EVENTPATH         (1 << 2)
#define TSLIB_VERSION_VERSION           (1 << 3)

enum ts_param {
    TS_SCREEN_RES = 0,
    TS_SCREEN_ROT
};

struct ts_module_conf {
    char                  *name;
    char                  *params;
    int                   raw;
    int                   nr;
    struct ts_module_conf *next;
    struct ts_module_conf *prev;
};

TSAPI int ts_close(struct tsdev *);

TSAPI int ts_reconfig(struct tsdev *);
TSAPI int ts_config(struct tsdev *);

TSAPI int ts_option(struct tsdev *, enum ts_param, ...);

extern TSAPI int (*ts_error_fn)(const char *fmt, va_list ap);
extern TSAPI int (*ts_open_restricted)(const char *path, int flags, void *user_data);
extern TSAPI void (*ts_close_restricted)(int fd, void *user_data);

TSAPI int ts_fd(struct tsdev *);

TSAPI int ts_load_module(struct tsdev *, const char *mod, const char *params);

TSAPI struct tsdev *ts_open(const char *dev_name, int nonblock);
TSAPI struct tsdev *ts_setup(const char *dev_name, int nonblock);

TSAPI int ts_read(struct tsdev *, struct ts_sample *, int);
TSAPI int ts_read_raw(struct tsdev *, struct ts_sample *, int);
TSAPI int ts_read_mt(struct tsdev *, struct ts_sample_mt **, int slots, int nr);
TSAPI int ts_read_raw_mt(struct tsdev *, struct ts_sample_mt **, int slots, int nr);

TSAPI struct ts_lib_version_data *ts_libversion(void);

TSAPI struct ts_module_conf *ts_conf_get(struct tsdev *ts);
TSAPI int ts_conf_set(struct ts_module_conf *conf);

TSAPI char *ts_get_eventpath(struct tsdev *tsdev);

TSAPI char *tslib_version(void);
TSAPI void ts_print_ascii_logo(unsigned int pos);

#ifdef __cplusplus
}
#endif

#endif
