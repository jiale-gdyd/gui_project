#ifndef __HAL_H264E_VEPU580_PRIVATE_H__
#define __HAL_H264E_VEPU580_PRIVATE_H__

#include <string.h>
#include <stdlib.h>

#include "../../../../osal/inc/mpp_env.h"
#include "../../../../osal/inc/mpp_mem.h"
#include "../../../../osal/inc/mpp_common.h"
#include "../../../base/inc/mpp_frame_impl.h"
#include "../../../base/inc/mpp_packet_impl.h"
#include "../../../codec/inc/mpp_rc.h"

#include "../../../codec/enc/h264/h264e_sps.h"
#include "../../../codec/enc/h264/h264e_pps.h"
#include "../../../codec/enc/h264/h264e_dpb.h"
#include "../../../codec/enc/h264/h264e_slice.h"

#include "../../common/h264/hal_h264e_debug.h"
#include "../../common/hal_bufs.h"
#include "../../inc/mpp_enc_hal.h"
#include "../common/vepu541_common.h"
#include "hal_h264e_vepu580_reg.h"
#include "../../../base/inc/mpp_enc_cb_param.h"
#include "../../common/h264/hal_h264e_stream_amend.h"

#define DUMP_REG 0
#define MAX_TASK_CNT        2

#define CHROMA_KLUT_TAB_SIZE    (24 * sizeof(RK_U32))

typedef struct HalH264eVepu580Ctx_t {
    MppEncCfgSet            *cfg;

    MppDev                  dev;
    RK_S32                  frame_cnt;
    RK_U32                  task_cnt;

    /* buffers management */
    HalBufs                 hw_recn;
    RK_S32                  pixel_buf_fbc_hdr_size;
    RK_S32                  pixel_buf_fbc_bdy_size;
    RK_S32                  pixel_buf_size;
    RK_S32                  thumb_buf_size;
    RK_S32                  max_buf_cnt;
    MppDevRegOffCfgs        *offsets;

    /* external line buffer over 4K */
    MppBufferGroup          ext_line_buf_grp;
    MppBuffer               ext_line_bufs[MAX_TASK_CNT];
    RK_S32                  ext_line_buf_size;

    /* syntax for input from enc_impl */
    RK_U32                  updated;
    H264eSps                *sps;
    H264ePps                *pps;
    H264eDpb                *dpb;
    H264eSlice              *slice;
    H264eFrmInfo            *frms;
    H264eReorderInfo        *reorder;
    H264eMarkingInfo        *marking;
    H264ePrefixNal          *prefix;
    HalH264eVepuStreamAmend  amend;

    /* syntax for output to enc_impl */
    EncRcTaskInfo           hal_rc_cfg;

    /* roi */
    void                    *roi_data;

    /* osd */
    Vepu541OsdCfg           osd_cfg;

    /* finetune */
    void                    *tune;

    /* two-pass deflicker */
    MppBuffer               buf_pass1;

    /* register */
    HalVepu580RegSet        *regs_sets;

    /* frame parallel info */
    RK_S32                  task_idx;
    RK_S32                  curr_idx;
    RK_S32                  prev_idx;
    HalVepu580RegSet        *regs_set;
    MppBuffer               ext_line_buf;

    /* slice low delay output callback */
    MppCbCtx                *output_cb;
    RK_S32                  poll_slice_max;
    RK_S32                  poll_cfg_size;
    MppDevPollCfg           *poll_cfgs;
} HalH264eVepu580Ctx;

extern RK_U32 h264e_klut_weight[30];

#endif
