#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <linux/kconfig.h>

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <dirent.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <strings.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/sysmacros.h>

#define ARRAY_SIZE(a)           (sizeof(a) / sizeof((a)[0]))

#ifndef MAP_FAILED
#define MAP_FAILED              ((void *)-1)
#endif

#include <libdrm/xf86drm.h>
#include <libdrm/util_math.h>
#include <libdrm/drm_fourcc.h>
#include <libdrm/libdrm_macros.h>

#ifndef DRM_MAJOR
#define DRM_MAJOR               226
#endif

#define DRM_MSG_VERBOSITY       3

#define memclear(s)             memset(&s, 0, sizeof(s))

static drmServerInfoPtr drm_server_info;

static bool drmNodeIsDRM(int maj, int min);
static char *drmGetMinorNameForFD(int fd, int type);

#define DRM_MODIFIER(v, f, f_name)              \
    .modifier = DRM_FORMAT_MOD_##v ## _ ##f,    \
    .modifier_name = #f_name

#define DRM_MODIFIER_INVALID(v, f_name)         \
    .modifier = DRM_FORMAT_MOD_INVALID, .modifier_name = #f_name

#define DRM_MODIFIER_LINEAR(v, f_name)          \
    .modifier = DRM_FORMAT_MOD_LINEAR, .modifier_name = #f_name

#define DRM_MODIFIER_INTEL(f, f_name)           \
    .modifier = I915_FORMAT_MOD_##f, .modifier_name = #f_name

struct drmFormatModifierInfo {
    uint64_t   modifier;
    const char *modifier_name;
};

struct drmFormatModifierVendorInfo {
    uint8_t    vendor;
    const char *vendor_name;
};

static const struct drmFormatModifierInfo drm_format_modifier_table[] = {
    { DRM_MODIFIER_INVALID(NONE, INVALID) },
    { DRM_MODIFIER_LINEAR(NONE, LINEAR) },
    { DRM_MODIFIER_INTEL(X_TILED, X_TILED) },
    { DRM_MODIFIER_INTEL(Y_TILED, Y_TILED) },
    { DRM_MODIFIER_INTEL(Yf_TILED, Yf_TILED) },
    { DRM_MODIFIER_INTEL(Y_TILED_CCS, Y_TILED_CCS) },
    { DRM_MODIFIER_INTEL(Yf_TILED_CCS, Yf_TILED_CCS) },
    { DRM_MODIFIER_INTEL(Y_TILED_GEN12_RC_CCS, Y_TILED_GEN12_RC_CCS) },
    { DRM_MODIFIER_INTEL(Y_TILED_GEN12_MC_CCS, Y_TILED_GEN12_MC_CCS) },
    { DRM_MODIFIER_INTEL(Y_TILED_GEN12_RC_CCS_CC, Y_TILED_GEN12_RC_CCS_CC) },
    { DRM_MODIFIER(SAMSUNG, 64_32_TILE, 64_32_TILE) },
    { DRM_MODIFIER(SAMSUNG, 16_16_TILE, 16_16_TILE) },
    { DRM_MODIFIER(QCOM, COMPRESSED, COMPRESSED) },
    { DRM_MODIFIER(VIVANTE, TILED, TILED) },
    { DRM_MODIFIER(VIVANTE, SUPER_TILED, SUPER_TILED) },
    { DRM_MODIFIER(VIVANTE, SPLIT_TILED, SPLIT_TILED) },
    { DRM_MODIFIER(VIVANTE, SPLIT_SUPER_TILED, SPLIT_SUPER_TILED) },
    { DRM_MODIFIER(NVIDIA, TEGRA_TILED, TEGRA_TILED) },
    { DRM_MODIFIER(NVIDIA, 16BX2_BLOCK_ONE_GOB, 16BX2_BLOCK_ONE_GOB) },
    { DRM_MODIFIER(NVIDIA, 16BX2_BLOCK_TWO_GOB, 16BX2_BLOCK_TWO_GOB) },
    { DRM_MODIFIER(NVIDIA, 16BX2_BLOCK_FOUR_GOB, 16BX2_BLOCK_FOUR_GOB) },
    { DRM_MODIFIER(NVIDIA, 16BX2_BLOCK_EIGHT_GOB, 16BX2_BLOCK_EIGHT_GOB) },
    { DRM_MODIFIER(NVIDIA, 16BX2_BLOCK_SIXTEEN_GOB, 16BX2_BLOCK_SIXTEEN_GOB) },
    { DRM_MODIFIER(NVIDIA, 16BX2_BLOCK_THIRTYTWO_GOB, 16BX2_BLOCK_THIRTYTWO_GOB) },
    { DRM_MODIFIER(BROADCOM, VC4_T_TILED, VC4_T_TILED) },
    { DRM_MODIFIER(BROADCOM, SAND32, SAND32) },
    { DRM_MODIFIER(BROADCOM, SAND64, SAND64) },
    { DRM_MODIFIER(BROADCOM, SAND128, SAND128) },
    { DRM_MODIFIER(BROADCOM, SAND256, SAND256) },
    { DRM_MODIFIER(BROADCOM, UIF, UIF) },
    { DRM_MODIFIER(ARM, 16X16_BLOCK_U_INTERLEAVED, 16X16_BLOCK_U_INTERLEAVED) },
    { DRM_MODIFIER(ALLWINNER, TILED, TILED) },
};

static const struct drmFormatModifierVendorInfo drm_format_modifier_vendor_table[] = {
    { DRM_FORMAT_MOD_VENDOR_NONE, "NONE" },
    { DRM_FORMAT_MOD_VENDOR_INTEL, "INTEL" },
    { DRM_FORMAT_MOD_VENDOR_AMD, "AMD" },
    { DRM_FORMAT_MOD_VENDOR_NVIDIA, "NVIDIA" },
    { DRM_FORMAT_MOD_VENDOR_SAMSUNG, "SAMSUNG" },
    { DRM_FORMAT_MOD_VENDOR_QCOM, "QCOM" },
    { DRM_FORMAT_MOD_VENDOR_VIVANTE, "VIVANTE" },
    { DRM_FORMAT_MOD_VENDOR_BROADCOM, "BROADCOM" },
    { DRM_FORMAT_MOD_VENDOR_ARM, "ARM" },
    { DRM_FORMAT_MOD_VENDOR_ALLWINNER, "ALLWINNER" },
    { DRM_FORMAT_MOD_VENDOR_AMLOGIC, "AMLOGIC" },
};

struct drmVendorInfo {
    uint8_t vendor;
    char *(*vendor_cb)(uint64_t modifier);
};

struct drmFormatVendorModifierInfo {
    uint64_t   modifier;
    const char *modifier_name;
};

static char *drmGetFormatModifierNameFromArm(uint64_t modifier);
static char *drmGetFormatModifierNameFromNvidia(uint64_t modifier);
static char *drmGetFormatModifierNameFromAmd(uint64_t modifier);
static char *drmGetFormatModifierNameFromAmlogic(uint64_t modifier);
static char *drmGetFormatModifierNameFromVivante(uint64_t modifier);

static const struct drmVendorInfo modifier_format_vendor_table[] = {
    { DRM_FORMAT_MOD_VENDOR_ARM, drmGetFormatModifierNameFromArm },
    { DRM_FORMAT_MOD_VENDOR_NVIDIA, drmGetFormatModifierNameFromNvidia },
    { DRM_FORMAT_MOD_VENDOR_AMD, drmGetFormatModifierNameFromAmd },
    { DRM_FORMAT_MOD_VENDOR_AMLOGIC, drmGetFormatModifierNameFromAmlogic },
    { DRM_FORMAT_MOD_VENDOR_VIVANTE, drmGetFormatModifierNameFromVivante },
};

#ifndef AFBC_FORMAT_MOD_MODE_VALUE_MASK
#define AFBC_FORMAT_MOD_MODE_VALUE_MASK         0x000fffffffffffffULL
#endif

static const struct drmFormatVendorModifierInfo arm_mode_value_table[] = {
    { AFBC_FORMAT_MOD_YTR,    "YTR" },
    { AFBC_FORMAT_MOD_SPLIT,  "SPLIT" },
    { AFBC_FORMAT_MOD_SPARSE, "SPARSE" },
    { AFBC_FORMAT_MOD_CBR,    "CBR" },
    { AFBC_FORMAT_MOD_TILED,  "TILED" },
    { AFBC_FORMAT_MOD_SC,     "SC" },
    { AFBC_FORMAT_MOD_DB,     "DB" },
    { AFBC_FORMAT_MOD_BCH,    "BCH" },
    { AFBC_FORMAT_MOD_USM,    "USM" },
};

static bool is_x_t_amd_gfx9_tile(uint64_t tile)
{
    switch (tile) {
        case AMD_FMT_MOD_TILE_GFX9_64K_S_X:
        case AMD_FMT_MOD_TILE_GFX9_64K_D_X:
        case AMD_FMT_MOD_TILE_GFX9_64K_R_X:
            return true;
    }

    return false;
}

static bool drmGetAfbcFormatModifierNameFromArm(uint64_t modifier, FILE *fp)
{
    const char *mode = NULL;
    const char *block = NULL;
    bool did_print_mode = false;
    uint64_t mode_value = modifier & AFBC_FORMAT_MOD_MODE_VALUE_MASK;
    uint64_t block_size = mode_value & AFBC_FORMAT_MOD_BLOCK_SIZE_MASK;

    switch (block_size) {
        case AFBC_FORMAT_MOD_BLOCK_SIZE_16x16:
            block = "16x16";
            break;

        case AFBC_FORMAT_MOD_BLOCK_SIZE_32x8:
            block = "32x8";
            break;

        case AFBC_FORMAT_MOD_BLOCK_SIZE_64x4:
            block = "64x4";
            break;

        case AFBC_FORMAT_MOD_BLOCK_SIZE_32x8_64x4:
            block = "32x8_64x4";
            break;
    }

    if (!block) {
        return false;
    }

    fprintf(fp, "BLOCK_SIZE=%s,", block);

    for (unsigned int i = 0; i < ARRAY_SIZE(arm_mode_value_table); i++) {
        if (arm_mode_value_table[i].modifier & mode_value) {
            mode = arm_mode_value_table[i].modifier_name;
            if (!did_print_mode) {
                fprintf(fp, "MODE=%s", mode);
                did_print_mode = true;
            } else {
                fprintf(fp, "|%s", mode);
            }
        }
    }

    return true;
}

static bool drmGetAfrcFormatModifierNameFromArm(uint64_t modifier, FILE *fp)
{
    bool scan_layout;

    for (unsigned int i = 0; i < 2; ++i) {
        const char *coding_unit_size = NULL;
        uint64_t coding_unit_block = (modifier >> (i * 4)) & AFRC_FORMAT_MOD_CU_SIZE_MASK;

        switch (coding_unit_block) {
            case AFRC_FORMAT_MOD_CU_SIZE_16:
                coding_unit_size = "CU_16";
                break;

            case AFRC_FORMAT_MOD_CU_SIZE_24:
                coding_unit_size = "CU_24";
                break;

            case AFRC_FORMAT_MOD_CU_SIZE_32:
                coding_unit_size = "CU_32";
                break;
        }

        if (!coding_unit_size) {
            if (i == 0) {
                return false;
            }

            break;
        }

        if (i == 0) {
            fprintf(fp, "P0=%s,", coding_unit_size);
        } else {
            fprintf(fp, "P12=%s,", coding_unit_size);
        }
    }

    scan_layout = (modifier & AFRC_FORMAT_MOD_LAYOUT_SCAN) == AFRC_FORMAT_MOD_LAYOUT_SCAN;
    if (scan_layout) {
        fprintf(fp, "SCAN");
    } else {
        fprintf(fp, "ROT");
    }

    return true;
}

static char *drmGetFormatModifierNameFromArm(uint64_t modifier)
{
    FILE *fp;
    size_t size = 0;
    bool result = false;
    char *modifier_name = NULL;
    uint64_t type = (modifier >> 52) & 0xf;

    fp = open_memstream(&modifier_name, &size);
    if (!fp) {
        libdrm_error("open_memstream failed, errstr:[%m]");
        return NULL;
    }

    switch (type) {
        case DRM_FORMAT_MOD_ARM_TYPE_AFBC:
            result = drmGetAfbcFormatModifierNameFromArm(modifier, fp);
            break;

        case DRM_FORMAT_MOD_ARM_TYPE_AFRC:
            result = drmGetAfrcFormatModifierNameFromArm(modifier, fp);
            break;

        case DRM_FORMAT_MOD_ARM_TYPE_MISC:
        default:
            result = false;
            break;
    }

    fclose(fp);
    if (!result) {
        free(modifier_name);
        return NULL;
    }

    return modifier_name;
}

static char *drmGetFormatModifierNameFromNvidia(uint64_t modifier)
{
    uint64_t height, kind, gen, sector, compression;

    height = modifier & 0xf;
    kind = (modifier >> 12) & 0xff;

    gen = (modifier >> 20) & 0x3;
    sector = (modifier >> 22) & 0x1;
    compression = (modifier >> 23) & 0x7;

    if ((modifier & 0x10) == 0x10) {
        char *mod_nvidia;
        asprintf(&mod_nvidia, "BLOCK_LINEAR_2D,HEIGHT=%" PRIu64 ",KIND=%" PRIu64 "," "GEN=%" PRIu64 ",SECTOR=%" PRIu64 ",COMPRESSION=%" PRIu64 "", height, kind, gen, sector, compression);
        return mod_nvidia;
    }

    return NULL;
}

static void drmGetFormatModifierNameFromAmdDcc(uint64_t modifier, FILE *fp)
{
    const char *dcc_max_compressed_block_str = NULL;
    uint64_t dcc_max_compressed_block = AMD_FMT_MOD_GET(DCC_MAX_COMPRESSED_BLOCK, modifier);
    uint64_t dcc_retile = AMD_FMT_MOD_GET(DCC_RETILE, modifier);

    fprintf(fp, ",DCC");

    if (dcc_retile) {
        fprintf(fp, ",DCC_RETILE");
    }

    if (!dcc_retile && AMD_FMT_MOD_GET(DCC_PIPE_ALIGN, modifier)) {
        fprintf(fp, ",DCC_PIPE_ALIGN");
    }

    if (AMD_FMT_MOD_GET(DCC_INDEPENDENT_64B, modifier)) {
        fprintf(fp, ",DCC_INDEPENDENT_64B");
    }

    if (AMD_FMT_MOD_GET(DCC_INDEPENDENT_128B, modifier)) {
        fprintf(fp, ",DCC_INDEPENDENT_128B");
    }

    switch (dcc_max_compressed_block) {
        case AMD_FMT_MOD_DCC_BLOCK_64B:
            dcc_max_compressed_block_str = "64B";
            break;

        case AMD_FMT_MOD_DCC_BLOCK_128B:
            dcc_max_compressed_block_str = "128B";
            break;

        case AMD_FMT_MOD_DCC_BLOCK_256B:
            dcc_max_compressed_block_str = "256B";
            break;
    }

    if (dcc_max_compressed_block_str) {
        fprintf(fp, ",DCC_MAX_COMPRESSED_BLOCK=%s", dcc_max_compressed_block_str);
    }

    if (AMD_FMT_MOD_GET(DCC_CONSTANT_ENCODE, modifier)) {
        fprintf(fp, ",DCC_CONSTANT_ENCODE");
    }
}

