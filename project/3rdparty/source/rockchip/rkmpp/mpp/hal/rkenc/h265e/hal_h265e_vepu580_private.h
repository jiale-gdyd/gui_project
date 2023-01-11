#ifndef __HAL_H265E_VEPU580_PRIVATE_H__
#define __HAL_H265E_VEPU580_PRIVATE_H__

#include <string.h>
#include <stdlib.h>

#include "../../../../osal/inc/mpp_env.h"
#include "../../../../osal/inc/mpp_mem.h"
#include "../../../../osal/inc/mpp_soc.h"
#include "../../../../osal/inc/mpp_common.h"
#include "../../../base/inc/mpp_frame_impl.h"
#include "../../../base/inc/mpp_packet_impl.h"

#include "../../common/h265/hal_h265e_debug.h"
#include "../../../common/h265e_syntax_new.h"
#include "../../common/hal_bufs.h"
#include "../common/rkv_enc_def.h"
#include "../common/vepu541_common.h"
#include "hal_h265e_vepu580.h"
#include "hal_h265e_vepu580_reg.h"
#include "../../../base/inc/mpp_enc_cb_param.h"

#include "../../../../osal/inc/mpp_service.h"

#define MAX_TILE_NUM 4

typedef struct vepu580_h265_fbk_t {
    RK_U32 hw_status; /* 0:corret, 1:error */
    RK_U32 qp_sum;
    RK_U32 out_strm_size;
    RK_U32 out_hw_strm_size;
    RK_S64 sse_sum;
    RK_U32 st_lvl64_inter_num;
    RK_U32 st_lvl32_inter_num;
    RK_U32 st_lvl16_inter_num;
    RK_U32 st_lvl8_inter_num;
    RK_U32 st_lvl32_intra_num;
    RK_U32 st_lvl16_intra_num;
    RK_U32 st_lvl8_intra_num;
    RK_U32 st_lvl4_intra_num;
    RK_U32 st_cu_num_qp[52];
    RK_U32 st_madp;
    RK_U32 st_madi;
    RK_U32 st_md_sad_b16num0;
    RK_U32 st_md_sad_b16num1;
    RK_U32 st_md_sad_b16num2;
    RK_U32 st_md_sad_b16num3;
    RK_U32 st_madi_b16num0;
    RK_U32 st_madi_b16num1;
    RK_U32 st_madi_b16num2;
    RK_U32 st_madi_b16num3;
    RK_U32 st_mb_num;
    RK_U32 st_ctu_num;
} vepu580_h265_fbk;

typedef struct Vepu580RoiH265BsCfg_t {
    RK_U8 amv_en        : 1;
    RK_U8 qp_adj        : 1;
    RK_U8 force_split   : 1;
    RK_U8 force_intra   : 2;
    RK_U8 force_inter   : 2;
} Vepu580RoiH265BsCfg;

typedef struct H265eV580HalContext_t {
    MppEncHalApi        api;
    MppDev              dev;
    void                *regs[MAX_TILE_NUM];
    void                *reg_out[MAX_TILE_NUM];

    vepu580_h265_fbk    feedback;
    void                *dump_files;
    RK_U32              frame_cnt_gen_ready;

    RK_S32              frame_type;
    RK_S32              last_frame_type;

    /* @frame_cnt starts from ZERO */
    RK_U32              frame_cnt;
    Vepu541OsdCfg       osd_cfg;
    MppDevRegOffCfgs    *reg_cfg;
    void                *roi_data;
    RkvRoiCfg_v2        *roi_cfg_tmp;
    MppBufferGroup      roi_grp;
    MppBuffer           roi_base_cfg_buf;
    void                *roi_base_cfg_sw_buf;
    RK_S32              roi_base_buf_size;
    MppEncCfgSet        *cfg;

    MppBufferGroup      tile_grp;
    MppBuffer           hw_tile_buf[MAX_TILE_NUM];
    MppBuffer           hw_tile_stream[MAX_TILE_NUM - 1];
    MppBuffer           buf_pass1;

    RK_U32              enc_mode;
    RK_U32              frame_size;
    RK_S32              max_buf_cnt;
    RK_S32              hdr_status;
    void                *input_fmt;
    RK_U8               *src_buf;
    RK_U8               *dst_buf;
    RK_S32              buf_size;
    RK_U32              frame_num;
    HalBufs             dpb_bufs;
    RK_S32              fbc_header_len;
    RK_U32              tile_num;
    RK_U32              tile_parall_en;
    RK_U32              tile_dump_err;

    RK_S32              poll_slice_max;
    RK_S32              poll_cfg_size;
    MppDevPollCfg       *poll_cfgs;
    MppCbCtx            *output_cb;

    /* finetune */
    void                *tune;
} H265eV580HalContext;

#define TILE_BUF_SIZE  MPP_ALIGN(128 * 1024, 256)

#endif
