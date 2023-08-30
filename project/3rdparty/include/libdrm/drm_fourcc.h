#ifndef LIBDRM_DRM_FOURCC_H
#define LIBDRM_DRM_FOURCC_H

#include "drm.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define fourcc_code(a, b, c, d)                         ((__u32)(a) | ((__u32)(b) << 8) | ((__u32)(c) << 16) | ((__u32)(d) << 24))

#define DRM_FORMAT_BIG_ENDIAN                           (1U<<31)

#define DRM_FORMAT_INVALID                              0

#define DRM_FORMAT_C1                                   fourcc_code('C', '1', ' ', ' ') /* [7:0] C0:C1:C2:C3:C4:C5:C6:C7 1:1:1:1:1:1:1:1 eight pixels/byte */
#define DRM_FORMAT_C2                                   fourcc_code('C', '2', ' ', ' ') /* [7:0] C0:C1:C2:C3 2:2:2:2 four pixels/byte */
#define DRM_FORMAT_C4                                   fourcc_code('C', '4', ' ', ' ') /* [7:0] C0:C1 4:4 two pixels/byte */
#define DRM_FORMAT_C8                                   fourcc_code('C', '8', ' ', ' ') /* [7:0] C */

#define DRM_FORMAT_D1                                   fourcc_code('D', '1', ' ', ' ') /* [7:0] D0:D1:D2:D3:D4:D5:D6:D7 1:1:1:1:1:1:1:1 eight pixels/byte */
#define DRM_FORMAT_D2                                   fourcc_code('D', '2', ' ', ' ') /* [7:0] D0:D1:D2:D3 2:2:2:2 four pixels/byte */
#define DRM_FORMAT_D4                                   fourcc_code('D', '4', ' ', ' ') /* [7:0] D0:D1 4:4 two pixels/byte */
#define DRM_FORMAT_D8                                   fourcc_code('D', '8', ' ', ' ') /* [7:0] D */

#define DRM_FORMAT_R1                                   fourcc_code('R', '1', ' ', ' ') /* [7:0] R0:R1:R2:R3:R4:R5:R6:R7 1:1:1:1:1:1:1:1 eight pixels/byte */
#define DRM_FORMAT_R2                                   fourcc_code('R', '2', ' ', ' ') /* [7:0] R0:R1:R2:R3 2:2:2:2 four pixels/byte */
#define DRM_FORMAT_R4                                   fourcc_code('R', '4', ' ', ' ') /* [7:0] R0:R1 4:4 two pixels/byte */
#define DRM_FORMAT_R8                                   fourcc_code('R', '8', ' ', ' ') /* [7:0] R */
#define DRM_FORMAT_R10                                  fourcc_code('R', '1', '0', ' ') /* [15:0] x:R 6:10 little endian */
#define DRM_FORMAT_R12                                  fourcc_code('R', '1', '2', ' ') /* [15:0] x:R 4:12 little endian */
#define DRM_FORMAT_R16                                  fourcc_code('R', '1', '6', ' ') /* [15:0] R little endian */

#define DRM_FORMAT_RG88                                 fourcc_code('R', 'G', '8', '8') /* [15:0] R:G 8:8 little endian */
#define DRM_FORMAT_GR88                                 fourcc_code('G', 'R', '8', '8') /* [15:0] G:R 8:8 little endian */

#define DRM_FORMAT_RG1616                               fourcc_code('R', 'G', '3', '2') /* [31:0] R:G 16:16 little endian */
#define DRM_FORMAT_GR1616                               fourcc_code('G', 'R', '3', '2') /* [31:0] G:R 16:16 little endian */

#define DRM_FORMAT_RGB332                               fourcc_code('R', 'G', 'B', '8') /* [7:0] R:G:B 3:3:2 */
#define DRM_FORMAT_BGR233                               fourcc_code('B', 'G', 'R', '8') /* [7:0] B:G:R 2:3:3 */

#define DRM_FORMAT_XRGB4444                             fourcc_code('X', 'R', '1', '2') /* [15:0] x:R:G:B 4:4:4:4 little endian */
#define DRM_FORMAT_XBGR4444                             fourcc_code('X', 'B', '1', '2') /* [15:0] x:B:G:R 4:4:4:4 little endian */
#define DRM_FORMAT_RGBX4444                             fourcc_code('R', 'X', '1', '2') /* [15:0] R:G:B:x 4:4:4:4 little endian */
#define DRM_FORMAT_BGRX4444                             fourcc_code('B', 'X', '1', '2') /* [15:0] B:G:R:x 4:4:4:4 little endian */

#define DRM_FORMAT_ARGB4444                             fourcc_code('A', 'R', '1', '2') /* [15:0] A:R:G:B 4:4:4:4 little endian */
#define DRM_FORMAT_ABGR4444                             fourcc_code('A', 'B', '1', '2') /* [15:0] A:B:G:R 4:4:4:4 little endian */
#define DRM_FORMAT_RGBA4444                             fourcc_code('R', 'A', '1', '2') /* [15:0] R:G:B:A 4:4:4:4 little endian */
#define DRM_FORMAT_BGRA4444                             fourcc_code('B', 'A', '1', '2') /* [15:0] B:G:R:A 4:4:4:4 little endian */