static void drmGetFormatModifierNameFromAmdTile(uint64_t modifier, FILE *fp)
{
    uint64_t pipe_xor_bits, bank_xor_bits, packers, rb;
    uint64_t pipe, pipe_align, dcc, dcc_retile, tile_version;

    pipe_align = AMD_FMT_MOD_GET(DCC_PIPE_ALIGN, modifier);
    pipe_xor_bits = AMD_FMT_MOD_GET(PIPE_XOR_BITS, modifier);
    dcc = AMD_FMT_MOD_GET(DCC, modifier);
    dcc_retile = AMD_FMT_MOD_GET(DCC_RETILE, modifier);
    tile_version = AMD_FMT_MOD_GET(TILE_VERSION, modifier);

    fprintf(fp, ",PIPE_XOR_BITS=%" PRIu64, pipe_xor_bits);

    if (tile_version == AMD_FMT_MOD_TILE_VER_GFX9) {
        bank_xor_bits = AMD_FMT_MOD_GET(BANK_XOR_BITS, modifier);
        fprintf(fp, ",BANK_XOR_BITS=%" PRIu64, bank_xor_bits);
    }

    if (tile_version == AMD_FMT_MOD_TILE_VER_GFX10_RBPLUS) {
        packers = AMD_FMT_MOD_GET(PACKERS, modifier);
        fprintf(fp, ",PACKERS=%" PRIu64, packers);
    }

    if (dcc && tile_version == AMD_FMT_MOD_TILE_VER_GFX9) {
        rb = AMD_FMT_MOD_GET(RB, modifier);
        fprintf(fp, ",RB=%" PRIu64, rb);
    }

    if (dcc && tile_version == AMD_FMT_MOD_TILE_VER_GFX9 && (dcc_retile || pipe_align)) {
        pipe = AMD_FMT_MOD_GET(PIPE, modifier);
        fprintf(fp, ",PIPE_%" PRIu64, pipe);
    }
}

static char *drmGetFormatModifierNameFromAmd(uint64_t modifier)
{
    FILE *fp;
    size_t size = 0;
    char *mod_amd = NULL;
    const char *str_tile = NULL;
    uint64_t tile, tile_version, dcc;
    const char *str_tile_version = NULL;

    tile = AMD_FMT_MOD_GET(TILE, modifier);
    tile_version = AMD_FMT_MOD_GET(TILE_VERSION, modifier);
    dcc = AMD_FMT_MOD_GET(DCC, modifier);

    fp = open_memstream(&mod_amd, &size);
    if (!fp) {
        libdrm_error("open_memstream failed, errstr:[%m]");
        return NULL;
    }

    switch (tile_version) {
        case AMD_FMT_MOD_TILE_VER_GFX9:
            str_tile_version = "GFX9";
            break;

        case AMD_FMT_MOD_TILE_VER_GFX10:
            str_tile_version = "GFX10";
            break;

        case AMD_FMT_MOD_TILE_VER_GFX10_RBPLUS:
            str_tile_version = "GFX10_RBPLUS";
            break;

        case AMD_FMT_MOD_TILE_VER_GFX11:
            str_tile_version = "GFX11";
            break;
    }

    if (str_tile_version) {
        fprintf(fp, "%s", str_tile_version);
    } else {
        fclose(fp);
        free(mod_amd);
        return NULL;
    }

    switch (tile) {
        case AMD_FMT_MOD_TILE_GFX9_64K_S:
            str_tile = "GFX9_64K_S";
            break;

        case AMD_FMT_MOD_TILE_GFX9_64K_D:
            str_tile = "GFX9_64K_D";
            break;

        case AMD_FMT_MOD_TILE_GFX9_64K_S_X:
            str_tile = "GFX9_64K_S_X";
            break;

        case AMD_FMT_MOD_TILE_GFX9_64K_D_X:
            str_tile = "GFX9_64K_D_X";
            break;

        case AMD_FMT_MOD_TILE_GFX9_64K_R_X:
            str_tile = "GFX9_64K_R_X";
            break;

        case AMD_FMT_MOD_TILE_GFX11_256K_R_X:
            str_tile = "GFX11_256K_R_X";
            break;
    }

    if (str_tile) {
        fprintf(fp, ",%s", str_tile);
    }

    if (dcc) {
        drmGetFormatModifierNameFromAmdDcc(modifier, fp);
    }

    if (tile_version >= AMD_FMT_MOD_TILE_VER_GFX9 && is_x_t_amd_gfx9_tile(tile)) {
        drmGetFormatModifierNameFromAmdTile(modifier, fp);
    }

    fclose(fp);
    return mod_amd;
}

static char *drmGetFormatModifierNameFromAmlogic(uint64_t modifier)
{
    const char *opts_str;
    const char *layout_str;
    char *mod_amlogic = NULL;
    uint64_t layout = modifier & 0xff;
    uint64_t options = (modifier >> 8) & 0xff;

    switch (layout) {
        case AMLOGIC_FBC_LAYOUT_BASIC:
        layout_str = "BASIC";
        break;

        case AMLOGIC_FBC_LAYOUT_SCATTER:
        layout_str = "SCATTER";
        break;

        default:
        layout_str = "INVALID_LAYOUT";
        break;
    }

    if (options & AMLOGIC_FBC_OPTION_MEM_SAVING) {
        opts_str = "MEM_SAVING";
    } else {
        opts_str = "0";
    }

    asprintf(&mod_amlogic, "FBC,LAYOUT=%s,OPTIONS=%s", layout_str, opts_str);
    return mod_amlogic;
}

static char *drmGetFormatModifierNameFromVivante(uint64_t modifier)
{
    char *mod_vivante = NULL;
    const char *color_tiling, *tile_status, *compression;

    switch (modifier & VIVANTE_MOD_TS_MASK) {
        case 0:
            tile_status = "";
            break;

        case VIVANTE_MOD_TS_64_4:
            tile_status = ",TS=64B_4";
            break;

        case VIVANTE_MOD_TS_64_2:
            tile_status = ",TS=64B_2";
            break;

        case VIVANTE_MOD_TS_128_4:
            tile_status = ",TS=128B_4";
            break;

        case VIVANTE_MOD_TS_256_4:
            tile_status = ",TS=256B_4";
            break;

        default:
            tile_status = ",TS=UNKNOWN";
            break;
    }

    switch (modifier & VIVANTE_MOD_COMP_MASK) {
        case 0:
            compression = "";
            break;

        case VIVANTE_MOD_COMP_DEC400:
            compression = ",COMP=DEC400";
            break;

        default:
            compression = ",COMP=UNKNOWN";
            break;
    }

    switch (modifier & ~VIVANTE_MOD_EXT_MASK) {
        case 0:
            color_tiling = "LINEAR";
            break;

        case DRM_FORMAT_MOD_VIVANTE_TILED:
            color_tiling = "TILED";
            break;

        case DRM_FORMAT_MOD_VIVANTE_SUPER_TILED:
            color_tiling = "SUPER_TILED";
            break;

        case DRM_FORMAT_MOD_VIVANTE_SPLIT_TILED:
            color_tiling = "SPLIT_TILED";
            break;

        case DRM_FORMAT_MOD_VIVANTE_SPLIT_SUPER_TILED:
            color_tiling = "SPLIT_SUPER_TILED";
            break;

        default:
            color_tiling = "UNKNOWN";
            break;
    }

    asprintf((char **)&mod_vivante, "%s%s%s", color_tiling, tile_status, compression);
    return (char *)mod_vivante;
}

static unsigned log2_int(unsigned x)
{
    unsigned l;

    if (x < 2) {
        return 0;
    }

    for (l = 2; ; l++) {
        if ((unsigned)(1 << l) > x) {
            return l - 1;
        }
    }

    return 0;
}

drm_public void drmSetServerInfo(drmServerInfoPtr info)
{
    drm_server_info = info;
}

static int DRM_PRINTFLIKE(1, 0) drmDebugPrint(const char *format, va_list ap)
{
    return vfprintf(stderr, format, ap);
}

drm_public void drmMsg(const char *format, ...)
{
    va_list ap;
    const char *env;

    if (((env = getenv("LIBGL_DEBUG")) && strstr(env, "verbose")) || (drm_server_info && drm_server_info->debug_print)) {
        va_start(ap, format);
        if (drm_server_info) {
            drm_server_info->debug_print(format,ap);
        } else {
            drmDebugPrint(format, ap);
        }
        va_end(ap);
    }
}

static void *drmHashTable = NULL;

drm_public void *drmGetHashTable(void)
{
    return drmHashTable;
}

drm_public void *drmMalloc(int size)
{
    return calloc(1, size);
}

drm_public void drmFree(void *pt)
{
    free(pt);
}

drm_public int drmIoctl(int fd, unsigned long request, void *arg)
{
    int ret;

    do {
        ret = ioctl(fd, request, arg);
    } while ((ret == -1) && ((errno == EINTR) || (errno == EAGAIN)));

    return ret;
}

static unsigned long drmGetKeyFromFd(int fd)
{
    struct stat st;

    st.st_rdev = 0;
    fstat(fd, &st);

    return st.st_rdev;
}

drm_public drmHashEntry *drmGetEntry(int fd)
{
    void *value;
    drmHashEntry *entry;
    unsigned long key = drmGetKeyFromFd(fd);

    if (!drmHashTable) {
        drmHashTable = drmHashCreate();
    }

    if (drmHashLookup(drmHashTable, key, &value)) {
        entry = (drmHashEntry *)drmMalloc(sizeof(*entry));
        entry->fd = fd;
        entry->f = NULL;
        entry->tagTable = drmHashCreate();
        drmHashInsert(drmHashTable, key, entry);
    } else {
        entry = (drmHashEntry *)value;
    }

    return entry;
}

static int drmMatchBusID(const char *id1, const char *id2, int pci_domain_ok)
{
    if (strcasecmp(id1, id2) == 0) {
        return 1;
    }

    if (strncasecmp(id1, "pci", 3) == 0) {
        int ret;
        unsigned int o1, b1, d1, f1;
        unsigned int o2, b2, d2, f2;

        ret = sscanf(id1, "pci:%04x:%02x:%02x.%u", &o1, &b1, &d1, &f1);
        if (ret != 4) {
            o1 = 0;
            ret = sscanf(id1, "PCI:%u:%u:%u", &b1, &d1, &f1);
            if (ret != 3) {
                return 0;
            }
        }

        ret = sscanf(id2, "pci:%04x:%02x:%02x.%u", &o2, &b2, &d2, &f2);
        if (ret != 4) {
            o2 = 0;
            ret = sscanf(id2, "PCI:%u:%u:%u", &b2, &d2, &f2);
            if (ret != 3) {
                return 0;
            }
        }

        if (!pci_domain_ok) {
            o1 = o2 = 0;
        }

        if ((o1 != o2) || (b1 != b2) || (d1 != d2) || (f1 != f2)) {
            return 0;
        } else {
            return 1;
        }
    }

    return 0;
}

#if !UDEV
static int chown_check_return(const char *path, uid_t owner, gid_t group)
{
    int rv;

    do {
        rv = chown(path, owner, group);
    } while ((rv != 0) && (errno == EINTR));

    if (rv == 0) {
        return 0;
    }

    drmMsg("Failed to change owner or group for file %s! %d: %s\n", path, errno, strerror(errno));
    return -1;
}
#endif

static const char *drmGetDeviceName(int type)
{
    switch (type) {
        case DRM_NODE_PRIMARY:
            return DRM_DEV_NAME;

        case DRM_NODE_CONTROL:
            return DRM_CONTROL_DEV_NAME;

        case DRM_NODE_RENDER:
            return DRM_RENDER_DEV_NAME;
    }

    return NULL;
}

static int drmOpenDevice(dev_t dev, int minor, int type)
{
    int fd;
    struct stat st;
    gid_t serv_group;
    char buf[DRM_NODE_NAME_MAX];
    mode_t devmode = DRM_DEV_MODE, serv_mode;
    const char *dev_name = drmGetDeviceName(type);

#if !UDEV
    int isroot = !geteuid();
    uid_t user = DRM_DEV_UID;
    gid_t group = DRM_DEV_GID;
#endif

    if (!dev_name) {
        return -EINVAL;
    }

    sprintf(buf, dev_name, DRM_DIR_NAME, minor);
    drmMsg("drmOpenDevice: node name is %s\n", buf);

    if (drm_server_info && drm_server_info->get_perms) {
        drm_server_info->get_perms(&serv_group, &serv_mode);
        devmode = serv_mode ? serv_mode : DRM_DEV_MODE;
        devmode &= ~(S_IXUSR|S_IXGRP|S_IXOTH);
    }

#if !UDEV
    if (stat(DRM_DIR_NAME, &st)) {
        if (!isroot) {
            return DRM_ERR_NOT_ROOT;
        }

        mkdir(DRM_DIR_NAME, DRM_DEV_DIRMODE);
        chown_check_return(DRM_DIR_NAME, 0, 0);
        chmod(DRM_DIR_NAME, DRM_DEV_DIRMODE);
    }

    if (stat(buf, &st)) {
        if (!isroot) {
            return DRM_ERR_NOT_ROOT;
        }

        remove(buf);
        mknod(buf, S_IFCHR | devmode, dev);
    }

    if (drm_server_info && drm_server_info->get_perms) {
        group = ((int)serv_group >= 0) ? serv_group : DRM_DEV_GID;
        chown_check_return(buf, user, group);
        chmod(buf, devmode);
    }
#else
    {
        int udev_count = 0;
wait_for_udev:
        if (stat(DRM_DIR_NAME, &st)) {
            usleep(20);
            udev_count++;

            if (udev_count == 50) {
                return -1;
            }

            goto wait_for_udev;
        }

        if (stat(buf, &st)) {
            usleep(20);
            udev_count++;

            if (udev_count == 50) {
                return -1;
            }

            goto wait_for_udev;
        }
    }
#endif

    fd = open(buf, O_RDWR | O_CLOEXEC);
    drmMsg("drmOpenDevice: open result is %d, (%s)\n", fd, fd < 0 ? strerror(errno) : "OK");
    if (fd >= 0) {
        return fd;
    }

#if !UDEV
    if (st.st_rdev != dev) {
        if (!isroot) {
            return DRM_ERR_NOT_ROOT;
        }

        remove(buf);
        mknod(buf, S_IFCHR | devmode, dev);

        if (drm_server_info && drm_server_info->get_perms) {
            chown_check_return(buf, user, group);
            chmod(buf, devmode);
        }
    }

    fd = open(buf, O_RDWR | O_CLOEXEC);
    drmMsg("drmOpenDevice: open result is %d, (%s)\n", fd, fd < 0 ? strerror(errno) : "OK");
    if (fd >= 0) {
        return fd;
    }

    drmMsg("drmOpenDevice: Open failed\n");
    remove(buf);
#endif

    return -errno;
}

