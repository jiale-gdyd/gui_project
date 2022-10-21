#ifndef LIBDRM_DISPLAY_UTILS_H
#define LIBDRM_DISPLAY_UTILS_H

#include <stdarg.h>
#include <string.h>

#include "../drm_media_common.h"

#define DRM_LOG_LEVEL_ERR                   0
#define DRM_LOG_LEVEL_WARN                  1
#define DRM_LOG_LEVEL_INFO                  2
#define DRM_LOG_LEVEL_DEBUG                 3

#ifndef MOD_TAG
#define MOD_TAG                             MOD_ID_SYS
#endif

#define _DRM_UNUSED                         __attribute__((unused))
#define DRM_UNUSED(x)                       (void)x

#define DRM_UPALIGNTO(value, align)         ((value + align - 1) & (~(align - 1)))
#define DRM_UPALIGNTO16(value)              DRM_UPALIGNTO(value, 16)
#define DRM_DOWNALIGNTO16(value)            (DRM_UPALIGNTO(value, 16) - 16)

#ifndef ARRAY_ELEMS
#define ARRAY_ELEMS(a)                      (sizeof(a) / sizeof((a)[0]))
#endif

#define MATH_LOG2(x)                        (31 - __builtin_clz((x) | 1))

#ifndef PAGE_SIZE
#define PAGE_SIZE                           (sysconf(_SC_PAGESIZE))
#endif

#define DRM_DUMP_FOURCC(f)                  f & 0xFF, (f >> 8) & 0xFF, (f >> 16) & 0xFF, (f >> 24) & 0xFF

#define DRM_VALUE_MAX(A, B)                 ((A) > (B) ? (A) : (B))
#define DRM_VALUE_MIN(A, B)                 ((A) < (B) ? (A) : (B))
#define DRM_VALUE_SCOPE_CHECK(X, MIN, MAX)  assert((X >= MIN) && (X <= MAX))

extern int drm_media_log_method;
extern short g_level_list[MOD_ID_BUTT];

#define DRM_LOG_LEVEL_JUDGE(FILTER_LEVEL)                                                                               \
    {                                                                                                                   \
        if (g_level_list[MOD_TAG] < FILTER_LEVEL) {                                                                     \
            break;                                                                                                      \
        }                                                                                                               \
    }

