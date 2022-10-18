#ifndef RKRGA_NORMALRGA_H
#define RKRGA_NORMALRGA_H

#include <vector>

#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <linux/stddef.h>

#include "rga.h"
#include "rgadbg.h"
#include "drmrga.h"
#include "NormalRgaContext.h"

int RgaInit(void **ctx);
int RgaDeInit(void **ctx);

int RgaBlit(rga_info_t *src, rga_info_t *dst, rga_info_t *src1);
int RgaFlush();

int RgaCollorFill(rga_info_t *dst);
int RgaCollorPalette(rga_info *src, rga_info *dst, rga_info *lut);

int NormalRgaInitTables();
int NormalRgaScale();
int NormalRgaRoate();
int NormalRgaRoateScale();

int RkRgaCompatibleFormat(int format);
int RkRgaGetRgaFormat(int format);
int RkRgaGetRgaFormatFromAndroid(int format);

uint32_t bytesPerPixel(int format);

int checkRectForRga(rga_rect_t rect);
int isRectValid(rga_rect_t rect);

int NormalRgaSetRect(rga_rect_t *rect, int x, int y, int w, int h, int s, int f);
void NormalRgaSetLogOnceFlag(int log);
void NormalRgaSetAlwaysLogFlag(bool log);
void NormalRgaLogOutRgaReq(struct rga_req rgaReg);

int NormalRgaSetFdsOffsets(struct rga_req *req, uint16_t src_fd, uint16_t dst_fd, uint32_t src_offset, uint32_t dst_offset);
int NormalRgaSetSrcActiveInfo(struct rga_req *req, unsigned int width, unsigned int height, unsigned int x_off, unsigned int y_off);

#if defined(__arm64__) || defined(__aarch64__)
int NormalRgaSetSrcVirtualInfo(struct rga_req *req, unsigned long yrgb_addr,unsigned long uv_addr,unsigned long v_addr, unsigned int vir_w,unsigned int vir_h, unsigned int format, unsigned char a_swap_en);
#else
int NormalRgaSetSrcVirtualInfo(struct rga_req *req, unsigned int yrgb_addr, unsigned int uv_addr,unsigned int v_addr, unsigned int vir_w, unsigned int vir_h, unsigned int format, unsigned char a_swap_en);
#endif

int NormalRgaSetDstActiveInfo(struct rga_req *req, unsigned int width, unsigned int height, unsigned int x_off, unsigned int y_off);

#if defined(__arm64__) || defined(__aarch64__)
int NormalRgaSetDstVirtualInfo(struct rga_req *msg, unsigned long yrgb_addr,unsigned long uv_addr,unsigned long v_addr, unsigned int vir_w, unsigned int vir_h, RECT *clip, unsigned int format, unsigned char a_swap_en);
#else
int NormalRgaSetDstVirtualInfo(struct rga_req *msg, unsigned int yrgb_addr,unsigned int uv_addr,  unsigned int v_addr, unsigned int vir_w, unsigned int vir_h, RECT *clip, unsigned int format, unsigned char a_swap_en);
#endif

int NormalRgaSetPatInfo(struct rga_req *msg, unsigned int width,unsigned int height,unsigned int x_off, unsigned int y_off, unsigned int pat_format);
int NormalRgaSetPatActiveInfo(struct rga_req *req, unsigned int width, unsigned int height, unsigned int x_off, unsigned int y_off);

#if defined(__arm64__) || defined(__aarch64__)
int NormalRgaSetPatVirtualInfo(struct rga_req *msg, unsigned long yrgb_addr,unsigned long uv_addr,unsigned long v_addr, unsigned int  vir_w, unsigned int vir_h, RECT *clip, unsigned int format, unsigned char a_swap_en);
#else
int NormalRgaSetPatVirtualInfo(struct rga_req *msg, unsigned int yrgb_addr,unsigned int uv_addr,  unsigned int v_addr, unsigned int vir_w, unsigned int vir_h, RECT *clip, unsigned int format, unsigned char a_swap_en);
#endif

