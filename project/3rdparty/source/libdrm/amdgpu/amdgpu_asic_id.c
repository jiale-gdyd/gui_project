#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <libdrm/xf86drm.h>
#include <libdrm/drm/amdgpu_drm.h>

#include "amdgpu_internal.h"

static int parse_one_line(struct amdgpu_device *dev, const char *line)
{
    char *s_did;
    char *s_rid;
    uint32_t did;
    uint32_t rid;
    char *s_name;
    char *endptr;
    int r = -EINVAL;
    char *buf, *saveptr;

    if ((strlen(line) == 0) || (line[0] == '#')) {
        return -EAGAIN;
    }

    buf = strdup(line);
    if (!buf) {
        return -ENOMEM;
    }

    s_did = strtok_r(buf, ",", &saveptr);
    if (!s_did) {
        goto out;
    }

    did = strtol(s_did, &endptr, 16);
    if (*endptr) {
        goto out;
    }

    if (did != dev->info.asic_id) {
        r = -EAGAIN;
        goto out;
    }

    s_rid = strtok_r(NULL, ",", &saveptr);
    if (!s_rid) {
        goto out;
    }

    rid = strtol(s_rid, &endptr, 16);
    if (*endptr) {
        goto out;
    }

    if (rid != dev->info.pci_rev_id) {
        r = -EAGAIN;
        goto out;
    }

    s_name = strtok_r(NULL, ",", &saveptr);
    if (!s_name) {
        goto out;
    }

    while (isblank(*s_name)) {
        s_name++;
    }

    if (strlen(s_name) == 0) {
        goto out;
    }

    dev->marketing_name = strdup(s_name);
    if (dev->marketing_name) {
        r = 0;
    } else {
        r = -ENOMEM;
    }

out:
    free(buf);
    return r;
}

void amdgpu_parse_asic_ids(struct amdgpu_device *dev)
{
    FILE *fp;
    ssize_t n;
    int r = 0;
    size_t len = 0;
    int line_num = 1;
    char *line = NULL;
    const char *AMDGPU_ASIC_ID_TABLE = "/etc/amdgpu.ids";

    fp = fopen(AMDGPU_ASIC_ID_TABLE, "r");
    if (!fp) {
        fprintf(stderr, "open:[%s] failed, errstr:[%s]\n", AMDGPU_ASIC_ID_TABLE, strerror(errno));
        return;
    }

    while ((n = getline(&line, &len, fp)) != -1) {
        if (line[n - 1] == '\n') {
            line[n - 1] = '\0';
        }

        if ((strlen(line) == 0) || (line[0] == '#')) {
            line_num++;
            continue;
        }

        break;
    }

    while ((n = getline(&line, &len, fp)) != -1) {
        if (line[n - 1] == '\n') {
            line[n - 1] = '\0';
        }

        r = parse_one_line(dev, line);
        if (r != -EAGAIN) {
            break;
        }

        line_num++;
    }

    free(line);
    fclose(fp);
}