#define DRM_FORMAT_XRGB1555                             fourcc_code('X', 'R', '1', '5') /* [15:0] x:R:G:B 1:5:5:5 little endian */
#define DRM_FORMAT_XBGR1555                             fourcc_code('X', 'B', '1', '5') /* [15:0] x:B:G:R 1:5:5:5 little endian */
#define DRM_FORMAT_RGBX5551                             fourcc_code('R', 'X', '1', '5') /* [15:0] R:G:B:x 5:5:5:1 little endian */
#define DRM_FORMAT_BGRX5551                             fourcc_code('B', 'X', '1', '5') /* [15:0] B:G:R:x 5:5:5:1 little endian */

#define DRM_FORMAT_ARGB1555                             fourcc_code('A', 'R', '1', '5') /* [15:0] A:R:G:B 1:5:5:5 little endian */
#define DRM_FORMAT_ABGR1555                             fourcc_code('A', 'B', '1', '5') /* [15:0] A:B:G:R 1:5:5:5 little endian */
#define DRM_FORMAT_RGBA5551                             fourcc_code('R', 'A', '1', '5') /* [15:0] R:G:B:A 5:5:5:1 little endian */
#define DRM_FORMAT_BGRA5551                             fourcc_code('B', 'A', '1', '5') /* [15:0] B:G:R:A 5:5:5:1 little endian */

#define DRM_FORMAT_RGB565                               fourcc_code('R', 'G', '1', '6') /* [15:0] R:G:B 5:6:5 little endian */
#define DRM_FORMAT_BGR565                               fourcc_code('B', 'G', '1', '6') /* [15:0] B:G:R 5:6:5 little endian */

#define DRM_FORMAT_RGB888                               fourcc_code('R', 'G', '2', '4') /* [23:0] R:G:B little endian */
#define DRM_FORMAT_BGR888                               fourcc_code('B', 'G', '2', '4') /* [23:0] B:G:R little endian */

#define DRM_FORMAT_XRGB8888                             fourcc_code('X', 'R', '2', '4') /* [31:0] x:R:G:B 8:8:8:8 little endian */
#define DRM_FORMAT_XBGR8888                             fourcc_code('X', 'B', '2', '4') /* [31:0] x:B:G:R 8:8:8:8 little endian */
#define DRM_FORMAT_RGBX8888                             fourcc_code('R', 'X', '2', '4') /* [31:0] R:G:B:x 8:8:8:8 little endian */
#define DRM_FORMAT_BGRX8888                             fourcc_code('B', 'X', '2', '4') /* [31:0] B:G:R:x 8:8:8:8 little endian */

#define DRM_FORMAT_ARGB8888                             fourcc_code('A', 'R', '2', '4') /* [31:0] A:R:G:B 8:8:8:8 little endian */
#define DRM_FORMAT_ABGR8888                             fourcc_code('A', 'B', '2', '4') /* [31:0] A:B:G:R 8:8:8:8 little endian */
#define DRM_FORMAT_RGBA8888                             fourcc_code('R', 'A', '2', '4') /* [31:0] R:G:B:A 8:8:8:8 little endian */
#define DRM_FORMAT_BGRA8888                             fourcc_code('B', 'A', '2', '4') /* [31:0] B:G:R:A 8:8:8:8 little endian */

#define DRM_FORMAT_XRGB2101010                          fourcc_code('X', 'R', '3', '0') /* [31:0] x:R:G:B 2:10:10:10 little endian */
#define DRM_FORMAT_XBGR2101010                          fourcc_code('X', 'B', '3', '0') /* [31:0] x:B:G:R 2:10:10:10 little endian */
#define DRM_FORMAT_RGBX1010102                          fourcc_code('R', 'X', '3', '0') /* [31:0] R:G:B:x 10:10:10:2 little endian */
#define DRM_FORMAT_BGRX1010102                          fourcc_code('B', 'X', '3', '0') /* [31:0] B:G:R:x 10:10:10:2 little endian */

#define DRM_FORMAT_ARGB2101010                          fourcc_code('A', 'R', '3', '0') /* [31:0] A:R:G:B 2:10:10:10 little endian */
#define DRM_FORMAT_ABGR2101010                          fourcc_code('A', 'B', '3', '0') /* [31:0] A:B:G:R 2:10:10:10 little endian */
#define DRM_FORMAT_RGBA1010102                          fourcc_code('R', 'A', '3', '0') /* [31:0] R:G:B:A 10:10:10:2 little endian */
#define DRM_FORMAT_BGRA1010102                          fourcc_code('B', 'A', '3', '0') /* [31:0] B:G:R:A 10:10:10:2 little endian */