static int drmOpenMinor(int minor, int create, int type)
{
    int  fd;
    char buf[DRM_NODE_NAME_MAX];
    const char *dev_name = drmGetDeviceName(type);

    if (create) {
        return drmOpenDevice(makedev(DRM_MAJOR, minor), minor, type);
    }

    if (!dev_name) {
        return -EINVAL;
    }

    sprintf(buf, dev_name, DRM_DIR_NAME, minor);
    if ((fd = open(buf, O_RDWR | O_CLOEXEC)) >= 0) {
        return fd;
    }

    return -errno;
}

drm_public int drmAvailable(void)
{
    int fd;
    int retval = 0;
    drmVersionPtr version;

    if ((fd = drmOpenMinor(0, 1, DRM_NODE_PRIMARY)) < 0) {
        if (!access("/proc/dri/0", R_OK)) {
            return 1;
        }

        return 0;
    }

    if ((version = drmGetVersion(fd))) {
        retval = 1;
        drmFreeVersion(version);
    }
    close(fd);

    return retval;
}

static int drmGetMinorBase(int type)
{
    switch (type) {
        case DRM_NODE_PRIMARY:
            return 0;

        case DRM_NODE_CONTROL:
            return 64;

        case DRM_NODE_RENDER:
            return 128;

        default:
            return -1;
    };
}

static int drmGetMinorType(int major, int minor)
{
    int type = minor >> 6;

    if (minor < 0) {
        return -1;
    }

    switch (type) {
        case DRM_NODE_PRIMARY:
        case DRM_NODE_CONTROL:
        case DRM_NODE_RENDER:
            return type;

        default:
            return -1;
    }
}

static const char *drmGetMinorName(int type)
{
    switch (type) {
        case DRM_NODE_PRIMARY:
            return DRM_PRIMARY_MINOR_NAME;

        case DRM_NODE_CONTROL:
            return DRM_CONTROL_MINOR_NAME;

        case DRM_NODE_RENDER:
            return DRM_RENDER_MINOR_NAME;

        default:
            return NULL;
    }
}

static int drmOpenByBusid(const char *busid, int type)
{
    int fd;
    const char *buf;
    drmSetVersion sv;
    int i, pci_domain_ok = 1;
    int base = drmGetMinorBase(type);

    if (base < 0) {
        return -1;
    }

    drmMsg("drmOpenByBusid: Searching for BusID %s\n", busid);
    for (i = base; i < base + DRM_MAX_MINOR; i++) {
        fd = drmOpenMinor(i, 1, type);
        drmMsg("drmOpenByBusid: drmOpenMinor returns %d\n", fd);
        if (fd >= 0) {
            sv.drm_di_major = 1;
            sv.drm_di_minor = 4;
            sv.drm_dd_major = -1;
            sv.drm_dd_minor = -1;
            if (drmSetInterfaceVersion(fd, &sv)) {
#ifndef __alpha__
                pci_domain_ok = 0;
#endif
                sv.drm_di_major = 1;
                sv.drm_di_minor = 1;
                sv.drm_dd_major = -1;
                sv.drm_dd_minor = -1;
                drmMsg("drmOpenByBusid: Interface 1.4 failed, trying 1.1\n");
                drmSetInterfaceVersion(fd, &sv);
            }

            buf = drmGetBusid(fd);
            drmMsg("drmOpenByBusid: drmGetBusid reports %s\n", buf);
            if (buf && drmMatchBusID(buf, busid, pci_domain_ok)) {
                drmFreeBusid(buf);
                return fd;
            }

            if (buf) {
                drmFreeBusid(buf);
            }
            close(fd);
        }
    }

    return -1;
}

static int drmOpenByName(const char *name, int type)
{
    int i;
    int fd;
    char *id;
    drmVersionPtr version;
    int base = drmGetMinorBase(type);

    if (base < 0) {
        return -1;
    }

    for (i = base; i < base + DRM_MAX_MINOR; i++) {
        if ((fd = drmOpenMinor(i, 1, type)) >= 0) {
            if ((version = drmGetVersion(fd))) {
                if (!strcmp(version->name, name)) {
                    drmFreeVersion(version);
                    id = drmGetBusid(fd);
                    drmMsg("drmGetBusid returned '%s'\n", id ? id : "NULL");
                    if (id) {
                        drmFreeBusid(id);
                    }

                    return fd;
                } else {
                    drmFreeVersion(version);
                }
            }

            close(fd);
        }
    }

    for (i = 0; i < 8; i++) {
        int  retcode;
        char proc_name[64], buf[512];
        char *driver, *pt, *devstring;

        sprintf(proc_name, "/proc/dri/%d/name", i);
        if ((fd = open(proc_name, O_RDONLY)) >= 0) {
            retcode = read(fd, buf, sizeof(buf)-1);
            close(fd);
            if (retcode) {
                buf[retcode-1] = '\0';
                for (driver = pt = buf; *pt && *pt != ' '; ++pt);
                if (*pt) {
                    *pt = '\0';
                    if (!strcmp(driver, name)) {
                        for (devstring = ++pt; *pt && *pt != ' '; ++pt);
                        if (*pt) {
                            return drmOpenByBusid(++pt, type);
                        } else {
                            return drmOpenDevice(strtol(devstring, NULL, 0),i, type);
                        }
                    }
                }
            }
        }
    }

    return -1;
}

drm_public int drmOpen(const char *name, const char *busid)
{
#if defined(CONFIG_ROCKCHIP)
    if (!name && !busid) {
        name = "rockchip";
    }
#endif

    return drmOpenWithType(name, busid, DRM_NODE_PRIMARY);
}

drm_public int drmOpenWithType(const char *name, const char *busid, int type)
{
    if (name != NULL && drm_server_info && drm_server_info->load_module && !drmAvailable()) {
        if (!drm_server_info->load_module(name)) {
            drmMsg("[drm] failed to load kernel module \"%s\"\n", name);
            return -1;
        }
    }

    if (busid) {
        int fd = drmOpenByBusid(busid, type);
        if (fd >= 0) {
            return fd;
        }
    }

    if (name) {
        return drmOpenByName(name, type);
    }

    return -1;
}

drm_public int drmOpenControl(int minor)
{
    return drmOpenMinor(minor, 0, DRM_NODE_CONTROL);
}

drm_public int drmOpenRender(int minor)
{
    return drmOpenMinor(minor, 0, DRM_NODE_RENDER);
}

drm_public void drmFreeVersion(drmVersionPtr v)
{
    if (!v) {
        return;
    }

    drmFree(v->name);
    drmFree(v->date);
    drmFree(v->desc);
    drmFree(v);
}

static void drmFreeKernelVersion(drm_version_t *v)
{
    if (!v) {
        return;
    }

    drmFree(v->name);
    drmFree(v->date);
    drmFree(v->desc);
    drmFree(v);
}

static void drmCopyVersion(drmVersionPtr d, const drm_version_t *s)
{
    d->version_major = s->version_major;
    d->version_minor = s->version_minor;
    d->version_patchlevel = s->version_patchlevel;
    d->name_len = s->name_len;
    d->name = strdup(s->name);
    d->date_len = s->date_len;
    d->date = strdup(s->date);
    d->desc_len = s->desc_len;
    d->desc = strdup(s->desc);
}

drm_public drmVersionPtr drmGetVersion(int fd)
{
    drmVersionPtr retval;
    drm_version_t *version = (drm_version_t *)drmMalloc(sizeof(*version));

    if (drmIoctl(fd, DRM_IOCTL_VERSION, version)) {
        drmFreeKernelVersion(version);
        return NULL;
    }

    if (version->name_len) {
        version->name = (char *)drmMalloc(version->name_len + 1);
    }

    if (version->date_len) {
        version->date = (char *)drmMalloc(version->date_len + 1);
    }

    if (version->desc_len) {
        version->desc = (char *)drmMalloc(version->desc_len + 1);
    }

    if (drmIoctl(fd, DRM_IOCTL_VERSION, version)) {
        drmMsg("DRM_IOCTL_VERSION: %s\n", strerror(errno));
        drmFreeKernelVersion(version);
        return NULL;
    }

    if (version->name_len)  {
        version->name[version->name_len] = '\0';
    }

    if (version->date_len)  {
        version->date[version->date_len] = '\0';
    }

    if (version->desc_len)  {
        version->desc[version->desc_len] = '\0';
    }

    retval = (drmVersionPtr)drmMalloc(sizeof(*retval));
    drmCopyVersion(retval, version);
    drmFreeKernelVersion(version);

    return retval;
}

drm_public drmVersionPtr drmGetLibVersion(int fd)
{
    drm_version_t *version = (drm_version_t *)drmMalloc(sizeof(*version));

    version->version_major = 1;
    version->version_minor = 3;
    version->version_patchlevel = 0;

    return (drmVersionPtr)version;
}

drm_public int drmGetCap(int fd, uint64_t capability, uint64_t *value)
{
    int ret;
    struct drm_get_cap cap;

    memclear(cap);
    cap.capability = capability;

    ret = drmIoctl(fd, DRM_IOCTL_GET_CAP, &cap);
    if (ret) {
        return ret;
    }
    *value = cap.value;

    return 0;
}

drm_public int drmSetClientCap(int fd, uint64_t capability, uint64_t value)
{
    struct drm_set_client_cap cap;

    memclear(cap);
    cap.capability = capability;
    cap.value = value;

    return drmIoctl(fd, DRM_IOCTL_SET_CLIENT_CAP, &cap);
}

drm_public void drmFreeBusid(const char *busid)
{
    drmFree((void *)busid);
}

drm_public char *drmGetBusid(int fd)
{
    drm_unique_t u;

#if defined(CONFIG_ROCKCHIP)
    if (!getenv("DRM_ALLOW_GET_BUSID")) {
        return strdup("");
    }
#endif

    memclear(u);

    if (drmIoctl(fd, DRM_IOCTL_GET_UNIQUE, &u)) {
        return NULL;
    }

    u.unique = (char *)drmMalloc(u.unique_len + 1);
    if (drmIoctl(fd, DRM_IOCTL_GET_UNIQUE, &u)) {
        drmFree(u.unique);
        return NULL;
    }

    u.unique[u.unique_len] = '\0';
    return u.unique;
}

drm_public int drmSetBusid(int fd, const char *busid)
{
    drm_unique_t u;

    memclear(u);
    u.unique = (char *)busid;
    u.unique_len = strlen(busid);

    if (drmIoctl(fd, DRM_IOCTL_SET_UNIQUE, &u)) {
        return -errno;
    }

    return 0;
}

drm_public int drmGetMagic(int fd, drm_magic_t * magic)
{
    drm_auth_t auth;

    memclear(auth);

    *magic = 0;
    if (drmIoctl(fd, DRM_IOCTL_GET_MAGIC, &auth)) {
        return -errno;
    }
    *magic = auth.magic;

    return 0;
}

drm_public int drmAuthMagic(int fd, drm_magic_t magic)
{
    drm_auth_t auth;

#if defined(CONFIG_ROCKCHIP)
    if (!getenv("DRM_ALLOW_AUTH_MAGIC")) {
        return 0;
    }
#endif

    memclear(auth);
    auth.magic = magic;
    if (drmIoctl(fd, DRM_IOCTL_AUTH_MAGIC, &auth)) {
        return -errno;
    }

    return 0;
}

drm_public int drmAddMap(int fd, drm_handle_t offset, drmSize size, drmMapType type, drmMapFlags flags, drm_handle_t *handle)
{
    drm_map_t map;

    memclear(map);
    map.offset = offset;
    map.size = size;
    map.type = (enum drm_map_type)type;
    map.flags = (enum drm_map_flags)flags;

    if (drmIoctl(fd, DRM_IOCTL_ADD_MAP, &map)) {
        return -errno;
    }

    if (handle) {
        *handle = (drm_handle_t)(uintptr_t)map.handle;
    }

    return 0;
}

drm_public int drmRmMap(int fd, drm_handle_t handle)
{
    drm_map_t map;

    memclear(map);
    map.handle = (void *)(uintptr_t)handle;

    if(drmIoctl(fd, DRM_IOCTL_RM_MAP, &map)) {
        return -errno;
    }

    return 0;
}

drm_public int drmAddBufs(int fd, int count, int size, drmBufDescFlags flags, int agp_offset)
{
    drm_buf_desc_t request;

    memclear(request);
    request.count = count;
    request.size = size;
    request.flags = (int)flags;
    request.agp_start = agp_offset;

    if (drmIoctl(fd, DRM_IOCTL_ADD_BUFS, &request)) {
        return -errno;
    }

    return request.count;
}

drm_public int drmMarkBufs(int fd, double low, double high)
{
    int i;
    drm_buf_info_t info;

    memclear(info);

    if (drmIoctl(fd, DRM_IOCTL_INFO_BUFS, &info)) {
        return -EINVAL;
    }

    if (!info.count) {
        return -EINVAL;
    }

    if (!(info.list = (struct drm_buf_desc *)drmMalloc(info.count * sizeof(*info.list)))) {
        return -ENOMEM;
    }

    if (drmIoctl(fd, DRM_IOCTL_INFO_BUFS, &info)) {
        int retval = -errno;
        drmFree(info.list);
        return retval;
    }

    for (i = 0; i < info.count; i++) {
        info.list[i].low_mark  = low  * info.list[i].count;
        info.list[i].high_mark = high * info.list[i].count;

        if (drmIoctl(fd, DRM_IOCTL_MARK_BUFS, &info.list[i])) {
            int retval = -errno;
            drmFree(info.list);
            return retval;
        }
    }
    drmFree(info.list);

    return 0;
}

drm_public int drmFreeBufs(int fd, int count, int *list)
{
    drm_buf_free_t request;

    memclear(request);
    request.count = count;
    request.list = list;
    if (drmIoctl(fd, DRM_IOCTL_FREE_BUFS, &request)) {
        return -errno;
    }

    return 0;
}

