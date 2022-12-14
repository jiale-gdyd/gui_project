/*
 *
 * Copyright 2010 Rockchip Electronics Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __H263D_PARSER_H__
#define __H263D_PARSER_H__

#include "rockchip/rkmpp/mpp_packet.h"
#include "../../../base/inc/mpp_buf_slot.h"
#include "../../../hal/inc/hal_dec_task.h"

#define H263D_DBG_FUNCTION          (0x00000001)
#define H263D_DBG_STARTCODE         (0x00000002)
#define H263D_DBG_BITS              (0x00000004)
#define H263D_DBG_STATUS            (0x00000008)
#define H263D_DBG_TIME              (0x00000100)

typedef void* H263dParser;

#ifdef __cplusplus
extern "C" {
#endif

MPP_RET mpp_h263_parser_init(H263dParser *ctx, MppBufSlots frame_slots);
MPP_RET mpp_h263_parser_deinit(H263dParser ctx);
MPP_RET mpp_h263_parser_flush(H263dParser ctx);
MPP_RET mpp_h263_parser_reset(H263dParser ctx);

MPP_RET mpp_h263_parser_split(H263dParser ctx, MppPacket dst, MppPacket src);
MPP_RET mpp_h263_parser_decode(H263dParser ctx, MppPacket pkt);
MPP_RET mpp_h263_parser_setup_syntax(H263dParser ctx, MppSyntax *syntax);
MPP_RET mpp_h263_parser_setup_hal_output(H263dParser ctx, RK_S32 *output);
MPP_RET mpp_h263_parser_setup_refer(H263dParser ctx, RK_S32 *refer, RK_S32 max_ref);
MPP_RET mpp_h263_parser_update_dpb(H263dParser ctx);

#ifdef __cplusplus
}
#endif

#endif/* __H263D_PAESER_H__ */