#define DRM_FORMAT_XRGB16161616                         fourcc_code('X', 'R', '4', '8') /* [63:0] x:R:G:B 16:16:16:16 little endian */
#define DRM_FORMAT_XBGR16161616                         fourcc_code('X', 'B', '4', '8') /* [63:0] x:B:G:R 16:16:16:16 little endian */

#define DRM_FORMAT_ARGB16161616                         fourcc_code('A', 'R', '4', '8') /* [63:0] A:R:G:B 16:16:16:16 little endian */
#define DRM_FORMAT_ABGR16161616                         fourcc_code('A', 'B', '4', '8') /* [63:0] A:B:G:R 16:16:16:16 little endian */

#define DRM_FORMAT_XRGB16161616F                        fourcc_code('X', 'R', '4', 'H') /* [63:0] x:R:G:B 16:16:16:16 little endian */
#define DRM_FORMAT_XBGR16161616F                        fourcc_code('X', 'B', '4', 'H') /* [63:0] x:B:G:R 16:16:16:16 little endian */

#define DRM_FORMAT_ARGB16161616F                        fourcc_code('A', 'R', '4', 'H') /* [63:0] A:R:G:B 16:16:16:16 little endian */
#define DRM_FORMAT_ABGR16161616F                        fourcc_code('A', 'B', '4', 'H') /* [63:0] A:B:G:R 16:16:16:16 little endian */

#define DRM_FORMAT_AXBXGXRX106106106106                 fourcc_code('A', 'B', '1', '0') /* [63:0] A:x:B:x:G:x:R:x 10:6:10:6:10:6:10:6 little endian */

#define DRM_FORMAT_YUYV                                 fourcc_code('Y', 'U', 'Y', 'V') /* [31:0] Cr0:Y1:Cb0:Y0 8:8:8:8 little endian */
#define DRM_FORMAT_YVYU                                 fourcc_code('Y', 'V', 'Y', 'U') /* [31:0] Cb0:Y1:Cr0:Y0 8:8:8:8 little endian */
#define DRM_FORMAT_UYVY                                 fourcc_code('U', 'Y', 'V', 'Y') /* [31:0] Y1:Cr0:Y0:Cb0 8:8:8:8 little endian */
#define DRM_FORMAT_VYUY                                 fourcc_code('V', 'Y', 'U', 'Y') /* [31:0] Y1:Cb0:Y0:Cr0 8:8:8:8 little endian */

#define DRM_FORMAT_AYUV                                 fourcc_code('A', 'Y', 'U', 'V') /* [31:0] A:Y:Cb:Cr 8:8:8:8 little endian */
#define DRM_FORMAT_AVUY8888                             fourcc_code('A', 'V', 'U', 'Y') /* [31:0] A:Cr:Cb:Y 8:8:8:8 little endian */
#define DRM_FORMAT_XYUV8888                             fourcc_code('X', 'Y', 'U', 'V') /* [31:0] X:Y:Cb:Cr 8:8:8:8 little endian */
#define DRM_FORMAT_XVUY8888                             fourcc_code('X', 'V', 'U', 'Y') /* [31:0] X:Cr:Cb:Y 8:8:8:8 little endian */
#define DRM_FORMAT_VUY888                               fourcc_code('V', 'U', '2', '4') /* [23:0] Cr:Cb:Y 8:8:8 little endian */
#define DRM_FORMAT_VUY101010                            fourcc_code('V', 'U', '3', '0') /* Y followed by U then V, 10:10:10. Non-linear modifier only */

#define DRM_FORMAT_Y210                                 fourcc_code('Y', '2', '1', '0') /* [63:0] Cr0:0:Y1:0:Cb0:0:Y0:0 10:6:10:6:10:6:10:6 little endian per 2 Y pixels */
#define DRM_FORMAT_Y212                                 fourcc_code('Y', '2', '1', '2') /* [63:0] Cr0:0:Y1:0:Cb0:0:Y0:0 12:4:12:4:12:4:12:4 little endian per 2 Y pixels */
#define DRM_FORMAT_Y216                                 fourcc_code('Y', '2', '1', '6') /* [63:0] Cr0:Y1:Cb0:Y0 16:16:16:16 little endian per 2 Y pixels */

#define DRM_FORMAT_Y410                                 fourcc_code('Y', '4', '1', '0') /* [31:0] A:Cr:Y:Cb 2:10:10:10 little endian */
#define DRM_FORMAT_Y412                                 fourcc_code('Y', '4', '1', '2') /* [63:0] A:0:Cr:0:Y:0:Cb:0 12:4:12:4:12:4:12:4 little endian */
#define DRM_FORMAT_Y416                                 fourcc_code('Y', '4', '1', '6') /* [63:0] A:Cr:Y:Cb 16:16:16:16 little endian */

