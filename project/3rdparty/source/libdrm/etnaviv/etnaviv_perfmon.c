#include "etnaviv_priv.h"

static int etna_perfmon_query_signals(struct etna_perfmon *pm, struct etna_perfmon_domain *dom)
{
    struct etna_device *dev = pm->pipe->gpu->dev;
    struct drm_etnaviv_pm_signal req = {
        .pipe   = pm->pipe->id,
        .domain = dom->id
    };

    do {
        int ret;
        struct etna_perfmon_signal *sig;

        ret = drmCommandWriteRead(dev->fd, DRM_ETNAVIV_PM_QUERY_SIG, &req, sizeof(req));
        if (ret) {
            break;
        }

        sig = calloc(1, sizeof(*sig));
        if (!sig) {
            return -ENOMEM;
        }

        INFO_MSG("perfmon signal:");
        INFO_MSG("id         = %d", req.id);
        INFO_MSG("name       = %s", req.name);

        sig->domain = dom;
        sig->signal = req.id;
        strncpy(sig->name, req.name, sizeof(sig->name));
        list_addtail(&sig->head, &dom->signals);
    } while (req.iter != 0xffff);

    return 0;
}

static int etna_perfmon_query_domains(struct etna_perfmon *pm)
{
    struct etna_device *dev = pm->pipe->gpu->dev;
    struct drm_etnaviv_pm_domain req = {
        .pipe = pm->pipe->id
    };

    do {
        int ret;
        struct etna_perfmon_domain *dom;

        ret = drmCommandWriteRead(dev->fd, DRM_ETNAVIV_PM_QUERY_DOM, &req, sizeof(req));
        if (ret) {
            break;
        }

        dom = calloc(1, sizeof(*dom));
        if (!dom) {
            return -ENOMEM;
        }

        list_inithead(&dom->signals);
        dom->id = req.id;
        strncpy(dom->name, req.name, sizeof(dom->name));
        list_addtail(&dom->head, &pm->domains);

        INFO_MSG("perfmon domain:");
        INFO_MSG("id         = %d", req.id);
        INFO_MSG("name       = %s", req.name);
        INFO_MSG("nr_signals = %d", req.nr_signals);

        if (req.nr_signals > 0) {
            ret = etna_perfmon_query_signals(pm, dom);
            if (ret) {
                return ret;
            }
        }
    } while (req.iter != 0xff);

    return 0;
}

static void etna_perfmon_free_signals(struct etna_perfmon_domain *dom)
{
    struct etna_perfmon_signal *sig, *next;

    LIST_FOR_EACH_ENTRY_SAFE(sig, next, &dom->signals, head) {
        list_del(&sig->head);
        free(sig);
    }
}

static void etna_perfmon_free_domains(struct etna_perfmon *pm)
{
    struct etna_perfmon_domain *dom, *next;

    LIST_FOR_EACH_ENTRY_SAFE(dom, next, &pm->domains, head) {
        etna_perfmon_free_signals(dom);
        list_del(&dom->head);
        free(dom);
    }
}

drm_public struct etna_perfmon *etna_perfmon_create(struct etna_pipe *pipe)
{
    int ret;
    struct etna_perfmon *pm;

    pm = calloc(1, sizeof(*pm));
    if (!pm) {
        ERROR_MSG("allocation failed");
        return NULL;
    }

    list_inithead(&pm->domains);
    pm->pipe = pipe;

    ret = etna_perfmon_query_domains(pm);
    if (ret) {
        goto fail;
    }

    return pm;

fail:
    etna_perfmon_del(pm);
    return NULL;
}

drm_public void etna_perfmon_del(struct etna_perfmon *pm)
{
    if (!pm) {
        return;
    }

    etna_perfmon_free_domains(pm);
    free(pm);
}

drm_public struct etna_perfmon_domain *etna_perfmon_get_dom_by_name(struct etna_perfmon *pm, const char *name)
{
    struct etna_perfmon_domain *dom;

    if (pm) {
        LIST_FOR_EACH_ENTRY(dom, &pm->domains, head) {
            if (!strcmp(dom->name, name)) {
                return dom;
            }
        }
    }

    return NULL;
}

drm_public struct etna_perfmon_signal *etna_perfmon_get_sig_by_name(struct etna_perfmon_domain *dom, const char *name)
{
    struct etna_perfmon_signal *signal;

    if (dom) {
        LIST_FOR_EACH_ENTRY(signal, &dom->signals, head) {
            if (!strcmp(signal->name, name)) {
                return signal;
            }
        }
    }

    return NULL;
}