drm_public int drmClose(int fd)
{
    unsigned long key = drmGetKeyFromFd(fd);
    drmHashEntry *entry = drmGetEntry(fd);

    drmHashDestroy(entry->tagTable);
    entry->fd = 0;
    entry->f = NULL;
    entry->tagTable = NULL;

    drmHashDelete(drmHashTable, key);
    drmFree(entry);

    return close(fd);
}

drm_public int drmMap(int fd, drm_handle_t handle, drmSize size, drmAddressPtr address)
{
    static unsigned long pagesize_mask = 0;

    if (fd < 0) {
        return -EINVAL;
    }

    if (!pagesize_mask) {
        pagesize_mask = getpagesize() - 1;
    }

    size = (size + pagesize_mask) & ~pagesize_mask;

    *address = drm_mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, handle);
    if (*address == MAP_FAILED) {
        return -errno;
    }

    return 0;
}

drm_public int drmUnmap(drmAddress address, drmSize size)
{
    return drm_munmap(address, size);
}

drm_public drmBufInfoPtr drmGetBufInfo(int fd)
{
    int i;
    drm_buf_info_t info;
    drmBufInfoPtr retval;

    memclear(info);

    if (drmIoctl(fd, DRM_IOCTL_INFO_BUFS, &info)) {
        return NULL;
    }

    if (info.count) {
        if (!(info.list = (struct drm_buf_desc *)drmMalloc(info.count * sizeof(*info.list)))) {
            return NULL;
        }

        if (drmIoctl(fd, DRM_IOCTL_INFO_BUFS, &info)) {
            drmFree(info.list);
            return NULL;
        }

        retval = (drmBufInfoPtr)drmMalloc(sizeof(*retval));
        retval->count = info.count;
        if (!(retval->list = (drmBufDescPtr)drmMalloc(info.count * sizeof(*retval->list)))) {
            drmFree(retval);
            drmFree(info.list);
            return NULL;
        }

        for (i = 0; i < info.count; i++) {
            retval->list[i].count = info.list[i].count;
            retval->list[i].size = info.list[i].size;
            retval->list[i].low_mark = info.list[i].low_mark;
            retval->list[i].high_mark = info.list[i].high_mark;
        }

        drmFree(info.list);
        return retval;
    }

    return NULL;
}

drm_public drmBufMapPtr drmMapBufs(int fd)
{
    int i;
    drm_buf_map_t bufs;
    drmBufMapPtr retval;

    memclear(bufs);
    if (drmIoctl(fd, DRM_IOCTL_MAP_BUFS, &bufs)) {
        return NULL;
    }

    if (!bufs.count) {
        return NULL;
    }

    if (!(bufs.list = (struct drm_buf_pub *)drmMalloc(bufs.count * sizeof(*bufs.list)))) {
        return NULL;
    }

    if (drmIoctl(fd, DRM_IOCTL_MAP_BUFS, &bufs)) {
        drmFree(bufs.list);
        return NULL;
    }

    retval = (drmBufMapPtr)drmMalloc(sizeof(*retval));
    retval->count = bufs.count;
    retval->list = (drmBufPtr)drmMalloc(bufs.count * sizeof(*retval->list));
    for (i = 0; i < bufs.count; i++) {
        retval->list[i].idx = bufs.list[i].idx;
        retval->list[i].total = bufs.list[i].total;
        retval->list[i].used = 0;
        retval->list[i].address = bufs.list[i].address;
    }

    drmFree(bufs.list);
    return retval;
}

drm_public int drmUnmapBufs(drmBufMapPtr bufs)
{
    int i;

    for (i = 0; i < bufs->count; i++) {
        drm_munmap(bufs->list[i].address, bufs->list[i].total);
    }

    drmFree(bufs->list);
    drmFree(bufs);

    return 0;
}

#define DRM_DMA_RETRY           16

drm_public int drmDMA(int fd, drmDMAReqPtr request)
{
    drm_dma_t dma;
    int ret, i = 0;

    dma.context = request->context;
    dma.send_count = request->send_count;
    dma.send_indices = request->send_list;
    dma.send_sizes = request->send_sizes;
    dma.flags = (enum drm_dma_flags)request->flags;
    dma.request_count = request->request_count;
    dma.request_size = request->request_size;
    dma.request_indices = request->request_list;
    dma.request_sizes = request->request_sizes;
    dma.granted_count = 0;

    do {
        ret = ioctl( fd, DRM_IOCTL_DMA, &dma );
    } while (ret && errno == EAGAIN && i++ < DRM_DMA_RETRY);

    if ( ret == 0 ) {
        request->granted_count = dma.granted_count;
        return 0;
    } else {
        return -errno;
    }
}

drm_public int drmGetLock(int fd, drm_context_t context, drmLockFlags flags)
{
    drm_lock_t lock;

    memclear(lock);
    lock.context = context;
    lock.flags = (enum drm_lock_flags)0;
    if (flags & DRM_LOCK_READY)  {
        lock.flags = (enum drm_lock_flags)(lock.flags | _DRM_LOCK_READY);
    }

    if (flags & DRM_LOCK_QUIESCENT)  {
        lock.flags = (enum drm_lock_flags)(lock.flags | _DRM_LOCK_QUIESCENT);
    }

    if (flags & DRM_LOCK_FLUSH)  {
        lock.flags = (enum drm_lock_flags)(lock.flags | _DRM_LOCK_FLUSH);
    }

    if (flags & DRM_LOCK_FLUSH_ALL)  {
        lock.flags = (enum drm_lock_flags)(lock.flags | _DRM_LOCK_FLUSH_ALL);
    }

    if (flags & DRM_HALT_ALL_QUEUES)  {
        lock.flags = (enum drm_lock_flags)(lock.flags | _DRM_HALT_ALL_QUEUES);
    }

    if (flags & DRM_HALT_CUR_QUEUES)  {
        lock.flags = (enum drm_lock_flags)(lock.flags | _DRM_HALT_CUR_QUEUES);
    }

    while (drmIoctl(fd, DRM_IOCTL_LOCK, &lock));
    return 0;
}

drm_public int drmUnlock(int fd, drm_context_t context)
{
    drm_lock_t lock;

    memclear(lock);
    lock.context = context;

    return drmIoctl(fd, DRM_IOCTL_UNLOCK, &lock);
}

drm_public drm_context_t *drmGetReservedContextList(int fd, int *count)
{
    int i;
    drm_ctx_t *list;
    drm_ctx_res_t res;
    drm_context_t *retval;

    memclear(res);
    if (drmIoctl(fd, DRM_IOCTL_RES_CTX, &res)) {
        return NULL;
    }

    if (!res.count) {
        return NULL;
    }

    if (!(list = (drm_ctx_t *)drmMalloc(res.count * sizeof(*list)))) {
        return NULL;
    }

    if (!(retval = (drm_context_t *)drmMalloc(res.count * sizeof(*retval)))) {
        goto err_free_list;
    }

    res.contexts = list;
    if (drmIoctl(fd, DRM_IOCTL_RES_CTX, &res)) {
        goto err_free_context;
    }

    for (i = 0; i < res.count; i++) {
        retval[i] = list[i].handle;
    }
    drmFree(list);

    *count = res.count;
    return retval;

err_free_list:
    drmFree(list);

err_free_context:
    drmFree(retval);
    return NULL;
}

drm_public void drmFreeReservedContextList(drm_context_t *pt)
{
    drmFree(pt);
}

drm_public int drmCreateContext(int fd, drm_context_t *handle)
{
    drm_ctx_t ctx;

    memclear(ctx);
    if (drmIoctl(fd, DRM_IOCTL_ADD_CTX, &ctx)) {
        return -errno;
    }
    *handle = ctx.handle;

    return 0;
}

drm_public int drmSwitchToContext(int fd, drm_context_t context)
{
    drm_ctx_t ctx;

    memclear(ctx);
    ctx.handle = context;
    if (drmIoctl(fd, DRM_IOCTL_SWITCH_CTX, &ctx)) {
        return -errno;
    }

    return 0;
}

drm_public int drmSetContextFlags(int fd, drm_context_t context, drm_context_tFlags flags)
{
    drm_ctx_t ctx;

    memclear(ctx);
    ctx.handle = context;
    if (flags & DRM_CONTEXT_PRESERVED) {
        ctx.flags = (enum drm_ctx_flags)(ctx.flags | _DRM_CONTEXT_PRESERVED);
    }

    if (flags & DRM_CONTEXT_2DONLY) {
        ctx.flags = (enum drm_ctx_flags)(ctx.flags | _DRM_CONTEXT_2DONLY);
    }

    if (drmIoctl(fd, DRM_IOCTL_MOD_CTX, &ctx)) {
        return -errno;
    }

    return 0;
}

drm_public int drmGetContextFlags(int fd, drm_context_t context, drm_context_tFlagsPtr flags)
{
    drm_ctx_t ctx;

    memclear(ctx);
    ctx.handle = context;
    if (drmIoctl(fd, DRM_IOCTL_GET_CTX, &ctx)) {
        return -errno;
    }

    *flags = (drm_context_tFlags)0;
    if (ctx.flags & _DRM_CONTEXT_PRESERVED) {
        *flags = (drm_context_tFlags)(*flags | DRM_CONTEXT_PRESERVED);
    }

    if (ctx.flags & _DRM_CONTEXT_2DONLY) {
        *flags = (drm_context_tFlags)(*flags | DRM_CONTEXT_2DONLY);
    }

    return 0;
}

drm_public int drmDestroyContext(int fd, drm_context_t handle)
{
    drm_ctx_t ctx;

    memclear(ctx);
    ctx.handle = handle;
    if (drmIoctl(fd, DRM_IOCTL_RM_CTX, &ctx)) {
        return -errno;
    }

    return 0;
}

drm_public int drmCreateDrawable(int fd, drm_drawable_t *handle)
{
    drm_draw_t draw;

    memclear(draw);
    if (drmIoctl(fd, DRM_IOCTL_ADD_DRAW, &draw)) {
        return -errno;
    }
    *handle = draw.handle;

    return 0;
}

drm_public int drmDestroyDrawable(int fd, drm_drawable_t handle)
{
    drm_draw_t draw;

    memclear(draw);
    draw.handle = handle;
    if (drmIoctl(fd, DRM_IOCTL_RM_DRAW, &draw)) {
        return -errno;
    }

    return 0;
}

drm_public int drmUpdateDrawableInfo(int fd, drm_drawable_t handle, drm_drawable_info_type_t type, unsigned int num, void *data)
{
    drm_update_draw_t update;

    memclear(update);
    update.handle = handle;
    update.type = type;
    update.num = num;
    update.data = (unsigned long long)(unsigned long)data;

    if (drmIoctl(fd, DRM_IOCTL_UPDATE_DRAW, &update)) {
        return -errno;
    }

    return 0;
}

drm_public int drmCrtcGetSequence(int fd, uint32_t crtcId, uint64_t *sequence, uint64_t *ns)
{
    int ret;
    struct drm_crtc_get_sequence get_seq;

    memclear(get_seq);
    get_seq.crtc_id = crtcId;
    ret = drmIoctl(fd, DRM_IOCTL_CRTC_GET_SEQUENCE, &get_seq);
    if (ret) {
        return ret;
    }

    if (sequence) {
        *sequence = get_seq.sequence;
    }

    if (ns) {
        *ns = get_seq.sequence_ns;
    }

    return 0;
}

drm_public int drmCrtcQueueSequence(int fd, uint32_t crtcId, uint32_t flags, uint64_t sequence, uint64_t *sequence_queued, uint64_t user_data)
{
    int ret;
    struct drm_crtc_queue_sequence queue_seq;

    memclear(queue_seq);
    queue_seq.crtc_id = crtcId;
    queue_seq.flags = flags;
    queue_seq.sequence = sequence;
    queue_seq.user_data = user_data;

    ret = drmIoctl(fd, DRM_IOCTL_CRTC_QUEUE_SEQUENCE, &queue_seq);
    if (ret == 0 && sequence_queued) {
        *sequence_queued = queue_seq.sequence;
    }

    return ret;
}

drm_public int drmAgpAcquire(int fd)
{
    if (drmIoctl(fd, DRM_IOCTL_AGP_ACQUIRE, NULL)) {
        return -errno;
    }

    return 0;
}

drm_public int drmAgpRelease(int fd)
{
    if (drmIoctl(fd, DRM_IOCTL_AGP_RELEASE, NULL)) {
        return -errno;
    }

    return 0;
}

drm_public int drmAgpEnable(int fd, unsigned long mode)
{
    drm_agp_mode_t m;

    memclear(m);
    m.mode = mode;
    if (drmIoctl(fd, DRM_IOCTL_AGP_ENABLE, &m)) {
        return -errno;
    }

    return 0;
}

drm_public int drmAgpAlloc(int fd, unsigned long size, unsigned long type, unsigned long *address, drm_handle_t *handle)
{
    drm_agp_buffer_t b;

    memclear(b);
    *handle = DRM_AGP_NO_HANDLE;
    b.size = size;
    b.type = type;
    if (drmIoctl(fd, DRM_IOCTL_AGP_ALLOC, &b)) {
        return -errno;
    }

    if (address != 0UL) {
        *address = b.physical;
    }
    *handle = b.handle;

    return 0;
}

drm_public int drmAgpFree(int fd, drm_handle_t handle)
{
    drm_agp_buffer_t b;

    memclear(b);
    b.handle = handle;
    if (drmIoctl(fd, DRM_IOCTL_AGP_FREE, &b)) {
        return -errno;
    }

    return 0;
}

drm_public int drmAgpBind(int fd, drm_handle_t handle, unsigned long offset)
{
    drm_agp_binding_t b;

    memclear(b);
    b.handle = handle;
    b.offset = offset;
    if (drmIoctl(fd, DRM_IOCTL_AGP_BIND, &b)) {
        return -errno;
    }

    return 0;
}

drm_public int drmAgpUnbind(int fd, drm_handle_t handle)
{
    drm_agp_binding_t b;

    memclear(b);
    b.handle = handle;
    if (drmIoctl(fd, DRM_IOCTL_AGP_UNBIND, &b)) {
        return -errno;
    }

    return 0;
}

drm_public int drmAgpVersionMajor(int fd)
{
    drm_agp_info_t i;

    memclear(i);
    if (drmIoctl(fd, DRM_IOCTL_AGP_INFO, &i)) {
        return -errno;
    }

    return i.agp_version_major;
}

drm_public int drmAgpVersionMinor(int fd)
{
    drm_agp_info_t i;

    memclear(i);

    if (drmIoctl(fd, DRM_IOCTL_AGP_INFO, &i)) {
        return -errno;
    }

    return i.agp_version_minor;
}