#define DRM_FORMAT_XVYU2101010                          fourcc_code('X', 'V', '3', '0') /* [31:0] X:Cr:Y:Cb 2:10:10:10 little endian */
#define DRM_FORMAT_XVYU12_16161616                      fourcc_code('X', 'V', '3', '6') /* [63:0] X:0:Cr:0:Y:0:Cb:0 12:4:12:4:12:4:12:4 little endian */
#define DRM_FORMAT_XVYU16161616                         fourcc_code('X', 'V', '4', '8') /* [63:0] X:Cr:Y:Cb 16:16:16:16 little endian */

/* [63:0]   A3:A2:Y3:0:Cr0:0:Y2:0:A1:A0:Y1:0:Cb0:0:Y0:0  1:1:8:2:8:2:8:2:1:1:8:2:8:2:8:2 little endian */
#define DRM_FORMAT_Y0L0                                 fourcc_code('Y', '0', 'L', '0')

/* [63:0]   X3:X2:Y3:0:Cr0:0:Y2:0:X1:X0:Y1:0:Cb0:0:Y0:0  1:1:8:2:8:2:8:2:1:1:8:2:8:2:8:2 little endian */
#define DRM_FORMAT_X0L0                                 fourcc_code('X', '0', 'L', '0')

/* [63:0]   A3:A2:Y3:Cr0:Y2:A1:A0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10 little endian */
#define DRM_FORMAT_Y0L2                                 fourcc_code('Y', '0', 'L', '2')

/* [63:0]   X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0  1:1:10:10:10:1:1:10:10:10 little endian */
#define DRM_FORMAT_X0L2                                 fourcc_code('X', '0', 'L', '2')

#define DRM_FORMAT_YUV420_8BIT                          fourcc_code('Y', 'U', '0', '8')
#define DRM_FORMAT_YUV420_10BIT                         fourcc_code('Y', 'U', '1', '0')

#define DRM_FORMAT_XRGB8888_A8                          fourcc_code('X', 'R', 'A', '8')
#define DRM_FORMAT_XBGR8888_A8                          fourcc_code('X', 'B', 'A', '8')
#define DRM_FORMAT_RGBX8888_A8                          fourcc_code('R', 'X', 'A', '8')
#define DRM_FORMAT_BGRX8888_A8                          fourcc_code('B', 'X', 'A', '8')
#define DRM_FORMAT_RGB888_A8                            fourcc_code('R', '8', 'A', '8')
#define DRM_FORMAT_BGR888_A8                            fourcc_code('B', '8', 'A', '8')
#define DRM_FORMAT_RGB565_A8                            fourcc_code('R', '5', 'A', '8')
#define DRM_FORMAT_BGR565_A8                            fourcc_code('B', '5', 'A', '8')

#define DRM_FORMAT_NV12                                 fourcc_code('N', 'V', '1', '2') /* 2x2 subsampled Cr:Cb plane */
#define DRM_FORMAT_NV21                                 fourcc_code('N', 'V', '2', '1') /* 2x2 subsampled Cb:Cr plane */
#define DRM_FORMAT_NV16                                 fourcc_code('N', 'V', '1', '6') /* 2x1 subsampled Cr:Cb plane */
#define DRM_FORMAT_NV61                                 fourcc_code('N', 'V', '6', '1') /* 2x1 subsampled Cb:Cr plane */
#define DRM_FORMAT_NV24                                 fourcc_code('N', 'V', '2', '4') /* non-subsampled Cr:Cb plane */
#define DRM_FORMAT_NV42                                 fourcc_code('N', 'V', '4', '2') /* non-subsampled Cb:Cr plane */
#define DRM_FORMAT_NV15                                 fourcc_code('N', 'V', '1', '5') /* 2x2 subsampled Cr:Cb plane */
#define DRM_FORMAT_NV12_10                              fourcc_code('N', 'A', '1', '2')

#define DRM_FORMAT_P210                                 fourcc_code('P', '2', '1', '0') /* 2x1 subsampled Cr:Cb plane, 10 bit per channel */
#define DRM_FORMAT_P010                                 fourcc_code('P', '0', '1', '0') /* 2x2 subsampled Cr:Cb plane 10 bits per channel */
#define DRM_FORMAT_P012                                 fourcc_code('P', '0', '1', '2') /* 2x2 subsampled Cr:Cb plane 12 bits per channel */
#define DRM_FORMAT_P016                                 fourcc_code('P', '0', '1', '6') /* 2x2 subsampled Cr:Cb plane 16 bits per channel */
#define DRM_FORMAT_P030                                 fourcc_code('P', '0', '3', '0') /* 2x2 subsampled Cr:Cb plane 10 bits per channel packed */

#define DRM_FORMAT_Q410                                 fourcc_code('Q', '4', '1', '0')
#define DRM_FORMAT_Q401                                 fourcc_code('Q', '4', '0', '1')

