/*
*
* Copyright 2015 Rockchip Electronics Co. LTD
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

#ifndef _H264D_INIT_H_
#define _H264D_INIT_H_


#include "rockchip/rkmpp/rk_type.h"
#include "rockchip/rkmpp/mpp_err.h"
#include "h264d_global.h"


#ifdef  __cplusplus
extern "C" {
#endif

MPP_RET update_dpb    (H264_DecCtx_t  *p_Dec);
MPP_RET init_picture  (H264_SLICE_t   *currSlice);
MPP_RET reset_dpb_mark(H264_DpbMark_t *p_mark);
void flush_dpb_buf_slot(H264_DecCtx_t *p_Dec);

#ifdef  __cplusplus
}
#endif

//========================================
#endif /* end of _RKV_H264_DECODER_INIT_H_ */

