#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>

#include <libdrm/xf86drm.h>
#include <libdrm/xf86drmMode.h>

#include <libdrm/util/kms.h>
#include <libdrm/util/common.h>

#include <libdrm/libdrm.h>

static int fbs;
static int edid;
static int modes;
static int crtcs;
static int current;
static int encoders;
static int full_modes;
static int connectors;
static int full_props;
static char *module_name;

static int printMode(struct drm_mode_modeinfo *mode)
{
    if (full_modes) {
        printf("Mode: %s\n", mode->name);
        printf("\tclock       : %i\n", mode->clock);
        printf("\thdisplay    : %i\n", mode->hdisplay);
        printf("\thsync_start : %i\n", mode->hsync_start);
        printf("\thsync_end   : %i\n", mode->hsync_end);
        printf("\thtotal      : %i\n", mode->htotal);
        printf("\thskew       : %i\n", mode->hskew);
        printf("\tvdisplay    : %i\n", mode->vdisplay);
        printf("\tvsync_start : %i\n", mode->vsync_start);
        printf("\tvsync_end   : %i\n", mode->vsync_end);
        printf("\tvtotal      : %i\n", mode->vtotal);
        printf("\tvscan       : %i\n", mode->vscan);
        printf("\tvrefresh    : %i\n", mode->vrefresh);
        printf("\tflags       : %i\n", mode->flags);
    } else {
        printf("Mode: \"%s\" %ix%i %i\n", mode->name, mode->hdisplay, mode->vdisplay, mode->vrefresh);
    }

    return 0;
}

static int printProperty(int fd, drmModeResPtr res, drmModePropertyPtr props, uint64_t value)
{
    int j;
    const char *name = NULL;

    printf("Property: %s\n", props->name);
    printf("\tid           : %i\n", props->prop_id);
    printf("\tflags        : %i\n", props->flags);
    printf("\tcount_values : %d\n", props->count_values);


    if (props->count_values) {
        printf("\tvalues       :");
        for (j = 0; j < props->count_values; j++) {
            printf(" %" PRIu64, props->values[j]);
        }
        printf("\n");
    }

    printf("\tcount_enums  : %d\n", props->count_enums);

    if (props->flags & DRM_MODE_PROP_BLOB) {
        drmModePropertyBlobPtr blob;

        blob = drmModeGetPropertyBlob(fd, value);
        if (blob) {
            printf("blob is %d length, %08X\n", blob->length, *(uint32_t *)blob->data);
            drmModeFreePropertyBlob(blob);
        } else {
            printf("error getting blob %" PRIu64 "\n", value);
        }
    } else {
        for (j = 0; j < props->count_enums; j++) {
            printf("\t\t%" PRIu64" = %s\n", (uint64_t)props->enums[j].value, props->enums[j].name);
            if (props->enums[j].value == value) {
                name = props->enums[j].name;
            }
        }

        if (props->count_enums && name) {
            printf("\tcon_value    : %s\n", name);
        } else {
            printf("\tcon_value    : %" PRIu64 "\n", value);
        }
    }

    return 0;
}