#define DRM_FORMAT_YUV410                               fourcc_code('Y', 'U', 'V', '9') /* 4x4 subsampled Cb (1) and Cr (2) planes */
#define DRM_FORMAT_YVU410                               fourcc_code('Y', 'V', 'U', '9') /* 4x4 subsampled Cr (1) and Cb (2) planes */
#define DRM_FORMAT_YUV411                               fourcc_code('Y', 'U', '1', '1') /* 4x1 subsampled Cb (1) and Cr (2) planes */
#define DRM_FORMAT_YVU411                               fourcc_code('Y', 'V', '1', '1') /* 4x1 subsampled Cr (1) and Cb (2) planes */
#define DRM_FORMAT_YUV420                               fourcc_code('Y', 'U', '1', '2') /* 2x2 subsampled Cb (1) and Cr (2) planes */
#define DRM_FORMAT_YVU420                               fourcc_code('Y', 'V', '1', '2') /* 2x2 subsampled Cr (1) and Cb (2) planes */
#define DRM_FORMAT_YUV422                               fourcc_code('Y', 'U', '1', '6') /* 2x1 subsampled Cb (1) and Cr (2) planes */
#define DRM_FORMAT_YVU422                               fourcc_code('Y', 'V', '1', '6') /* 2x1 subsampled Cr (1) and Cb (2) planes */
#define DRM_FORMAT_YUV444                               fourcc_code('Y', 'U', '2', '4') /* non-subsampled Cb (1) and Cr (2) planes */
#define DRM_FORMAT_YVU444                               fourcc_code('Y', 'V', '2', '4') /* non-subsampled Cr (1) and Cb (2) planes */

#define DRM_FORMAT_MOD_VENDOR_NONE                      0
#define DRM_FORMAT_MOD_VENDOR_INTEL                     0x01
#define DRM_FORMAT_MOD_VENDOR_AMD                       0x02
#define DRM_FORMAT_MOD_VENDOR_NVIDIA                    0x03
#define DRM_FORMAT_MOD_VENDOR_SAMSUNG                   0x04
#define DRM_FORMAT_MOD_VENDOR_QCOM                      0x05
#define DRM_FORMAT_MOD_VENDOR_VIVANTE                   0x06
#define DRM_FORMAT_MOD_VENDOR_BROADCOM                  0x07
#define DRM_FORMAT_MOD_VENDOR_ARM                       0x08
#define DRM_FORMAT_MOD_VENDOR_ALLWINNER                 0x09
#define DRM_FORMAT_MOD_VENDOR_AMLOGIC                   0x0a
#define DRM_FORMAT_MOD_VENDOR_ROCKCHIP                  0x0b

#define DRM_FORMAT_RESERVED                             ((1ULL << 56) - 1)

#define fourcc_mod_get_vendor(modifier)                 (((modifier) >> 56) & 0xff)
#define fourcc_mod_is_vendor(modifier, vendor)          (fourcc_mod_get_vendor(modifier) == DRM_FORMAT_MOD_VENDOR_## vendor)

#define fourcc_mod_code(vendor, val)                    ((((__u64)DRM_FORMAT_MOD_VENDOR_## vendor) << 56) | ((val) & 0x00ffffffffffffffULL))

#define DRM_FORMAT_MOD_GENERIC_16_16_TILE               DRM_FORMAT_MOD_SAMSUNG_16_16_TILE

#define DRM_FORMAT_MOD_INVALID                          fourcc_mod_code(NONE, DRM_FORMAT_RESERVED)

#define DRM_FORMAT_MOD_LINEAR                           fourcc_mod_code(NONE, 0)

#define DRM_FORMAT_MOD_NONE                             0

#define DRM_FORMAT_MOD_ROCKCHIP_10BITS                  fourcc_mod_code(ROCKCHIP, 1)

#define I915_FORMAT_MOD_X_TILED                         fourcc_mod_code(INTEL, 1)
#define I915_FORMAT_MOD_Y_TILED                         fourcc_mod_code(INTEL, 2)
#define I915_FORMAT_MOD_Yf_TILED                        fourcc_mod_code(INTEL, 3)

#define I915_FORMAT_MOD_Y_TILED_CCS                     fourcc_mod_code(INTEL, 4)
#define I915_FORMAT_MOD_Yf_TILED_CCS                    fourcc_mod_code(INTEL, 5)

#define I915_FORMAT_MOD_Y_TILED_GEN12_RC_CCS            fourcc_mod_code(INTEL, 6)
#define I915_FORMAT_MOD_Y_TILED_GEN12_MC_CCS            fourcc_mod_code(INTEL, 7)
#define I915_FORMAT_MOD_Y_TILED_GEN12_RC_CCS_CC         fourcc_mod_code(INTEL, 8)

#define I915_FORMAT_MOD_4_TILED                         fourcc_mod_code(INTEL, 9)
#define I915_FORMAT_MOD_4_TILED_DG2_RC_CCS              fourcc_mod_code(INTEL, 10)
#define I915_FORMAT_MOD_4_TILED_DG2_MC_CCS              fourcc_mod_code(INTEL, 11)
#define I915_FORMAT_MOD_4_TILED_DG2_RC_CCS_CC           fourcc_mod_code(INTEL, 12)
#define I915_FORMAT_MOD_4_TILED_MTL_RC_CCS              fourcc_mod_code(INTEL, 13)
#define I915_FORMAT_MOD_4_TILED_MTL_MC_CCS              fourcc_mod_code(INTEL, 14)
#define I915_FORMAT_MOD_4_TILED_MTL_RC_CCS_CC           fourcc_mod_code(INTEL, 15)