drm_public unsigned long drmAgpGetMode(int fd)
{
    drm_agp_info_t i;

    memclear(i);
    if (drmIoctl(fd, DRM_IOCTL_AGP_INFO, &i)) {
        return 0;
    }

    return i.mode;
}

drm_public unsigned long drmAgpBase(int fd)
{
    drm_agp_info_t i;

    memclear(i);
    if (drmIoctl(fd, DRM_IOCTL_AGP_INFO, &i)) {
        return 0;
    }

    return i.aperture_base;
}

drm_public unsigned long drmAgpSize(int fd)
{
    drm_agp_info_t i;

    memclear(i);
    if (drmIoctl(fd, DRM_IOCTL_AGP_INFO, &i)) {
        return 0;
    }

    return i.aperture_size;
}

drm_public unsigned long drmAgpMemoryUsed(int fd)
{
    drm_agp_info_t i;

    memclear(i);
    if (drmIoctl(fd, DRM_IOCTL_AGP_INFO, &i)) {
        return 0;
    }

    return i.memory_used;
}

drm_public unsigned long drmAgpMemoryAvail(int fd)
{
    drm_agp_info_t i;

    memclear(i);
    if (drmIoctl(fd, DRM_IOCTL_AGP_INFO, &i)) {
        return 0;
    }

    return i.memory_allowed;
}

drm_public unsigned int drmAgpVendorId(int fd)
{
    drm_agp_info_t i;

    memclear(i);
    if (drmIoctl(fd, DRM_IOCTL_AGP_INFO, &i)) {
        return 0;
    }

    return i.id_vendor;
}

drm_public unsigned int drmAgpDeviceId(int fd)
{
    drm_agp_info_t i;

    memclear(i);
    if (drmIoctl(fd, DRM_IOCTL_AGP_INFO, &i)) {
        return 0;
    }

    return i.id_device;
}

drm_public int drmScatterGatherAlloc(int fd, unsigned long size, drm_handle_t *handle)
{
    drm_scatter_gather_t sg;

    memclear(sg);

    *handle = 0;
    sg.size = size;
    if (drmIoctl(fd, DRM_IOCTL_SG_ALLOC, &sg)) {
        return -errno;
    }
    *handle = sg.handle;

    return 0;
}

drm_public int drmScatterGatherFree(int fd, drm_handle_t handle)
{
    drm_scatter_gather_t sg;

    memclear(sg);
    sg.handle = handle;
    if (drmIoctl(fd, DRM_IOCTL_SG_FREE, &sg)) {
        return -errno;
    }

    return 0;
}

drm_public int drmWaitVBlank(int fd, drmVBlankPtr vbl)
{
    int ret;
    struct timespec timeout, cur;

    ret = clock_gettime(CLOCK_MONOTONIC, &timeout);
    if (ret < 0) {
        fprintf(stderr, "clock_gettime failed: %s\n", strerror(errno));
        goto out;
    }
    timeout.tv_sec++;

    do {
       ret = ioctl(fd, DRM_IOCTL_WAIT_VBLANK, vbl);
       vbl->request.type = (drmVBlankSeqType)(vbl->request.type & ~DRM_VBLANK_RELATIVE);

       if (ret && errno == EINTR) {
           clock_gettime(CLOCK_MONOTONIC, &cur);
           if (cur.tv_sec > timeout.tv_sec + 1 || (cur.tv_sec == timeout.tv_sec && cur.tv_nsec >= timeout.tv_nsec)) {
                errno = EBUSY;
                ret = -1;
                break;
           }
       }
    } while (ret && errno == EINTR);

out:
    return ret;
}

drm_public int drmError(int err, const char *label)
{
    switch (err) {
        case DRM_ERR_NO_DEVICE:
            fprintf(stderr, "%s: no device\n", label);
            break;

        case DRM_ERR_NO_ACCESS:
            fprintf(stderr, "%s: no access\n", label);
            break;

        case DRM_ERR_NOT_ROOT:
            fprintf(stderr, "%s: not root\n", label);
            break;

        case DRM_ERR_INVALID:
            fprintf(stderr, "%s: invalid args\n", label);
            break;

        default:
            if (err < 0) {
                err = -err;
            }
            fprintf( stderr, "%s: error %d (%s)\n", label, err, strerror(err) );
            break;
    }

    return 1;
}

drm_public int drmCtlInstHandler(int fd, int irq)
{
    drm_control_t ctl;

    memclear(ctl);
    ctl.func = DRM_INST_HANDLER;
    ctl.irq = irq;
    if (drmIoctl(fd, DRM_IOCTL_CONTROL, &ctl)) {
        return -errno;
    }

    return 0;
}

drm_public int drmCtlUninstHandler(int fd)
{
    drm_control_t ctl;

    memclear(ctl);
    ctl.func = DRM_UNINST_HANDLER;
    ctl.irq = 0;
    if (drmIoctl(fd, DRM_IOCTL_CONTROL, &ctl)) {
        return -errno;
    }

    return 0;
}

drm_public int drmFinish(int fd, int context, drmLockFlags flags)
{
    drm_lock_t lock;

    memclear(lock);
    lock.context = context;
    if (flags & DRM_LOCK_READY)  {
        lock.flags = (enum drm_lock_flags)(lock.flags | _DRM_LOCK_READY);
    }

    if (flags & DRM_LOCK_QUIESCENT)  {
        lock.flags = (enum drm_lock_flags)(lock.flags | _DRM_LOCK_QUIESCENT);
    }

    if (flags & DRM_LOCK_FLUSH)  {
        lock.flags = (enum drm_lock_flags)(lock.flags | _DRM_LOCK_FLUSH);
    }

    if (flags & DRM_LOCK_FLUSH_ALL)  {
        lock.flags = (enum drm_lock_flags)(lock.flags | _DRM_LOCK_FLUSH_ALL);
    }

    if (flags & DRM_HALT_ALL_QUEUES)  {
        lock.flags = (enum drm_lock_flags)(lock.flags | _DRM_HALT_ALL_QUEUES);
    }

    if (flags & DRM_HALT_CUR_QUEUES)  {
        lock.flags = (enum drm_lock_flags)(lock.flags | _DRM_HALT_CUR_QUEUES);
    }

    if (drmIoctl(fd, DRM_IOCTL_FINISH, &lock)) {
        return -errno;
    }

    return 0;
}

drm_public int drmGetInterruptFromBusID(int fd, int busnum, int devnum, int funcnum)
{
    drm_irq_busid_t p;

    memclear(p);
    p.busnum = busnum;
    p.devnum = devnum;
    p.funcnum = funcnum;
    if (drmIoctl(fd, DRM_IOCTL_IRQ_BUSID, &p)) {
        return -errno;
    }

    return p.irq;
}

drm_public int drmAddContextTag(int fd, drm_context_t context, void *tag)
{
    drmHashEntry *entry = drmGetEntry(fd);

    if (drmHashInsert(entry->tagTable, context, tag)) {
        drmHashDelete(entry->tagTable, context);
        drmHashInsert(entry->tagTable, context, tag);
    }

    return 0;
}

drm_public int drmDelContextTag(int fd, drm_context_t context)
{
    drmHashEntry *entry = drmGetEntry(fd);
    return drmHashDelete(entry->tagTable, context);
}

drm_public void *drmGetContextTag(int fd, drm_context_t context)
{
    void *value;
    drmHashEntry *entry = drmGetEntry(fd);

    if (drmHashLookup(entry->tagTable, context, &value)) {
        return NULL;
    }

    return value;
}

drm_public int drmAddContextPrivateMapping(int fd, drm_context_t ctx_id, drm_handle_t handle)
{
    drm_ctx_priv_map_t map;

    memclear(map);
    map.ctx_id = ctx_id;
    map.handle = (void *)(uintptr_t)handle;

    if (drmIoctl(fd, DRM_IOCTL_SET_SAREA_CTX, &map)) {
        return -errno;
    }

    return 0;
}

drm_public int drmGetContextPrivateMapping(int fd, drm_context_t ctx_id, drm_handle_t *handle)
{
    drm_ctx_priv_map_t map;

    memclear(map);
    map.ctx_id = ctx_id;

    if (drmIoctl(fd, DRM_IOCTL_GET_SAREA_CTX, &map)) {
        return -errno;
    }

    if (handle) {
        *handle = (drm_handle_t)(uintptr_t)map.handle;
    }

    return 0;
}

drm_public int drmGetMap(int fd, int idx, drm_handle_t *offset, drmSize *size, drmMapType *type, drmMapFlags *flags, drm_handle_t *handle, int *mtrr)
{
    drm_map_t map;

    memclear(map);
    map.offset = idx;
    if (drmIoctl(fd, DRM_IOCTL_GET_MAP, &map)) {
        return -errno;
    }

    *offset = map.offset;
    *size = map.size;
    *type = (drmMapType)map.type;
    *flags = (drmMapFlags)map.flags;
    *handle = (unsigned long)map.handle;
    *mtrr = map.mtrr;

    return 0;
}

drm_public int drmGetClient(int fd, int idx, int *auth, int *pid, int *uid, unsigned long *magic, unsigned long *iocs)
{
    drm_client_t client;

    memclear(client);
    client.idx = idx;
    if (drmIoctl(fd, DRM_IOCTL_GET_CLIENT, &client)) {
        return -errno;
    }

    *auth = client.auth;
    *pid = client.pid;
    *uid = client.uid;
    *magic = client.magic;
    *iocs = client.iocs;

    return 0;
}

drm_public int drmGetStats(int fd, drmStatsT *stats)
{
    unsigned i;
    drm_stats_t s;

    memclear(s);
    if (drmIoctl(fd, DRM_IOCTL_GET_STATS, &s)) {
        return -errno;
    }

    stats->count = 0;
    memset(stats, 0, sizeof(*stats));
    if (s.count > sizeof(stats->data)/sizeof(stats->data[0])) {
        return -1;
    }

#define SET_VALUE                              \
    stats->data[i].long_format = "%-20.20s";   \
    stats->data[i].rate_format = "%8.8s";      \
    stats->data[i].isvalue = 1;                \
    stats->data[i].verbose = 0

#define SET_COUNT                              \
    stats->data[i].long_format = "%-20.20s";   \
    stats->data[i].rate_format = "%5.5s";      \
    stats->data[i].isvalue = 0;                \
    stats->data[i].mult_names = "kgm";         \
    stats->data[i].mult = 1000;                \
    stats->data[i].verbose = 0

#define SET_BYTE                               \
    stats->data[i].long_format = "%-20.20s";   \
    stats->data[i].rate_format = "%5.5s";      \
    stats->data[i].isvalue = 0;                \
    stats->data[i].mult_names = "KGM";         \
    stats->data[i].mult = 1024;                \
    stats->data[i].verbose = 0

    stats->count = s.count;
    for (i = 0; i < s.count; i++) {
        stats->data[i].value = s.data[i].value;
        switch (s.data[i].type) {
            case _DRM_STAT_LOCK:
                stats->data[i].long_name = "Lock";
                stats->data[i].rate_name = "Lock";
                SET_VALUE;
                break;

            case _DRM_STAT_OPENS:
                stats->data[i].long_name = "Opens";
                stats->data[i].rate_name = "O";
                SET_COUNT;
                stats->data[i].verbose = 1;
                break;

            case _DRM_STAT_CLOSES:
                stats->data[i].long_name = "Closes";
                stats->data[i].rate_name = "Lock";
                SET_COUNT;
                stats->data[i].verbose = 1;
                break;

            case _DRM_STAT_IOCTLS:
                stats->data[i].long_name = "Ioctls";
                stats->data[i].rate_name = "Ioc/s";
                SET_COUNT;
                break;

            case _DRM_STAT_LOCKS:
                stats->data[i].long_name = "Locks";
                stats->data[i].rate_name = "Lck/s";
                SET_COUNT;
                break;

            case _DRM_STAT_UNLOCKS:
                stats->data[i].long_name = "Unlocks";
                stats->data[i].rate_name = "Unl/s";
                SET_COUNT;
                break;

            case _DRM_STAT_IRQ:
                stats->data[i].long_name = "IRQs";
                stats->data[i].rate_name = "IRQ/s";
                SET_COUNT;
                break;

            case _DRM_STAT_PRIMARY:
                stats->data[i].long_name = "Primary Bytes";
                stats->data[i].rate_name = "PB/s";
                SET_BYTE;
                break;

            case _DRM_STAT_SECONDARY:
                stats->data[i].long_name = "Secondary Bytes";
                stats->data[i].rate_name = "SB/s";
                SET_BYTE;
                break;

            case _DRM_STAT_DMA:
                stats->data[i].long_name = "DMA";
                stats->data[i].rate_name = "DMA/s";
                SET_COUNT;
                break;

            case _DRM_STAT_SPECIAL:
                stats->data[i].long_name = "Special DMA";
                stats->data[i].rate_name = "dma/s";
                SET_COUNT;
                break;

            case _DRM_STAT_MISSED:
                stats->data[i].long_name = "Miss";
                stats->data[i].rate_name = "Ms/s";
                SET_COUNT;
                break;

            case _DRM_STAT_VALUE:
                stats->data[i].long_name = "Value";
                stats->data[i].rate_name = "Value";
                SET_VALUE;
                break;

            case _DRM_STAT_BYTE:
                stats->data[i].long_name = "Bytes";
                stats->data[i].rate_name = "B/s";
                SET_BYTE;
                break;

            case _DRM_STAT_COUNT:
            default:
                stats->data[i].long_name = "Count";
                stats->data[i].rate_name = "Cnt/s";
                SET_COUNT;
                break;
        }
    }

    return 0;
}

drm_public int drmSetInterfaceVersion(int fd, drmSetVersion *version)
{
    int retcode = 0;
    drm_set_version_t sv;

#if defined(CONFIG_ROCKCHIP)
    if (!getenv("DRM_ALLOW_SET_VERSION")) {
        return 0;
    }
#endif

    memclear(sv);
    sv.drm_di_major = version->drm_di_major;
    sv.drm_di_minor = version->drm_di_minor;
    sv.drm_dd_major = version->drm_dd_major;
    sv.drm_dd_minor = version->drm_dd_minor;

    if (drmIoctl(fd, DRM_IOCTL_SET_VERSION, &sv)) {
        retcode = -errno;
    }

    version->drm_di_major = sv.drm_di_major;
    version->drm_di_minor = sv.drm_di_minor;
    version->drm_dd_major = sv.drm_dd_major;
    version->drm_dd_minor = sv.drm_dd_minor;

    return retcode;
}

drm_public int drmCommandNone(int fd, unsigned long drmCommandIndex)
{
    unsigned long request;

    request = DRM_IO(DRM_COMMAND_BASE + drmCommandIndex);
    if (drmIoctl(fd, request, NULL)) {
        return -errno;
    }

    return 0;
}