static int printConnector(int fd, drmModeResPtr res, drmModeConnectorPtr connector, uint32_t id)
{
    int i = 0;
    drmModePropertyPtr props;
    struct drm_mode_modeinfo *mode = NULL;
    const char *connector_type_name = NULL;

    connector_type_name = drmModeGetConnectorTypeName(connector->connector_type);

    if (connector_type_name) {
        printf("Connector: %s-%d\n", connector_type_name, connector->connector_type_id);
    } else {
        printf("Connector: %d-%d\n", connector->connector_type, connector->connector_type_id);
    }

    printf("\tid             : %i\n", id);
    printf("\tencoder id     : %i\n", connector->encoder_id);
    printf("\tconn           : %s\n", util_lookup_connector_status_name(connector->connection));
    printf("\tsize           : %ix%i (mm)\n", connector->mmWidth, connector->mmHeight);
    printf("\tcount_modes    : %i\n", connector->count_modes);
    printf("\tcount_props    : %i\n", connector->count_props);

    if (connector->count_props) {
        printf("\tprops          :");
        for (i = 0; i < connector->count_props; i++) {
            printf(" %i", connector->props[i]);
        }
        printf("\n");
    }

    printf("\tcount_encoders : %i\n", connector->count_encoders);
    if (connector->count_encoders) {
        printf("\tencoders       :");
        for (i = 0; i < connector->count_encoders; i++) {
            printf(" %i", connector->encoders[i]);
        }
        printf("\n");
    }

    if (modes) {
        for (i = 0; i < connector->count_modes; i++) {
            mode = (struct drm_mode_modeinfo *)&connector->modes[i];
            printMode(mode);
        }
    }

    if (full_props) {
        for (i = 0; i < connector->count_props; i++) {
            props = drmModeGetProperty(fd, connector->props[i]);
            if (props) {
                printProperty(fd, res, props, connector->prop_values[i]);
                drmModeFreeProperty(props);
            }
        }
    }

    return 0;
}

static int printEncoder(int fd, drmModeResPtr res, drmModeEncoderPtr encoder, uint32_t id)
{
    const char *encoder_name;

    encoder_name = util_lookup_encoder_type_name(encoder->encoder_type);
    if (encoder_name) {
        printf("Encoder: %s\n", encoder_name);
    } else {
        printf("Encoder\n");
    }

    printf("\tid              :%i\n", id);
    printf("\tcrtc_id         :%d\n", encoder->crtc_id);
    printf("\ttype            :%d\n", encoder->encoder_type);
    printf("\tpossible_crtcs  :0x%x\n", encoder->possible_crtcs);
    printf("\tpossible_clones :0x%x\n", encoder->possible_clones);
    return 0;
}

static int printCrtc(int fd, drmModeResPtr res, drmModeCrtcPtr crtc, uint32_t id)
{
    printf("Crtc\n");
    printf("\tid             : %i\n", id);
    printf("\tx              : %i\n", crtc->x);
    printf("\ty              : %i\n", crtc->y);
    printf("\twidth          : %i\n", crtc->width);
    printf("\theight         : %i\n", crtc->height);
    printf("\tmode           : %p\n", &crtc->mode);
    printf("\tgamma size     : %d\n", crtc->gamma_size);

    return 0;
}

static int printFrameBuffer(int fd, drmModeResPtr res, drmModeFBPtr fb)
{
    printf("Framebuffer\n");
    printf("\thandle    : %i\n", fb->handle);
    printf("\twidth     : %i\n", fb->width);
    printf("\theight    : %i\n", fb->height);
    printf("\tpitch     : %i\n", fb->pitch);
    printf("\tbpp       : %i\n", fb->bpp);
    printf("\tdepth     : %i\n", fb->depth);
    printf("\tbuffer_id : %i\n", fb->handle);

    return 0;
}