#define DRM_FORMAT_MOD_SAMSUNG_64_32_TILE               fourcc_mod_code(SAMSUNG, 1)
#define DRM_FORMAT_MOD_SAMSUNG_16_16_TILE               fourcc_mod_code(SAMSUNG, 2)

#define DRM_FORMAT_MOD_QCOM_COMPRESSED                  fourcc_mod_code(QCOM, 1)

#define DRM_FORMAT_MOD_QCOM_TILED3                      fourcc_mod_code(QCOM, 3)
#define DRM_FORMAT_MOD_QCOM_TILED2                      fourcc_mod_code(QCOM, 2)

#define DRM_FORMAT_MOD_VIVANTE_TILED                    fourcc_mod_code(VIVANTE, 1)
#define DRM_FORMAT_MOD_VIVANTE_SUPER_TILED              fourcc_mod_code(VIVANTE, 2)
#define DRM_FORMAT_MOD_VIVANTE_SPLIT_TILED              fourcc_mod_code(VIVANTE, 3)
#define DRM_FORMAT_MOD_VIVANTE_SPLIT_SUPER_TILED        fourcc_mod_code(VIVANTE, 4)

#define VIVANTE_MOD_TS_64_4                             (1ULL << 48)
#define VIVANTE_MOD_TS_64_2                             (2ULL << 48)
#define VIVANTE_MOD_TS_128_4                            (3ULL << 48)
#define VIVANTE_MOD_TS_256_4                            (4ULL << 48)
#define VIVANTE_MOD_TS_MASK                             (0xfULL << 48)

#define VIVANTE_MOD_COMP_DEC400                         (1ULL << 52)
#define VIVANTE_MOD_COMP_MASK                           (0xfULL << 52)

#define VIVANTE_MOD_EXT_MASK                            (VIVANTE_MOD_TS_MASK | VIVANTE_MOD_COMP_MASK)

#define DRM_FORMAT_MOD_NVIDIA_TEGRA_TILED               fourcc_mod_code(NVIDIA, 1)

#define DRM_FORMAT_MOD_NVIDIA_BLOCK_LINEAR_2D(c, s, g, k, h) \
    fourcc_mod_code(NVIDIA, (0x10 | ((h) & 0xf) | (((k) & 0xff) << 12) | (((g) & 0x3) << 20) | (((s) & 0x1) << 22) | (((c) & 0x7) << 23)))

static __inline__ __u64 drm_fourcc_canonicalize_nvidia_format_mod(__u64 modifier)
{
    if (!(modifier & 0x10) || (modifier & (0xff << 12))) {
        return modifier;
    } else {
        return modifier | (0xfe << 12);
    }
}

#define DRM_FORMAT_MOD_NVIDIA_16BX2_BLOCK(v)            DRM_FORMAT_MOD_NVIDIA_BLOCK_LINEAR_2D(0, 0, 0, 0, (v))

#define DRM_FORMAT_MOD_NVIDIA_16BX2_BLOCK_ONE_GOB       DRM_FORMAT_MOD_NVIDIA_16BX2_BLOCK(0)
#define DRM_FORMAT_MOD_NVIDIA_16BX2_BLOCK_TWO_GOB       DRM_FORMAT_MOD_NVIDIA_16BX2_BLOCK(1)
#define DRM_FORMAT_MOD_NVIDIA_16BX2_BLOCK_FOUR_GOB      DRM_FORMAT_MOD_NVIDIA_16BX2_BLOCK(2)
#define DRM_FORMAT_MOD_NVIDIA_16BX2_BLOCK_EIGHT_GOB     DRM_FORMAT_MOD_NVIDIA_16BX2_BLOCK(3)
#define DRM_FORMAT_MOD_NVIDIA_16BX2_BLOCK_SIXTEEN_GOB   DRM_FORMAT_MOD_NVIDIA_16BX2_BLOCK(4)
#define DRM_FORMAT_MOD_NVIDIA_16BX2_BLOCK_THIRTYTWO_GOB DRM_FORMAT_MOD_NVIDIA_16BX2_BLOCK(5)

#define __fourcc_mod_broadcom_param_shift               8
#define __fourcc_mod_broadcom_param_bits                48
#define fourcc_mod_broadcom_code(val, params)           fourcc_mod_code(BROADCOM, ((((__u64)params) << __fourcc_mod_broadcom_param_shift) | val))
#define fourcc_mod_broadcom_param(m)                    ((int)(((m) >> __fourcc_mod_broadcom_param_shift) & ((1ULL << __fourcc_mod_broadcom_param_bits) - 1)))
#define fourcc_mod_broadcom_mod(m)                      ((m) & ~(((1ULL << __fourcc_mod_broadcom_param_bits) - 1) << __fourcc_mod_broadcom_param_shift))