#if defined(__arm64__) || defined(__aarch64__)
int NormalRgaSetRopMaskInfo(struct rga_req *msg, unsigned long rop_mask_addr,unsigned int rop_mask_endian_mode);
#else
int NormalRgaSetRopMaskInfo(struct rga_req *msg, unsigned int rop_mask_addr,unsigned int rop_mask_endian_mode);
#endif

int NormalRgaSetAlphaEnInfo(struct rga_req *msg, unsigned int alpha_cal_mode, unsigned int alpha_mode, unsigned int global_a_value, unsigned int PD_en, unsigned int PD_mode, unsigned int dst_alpha_en);

int NormalRgaSetRopEnInfo(struct rga_req *msg, unsigned int ROP_mode, unsigned int ROP_code, unsigned int color_mode,unsigned int solid_color);

int NormalRgaSetFadingEnInfo(struct rga_req *msg, unsigned char r,unsigned char g,unsigned char b);
int NormalRgaSetSrcTransModeInfo(struct rga_req *msg, unsigned char trans_mode,unsigned char a_en,unsigned char b_en, unsigned char g_en,unsigned char r_en,unsigned int color_key_min, unsigned int color_key_max,unsigned char zero_mode_en);

bool NormalRgaIsBppFormat(int format);
bool NormalRgaIsYuvFormat(int format);
bool NormalRgaIsRgbFormat(int format);
bool NormalRgaFormatHasAlpha(int format);

int NormalRgaSetBitbltMode(struct rga_req *msg, unsigned char scale_mode, unsigned char rotate_mode, unsigned int angle, unsigned int dither_en, unsigned int AA_en, unsigned int yuv2rgb_mode);
int NormalRgaSetColorPaletteMode(struct rga_req *msg, unsigned char palette_mode,unsigned char endian_mode, unsigned int bpp1_0_color, unsigned int bpp1_1_color);

int NormalRgaSetColorFillMode(struct rga_req *msg, COLOR_FILL  *gr_color, unsigned char gr_satur_mode, unsigned char  cf_mode, unsigned int color, unsigned short pat_width, unsigned short pat_height, unsigned char pat_x_off, unsigned char pat_y_off, unsigned char aa_en);
int NormalRgaSetLineDrawingMode(struct rga_req *msg, POINT sp, POINT ep, unsigned int color, unsigned int line_width, unsigned char AA_en, unsigned char last_point_en);

int NormalRgaSetBlurSharpFilterMode(struct rga_req *msg, unsigned char filter_mode, unsigned char filter_type, unsigned char dither_en);

int NormalRgaSetPreScalingMode(struct rga_req *msg, unsigned char dither_en);

#if defined(__arm64__) || defined(__aarch64__)
int NormalRgaUpdatePaletteTableMode(struct rga_req *msg,unsigned long LUT_addr,unsigned int palette_mode);
#else
int NormalRgaUpdatePaletteTableMode(struct rga_req *msg,unsigned int LUT_addr, unsigned int palette_mode);
#endif

int NormalRgaUpdatePattenBuffMode(struct rga_req *msg, unsigned int pat_addr, unsigned int w, unsigned int h, unsigned int format);

int NormalRgaNNQuantizeMode(struct rga_req *msg, rga_info *dst);

int NormalRgaFullColorSpaceConvert(struct rga_req *msg, int color_space_mode);

int NormalRgaDitherMode(struct rga_req *msg, rga_info *dst, int format);

#if defined(__arm64__) || defined(__aarch64__)
int NormalRgaMmuInfo(struct rga_req *msg, unsigned char mmu_en, unsigned char src_flush, unsigned char dst_flush,unsigned char cmd_flush, unsigned long base_addr, unsigned char page_size);
#else
int NormalRgaMmuInfo(struct rga_req *msg, unsigned char mmu_en, unsigned char src_flush, unsigned char dst_flush,unsigned char cmd_flush, unsigned int base_addr, unsigned char page_size);
#endif

int NormalRgaMmuFlag(struct rga_req *msg, int src_mmu_en, int dst_mmu_en);

#endif