drm_public int drmCommandRead(int fd, unsigned long drmCommandIndex, void *data, unsigned long size)
{
    unsigned long request;

    request = DRM_IOC(DRM_IOC_READ, DRM_IOCTL_BASE, DRM_COMMAND_BASE + drmCommandIndex, size);
    if (drmIoctl(fd, request, data)) {
        return -errno;
    }

    return 0;
}

drm_public int drmCommandWrite(int fd, unsigned long drmCommandIndex, void *data, unsigned long size)
{
    unsigned long request;

    request = DRM_IOC(DRM_IOC_WRITE, DRM_IOCTL_BASE, DRM_COMMAND_BASE + drmCommandIndex, size);
    if (drmIoctl(fd, request, data)) {
        return -errno;
    }

    return 0;
}

drm_public int drmCommandWriteRead(int fd, unsigned long drmCommandIndex, void *data, unsigned long size)
{
    unsigned long request;

    request = DRM_IOC(DRM_IOC_READ|DRM_IOC_WRITE, DRM_IOCTL_BASE, DRM_COMMAND_BASE + drmCommandIndex, size);
    if (drmIoctl(fd, request, data)) {
        return -errno;
    }

    return 0;
}

#define DRM_MAX_FDS         16
static struct {
    char *BusID;
    int  fd;
    int  refcount;
    int  type;
} connection[DRM_MAX_FDS];

static int nr_fds = 0;

drm_public int drmOpenOnce(void *unused, const char *BusID, int *newlyopened)
{
    return drmOpenOnceWithType(BusID, newlyopened, DRM_NODE_PRIMARY);
}

drm_public int drmOpenOnceWithType(const char *BusID, int *newlyopened, int type)
{
    int i;
    int fd;

    for (i = 0; i < nr_fds; i++) {
        if ((strcmp(BusID, connection[i].BusID) == 0) && (connection[i].type == type)) {
            connection[i].refcount++;
            *newlyopened = 0;
            return connection[i].fd;
        }
    }

    fd = drmOpenWithType(NULL, BusID, type);
    if (fd < 0 || nr_fds == DRM_MAX_FDS) {
        return fd;
    }

    connection[nr_fds].BusID = strdup(BusID);
    connection[nr_fds].fd = fd;
    connection[nr_fds].refcount = 1;
    connection[nr_fds].type = type;
    *newlyopened = 1;

    if (0) {
        fprintf(stderr, "saved connection %d for %s %d\n", nr_fds, connection[nr_fds].BusID, strcmp(BusID, connection[nr_fds].BusID));
    }
    nr_fds++;

    return fd;
}

drm_public void drmCloseOnce(int fd)
{
    int i;

    for (i = 0; i < nr_fds; i++) {
        if (fd == connection[i].fd) {
            if (--connection[i].refcount == 0) {
                drmClose(connection[i].fd);
                free(connection[i].BusID);

                if (i < --nr_fds) {
                    connection[i] = connection[nr_fds];
                }

                return;
            }
        }
    }
}

drm_public int drmSetMaster(int fd)
{
    return drmIoctl(fd, DRM_IOCTL_SET_MASTER, NULL);
}

drm_public int drmDropMaster(int fd)
{
    return drmIoctl(fd, DRM_IOCTL_DROP_MASTER, NULL);
}

drm_public int drmIsMaster(int fd)
{
    return drmAuthMagic(fd, 0) != -EACCES;
}

drm_public char *drmGetDeviceNameFromFd(int fd)
{
    int i;
    dev_t d;
    char name[128];
    struct stat sbuf;

    fstat(fd, &sbuf);
    d = sbuf.st_rdev;

    for (i = 0; i < DRM_MAX_MINOR; i++) {
        snprintf(name, sizeof name, DRM_DEV_NAME, DRM_DIR_NAME, i);
        if (stat(name, &sbuf) == 0 && sbuf.st_rdev == d) {
            break;
        }
    }

    if (i == DRM_MAX_MINOR) {
        return NULL;
    }

    return strdup(name);
}

static bool drmNodeIsDRM(int maj, int min)
{
    char path[64];
    struct stat sbuf;

    snprintf(path, sizeof(path), "/sys/dev/char/%d:%d/device/drm", maj, min);
    return stat(path, &sbuf) == 0;
}

drm_public int drmGetNodeTypeFromFd(int fd)
{
    struct stat sbuf;
    int maj, min, type;

    if (fstat(fd, &sbuf)) {
        return -1;
    }

    maj = major(sbuf.st_rdev);
    min = minor(sbuf.st_rdev);

    if (!drmNodeIsDRM(maj, min) || !S_ISCHR(sbuf.st_mode)) {
        errno = EINVAL;
        return -1;
    }

    type = drmGetMinorType(maj, min);
    if (type == -1) {
        errno = ENODEV;
    }

    return type;
}

drm_public int drmPrimeHandleToFD(int fd, uint32_t handle, uint32_t flags, int *prime_fd)
{
    int ret;
    struct drm_prime_handle args;

    memclear(args);
    args.fd = -1;
    args.handle = handle;
    args.flags = flags;
    ret = drmIoctl(fd, DRM_IOCTL_PRIME_HANDLE_TO_FD, &args);
    if (ret) {
        return ret;
    }

    *prime_fd = args.fd;

    return 0;
}

drm_public int drmPrimeFDToHandle(int fd, int prime_fd, uint32_t *handle)
{
    int ret;
    struct drm_prime_handle args;

    memclear(args);
    args.fd = prime_fd;
    ret = drmIoctl(fd, DRM_IOCTL_PRIME_FD_TO_HANDLE, &args);
    if (ret) {
        return ret;
    }
    *handle = args.handle;

    return 0;
}

drm_public int drmCloseBufferHandle(int fd, uint32_t handle)
{
    struct drm_gem_close args;

    memclear(args);
    args.handle = handle;
    return drmIoctl(fd, DRM_IOCTL_GEM_CLOSE, &args);
}

static char *drmGetMinorNameForFD(int fd, int type)
{
    int len;
    DIR *sysdir;
    int maj, min;
    struct stat sbuf;
    struct dirent *ent;
    char dev_name[64], buf[64];
    const char *name = drmGetMinorName(type);

    if (!name) {
        return NULL;
    }

    len = strlen(name);
    if (fstat(fd, &sbuf)) {
        return NULL;
    }

    maj = major(sbuf.st_rdev);
    min = minor(sbuf.st_rdev);

    if (!drmNodeIsDRM(maj, min) || !S_ISCHR(sbuf.st_mode)) {
        return NULL;
    }

    snprintf(buf, sizeof(buf), "/sys/dev/char/%d:%d/device/drm", maj, min);
    sysdir = opendir(buf);
    if (!sysdir) {
        return NULL;
    }

    while ((ent = readdir(sysdir))) {
        if (strncmp(ent->d_name, name, len) == 0) {
            if (snprintf(dev_name, sizeof(dev_name), DRM_DIR_NAME "/%s", ent->d_name) < 0) {
                return NULL;
            }

            closedir(sysdir);
            return strdup(dev_name);
        }
    }

    closedir(sysdir);
    return NULL;
}

drm_public char *drmGetPrimaryDeviceNameFromFd(int fd)
{
    return drmGetMinorNameForFD(fd, DRM_NODE_PRIMARY);
}

drm_public char *drmGetRenderDeviceNameFromFd(int fd)
{
    return drmGetMinorNameForFD(fd, DRM_NODE_RENDER);
}

static char *DRM_PRINTFLIKE(2, 3) sysfs_uevent_get(const char *path, const char *fmt, ...)
{
    FILE *fp;
    va_list ap;
    ssize_t num;
    size_t size = 0, len;
    char filename[PATH_MAX + 1], *key, *line = NULL, *value = NULL;

    va_start(ap, fmt);
    num = vasprintf(&key, fmt, ap);
    va_end(ap);
    len = num;

    snprintf(filename, sizeof(filename), "%s/uevent", path);
    fp = fopen(filename, "r");
    if (!fp) {
        free(key);
        return NULL;
    }

    while ((num = getline(&line, &size, fp)) >= 0) {
        if ((strncmp(line, key, len) == 0) && (line[len] == '=')) {
            char *start = line + len + 1, *end = line + num - 1;

            if (*end != '\n') {
                end++;
            }

            value = strndup(start, end - start);
            break;
        }
    }

    free(line);
    fclose(fp);
    free(key);

    return value;
}

#define DRM_BUS_VIRTIO 0x10

static int get_subsystem_type(const char *device_path)
{
    char *name;
    char path[PATH_MAX + 1] = "";
    char link[PATH_MAX + 1] = "";

    struct {
        const char *name;
        int bus_type;
    } bus_types[] = {
        { "/pci", DRM_BUS_PCI },
        { "/usb", DRM_BUS_USB },
        { "/platform", DRM_BUS_PLATFORM },
        { "/spi", DRM_BUS_PLATFORM },
        { "/host1x", DRM_BUS_HOST1X },
        { "/virtio", DRM_BUS_VIRTIO },
    };

    strncpy(path, device_path, PATH_MAX);
    strncat(path, "/subsystem", PATH_MAX);

    if (readlink(path, link, PATH_MAX) < 0) {
        return -errno;
    }

    name = strrchr(link, '/');
    if (!name) {
        return -EINVAL;
    }

    for (unsigned i = 0; i < ARRAY_SIZE(bus_types); i++) {
        if (strncmp(name, bus_types[i].name, strlen(bus_types[i].name)) == 0) {
            return bus_types[i].bus_type;
        }
    }

    return -EINVAL;
}

static int drmParseSubsystemType(int maj, int min)
{
    int subsystem_type;
    char path[PATH_MAX + 1] = "";
    char real_path[PATH_MAX + 1] = "";

    snprintf(path, sizeof(path), "/sys/dev/char/%d:%d/device", maj, min);

    subsystem_type = get_subsystem_type(path);
    if (subsystem_type == DRM_BUS_VIRTIO) {
        if (!realpath(path, real_path)) {
            return DRM_BUS_VIRTIO;
        }

        strncat(path, "/..", PATH_MAX);
        subsystem_type = get_subsystem_type(path);
        if (subsystem_type < 0) {
            return DRM_BUS_VIRTIO;
        }
    }

    return subsystem_type;
}

static void get_pci_path(int maj, int min, char *pci_path)
{
    char path[PATH_MAX + 1], *term;

    snprintf(path, sizeof(path), "/sys/dev/char/%d:%d/device", maj, min);
    if (!realpath(path, pci_path)) {
        strcpy(pci_path, path);
        return;
    }

    term = strrchr(pci_path, '/');
    if (term && strncmp(term, "/virtio", 7) == 0) {
        *term = 0;
    }
}

static int drmParsePciBusInfo(int maj, int min, drmPciBusInfoPtr info)
{
    int num;
    unsigned int domain, bus, dev, func;
    char pci_path[PATH_MAX + 1], *value;

    get_pci_path(maj, min, pci_path);

    value = sysfs_uevent_get(pci_path, "PCI_SLOT_NAME");
    if (!value) {
        return -ENOENT;
    }

    num = sscanf(value, "%04x:%02x:%02x.%1u", &domain, &bus, &dev, &func);
    free(value);

    if (num != 4) {
        return -EINVAL;
    }

    info->domain = domain;
    info->bus = bus;
    info->dev = dev;
    info->func = func;

    return 0;
}

drm_public int drmDevicesEqual(drmDevicePtr a, drmDevicePtr b)
{
    if (a == NULL || b == NULL) {
        return 0;
    }

    if (a->bustype != b->bustype) {
        return 0;
    }

    switch (a->bustype) {
        case DRM_BUS_PCI:
            return memcmp(a->businfo.pci, b->businfo.pci, sizeof(drmPciBusInfo)) == 0;

        case DRM_BUS_USB:
            return memcmp(a->businfo.usb, b->businfo.usb, sizeof(drmUsbBusInfo)) == 0;

        case DRM_BUS_PLATFORM:
            return memcmp(a->businfo.platform, b->businfo.platform, sizeof(drmPlatformBusInfo)) == 0;

        case DRM_BUS_HOST1X:
            return memcmp(a->businfo.host1x, b->businfo.host1x, sizeof(drmHost1xBusInfo)) == 0;

        default:
            break;
    }

    return 0;
}

static int drmGetNodeType(const char *name)
{
    if (strncmp(name, DRM_CONTROL_MINOR_NAME, sizeof(DRM_CONTROL_MINOR_NAME) - 1) == 0) {
        return DRM_NODE_CONTROL;
    }

    if (strncmp(name, DRM_RENDER_MINOR_NAME, sizeof(DRM_RENDER_MINOR_NAME) - 1) == 0) {
        return DRM_NODE_RENDER;
    }

    if (strncmp(name, DRM_PRIMARY_MINOR_NAME, sizeof(DRM_PRIMARY_MINOR_NAME) - 1) == 0) {
        return DRM_NODE_PRIMARY;
    }

    return -EINVAL;
}

static int drmGetMaxNodeName(void)
{
    return sizeof(DRM_DIR_NAME) + MAX3(sizeof(DRM_PRIMARY_MINOR_NAME), sizeof(DRM_CONTROL_MINOR_NAME), sizeof(DRM_RENDER_MINOR_NAME)) + 3;
}

static int parse_separate_sysfs_files(int maj, int min, drmPciDeviceInfoPtr device, bool ignore_revision)
{
    static const char *attrs[] = {
      "revision",
      "vendor",
      "device",
      "subsystem_vendor",
      "subsystem_device",
    };

    int ret;
    FILE *fp;
    unsigned int data[ARRAY_SIZE(attrs)];
    char path[PATH_MAX + 1], pci_path[PATH_MAX + 1];

    get_pci_path(maj, min, pci_path);

    for (unsigned i = ignore_revision ? 1 : 0; i < ARRAY_SIZE(attrs); i++) {
        if (snprintf(path, PATH_MAX, "%s/%s", pci_path, attrs[i]) < 0) {
            return -errno;
        }

        fp = fopen(path, "r");
        if (!fp) {
            return -errno;
        }

        ret = fscanf(fp, "%x", &data[i]);
        fclose(fp);
        if (ret != 1) {
            return -errno;
        }
    }

    device->revision_id = ignore_revision ? 0xff : data[0] & 0xff;
    device->vendor_id = data[1] & 0xffff;
    device->device_id = data[2] & 0xffff;
    device->subvendor_id = data[3] & 0xffff;
    device->subdevice_id = data[4] & 0xffff;

    return 0;
}

