#include "etnaviv_priv.h"

static uint64_t get_param(struct etna_device *dev, uint32_t core, uint32_t param)
{
    int ret;
    struct drm_etnaviv_param req = {
        .pipe  = core,
        .param = param,
    };

    ret = drmCommandWriteRead(dev->fd, DRM_ETNAVIV_GET_PARAM, &req, sizeof(req));
    if (ret) {
        ERROR_MSG("get-param (%x) failed! %d (%s)", param, ret, strerror(errno));
        return 0;
    }

    return req.value;
}

drm_public struct etna_gpu *etna_gpu_new(struct etna_device *dev, unsigned int core)
{
    struct etna_gpu *gpu;

    gpu = calloc(1, sizeof(*gpu));
    if (!gpu) {
        ERROR_MSG("allocation failed");
        goto fail;
    }

    gpu->dev = dev;
    gpu->core = core;

    gpu->model = get_param(dev, core, ETNAVIV_PARAM_GPU_MODEL);
    gpu->revision = get_param(dev, core, ETNAVIV_PARAM_GPU_REVISION);

    if (!gpu->model) {
        goto fail;
    }

    INFO_MSG(" GPU model:          0x%x (rev %x)", gpu->model, gpu->revision);

    return gpu;

fail:
    if (gpu) {
        etna_gpu_del(gpu);
    }

    return NULL;
}

drm_public void etna_gpu_del(struct etna_gpu *gpu)
{
    free(gpu);
}

drm_public int etna_gpu_get_param(struct etna_gpu *gpu, enum etna_param_id param, uint64_t *value)
{
    unsigned int core = gpu->core;
    struct etna_device *dev = gpu->dev;

    switch(param) {
        case ETNA_GPU_MODEL:
            *value = gpu->model;
            return 0;

        case ETNA_GPU_REVISION:
            *value = gpu->revision;
            return 0;

        case ETNA_GPU_FEATURES_0:
            *value = get_param(dev, core, ETNAVIV_PARAM_GPU_FEATURES_0);
            return 0;

        case ETNA_GPU_FEATURES_1:
            *value = get_param(dev, core, ETNAVIV_PARAM_GPU_FEATURES_1);
            return 0;

        case ETNA_GPU_FEATURES_2:
            *value = get_param(dev, core, ETNAVIV_PARAM_GPU_FEATURES_2);
            return 0;

        case ETNA_GPU_FEATURES_3:
            *value = get_param(dev, core, ETNAVIV_PARAM_GPU_FEATURES_3);
            return 0;

        case ETNA_GPU_FEATURES_4:
            *value = get_param(dev, core, ETNAVIV_PARAM_GPU_FEATURES_4);
            return 0;

        case ETNA_GPU_FEATURES_5:
            *value = get_param(dev, core, ETNAVIV_PARAM_GPU_FEATURES_5);
            return 0;

        case ETNA_GPU_FEATURES_6:
            *value = get_param(dev, core, ETNAVIV_PARAM_GPU_FEATURES_6);
            return 0;

        case ETNA_GPU_STREAM_COUNT:
            *value = get_param(dev, core, ETNA_GPU_STREAM_COUNT);
            return 0;

        case ETNA_GPU_REGISTER_MAX:
            *value = get_param(dev, core, ETNA_GPU_REGISTER_MAX);
            return 0;

        case ETNA_GPU_THREAD_COUNT:
            *value = get_param(dev, core, ETNA_GPU_THREAD_COUNT);
            return 0;

        case ETNA_GPU_VERTEX_CACHE_SIZE:
            *value = get_param(dev, core, ETNA_GPU_VERTEX_CACHE_SIZE);
            return 0;

        case ETNA_GPU_SHADER_CORE_COUNT:
            *value = get_param(dev, core, ETNA_GPU_SHADER_CORE_COUNT);
            return 0;

        case ETNA_GPU_PIXEL_PIPES:
            *value = get_param(dev, core, ETNA_GPU_PIXEL_PIPES);
            return 0;

        case ETNA_GPU_VERTEX_OUTPUT_BUFFER_SIZE:
            *value = get_param(dev, core, ETNA_GPU_VERTEX_OUTPUT_BUFFER_SIZE);
            return 0;

        case ETNA_GPU_BUFFER_SIZE:
            *value = get_param(dev, core, ETNA_GPU_BUFFER_SIZE);
            return 0;

        case ETNA_GPU_INSTRUCTION_COUNT:
            *value = get_param(dev, core, ETNA_GPU_INSTRUCTION_COUNT);
            return 0;

        case ETNA_GPU_NUM_CONSTANTS:
            *value = get_param(dev, core, ETNA_GPU_NUM_CONSTANTS);
            return 0;

        case ETNA_GPU_NUM_VARYINGS:
            *value = get_param(dev, core, ETNA_GPU_NUM_VARYINGS);
            return 0;

        default:
            ERROR_MSG("invalid param id: %d", param);
            return -1;
    }

    return 0;
}