static int printRes(int fd, drmModeResPtr res)
{
    int i;
    drmModeFBPtr fb;
    drmModeCrtcPtr crtc;
    drmModeEncoderPtr encoder;
    drmModeConnectorPtr connector;

    printf("Resources\n\n");

    printf("count_connectors : %i\n", res->count_connectors);
    printf("count_encoders   : %i\n", res->count_encoders);
    printf("count_crtcs      : %i\n", res->count_crtcs);
    printf("count_fbs        : %i\n", res->count_fbs);

    printf("\n");

    if (connectors) {
        for (i = 0; i < res->count_connectors; i++) {
            connector = (current ? drmModeGetConnectorCurrent : drmModeGetConnector) (fd, res->connectors[i]);

            if (!connector) {
                printf("Could not get connector %i\n", res->connectors[i]);
            } else {
                printConnector(fd, res, connector, res->connectors[i]);
                drmModeFreeConnector(connector);
            }
        }
        printf("\n");
    }

    if (encoders) {
        for (i = 0; i < res->count_encoders; i++) {
            encoder = drmModeGetEncoder(fd, res->encoders[i]);
            if (!encoder) {
                printf("Could not get encoder %i\n", res->encoders[i]);
            } else {
                printEncoder(fd, res, encoder, res->encoders[i]);
                drmModeFreeEncoder(encoder);
            }
        }
        printf("\n");
    }

    if (crtcs) {
        for (i = 0; i < res->count_crtcs; i++) {
            crtc = drmModeGetCrtc(fd, res->crtcs[i]);

            if (!crtc) {
                printf("Could not get crtc %i\n", res->crtcs[i]);
            } else {
                printCrtc(fd, res, crtc, res->crtcs[i]);
                drmModeFreeCrtc(crtc);
            }
        }
        printf("\n");
    }

    if (fbs) {
        for (i = 0; i < res->count_fbs; i++) {
            fb = drmModeGetFB(fd, res->fbs[i]);
            if (!fb) {
                printf("Could not get fb %i\n", res->fbs[i]);
            } else {
                printFrameBuffer(fd, res, fb);
                drmModeFreeFB(fb);
            }
        }
    }

    return 0;
}

static void args(int argc, char **argv)
{
    int i, defaults = 1;

    fbs = 0;
    edid = 0;
    crtcs = 0;
    modes = 0;
    encoders = 0;
    full_modes = 0;
    full_props = 0;
    connectors = 0;
    current = 0;

    module_name = argv[1];

    for (i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-fb") == 0) {
            fbs = 1;
            defaults = 0;
        } else if (strcmp(argv[i], "-crtcs") == 0) {
            crtcs = 1;
            defaults = 0;
        } else if (strcmp(argv[i], "-cons") == 0) {
            connectors = 1;
            modes = 1;
            defaults = 0;
        } else if (strcmp(argv[i], "-modes") == 0) {
            connectors = 1;
            modes = 1;
            defaults = 0;
        } else if (strcmp(argv[i], "-full") == 0) {
            connectors = 1;
            modes = 1;
            full_modes = 1;
            defaults = 0;
        } else if (strcmp(argv[i], "-props") == 0) {
            connectors = 1;
            full_props = 1;
            defaults = 0;
        } else if (strcmp(argv[i], "-edids") == 0) {
            connectors = 1;
            edid = 1;
            defaults = 0;
        } else if (strcmp(argv[i], "-encoders") == 0) {
            encoders = 1;
            defaults = 0;
        } else if (strcmp(argv[i], "-v") == 0) {
            fbs = 1;
            edid = 1;
            crtcs = 1;
            modes = 1;
            encoders = 1;
            full_modes = 1;
            full_props = 1;
            connectors = 1;
            defaults = 0;
        } else if (strcmp(argv[i], "-current") == 0) {
            current = 1;
        }
    }

    if (defaults) {
        fbs = 1;
        edid = 1;
        crtcs = 1;
        modes = 1;
        encoders = 1;
        full_modes = 0;
        full_props = 0;
        connectors = 1;
    }
}

int libdrm_modeprint_main(int argc, char **argv)
{
    int fd;
    drmModeResPtr res;

    if (argc == 1) {
        printf("Please add modulename as first argument\n");
        return 1;
    }

    args(argc, argv);

    printf("Starting test\n");

    fd = drmOpen(module_name, NULL);
    if (fd < 0) {
        printf("Failed to open the card fd (%d)\n",fd);
        return 1;
    }

    res = drmModeGetResources(fd);
    if (res == 0) {
        printf("Failed to get resources from card\n");
        drmClose(fd);
        return 1;
    }

    printRes(fd, res);
    drmModeFreeResources(res);

    printf("Ok\n");
    return 0;
}