static int parse_config_sysfs_file(int maj, int min, drmPciDeviceInfoPtr device)
{
    int fd, ret;
    unsigned char config[64];
    char path[PATH_MAX + 1], pci_path[PATH_MAX + 1];

    get_pci_path(maj, min, pci_path);

    if (snprintf(path, PATH_MAX, "%s/config", pci_path) < 0) {
        return -errno;
    }

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        return -errno;
    }

    ret = read(fd, config, sizeof(config));
    close(fd);
    if (ret < 0) {
        return -errno;
    }

    device->vendor_id = config[0] | (config[1] << 8);
    device->device_id = config[2] | (config[3] << 8);
    device->revision_id = config[8];
    device->subvendor_id = config[44] | (config[45] << 8);
    device->subdevice_id = config[46] | (config[47] << 8);

    return 0;
}

static int drmParsePciDeviceInfo(int maj, int min, drmPciDeviceInfoPtr device, uint32_t flags)
{
    if (!(flags & DRM_DEVICE_GET_PCI_REVISION)) {
        return parse_separate_sysfs_files(maj, min, device, true);
    }

    if (parse_separate_sysfs_files(maj, min, device, false)) {
        return parse_config_sysfs_file(maj, min, device);
    }

    return 0;
}

static void drmFreePlatformDevice(drmDevicePtr device)
{
    if (device->deviceinfo.platform) {
        if (device->deviceinfo.platform->compatible) {
            char **compatible = device->deviceinfo.platform->compatible;

            while (*compatible) {
                free(*compatible);
                compatible++;
            }

            free(device->deviceinfo.platform->compatible);
        }
    }
}

static void drmFreeHost1xDevice(drmDevicePtr device)
{
    if (device->deviceinfo.host1x) {
        if (device->deviceinfo.host1x->compatible) {
            char **compatible = device->deviceinfo.host1x->compatible;

            while (*compatible) {
                free(*compatible);
                compatible++;
            }

            free(device->deviceinfo.host1x->compatible);
        }
    }
}

drm_public void drmFreeDevice(drmDevicePtr *device)
{
    if (device == NULL) {
        return;
    }

    if (*device) {
        switch ((*device)->bustype) {
            case DRM_BUS_PLATFORM:
                drmFreePlatformDevice(*device);
                break;

            case DRM_BUS_HOST1X:
                drmFreeHost1xDevice(*device);
                break;
        }
    }

    free(*device);
    *device = NULL;
}

drm_public void drmFreeDevices(drmDevicePtr devices[], int count)
{
    int i;

    if (devices == NULL) {
        return;
    }

    for (i = 0; i < count; i++) {
        if (devices[i]) {
            drmFreeDevice(&devices[i]);
        }
    }
}

static drmDevicePtr drmDeviceAlloc(unsigned int type, const char *node, size_t bus_size, size_t device_size, char **ptrp)
{
    char *ptr;
    unsigned int i;
    drmDevicePtr device;
    size_t max_node_length, extra, size;

    max_node_length = ALIGN(drmGetMaxNodeName(), sizeof(void *));
    extra = DRM_NODE_MAX * (sizeof(void *) + max_node_length);

    size = sizeof(*device) + extra + bus_size + device_size;

    device = (drmDevicePtr)calloc(1, size);
    if (!device) {
        return NULL;
    }

    device->available_nodes = 1 << type;
    ptr = (char *)device + sizeof(*device);
    device->nodes = (char **)ptr;

    ptr += DRM_NODE_MAX * sizeof(void *);

    for (i = 0; i < DRM_NODE_MAX; i++) {
        device->nodes[i] = ptr;
        ptr += max_node_length;
    }

    memcpy(device->nodes[type], node, max_node_length);
    *ptrp = ptr;

    return device;
}

static int drmProcessPciDevice(drmDevicePtr *device, const char *node, int node_type, int maj, int min, bool fetch_deviceinfo, uint32_t flags)
{
    int ret;
    char *addr;
    drmDevicePtr dev;

    dev = drmDeviceAlloc(node_type, node, sizeof(drmPciBusInfo), sizeof(drmPciDeviceInfo), &addr);
    if (!dev) {
        return -ENOMEM;
    }

    dev->bustype = DRM_BUS_PCI;
    dev->businfo.pci = (drmPciBusInfoPtr)addr;

    ret = drmParsePciBusInfo(maj, min, dev->businfo.pci);
    if (ret) {
        goto free_device;
    }
    if (fetch_deviceinfo) {
        addr += sizeof(drmPciBusInfo);
        dev->deviceinfo.pci = (drmPciDeviceInfoPtr)addr;

        ret = drmParsePciDeviceInfo(maj, min, dev->deviceinfo.pci, flags);
        if (ret) {
            goto free_device;
        }
    }

    *device = dev;
    return 0;

free_device:
    free(dev);
    return ret;
}

static int drm_usb_dev_path(int maj, int min, char *path, size_t len)
{
    char *value, *tmp_path, *slash;
    bool usb_device, usb_interface;

    snprintf(path, len, "/sys/dev/char/%d:%d/device", maj, min);
    value = sysfs_uevent_get(path, "DEVTYPE");
    if (!value) {
        return -ENOENT;
    }

    usb_device = strcmp(value, "usb_device") == 0;
    usb_interface = strcmp(value, "usb_interface") == 0;
    free(value);

    if (usb_device) {
        return 0;
    }

    if (!usb_interface) {
        return -ENOTSUP;
    }

    tmp_path = realpath(path, NULL);
    if (!tmp_path) {
        return -errno;
    }

    slash = strrchr(tmp_path, '/');
    if (!slash) {
        free(tmp_path);
        return -EINVAL;
    }

    *slash = '\0';

    if (snprintf(path, len, "%s", tmp_path) >= (int)len) {
        free(tmp_path);
        return -EINVAL;
    }

    free(tmp_path);
    return 0;
}

static int drmParseUsbBusInfo(int maj, int min, drmUsbBusInfoPtr info)
{
    int ret;
    unsigned int bus, dev;
    char path[PATH_MAX + 1], *value;

    ret = drm_usb_dev_path(maj, min, path, sizeof(path));
    if (ret < 0) {
        return ret;
    }

    value = sysfs_uevent_get(path, "BUSNUM");
    if (!value) {
        return -ENOENT;
    }

    ret = sscanf(value, "%03u", &bus);
    free(value);

    if (ret <= 0) {
        return -errno;
    }

    value = sysfs_uevent_get(path, "DEVNUM");
    if (!value) {
        return -ENOENT;
    }

    ret = sscanf(value, "%03u", &dev);
    free(value);

    if (ret <= 0) {
        return -errno;
    }

    info->bus = bus;
    info->dev = dev;

    return 0;
}

static int drmParseUsbDeviceInfo(int maj, int min, drmUsbDeviceInfoPtr info)
{
    int ret;
    unsigned int vendor, product;
    char path[PATH_MAX + 1], *value;

    ret = drm_usb_dev_path(maj, min, path, sizeof(path));
    if (ret < 0) {
        return ret;
    }

    value = sysfs_uevent_get(path, "PRODUCT");
    if (!value) {
        return -ENOENT;
    }

    ret = sscanf(value, "%x/%x", &vendor, &product);
    free(value);
    if (ret <= 0) {
        return -errno;
    }

    info->vendor = vendor;
    info->product = product;

    return 0;
}

static int drmProcessUsbDevice(drmDevicePtr *device, const char *node, int node_type, int maj, int min, bool fetch_deviceinfo, uint32_t flags)
{
    int ret;
    char *ptr;
    drmDevicePtr dev;

    dev = drmDeviceAlloc(node_type, node, sizeof(drmUsbBusInfo), sizeof(drmUsbDeviceInfo), &ptr);
    if (!dev) {
        return -ENOMEM;
    }

    dev->bustype = DRM_BUS_USB;
    dev->businfo.usb = (drmUsbBusInfoPtr)ptr;

    ret = drmParseUsbBusInfo(maj, min, dev->businfo.usb);
    if (ret < 0) {
        goto free_device;
    }

    if (fetch_deviceinfo) {
        ptr += sizeof(drmUsbBusInfo);
        dev->deviceinfo.usb = (drmUsbDeviceInfoPtr)ptr;

        ret = drmParseUsbDeviceInfo(maj, min, dev->deviceinfo.usb);
        if (ret < 0) {
            goto free_device;
        }
    }

    *device = dev;
    return 0;

free_device:
    free(dev);
    return ret;
}

static int drmParseOFBusInfo(int maj, int min, char *fullname)
{
    char path[PATH_MAX + 1], *name, *tmp_name;

    snprintf(path, sizeof(path), "/sys/dev/char/%d:%d/device", maj, min);

    name = sysfs_uevent_get(path, "OF_FULLNAME");
    tmp_name = name;
    if (!name) {
        name = sysfs_uevent_get(path, "MODALIAS");
        if (!name) {
            return -ENOENT;
        }

        tmp_name = strrchr(name, ':');
        if (!tmp_name) {
            free(name);
            return -ENOENT;
        }

        tmp_name++;
    }

    strncpy(fullname, tmp_name, DRM_PLATFORM_DEVICE_NAME_LEN);
    fullname[DRM_PLATFORM_DEVICE_NAME_LEN - 1] = '\0';
    free(name);

    return 0;
}

static int drmParseOFDeviceInfo(int maj, int min, char ***compatible)
{
    int err;
    unsigned int count, i;
    char path[PATH_MAX + 1], *value, *tmp_name;

    snprintf(path, sizeof(path), "/sys/dev/char/%d:%d/device", maj, min);

    value = sysfs_uevent_get(path, "OF_COMPATIBLE_N");
    if (value) {
        sscanf(value, "%u", &count);
        free(value);
    } else {
        count = 1;
    }

    *compatible = (char **)calloc(count + 1, sizeof(char *));
    if (!*compatible) {
        return -ENOMEM;
    }

    for (i = 0; i < count; i++) {
        value = sysfs_uevent_get(path, "OF_COMPATIBLE_%u", i);
        tmp_name = value;
        if (!value) {
            value = sysfs_uevent_get(path, "MODALIAS");
            if (!value) {
                err = -ENOENT;
                goto free;
            }

            tmp_name = strrchr(value, ':');
            if (!tmp_name) {
                free(value);
                return -ENOENT;
            }

            tmp_name = strdup(tmp_name + 1);
            free(value);
        }

        (*compatible)[i] = tmp_name;
    }

    return 0;

free:
    while (i--) {
        free((*compatible)[i]);
    }

    free(*compatible);
    return err;
}

static int drmProcessPlatformDevice(drmDevicePtr *device, const char *node, int node_type, int maj, int min, bool fetch_deviceinfo, uint32_t flags)
{
    int ret;
    char *ptr;
    drmDevicePtr dev;

    dev = drmDeviceAlloc(node_type, node, sizeof(drmPlatformBusInfo), sizeof(drmPlatformDeviceInfo), &ptr);
    if (!dev) {
        return -ENOMEM;
    }

    dev->bustype = DRM_BUS_PLATFORM;
    dev->businfo.platform = (drmPlatformBusInfoPtr)ptr;

    ret = drmParseOFBusInfo(maj, min, dev->businfo.platform->fullname);
    if (ret < 0) {
        goto free_device;
    }

    if (fetch_deviceinfo) {
        ptr += sizeof(drmPlatformBusInfo);
        dev->deviceinfo.platform = (drmPlatformDeviceInfoPtr)ptr;

        ret = drmParseOFDeviceInfo(maj, min, &dev->deviceinfo.platform->compatible);
        if (ret < 0) {
            goto free_device;
        }
    }

    *device = dev;
    return 0;

free_device:
    free(dev);
    return ret;
}

static int drmProcessHost1xDevice(drmDevicePtr *device, const char *node, int node_type, int maj, int min, bool fetch_deviceinfo, uint32_t flags)
{
    int ret;
    char *ptr;
    drmDevicePtr dev;

    dev = drmDeviceAlloc(node_type, node, sizeof(drmHost1xBusInfo), sizeof(drmHost1xDeviceInfo), &ptr);
    if (!dev) {
        return -ENOMEM;
    }

    dev->bustype = DRM_BUS_HOST1X;
    dev->businfo.host1x = (drmHost1xBusInfoPtr)ptr;

    ret = drmParseOFBusInfo(maj, min, dev->businfo.host1x->fullname);
    if (ret < 0) {
        goto free_device;
    }

    if (fetch_deviceinfo) {
        ptr += sizeof(drmHost1xBusInfo);
        dev->deviceinfo.host1x = (drmHost1xDeviceInfoPtr)ptr;

        ret = drmParseOFDeviceInfo(maj, min, &dev->deviceinfo.host1x->compatible);
        if (ret < 0) {
            goto free_device;
        }
    }

    *device = dev;
    return 0;

free_device:
    free(dev);
    return ret;
}

static int process_device(drmDevicePtr *device, const char *d_name, int req_subsystem_type, bool fetch_deviceinfo, uint32_t flags)
{
    struct stat sbuf;
    unsigned int maj, min;
    char node[PATH_MAX + 1];
    int node_type, subsystem_type;

    node_type = drmGetNodeType(d_name);
    if (node_type < 0) {
        return -1;
    }

    snprintf(node, PATH_MAX, "%s/%s", DRM_DIR_NAME, d_name);
    if (stat(node, &sbuf)) {
        return -1;
    }

    maj = major(sbuf.st_rdev);
    min = minor(sbuf.st_rdev);

    if (!drmNodeIsDRM(maj, min) || !S_ISCHR(sbuf.st_mode)) {
        return -1;
    }

    subsystem_type = drmParseSubsystemType(maj, min);
    if (req_subsystem_type != -1 && req_subsystem_type != subsystem_type) {
        return -1;
    }

    switch (subsystem_type) {
        case DRM_BUS_PCI:
        case DRM_BUS_VIRTIO:
            return drmProcessPciDevice(device, node, node_type, maj, min, fetch_deviceinfo, flags);

        case DRM_BUS_USB:
            return drmProcessUsbDevice(device, node, node_type, maj, min, fetch_deviceinfo, flags);

        case DRM_BUS_PLATFORM:
            return drmProcessPlatformDevice(device, node, node_type, maj, min, fetch_deviceinfo, flags);

        case DRM_BUS_HOST1X:
            return drmProcessHost1xDevice(device, node, node_type, maj, min, fetch_deviceinfo, flags);

        default:
            return -1;
    }
}

static void drmFoldDuplicatedDevices(drmDevicePtr local_devices[], int count)
{
    int node_type, i, j;

    for (i = 0; i < count; i++) {
        for (j = i + 1; j < count; j++) {
            if (drmDevicesEqual(local_devices[i], local_devices[j])) {
                local_devices[i]->available_nodes |= local_devices[j]->available_nodes;
                node_type = log2_int(local_devices[j]->available_nodes);
                memcpy(local_devices[i]->nodes[node_type], local_devices[j]->nodes[node_type], drmGetMaxNodeName());
                drmFreeDevice(&local_devices[j]);
            }
        }
    }
}