#define DRM_FORMAT_MOD_BROADCOM_VC4_T_TILED             fourcc_mod_code(BROADCOM, 1)

#define DRM_FORMAT_MOD_BROADCOM_SAND32_COL_HEIGHT(v)    fourcc_mod_broadcom_code(2, v)
#define DRM_FORMAT_MOD_BROADCOM_SAND64_COL_HEIGHT(v)    fourcc_mod_broadcom_code(3, v)
#define DRM_FORMAT_MOD_BROADCOM_SAND128_COL_HEIGHT(v)   fourcc_mod_broadcom_code(4, v)
#define DRM_FORMAT_MOD_BROADCOM_SAND256_COL_HEIGHT(v)   fourcc_mod_broadcom_code(5, v)

#define DRM_FORMAT_MOD_BROADCOM_SAND32                  DRM_FORMAT_MOD_BROADCOM_SAND32_COL_HEIGHT(0)
#define DRM_FORMAT_MOD_BROADCOM_SAND64                  DRM_FORMAT_MOD_BROADCOM_SAND64_COL_HEIGHT(0)
#define DRM_FORMAT_MOD_BROADCOM_SAND128                 DRM_FORMAT_MOD_BROADCOM_SAND128_COL_HEIGHT(0)
#define DRM_FORMAT_MOD_BROADCOM_SAND256                 DRM_FORMAT_MOD_BROADCOM_SAND256_COL_HEIGHT(0)

#define DRM_FORMAT_MOD_BROADCOM_UIF                     fourcc_mod_code(BROADCOM, 6)

#define DRM_FORMAT_MOD_ARM_CODE(__type, __val)          fourcc_mod_code(ARM, ((__u64)(__type) << 52) | ((__val) & 0x000fffffffffffffULL))

#define DRM_FORMAT_MOD_ARM_TYPE_AFBC                    0x00
#define DRM_FORMAT_MOD_ARM_TYPE_MISC                    0x01

#define DRM_FORMAT_MOD_ARM_AFBC(__afbc_mode)            DRM_FORMAT_MOD_ARM_CODE(DRM_FORMAT_MOD_ARM_TYPE_AFBC, __afbc_mode)

#define AFBC_FORMAT_MOD_BLOCK_SIZE_MASK                 0xf
#define AFBC_FORMAT_MOD_BLOCK_SIZE_16x16                (1ULL)
#define AFBC_FORMAT_MOD_BLOCK_SIZE_32x8                 (2ULL)
#define AFBC_FORMAT_MOD_BLOCK_SIZE_64x4                 (3ULL)
#define AFBC_FORMAT_MOD_BLOCK_SIZE_32x8_64x4            (4ULL)

#define AFBC_FORMAT_MOD_YTR                             (1ULL <<  4)
#define AFBC_FORMAT_MOD_SPLIT                           (1ULL <<  5)
#define AFBC_FORMAT_MOD_SPARSE                          (1ULL <<  6)
#define AFBC_FORMAT_MOD_CBR                             (1ULL <<  7)
#define AFBC_FORMAT_MOD_TILED                           (1ULL <<  8)
#define AFBC_FORMAT_MOD_SC                              (1ULL <<  9)
#define AFBC_FORMAT_MOD_DB                              (1ULL << 10)
#define AFBC_FORMAT_MOD_BCH                             (1ULL << 11)
#define AFBC_FORMAT_MOD_USM                             (1ULL << 12)

#define DRM_FORMAT_MOD_ARM_TYPE_AFRC                    0x02

#define DRM_FORMAT_MOD_ARM_AFRC(__afrc_mode)            DRM_FORMAT_MOD_ARM_CODE(DRM_FORMAT_MOD_ARM_TYPE_AFRC, __afrc_mode)

#define AFRC_FORMAT_MOD_CU_SIZE_MASK                    0xf
#define AFRC_FORMAT_MOD_CU_SIZE_16                      (1ULL)
#define AFRC_FORMAT_MOD_CU_SIZE_24                      (2ULL)
#define AFRC_FORMAT_MOD_CU_SIZE_32                      (3ULL)

#define AFRC_FORMAT_MOD_CU_SIZE_P0(__afrc_cu_size)      (__afrc_cu_size)
#define AFRC_FORMAT_MOD_CU_SIZE_P12(__afrc_cu_size)     ((__afrc_cu_size) << 4)

#define AFRC_FORMAT_MOD_LAYOUT_SCAN                     (1ULL << 8)

#define DRM_FORMAT_MOD_ARM_16X16_BLOCK_U_INTERLEAVED    DRM_FORMAT_MOD_ARM_CODE(DRM_FORMAT_MOD_ARM_TYPE_MISC, 1ULL)

#define DRM_FORMAT_MOD_ALLWINNER_TILED                  fourcc_mod_code(ALLWINNER, 1)

