#include <glob.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <libv4l/libv4l2.h>
#include <libv4l/libv4l-plugin.h>

#include "../config.h"
#include "libv4l2-priv.h"

#define PLUGINS_PATTERN             LIBV4L2_PLUGIN_DIR "/*.so"

void v4l2_plugin_init(int fd, void **plugin_lib_ret, void **plugin_priv_ret, const struct libv4l_dev_ops **dev_ops_ret)
{
    char *error;
    glob_t globbuf;
    int glob_ret, i;
    void *plugin_library = NULL;
    const struct libv4l_dev_ops *libv4l2_plugin = NULL;

    *dev_ops_ret = v4lconvert_get_default_dev_ops();
    *plugin_lib_ret = NULL;
    *plugin_priv_ret = NULL;

    glob_ret = glob(PLUGINS_PATTERN, 0, NULL, &globbuf);
    if (glob_ret == GLOB_NOSPACE) {
        return;
    }

    if ((glob_ret == GLOB_ABORTED) || (glob_ret == GLOB_NOMATCH)) {
        goto leave;
    }

    for (i = 0; i < globbuf.gl_pathc; i++) {
        V4L2_LOG("PLUGIN: dlopen(%s);\n", globbuf.gl_pathv[i]);

        plugin_library = dlopen(globbuf.gl_pathv[i], RTLD_LAZY);
        if (!plugin_library) {
            continue;
        }

        dlerror();
        libv4l2_plugin = (struct libv4l_dev_ops *)dlsym(plugin_library, "libv4l2_plugin");
        error = dlerror();
        if (error != NULL)  {
            V4L2_LOG_ERR("PLUGIN: dlsym failed: %s\n", error);
            dlclose(plugin_library);
            continue;
        }

        if (!libv4l2_plugin->init || !libv4l2_plugin->close || !libv4l2_plugin->ioctl) {
            V4L2_LOG("PLUGIN: does not have all mandatory ops\n");
            dlclose(plugin_library);
            continue;
        }

        *plugin_priv_ret = libv4l2_plugin->init(fd);
        if (!*plugin_priv_ret) {
            V4L2_LOG("PLUGIN: plugin open() returned NULL\n");
            dlclose(plugin_library);
            continue;
        }

        *plugin_lib_ret = plugin_library;
        *dev_ops_ret = libv4l2_plugin;
        break;
    }

leave:
    globfree(&globbuf);
}

void v4l2_plugin_cleanup(void *plugin_lib, void *plugin_priv, const struct libv4l_dev_ops *dev_ops)
{
    if (plugin_lib) {
        dev_ops->close(plugin_priv);
        dlclose(plugin_lib);
    }
}