static int drm_device_validate_flags(uint32_t flags)
{
    return (flags & ~DRM_DEVICE_GET_PCI_REVISION);
}

static bool drm_device_has_rdev(drmDevicePtr device, dev_t find_rdev)
{
    struct stat sbuf;

    for (int i = 0; i < DRM_NODE_MAX; i++) {
        if (device->available_nodes & 1 << i) {
            if (stat(device->nodes[i], &sbuf) == 0 && sbuf.st_rdev == find_rdev) {
                return true;
            }
        }
    }

    return false;
}

#define MAX_DRM_NODES       256

drm_public int drmGetDeviceFromDevId(dev_t find_rdev, uint32_t flags, drmDevicePtr *device)
{
    DIR *sysdir;
    int maj, min;
    drmDevicePtr d;
    int subsystem_type;
    struct dirent *dent;
    int ret, i, node_count;
    drmDevicePtr local_devices[MAX_DRM_NODES];

    if (drm_device_validate_flags(flags)) {
        return -EINVAL;
    }

    if (device == NULL) {
        return -EINVAL;
    }

    maj = major(find_rdev);
    min = minor(find_rdev);

    if (!drmNodeIsDRM(maj, min)) {
        return -EINVAL;
    }

    subsystem_type = drmParseSubsystemType(maj, min);
    if (subsystem_type < 0) {
        return subsystem_type;
    }

    sysdir = opendir(DRM_DIR_NAME);
    if (!sysdir) {
        return -errno;
    }

    i = 0;
    while ((dent = readdir(sysdir))) {
        ret = process_device(&d, dent->d_name, subsystem_type, true, flags);
        if (ret) {
            continue;
        }

        if (i >= MAX_DRM_NODES) {
            fprintf(stderr, "More than %d drm nodes detected. Please report a bug - that should not happen.\nSkipping extra nodes\n", MAX_DRM_NODES);
            break;
        }

        local_devices[i] = d;
        i++;
    }
    node_count = i;

    drmFoldDuplicatedDevices(local_devices, node_count);
    *device = NULL;

    for (i = 0; i < node_count; i++) {
        if (!local_devices[i]) {
            continue;
        }

        if (drm_device_has_rdev(local_devices[i], find_rdev)) {
            *device = local_devices[i];
        } else {
            drmFreeDevice(&local_devices[i]);
        }
    }

    closedir(sysdir);
    if (*device == NULL) {
        return -ENODEV;
    }

    return 0;
}

drm_public int drmGetDevice2(int fd, uint32_t flags, drmDevicePtr *device)
{
    struct stat sbuf;

    if (fd == -1) {
        return -EINVAL;
    }

    if (fstat(fd, &sbuf)) {
        return -errno;
    }

    if (!S_ISCHR(sbuf.st_mode)) {
        return -EINVAL;
    }

    return drmGetDeviceFromDevId(sbuf.st_rdev, flags, device);
}

drm_public int drmGetDevice(int fd, drmDevicePtr *device)
{
    return drmGetDevice2(fd, DRM_DEVICE_GET_PCI_REVISION, device);
}

drm_public int drmGetDevices2(uint32_t flags, drmDevicePtr devices[], int max_devices)
{
    DIR *sysdir;
    struct dirent *dent;
    drmDevicePtr device;
    int ret, i, node_count, device_count;
    drmDevicePtr local_devices[MAX_DRM_NODES];

    if (drm_device_validate_flags(flags)) {
        return -EINVAL;
    }

    sysdir = opendir(DRM_DIR_NAME);
    if (!sysdir) {
        return -errno;
    }

    i = 0;
    while ((dent = readdir(sysdir))) {
        ret = process_device(&device, dent->d_name, -1, devices != NULL, flags);
        if (ret) {
            continue;
        }

        if (i >= MAX_DRM_NODES) {
            fprintf(stderr, "More than %d drm nodes detected. Please report a bug - that should not happen.\nSkipping extra nodes\n", MAX_DRM_NODES);
            break;
        }

        local_devices[i] = device;
        i++;
    }
    node_count = i;

    drmFoldDuplicatedDevices(local_devices, node_count);

    device_count = 0;
    for (i = 0; i < node_count; i++) {
        if (!local_devices[i]) {
            continue;
        }

        if ((devices != NULL) && (device_count < max_devices)) {
            devices[device_count] = local_devices[i];
        } else {
            drmFreeDevice(&local_devices[i]);
        }

        device_count++;
    }

    closedir(sysdir);
    if (devices != NULL) {
        return MIN2(device_count, max_devices);
    }

    return device_count;
}

drm_public int drmGetDevices(drmDevicePtr devices[], int max_devices)
{
    return drmGetDevices2(DRM_DEVICE_GET_PCI_REVISION, devices, max_devices);
}

drm_public char *drmGetDeviceNameFromFd2(int fd)
{
    struct stat sbuf;
    unsigned int maj, min;
    char path[PATH_MAX + 1], *value;

    if (fstat(fd, &sbuf)) {
        return NULL;
    }

    maj = major(sbuf.st_rdev);
    min = minor(sbuf.st_rdev);

    if (!drmNodeIsDRM(maj, min) || !S_ISCHR(sbuf.st_mode)) {
        return NULL;
    }

    snprintf(path, sizeof(path), "/sys/dev/char/%d:%d", maj, min);

    value = sysfs_uevent_get(path, "DEVNAME");
    if (!value) {
        return NULL;
    }

    snprintf(path, sizeof(path), "/dev/%s", value);
    free(value);

    return strdup(path);
}

drm_public int drmSyncobjCreate(int fd, uint32_t flags, uint32_t *handle)
{
    int ret;
    struct drm_syncobj_create args;

    memclear(args);
    args.flags = flags;
    args.handle = 0;
    ret = drmIoctl(fd, DRM_IOCTL_SYNCOBJ_CREATE, &args);
    if (ret) {
        return ret;
    }
    *handle = args.handle;

    return 0;
}

drm_public int drmSyncobjDestroy(int fd, uint32_t handle)
{
    struct drm_syncobj_destroy args;

    memclear(args);
    args.handle = handle;
    return drmIoctl(fd, DRM_IOCTL_SYNCOBJ_DESTROY, &args);
}

drm_public int drmSyncobjHandleToFD(int fd, uint32_t handle, int *obj_fd)
{
    int ret;
    struct drm_syncobj_handle args;

    memclear(args);
    args.fd = -1;
    args.handle = handle;
    ret = drmIoctl(fd, DRM_IOCTL_SYNCOBJ_HANDLE_TO_FD, &args);
    if (ret) {
        return ret;
    }
    *obj_fd = args.fd;

    return 0;
}

drm_public int drmSyncobjFDToHandle(int fd, int obj_fd, uint32_t *handle)
{
    int ret;
    struct drm_syncobj_handle args;

    memclear(args);
    args.fd = obj_fd;
    args.handle = 0;
    ret = drmIoctl(fd, DRM_IOCTL_SYNCOBJ_FD_TO_HANDLE, &args);
    if (ret) {
        return ret;
    }
    *handle = args.handle;

    return 0;
}

drm_public int drmSyncobjImportSyncFile(int fd, uint32_t handle, int sync_file_fd)
{
    struct drm_syncobj_handle args;

    memclear(args);
    args.fd = sync_file_fd;
    args.handle = handle;
    args.flags = DRM_SYNCOBJ_FD_TO_HANDLE_FLAGS_IMPORT_SYNC_FILE;

    return drmIoctl(fd, DRM_IOCTL_SYNCOBJ_FD_TO_HANDLE, &args);
}

drm_public int drmSyncobjExportSyncFile(int fd, uint32_t handle, int *sync_file_fd)
{
    int ret;
    struct drm_syncobj_handle args;

    memclear(args);
    args.fd = -1;
    args.handle = handle;
    args.flags = DRM_SYNCOBJ_HANDLE_TO_FD_FLAGS_EXPORT_SYNC_FILE;
    ret = drmIoctl(fd, DRM_IOCTL_SYNCOBJ_HANDLE_TO_FD, &args);
    if (ret) {
        return ret;
    }
    *sync_file_fd = args.fd;

    return 0;
}

drm_public int drmSyncobjWait(int fd, uint32_t *handles, unsigned num_handles, int64_t timeout_nsec, unsigned flags, uint32_t *first_signaled)
{
    int ret;
    struct drm_syncobj_wait args;

    memclear(args);
    args.handles = (uintptr_t)handles;
    args.timeout_nsec = timeout_nsec;
    args.count_handles = num_handles;
    args.flags = flags;

    ret = drmIoctl(fd, DRM_IOCTL_SYNCOBJ_WAIT, &args);
    if (ret < 0) {
        return -errno;
    }

    if (first_signaled) {
        *first_signaled = args.first_signaled;
    }

    return ret;
}

drm_public int drmSyncobjReset(int fd, const uint32_t *handles, uint32_t handle_count)
{
    int ret;
    struct drm_syncobj_array args;

    memclear(args);
    args.handles = (uintptr_t)handles;
    args.count_handles = handle_count;

    ret = drmIoctl(fd, DRM_IOCTL_SYNCOBJ_RESET, &args);
    return ret;
}

drm_public int drmSyncobjSignal(int fd, const uint32_t *handles, uint32_t handle_count)
{
    int ret;
    struct drm_syncobj_array args;

    memclear(args);
    args.handles = (uintptr_t)handles;
    args.count_handles = handle_count;

    ret = drmIoctl(fd, DRM_IOCTL_SYNCOBJ_SIGNAL, &args);
    return ret;
}

drm_public int drmSyncobjTimelineSignal(int fd, const uint32_t *handles, uint64_t *points, uint32_t handle_count)
{
    int ret;
    struct drm_syncobj_timeline_array args;

    memclear(args);
    args.handles = (uintptr_t)handles;
    args.points = (uintptr_t)points;
    args.count_handles = handle_count;

    ret = drmIoctl(fd, DRM_IOCTL_SYNCOBJ_TIMELINE_SIGNAL, &args);
    return ret;
}

drm_public int drmSyncobjTimelineWait(int fd, uint32_t *handles, uint64_t *points, unsigned num_handles, int64_t timeout_nsec, unsigned flags, uint32_t *first_signaled)
{
    int ret;
    struct drm_syncobj_timeline_wait args;

    memclear(args);
    args.handles = (uintptr_t)handles;
    args.points = (uintptr_t)points;
    args.timeout_nsec = timeout_nsec;
    args.count_handles = num_handles;
    args.flags = flags;

    ret = drmIoctl(fd, DRM_IOCTL_SYNCOBJ_TIMELINE_WAIT, &args);
    if (ret < 0) {
        return -errno;
    }

    if (first_signaled) {
        *first_signaled = args.first_signaled;
    }

    return ret;
}

drm_public int drmSyncobjQuery(int fd, uint32_t *handles, uint64_t *points, uint32_t handle_count)
{
    int ret;
    struct drm_syncobj_timeline_array args;

    memclear(args);
    args.handles = (uintptr_t)handles;
    args.points = (uintptr_t)points;
    args.count_handles = handle_count;

    ret = drmIoctl(fd, DRM_IOCTL_SYNCOBJ_QUERY, &args);
    if (ret) {
        return ret;
    }

    return 0;
}

drm_public int drmSyncobjQuery2(int fd, uint32_t *handles, uint64_t *points, uint32_t handle_count, uint32_t flags)
{
    struct drm_syncobj_timeline_array args;

    memclear(args);
    args.handles = (uintptr_t)handles;
    args.points = (uintptr_t)points;
    args.count_handles = handle_count;
    args.flags = flags;

    return drmIoctl(fd, DRM_IOCTL_SYNCOBJ_QUERY, &args);
}

drm_public int drmSyncobjTransfer(int fd, uint32_t dst_handle, uint64_t dst_point, uint32_t src_handle, uint64_t src_point, uint32_t flags)
{
    int ret;
    struct drm_syncobj_transfer args;

    memclear(args);
    args.src_handle = src_handle;
    args.dst_handle = dst_handle;
    args.src_point = src_point;
    args.dst_point = dst_point;
    args.flags = flags;

    ret = drmIoctl(fd, DRM_IOCTL_SYNCOBJ_TRANSFER, &args);
    return ret;
}

static char *drmGetFormatModifierFromSimpleTokens(uint64_t modifier)
{
    unsigned int i;

    for (i = 0; i < ARRAY_SIZE(drm_format_modifier_table); i++) {
        if (drm_format_modifier_table[i].modifier == modifier) {
            return strdup(drm_format_modifier_table[i].modifier_name);
        }
    }

    return NULL;
}

drm_public char *drmGetFormatModifierVendor(uint64_t modifier)
{
    unsigned int i;
    uint8_t vendor = fourcc_mod_get_vendor(modifier);

    for (i = 0; i < ARRAY_SIZE(drm_format_modifier_vendor_table); i++) {
        if (drm_format_modifier_vendor_table[i].vendor == vendor) {
            return strdup(drm_format_modifier_vendor_table[i].vendor_name);
        }
    }

    return NULL;
}

drm_public char *drmGetFormatModifierName(uint64_t modifier)
{
    unsigned int i;
    char *modifier_found = NULL;
    uint8_t vendorid = fourcc_mod_get_vendor(modifier);

    for (i = 0; i < ARRAY_SIZE(modifier_format_vendor_table); i++) {
        if (modifier_format_vendor_table[i].vendor == vendorid) {
            modifier_found = modifier_format_vendor_table[i].vendor_cb(modifier);
        }
    }

    if (!modifier_found) {
        return drmGetFormatModifierFromSimpleTokens(modifier);
    }

    return modifier_found;
}

drm_public char *drmGetFormatName(uint32_t format)
{
    const char *be;
    size_t str_size, i;
    char *str, code[5];

    be = (format & DRM_FORMAT_BIG_ENDIAN) ? "_BE" : "";
    format &= ~DRM_FORMAT_BIG_ENDIAN;

    if (format == DRM_FORMAT_INVALID) {
        return strdup("INVALID");
    }

    code[0] = (char) ((format >> 0) & 0xFF);
    code[1] = (char) ((format >> 8) & 0xFF);
    code[2] = (char) ((format >> 16) & 0xFF);
    code[3] = (char) ((format >> 24) & 0xFF);
    code[4] = '\0';

    for (i = 3; i > 0 && code[i] == ' '; i--) {
        code[i] = '\0';
    }

    str_size = strlen(code) + strlen(be) + 1;
    str = (char *)malloc(str_size);
    if (!str) {
        return NULL;
    }

    snprintf(str, str_size, "%s%s", code, be);
    return str;
}