#define DRM_MEDIA_LOGE(format, ...)                                                                                     \
    do {                                                                                                                \
        DRM_LOG_LEVEL_JUDGE(DRM_LOG_LEVEL_ERR);                                                                         \
        fprintf(stderr, "\033[1;31m[MEDIA][E]: " format "\033[0m\n", ##__VA_ARGS__);                                    \
    } while (0);

#define DRM_MEDIA_LOGW(format, ...)                                                                                     \
    do {                                                                                                                \
        DRM_LOG_LEVEL_JUDGE(DRM_LOG_LEVEL_ERR);                                                                         \
        fprintf(stderr, "\033[1;33m[MEDIA][W]: " format "\033[0m\n", ##__VA_ARGS__);                                    \
    } while (0);

#define DRM_MEDIA_LOGI(format, ...)                                                                                     \
    do {                                                                                                                \
        DRM_LOG_LEVEL_JUDGE(DRM_LOG_LEVEL_ERR);                                                                         \
        fprintf(stderr, "\033[1;32m[MEDIA][I]: " format "\033[0m\n", ##__VA_ARGS__);                                    \
    } while (0);

#define DRM_MEDIA_LOGD(format, ...)                                                                                     \
    do {                                                                                                                \
        DRM_LOG_LEVEL_JUDGE(DRM_LOG_LEVEL_ERR);                                                                         \
        fprintf(stderr, "\033[1;34m[MEDIA][D]: " format "\033[0m\n", ##__VA_ARGS__);                                    \
    } while (0);

void drm_log_init();
void drm_log_exit();

#define DRM_LOG_MEMORY()                                                                                                \
    DRM_MEDIA_LOGI("no memory %s:%04d", __FUNCTION__, __LINE__)

#define DRM_FILE_FUNC_LINE()                                                                                            \
    DRM_MEDIA_LOGI("%s -> %s:%04d", __FILE__, __FUNCTION__, __LINE__)

#define DRM_TODO()                                                                                                      \
    DRM_MEDIA_LOGI("Todo: %s -> %s:%04d", __FILE__, __FUNCTION__, __LINE__)

template <typename T, typename TBase>
class IsDerived {
public:
    static int t(TBase *base) {
        return 1;
    }

    static char t(void *t2) {
        return 0;
    }

    static const bool Result = (sizeof(int) == sizeof(t((T *)nullptr)));
};

#include <map>
#include <list>
#include <string>
#include <thread>

namespace libdrm {
#define GET_STRING_TO_INT(var, map, key, defalut)                                       \
    if (!map[key].empty()) {                                                            \
        var = std::stoi(map[key]);                                                      \
    } else {                                                                            \
        var = defalut;                                                                  \
    }

#define CHECK_EMPTY_SETERRNO_RETURN(v_type, v, map, k, seterrno, ret)                   \
    v_type v = map[k];                                                                  \
    if (v.empty()) {                                                                    \
        DRM_MEDIA_LOGE("%s: miss %s", __func__, k);                                     \
        seterrno;                                                                       \
        return ret;                                                                     \
    }

#define CHECK_EMPTY(v, map, k)                                                          \
    CHECK_EMPTY_SETERRNO_RETURN(, v, map, k, , false)

#define CHECK_EMPTY_WITH_DECLARE(v_type, v, map, k)                                     \
    CHECK_EMPTY_SETERRNO_RETURN(v_type, v, map, k, , false)

#define CHECK_EMPTY_SETERRNO(v, map, k, err)                                            \
    CHECK_EMPTY_SETERRNO_RETURN(, v, map, k, errno = err, )

#define PARAM_STRING_APPEND(s, s1, s2)                                                  \
    s.append(s1 "=").append(s2).append("\n")

#define PARAM_STRING_APPEND_TO(s, s1, s2)                                               \
    s.append(s1 "=").append(std::to_string(s2)).append("\n")

#define PARAM_STRING_APPEND_PARAM_STRING(p1, p2)                                        \
    p1.append(" ").append(p2)

#define FIND_ENTRY_TARGET(INPUT, MAP, KEY, TARGET)                                      \
    for (size_t i = 0; i < ARRAY_ELEMS(MAP); i++) {                                     \
        if (INPUT == MAP[i].KEY) {                                                      \
            return MAP[i].TARGET;                                                       \
        }                                                                               \
    }

#define FIND_ENTRY_TARGET_BY_STRCMP(INPUT, MAP, KEY, TARGET)                            \
    if (INPUT) {                                                                        \
        for (size_t i = 0; i < ARRAY_ELEMS(MAP); i++) {                                 \
            if (!strcmp(INPUT, MAP[i].KEY)) {                                           \
                return MAP[i].TARGET;                                                   \
            }                                                                           \
        }                                                                               \
    }

#define CALL_MEMBER_FN(object, ptrToMember)                                             \
    ((object).*(ptrToMember))

bool parse_media_param_map(const char *param, std::map<std::string, std::string> &map);
bool parse_media_param_list(const char *param, std::list<std::string> &list, const char delim = '\n');
int parse_media_param_match(const char *param, std::map<std::string, std::string> &map, std::list<std::pair<const std::string, std::string &>> &list);

bool has_intersection(const char *str, const char *expect, std::list<std::string> *expect_list);

std::string get_media_value_by_key(const char *param, const char *key);

bool string_end_withs(std::string const &fullString, std::string const &ending);
bool string_start_withs(std::string const &fullString, std::string const &starting);

inline int64_t gettimeofday()
{
    std::chrono::microseconds us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
    return us.count();
}

inline void msleep(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

inline void usleep(int us)
{
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}

typedef int (*Ioctl_f)(int fd, unsigned long int request, ...);
inline int xioctl(Ioctl_f f, int fd, int request, void *argp)
{
    int r;

    do
        r = f(fd, request, argp);
    while ((-1 == r) && (EINTR == errno));

    return r;
}

bool DumpToFile(std::string path, const char *ptr, size_t len);

class AutoDuration {
public:
    AutoDuration() {
        Reset();
    }

    int64_t Get() {
        return (gettimeofday() - start);
    }

    void Reset() {
        start = gettimeofday();
    }

    int64_t GetAndReset() {
        int64_t now = gettimeofday();
        int64_t pretime = start;

        start = now;
        return (now - pretime);
    }

private:
    int64_t start;
};

class AutoPrintLine {
public:
public:
    AutoPrintLine(const char *f) : func(f) {
        DRM_MEDIA_LOGD("Enter %s", f);
    }

    ~AutoPrintLine() {
        DRM_MEDIA_LOGD("Exit %s", func);
    }

private:
    const char *func;
};
}

#endif