#define __fourcc_mod_amlogic_layout_mask                0xff
#define __fourcc_mod_amlogic_options_shift              8
#define __fourcc_mod_amlogic_options_mask               0xff

#define DRM_FORMAT_MOD_AMLOGIC_FBC(__layout, __options) \
    fourcc_mod_code(AMLOGIC, ((__layout) & __fourcc_mod_amlogic_layout_mask) | (((__options) & __fourcc_mod_amlogic_options_mask) << __fourcc_mod_amlogic_options_shift))

#define AMLOGIC_FBC_LAYOUT_BASIC                        (1ULL)
#define AMLOGIC_FBC_LAYOUT_SCATTER                      (2ULL)
#define AMLOGIC_FBC_OPTION_MEM_SAVING                   (1ULL << 0)

#define AMD_FMT_MOD                                     fourcc_mod_code(AMD, 0)

#define IS_AMD_FMT_MOD(val)                             (((val) >> 56) == DRM_FORMAT_MOD_VENDOR_AMD)

#define AMD_FMT_MOD_TILE_VER_GFX9                       1
#define AMD_FMT_MOD_TILE_VER_GFX10                      2
#define AMD_FMT_MOD_TILE_VER_GFX10_RBPLUS               3
#define AMD_FMT_MOD_TILE_VER_GFX11                      4

#define AMD_FMT_MOD_TILE_GFX9_64K_S                     9
#define AMD_FMT_MOD_TILE_GFX9_64K_D                     10
#define AMD_FMT_MOD_TILE_GFX9_64K_S_X                   25
#define AMD_FMT_MOD_TILE_GFX9_64K_D_X                   26
#define AMD_FMT_MOD_TILE_GFX9_64K_R_X                   27
#define AMD_FMT_MOD_TILE_GFX11_256K_R_X                 31

#define AMD_FMT_MOD_DCC_BLOCK_64B                       0
#define AMD_FMT_MOD_DCC_BLOCK_128B                      1
#define AMD_FMT_MOD_DCC_BLOCK_256B                      2

#define AMD_FMT_MOD_TILE_VERSION_SHIFT                  0
#define AMD_FMT_MOD_TILE_VERSION_MASK                   0xFF
#define AMD_FMT_MOD_TILE_SHIFT                          8
#define AMD_FMT_MOD_TILE_MASK                           0x1F

#define AMD_FMT_MOD_DCC_SHIFT                           13
#define AMD_FMT_MOD_DCC_MASK                            0x1

#define AMD_FMT_MOD_DCC_RETILE_SHIFT                    14
#define AMD_FMT_MOD_DCC_RETILE_MASK                     0x1

#define AMD_FMT_MOD_DCC_PIPE_ALIGN_SHIFT                15
#define AMD_FMT_MOD_DCC_PIPE_ALIGN_MASK                 0x1

#define AMD_FMT_MOD_DCC_INDEPENDENT_64B_SHIFT           16
#define AMD_FMT_MOD_DCC_INDEPENDENT_64B_MASK            0x1
#define AMD_FMT_MOD_DCC_INDEPENDENT_128B_SHIFT          17
#define AMD_FMT_MOD_DCC_INDEPENDENT_128B_MASK           0x1
#define AMD_FMT_MOD_DCC_MAX_COMPRESSED_BLOCK_SHIFT      18
#define AMD_FMT_MOD_DCC_MAX_COMPRESSED_BLOCK_MASK       0x3

#define AMD_FMT_MOD_DCC_CONSTANT_ENCODE_SHIFT           20
#define AMD_FMT_MOD_DCC_CONSTANT_ENCODE_MASK            0x1

#define AMD_FMT_MOD_PIPE_XOR_BITS_SHIFT                 21
#define AMD_FMT_MOD_PIPE_XOR_BITS_MASK                  0x7
#define AMD_FMT_MOD_BANK_XOR_BITS_SHIFT                 24
#define AMD_FMT_MOD_BANK_XOR_BITS_MASK                  0x7
#define AMD_FMT_MOD_PACKERS_SHIFT                       27
#define AMD_FMT_MOD_PACKERS_MASK                        0x7
#define AMD_FMT_MOD_RB_SHIFT                            30
#define AMD_FMT_MOD_RB_MASK                             0x7
#define AMD_FMT_MOD_PIPE_SHIFT                          33
#define AMD_FMT_MOD_PIPE_MASK                           0x7

#define AMD_FMT_MOD_SET(field, value)                   ((__u64)(value) << AMD_FMT_MOD_##field##_SHIFT)
#define AMD_FMT_MOD_GET(field, value)                   (((value) >> AMD_FMT_MOD_##field##_SHIFT) & AMD_FMT_MOD_##field##_MASK)
#define AMD_FMT_MOD_CLEAR(field)                        (~((__u64)AMD_FMT_MOD_##field##_MASK << AMD_FMT_MOD_##field##_SHIFT))

#if defined(__cplusplus)
}
#endif

#endif
