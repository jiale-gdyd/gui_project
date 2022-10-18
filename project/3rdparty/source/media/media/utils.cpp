#include <fcntl.h>
#include <stdio.h>
#include <getopt.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <sys/inotify.h>

#include <sstream>
#include <algorithm>

#include <media/utils/utils.h>

short g_level_list[MOD_ID_BUTT] = {
    DRM_LOG_LEVEL_INFO,
    DRM_LOG_LEVEL_INFO,
    DRM_LOG_LEVEL_INFO,
};

#define LOG_FILE_PATH       "/tmp/loglevel"

static bool monitor_log_level_quit = false;
static int media_log_level = DRM_LOG_LEVEL_INFO;

static void read_log_level()
{
    int log_level;
    char text[20], module[10];

    int fd = open(LOG_FILE_PATH, O_RDONLY);
    if (fd == -1) {
        return;
    }

    int len = read(fd, text, 20);
    close(fd);
    if (len < 5) {
        DRM_MEDIA_LOGE("cmd too short, example: echo \"all=4\" > /tmp/loglevel");
        return;
    }

    DRM_MEDIA_LOGI("text is %s\n", text);
    sscanf(text, "%[^=]=%d", module, &log_level);
    DRM_MEDIA_LOGI("module is %s, log_level is %d", module, log_level);

    if (!strcmp(module, "unknow")) {
        g_level_list[MOD_ID_UNKNOW] = log_level;
    } else if (!strcmp(module, "sys")) {
        g_level_list[MOD_ID_SYS] = log_level;
    } else if (!strcmp(module, "vo")) {
        g_level_list[MOD_ID_VO] = log_level;
    } else if (!strcmp(module, "all")) {
        for (int i = 0; i < MOD_ID_BUTT; i++) {
            g_level_list[i] = log_level;
        }
    } else {
        DRM_MEDIA_LOGE("unknow module is %s", module);
    }
}

static void *monitor_log_level(void *arg)
{
    int fd;
    int wd;
    char buf[BUFSIZ];
    struct inotify_event *event;

    fd = inotify_init();
    if (fd < 0) {
        fprintf(stderr, "inotify_init failed\n");
        return NULL;
    }

    wd = inotify_add_watch(fd, LOG_FILE_PATH, IN_CLOSE_WRITE);
    if (wd < 0) {
        fprintf(stderr, "inotify_add_watch failed\n");
        return NULL;
    }

    buf[sizeof(buf) - 1] = 0;

    fd_set rfds;
    int nfds = wd + 1;
    struct timeval timeout;

    while (!monitor_log_level_quit) {
        timeout.tv_sec = 1;

        FD_ZERO(&rfds);
        FD_SET(wd, &rfds);
        select(nfds, &rfds, NULL, NULL, &timeout);

        if (FD_ISSET(wd, &rfds)) {
            int nread = 0;
            int len = read(fd, buf, sizeof(buf) - 1);

            while (len > 0) {
                event = (struct inotify_event *)&buf[nread];
                read_log_level();
                nread = nread + sizeof(struct inotify_event) + event->len;
                len = len - sizeof(struct inotify_event) - event->len;
            }
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);

    return NULL;
}

void drm_log_init()
{
    char *ptr = NULL;

    ptr = getenv("DRM_MEDIA_LOG_LEVEL");
    if (ptr && strstr(ptr, "ERROR")) {
        media_log_level = DRM_LOG_LEVEL_ERR;
    } else if (ptr && strstr(ptr, "WARN")) {
        media_log_level = DRM_LOG_LEVEL_WARN;
    } else if (ptr && strstr(ptr, "DBG")) {
        media_log_level = DRM_LOG_LEVEL_DEBUG;
    } else {
        media_log_level = DRM_LOG_LEVEL_INFO;
    }

    fprintf(stderr, "DRM MEDIA Log level: %d\n", media_log_level);

    int log_file_fd = open(LOG_FILE_PATH, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (log_file_fd != -1) {
        char init_log_level[6];
        sprintf(init_log_level, "all=%d", media_log_level);

        write(log_file_fd, init_log_level, 6);
        close(log_file_fd);

        read_log_level();

        pthread_t log_level_read_thread;
        pthread_create(&log_level_read_thread, NULL, monitor_log_level, NULL);
    } else {
        DRM_MEDIA_LOGE("open " LOG_FILE_PATH " failed");
    }
}

void drm_log_exit()
{
    monitor_log_level_quit = true;
}

namespace libdrm {
bool parse_media_param_map(const char *param, std::map<std::string, std::string> &map)
{
    if (!param) {
        return false;
    }

    std::string token;
    std::istringstream tokenStream(param);

    while (std::getline(tokenStream, token)) {
        std::string key, value;
        std::istringstream subTokenStream(token);
        if (std::getline(subTokenStream, key, '=')) {
            std::getline(subTokenStream, value);
        }

        map[key] = value;
    }

    return true;
}

bool parse_media_param_list(const char *param, std::list<std::string> &list, const char delim)
{
    if (!param) {
        return false;
    }

    std::string token;
    std::istringstream tokenStream(param);
    while (std::getline(tokenStream, token, delim)) {
        list.push_back(token);
    }

    return true;
}

int parse_media_param_match(const char *param, std::map<std::string, std::string> &map, std::list<std::pair<const std::string, std::string &>> &list)
{
    if (!libdrm::parse_media_param_map(param, map)) {
        return 0;
    }

    int match_num = 0;
    for (auto &key : list) {
        const std::string &req_key = key.first;
        for (auto &entry : map) {
            const std::string &entry_key = entry.first;
            if (req_key == entry_key) {
                key.second = entry.second;
                match_num++;
                break;
            }
        }
    }

    return match_num;
}

bool has_intersection(const char *str, const char *expect, std::list<std::string> *expect_list)
{
    std::list<std::string> request;

    if (!expect || (strlen(expect) == 0)) {
        return true;
    }

    if (!parse_media_param_list(str, request, ',')) {
        return false;
    }

    if (expect_list->empty() && !parse_media_param_list(expect, *expect_list)) {
        return false;
    }

    for (auto &req : request) {
        if (std::find(expect_list->begin(), expect_list->end(), req) != expect_list->end()) {
            return true;
        }
    }

    return false;
}

std::string get_media_value_by_key(const char *param, const char *key)
{
    std::map<std::string, std::string> param_map;
    if (!parse_media_param_map(param, param_map)) {
        return std::string();
    }

    return param_map[key];
}

bool string_start_withs(std::string const &fullString, std::string const &starting)
{
    if (fullString.length() >= starting.length()) {
        return (0 == fullString.find(starting));
    } else {
        return false;
    }
}

bool string_end_withs(std::string const &fullString, std::string const &ending)
{
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

bool DumpToFile(std::string path, const char *ptr, size_t len)
{
    int fd = open(path.c_str(), O_CREAT | O_WRONLY | O_FSYNC, 0666);
    if (fd < 0) {
        DRM_MEDIA_LOGI("Fail to open %s", path.c_str());
        return false;
    }

    DRM_MEDIA_LOGD("dump to %s, size %d", path.c_str(), (int)len);

    write(fd, ptr, len);
    close(fd);

    return true;
}
}
