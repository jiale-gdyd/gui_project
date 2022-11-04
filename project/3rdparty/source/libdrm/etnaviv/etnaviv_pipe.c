#include "etnaviv_priv.h"

drm_public int etna_pipe_wait(struct etna_pipe *pipe, uint32_t timestamp, uint32_t ms)
{
    return etna_pipe_wait_ns(pipe, timestamp, ms * 1000000);
}

drm_public int etna_pipe_wait_ns(struct etna_pipe *pipe, uint32_t timestamp, uint64_t ns)
{
    int ret;
    struct etna_device *dev = pipe->gpu->dev;

    struct drm_etnaviv_wait_fence req = {
        .pipe  = pipe->gpu->core,
        .fence = timestamp,
    };

    if (ns == 0) {
        req.flags |= ETNA_WAIT_NONBLOCK;
    }

    get_abs_timeout(&req.timeout, ns);

    ret = drmCommandWrite(dev->fd, DRM_ETNAVIV_WAIT_FENCE, &req, sizeof(req));
    if (ret) {
        ERROR_MSG("wait-fence failed! %d (%s)", ret, strerror(errno));
        return ret;
    }

    return 0;
}

drm_public void etna_pipe_del(struct etna_pipe *pipe)
{
    free(pipe);
}

drm_public struct etna_pipe *etna_pipe_new(struct etna_gpu *gpu, enum etna_pipe_id id)
{
    struct etna_pipe *pipe;

    pipe = calloc(1, sizeof(*pipe));
    if (!pipe) {
        ERROR_MSG("allocation failed");
        goto fail;
    }

    pipe->id = id;
    pipe->gpu = gpu;

    return pipe;

fail:
    return NULL;
}
