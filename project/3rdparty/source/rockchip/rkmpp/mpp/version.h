/*
 * Copyright 2010 Rockchip Electronics S.LSI Co. LTD
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

#ifndef __VERSION_H__
#define __VERSION_H__

#define	MPP_VERSION             "d7f0b0e9 author: sayon.chen    2022-09-27 [hal_h265]: Fix memset strmbuf align_offset maybe overflow"
#define	MPP_VER_HIST_CNT        10
#define	MPP_VER_HIST_0          "d7f0b0e9 author: sayon.chen    2022-09-27 [hal_h265]: Fix memset strmbuf align_offset maybe overflow  (HEAD -> develop, origin/develop, origin/HEAD)"
#define	MPP_VER_HIST_1          "35625579 author: sayon.chen    2022-09-27 [vepu580_hevc]: Support enc max with up to 16384"
#define	MPP_VER_HIST_2          "27e4974b author: Herman Chen   2022-09-27 [hal_vepu580]: Adjusting default parameters"
#define	MPP_VER_HIST_3          "45739cd4 author: Herman Chen   2022-09-27 [mpp_enc]: Clear split arg and out when disabled"
#define	MPP_VER_HIST_4          "9b2c8f30 author: Herman Chen   2022-09-26 [mpp_enc]: reduce extra log on IDR request"
#define	MPP_VER_HIST_5          "b0ae2b71 author: sayon.chen    2022-09-24 [h265d_param]: DXVA slice param use dynamic malloc"
#define	MPP_VER_HIST_6          "ba13512f author: toby.zhang    2022-09-24 [hal_h265e_vepu580]: Adjusting tuning parameters"
#define	MPP_VER_HIST_7          "d654d25e author: sayon.chen    2022-09-23 [h265d_ref]: Add miss ref used by current check"
#define	MPP_VER_HIST_8          "69127d46 author: Yandong Lin   2022-09-23 [h264d_parser]: fix last empty eos pkt send to hw issue in avcc case"
#define	MPP_VER_HIST_9          "a2094ca3 author: sayon.chen    2022-09-22 [h265d]: Modify hw resolution capability check method"

#endif /*__VERSION_H__*/
