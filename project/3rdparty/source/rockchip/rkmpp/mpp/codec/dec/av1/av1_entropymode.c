/*
 * Copyright 2021 Rockchip Electronics Co. LTD
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

#include "av1_entropymode.h"
#include "../../../common/av1d_syntax.h"
#include <string.h>
#include "rockchip/rkmpp/rk_type.h"

#define AOM_ICDF ICDF
#define AOM_CDF2(a0) AOM_ICDF(a0)
#define AOM_CDF3(a0, a1) AOM_ICDF(a0), AOM_ICDF(a1)
#define AOM_CDF4(a0, a1, a2) AOM_ICDF(a0), AOM_ICDF(a1), AOM_ICDF(a2)
#define AOM_CDF5(a0, a1, a2, a3) \
    AOM_ICDF(a0), AOM_ICDF(a1), AOM_ICDF(a2), AOM_ICDF(a3)
#define AOM_CDF6(a0, a1, a2, a3, a4) \
    AOM_ICDF(a0), AOM_ICDF(a1), AOM_ICDF(a2), AOM_ICDF(a3), AOM_ICDF(a4)

#define AOM_CDF7(a0, a1, a2, a3, a4, a5) \
    AOM_ICDF(a0), AOM_ICDF(a1), AOM_ICDF(a2), \
    AOM_ICDF(a3), AOM_ICDF(a4), AOM_ICDF(a5)

#define AOM_CDF8(a0, a1, a2, a3, a4, a5, a6) \
    AOM_ICDF(a0), AOM_ICDF(a1), AOM_ICDF(a2), \
    AOM_ICDF(a3), AOM_ICDF(a4), AOM_ICDF(a5), AOM_ICDF(a6)

#define AOM_CDF9(a0, a1, a2, a3, a4, a5, a6, a7) \
    AOM_ICDF(a0), AOM_ICDF(a1), AOM_ICDF(a2), AOM_ICDF(a3), \
    AOM_ICDF(a4), AOM_ICDF(a5), AOM_ICDF(a6), AOM_ICDF(a7)

#define AOM_CDF10(a0, a1, a2, a3, a4, a5, a6, a7, a8)  \
    AOM_ICDF(a0), AOM_ICDF(a1), AOM_ICDF(a2), AOM_ICDF(a3),\
    AOM_ICDF(a4), AOM_ICDF(a5), AOM_ICDF(a6), AOM_ICDF(a7), AOM_ICDF(a8)

#define AOM_CDF11(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9)                 \
    AOM_ICDF(a0), AOM_ICDF(a1), AOM_ICDF(a2), AOM_ICDF(a3), AOM_ICDF(a4), \
    AOM_ICDF(a5), AOM_ICDF(a6), AOM_ICDF(a7), AOM_ICDF(a8), AOM_ICDF(a9)

#define AOM_CDF12(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)            \
    AOM_ICDF(a0), AOM_ICDF(a1), AOM_ICDF(a2), AOM_ICDF(a3), AOM_ICDF(a4), AOM_ICDF(a5), \
    AOM_ICDF(a6), AOM_ICDF(a7), AOM_ICDF(a8), AOM_ICDF(a9), AOM_ICDF(a10)

#define AOM_CDF13(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)          \
    AOM_ICDF(a0), AOM_ICDF(a1), AOM_ICDF(a2), AOM_ICDF(a3), AOM_ICDF(a4), AOM_ICDF(a5), \
    AOM_ICDF(a6), AOM_ICDF(a7), AOM_ICDF(a8), AOM_ICDF(a9), AOM_ICDF(a10), AOM_ICDF(a11)

#define AOM_CDF14(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12)     \
    AOM_ICDF(a0), AOM_ICDF(a1), AOM_ICDF(a2), AOM_ICDF(a3), AOM_ICDF(a4), \
    AOM_ICDF(a5), AOM_ICDF(a6), AOM_ICDF(a7), AOM_ICDF(a8), AOM_ICDF(a9), \
    AOM_ICDF(a10), AOM_ICDF(a11), AOM_ICDF(a12)

#define AOM_CDF15(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) \
    AOM_ICDF(a0), AOM_ICDF(a1), AOM_ICDF(a2), AOM_ICDF(a3), AOM_ICDF(a4), \
    AOM_ICDF(a5),AOM_ICDF(a6), AOM_ICDF(a7), AOM_ICDF(a8), AOM_ICDF(a9), \
    AOM_ICDF(a10), AOM_ICDF(a11), AOM_ICDF(a12), AOM_ICDF(a13)

#define AOM_CDF16(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14)                                                        \
    AOM_ICDF(a0), AOM_ICDF(a1), AOM_ICDF(a2), AOM_ICDF(a3), AOM_ICDF(a4), \
    AOM_ICDF(a5), AOM_ICDF(a6), AOM_ICDF(a7), AOM_ICDF(a8), AOM_ICDF(a9), \
    AOM_ICDF(a10), AOM_ICDF(a11), AOM_ICDF(a12), AOM_ICDF(a13), AOM_ICDF(a14)

typedef av1_prob Av1ProbMatrix1d[AV1_INTRA_MODES - 1];
typedef Av1ProbMatrix1d Av1ProbMatrix2d[AV1_INTRA_MODES];
typedef Av1ProbMatrix2d Av1ProbMatrix3d[AV1_INTRA_MODES];

typedef av1_prob Av1BlockModeProbmatrix2d[BLOCK_SIZE_GROUPS][AV1_INTRA_MODES - 1];

const av1_prob
av1_partition_probs[NUM_FRAME_TYPES][NUM_PARTITION_CONTEXTS][PARTITION_TYPES] = {
    /* 1 byte padding */
    {
        /* frame_type = keyframe */
        /* 8x8 -> 4x4 */
        {158, 97, 94, 0} /* a/l both not split */,
        {93, 24, 99, 0} /* a split, l not split */,
        {85, 119, 44, 0} /* l split, a not split */,
        {62, 59, 67, 0} /* a/l both split */,
        /* 16x16 -> 8x8 */
        {149, 53, 53, 0} /* a/l both not split */,
        {94, 20, 48, 0} /* a split, l not split */,
        {83, 53, 24, 0} /* l split, a not split */,
        {52, 18, 18, 0} /* a/l both split */,
        /* 32x32 -> 16x16 */
        {150, 40, 39, 0} /* a/l both not split */,
        {78, 12, 26, 0} /* a split, l not split */,
        {67, 33, 11, 0} /* l split, a not split */,
        {24, 7, 5, 0} /* a/l both split */,
        /* 64x64 -> 32x32 */
        {174, 35, 49, 0} /* a/l both not split */,
        {68, 11, 27, 0} /* a split, l not split */,
        {57, 15, 9, 0} /* l split, a not split */,
        {12, 3, 3, 0} /* a/l both split */
    },
    {
        /* frame_type = interframe */
        /* 8x8 -> 4x4 */
        {199, 122, 141, 0} /* a/l both not split */,
        {147, 63, 159, 0} /* a split, l not split */,
        {148, 133, 118, 0} /* l split, a not split */,
        {121, 104, 114, 0} /* a/l both split */,
        /* 16x16 -> 8x8 */
        {174, 73, 87, 0} /* a/l both not split */,
        {92, 41, 83, 0} /* a split, l not split */,
        {82, 99, 50, 0} /* l split, a not split */,
        {53, 39, 39, 0} /* a/l both split */,
        /* 32x32 -> 16x16 */
        {177, 58, 59, 0} /* a/l both not split */,
        {68, 26, 63, 0} /* a split, l not split */,
        {52, 79, 25, 0} /* l split, a not split */,
        {17, 14, 12, 0} /* a/l both split */,
        /* 64x64 -> 32x32 */
        {222, 34, 30, 0} /* a/l both not split */,
        {72, 16, 44, 0} /* a split, l not split */,
        {58, 32, 12, 0} /* l split, a not split */,
        {10, 7, 6, 0} /* a/l both split */
    }
};

/* Array indices are identical to previously-existing INTRAMODECONTEXTNODES. */

const RK_S8 av1hwd_intra_mode_tree[] = {
    -DC_PRED,
    2, /* 0 = DC_NODE */
    -TM_PRED_AV1,
    4, /* 1 = TM_NODE */
    -V_PRED,
    6, /* 2 = V_NODE */
    8,
    12, /* 3 = COM_NODE */
    -H_PRED,
    10, /* 4 = H_NODE */
    -D135_PRED,
    -D117_PRED, /* 5 = D135_NODE */
    -D45_PRED,
    14, /* 6 = D45_NODE */
    -D63_PRED,
    16, /* 7 = D63_NODE */
    -D153_PRED,
    -D27_PRED /* 8 = D153_NODE */
};

const RK_S8 av1_sb_mv_ref_tree[6] = {
    -ZEROMV, 2,       -NEARESTMV,
    4,       -NEARMV, -NEWMV
};

const RK_S8 av1hwd_partition_tree[6] = {
    -PARTITION_NONE, 2, -PARTITION_HORZ,
    4, -PARTITION_VERT, -PARTITION_SPLIT
};

const RK_S8 av1_segment_tree[2 * MAX_MB_SEGMENTS - 2] = {
    2, 4, 6, 8, 10, 12, 0, -1, -2, -3, -4, -5, -6, -7
};

// NOT USED IN AV1
#if 0
static const av1_prob default_intra_inter_p[INTRA_INTER_CONTEXTS] = {9,   102,
                                                                     187, 225
                                                                    };

static const av1_prob default_comp_inter_p[COMP_INTER_CONTEXTS] = {
    239, 183, 119, 96, 41
};

static const av1_prob default_comp_ref_p[REF_CONTEXTS] = {50,  126, 123,
                                                          221, 226
                                                         };

static const av1_prob default_single_ref_p[REF_CONTEXTS][2] = {
    {33, 16}, {77, 74}, {142, 142}, {172, 170}, {238, 247}
};
#endif

const av1_prob av1_default_tx_probs_32x32p[TX_SIZE_CONTEXTS]
[TX_SIZE_MAX_SB - 1] = {
    {
        3,
        136,
        37,
    },
    {
        5,
        52,
        13,
    },
};
const av1_prob av1_default_tx_probs_16x16p[TX_SIZE_CONTEXTS]
[TX_SIZE_MAX_SB - 2] = {
    {
        20,
        152,
    },
    {
        15,
        101,
    },
};
const av1_prob av1_default_tx_probs_8x8p[TX_SIZE_CONTEXTS][TX_SIZE_MAX_SB - 3] = {
    {
        100,
    },
    {
        66,
    },
};

const av1_prob av1_default_mbskip_probs[MBSKIP_CONTEXTS] = {192, 128, 64};

const av1_prob
av1_default_intra_ext_tx_prob[EXT_TX_SIZES][TX_TYPES][TX_TYPES - 1] = {
    {{240, 85, 128}, {4, 1, 248}, {4, 1, 8}, {4, 248, 128}},
    {{244, 85, 128}, {8, 2, 248}, {8, 2, 8}, {8, 248, 128}},
    {{248, 85, 128}, {16, 4, 248}, {16, 4, 8}, {16, 248, 128}},
};

const av1_prob av1_default_inter_ext_tx_prob[EXT_TX_SIZES][TX_TYPES - 1] = {
    {160, 85, 128},
    {176, 85, 128},
    {192, 85, 128},
};

const av1_prob av1_default_motion_mode_prob[MOTION_MODE_CONTEXTS][MOTION_MODE_COUNT - 1] = {
    {151}, {153}, {144}, {178},
    {165}, {160}, {207}, {195},
    {168}, {244}
};

static const aom_cdf_prob
default_kf_y_mode_cdf[KF_MODE_CONTEXTS][KF_MODE_CONTEXTS][CDF_SIZE(AV1_INTRA_MODES)] = {
    {
        {
            AOM_CDF13(15588, 17027, 19338, 20218, 20682, 21110, 21825, 23244,
            24189, 28165, 29093, 30466)
        },
        {
            AOM_CDF13(12016, 18066, 19516, 20303, 20719, 21444, 21888, 23032,
            24434, 28658, 30172, 31409)
        },
        {
            AOM_CDF13(10052, 10771, 22296, 22788, 23055, 23239, 24133, 25620,
            26160, 29336, 29929, 31567)
        },
        {
            AOM_CDF13(14091, 15406, 16442, 18808, 19136, 19546, 19998, 22096,
            24746, 29585, 30958, 32462)
        },
        {
            AOM_CDF13(12122, 13265, 15603, 16501, 18609, 20033, 22391, 25583,
            26437, 30261, 31073, 32475)
        }
    },
    {
        {
            AOM_CDF13(10023, 19585, 20848, 21440, 21832, 22760, 23089, 24023,
            25381, 29014, 30482, 31436)
        },
        {
            AOM_CDF13(5983, 24099, 24560, 24886, 25066, 25795, 25913, 26423,
            27610, 29905, 31276, 31794)
        },
        {
            AOM_CDF13(7444, 12781, 20177, 20728, 21077, 21607, 22170, 23405,
            24469, 27915, 29090, 30492)
        },
        {
            AOM_CDF13(8537, 14689, 15432, 17087, 17408, 18172, 18408, 19825,
            24649, 29153, 31096, 32210)
        },
        {
            AOM_CDF13(7543, 14231, 15496, 16195, 17905, 20717, 21984, 24516,
            26001, 29675, 30981, 31994)
        }
    },
    {
        {
            AOM_CDF13(12613, 13591, 21383, 22004, 22312, 22577, 23401, 25055,
            25729, 29538, 30305, 32077)
        },
        {
            AOM_CDF13(9687, 13470, 18506, 19230, 19604, 20147, 20695, 22062,
            23219, 27743, 29211, 30907)
        },
        {
            AOM_CDF13(6183, 6505, 26024, 26252, 26366, 26434, 27082, 28354, 28555,
            30467, 30794, 32086)
        },
        {
            AOM_CDF13(10718, 11734, 14954, 17224, 17565, 17924, 18561, 21523,
            23878, 28975, 30287, 32252)
        },
        {
            AOM_CDF13(9194, 9858, 16501, 17263, 18424, 19171, 21563, 25961, 26561,
            30072, 30737, 32463)
        }
    },
    {
        {
            AOM_CDF13(12602, 14399, 15488, 18381, 18778, 19315, 19724, 21419,
            25060, 29696, 30917, 32409)
        },
        {
            AOM_CDF13(8203, 13821, 14524, 17105, 17439, 18131, 18404, 19468,
            25225, 29485, 31158, 32342)
        },
        {
            AOM_CDF13(8451, 9731, 15004, 17643, 18012, 18425, 19070, 21538, 24605,
            29118, 30078, 32018)
        },
        {
            AOM_CDF13(7714, 9048, 9516, 16667, 16817, 16994, 17153, 18767, 26743,
            30389, 31536, 32528)
        },
        {
            AOM_CDF13(8843, 10280, 11496, 15317, 16652, 17943, 19108, 22718,
            25769, 29953, 30983, 32485)
        }
    },
    {
        {
            AOM_CDF13(12578, 13671, 15979, 16834, 19075, 20913, 22989, 25449,
            26219, 30214, 31150, 32477)
        },
        {
            AOM_CDF13(9563, 13626, 15080, 15892, 17756, 20863, 22207, 24236,
            25380, 29653, 31143, 32277)
        },
        {
            AOM_CDF13(8356, 8901, 17616, 18256, 19350, 20106, 22598, 25947, 26466,
            29900, 30523, 32261)
        },
        {
            AOM_CDF13(10835, 11815, 13124, 16042, 17018, 18039, 18947, 22753,
            24615, 29489, 30883, 32482)
        },
        {
            AOM_CDF13(7618, 8288, 9859, 10509, 15386, 18657, 22903, 28776, 29180,
            31355, 31802, 32593)
        }
    }
};

static const aom_cdf_prob default_angle_delta_cdf[DIRECTIONAL_MODES][CDF_SIZE(2 * MAX_ANGLE_DELTA + 1)] = {
    {AOM_CDF7(2180, 5032, 7567, 22776, 26989, 30217)},
    {AOM_CDF7(2301, 5608, 8801, 23487, 26974, 30330)},
    {AOM_CDF7(3780, 11018, 13699, 19354, 23083, 31286)},
    {AOM_CDF7(4581, 11226, 15147, 17138, 21834, 28397)},
    {AOM_CDF7(1737, 10927, 14509, 19588, 22745, 28823)},
    {AOM_CDF7(2664, 10176, 12485, 17650, 21600, 30495)},
    {AOM_CDF7(2240, 11096, 15453, 20341, 22561, 28917)},
    {AOM_CDF7(3605, 10428, 12459, 17676, 21244, 30655)}
};

static const aom_cdf_prob default_if_y_mode_cdf[BLOCK_SIZE_GROUPS][CDF_SIZE(AV1_INTRA_MODES)] = {
    {
        AOM_CDF13(22801, 23489, 24293, 24756, 25601, 26123,
        26606, 27418, 27945, 29228, 29685, 30349)
    },
    {
        AOM_CDF13(18673, 19845, 22631, 23318, 23950, 24649,
        25527, 27364, 28152, 29701, 29984, 30852)
    },
    {
        AOM_CDF13(19770, 20979, 23396, 23939, 24241, 24654,
        25136, 27073, 27830, 29360, 29730, 30659)
    },
    {
        AOM_CDF13(20155, 21301, 22838, 23178, 23261, 23533,
        23703, 24804, 25352, 26575, 27016, 28049)
    }
};

static const aom_cdf_prob
default_uv_mode_cdf[CFL_ALLOWED_TYPES][AV1_INTRA_MODES][CDF_SIZE(UV_INTRA_MODES)] = {
    {
        {
            AOM_CDF13(22631, 24152, 25378, 25661, 25986, 26520, 27055, 27923,
            28244, 30059, 30941, 31961)
        },
        {
            AOM_CDF13(9513, 26881, 26973, 27046, 27118, 27664, 27739, 27824,
            28359, 29505, 29800, 31796)
        },
        {
            AOM_CDF13(9845, 9915, 28663, 28704, 28757, 28780, 29198, 29822, 29854,
            30764, 31777, 32029)
        },
        {
            AOM_CDF13(13639, 13897, 14171, 25331, 25606, 25727, 25953, 27148,
            28577, 30612, 31355, 32493)
        },
        {
            AOM_CDF13(9764, 9835, 9930, 9954, 25386, 27053, 27958, 28148, 28243,
            31101, 31744, 32363)
        },
        {
            AOM_CDF13(11825, 13589, 13677, 13720, 15048, 29213, 29301, 29458,
            29711, 31161, 31441, 32550)
        },
        {
            AOM_CDF13(14175, 14399, 16608, 16821, 17718, 17775, 28551, 30200,
            30245, 31837, 32342, 32667)
        },
        {
            AOM_CDF13(12885, 13038, 14978, 15590, 15673, 15748, 16176, 29128,
            29267, 30643, 31961, 32461)
        },
        {
            AOM_CDF13(12026, 13661, 13874, 15305, 15490, 15726, 15995, 16273,
            28443, 30388, 30767, 32416)
        },
        {
            AOM_CDF13(19052, 19840, 20579, 20916, 21150, 21467, 21885, 22719,
            23174, 28861, 30379, 32175)
        },
        {
            AOM_CDF13(18627, 19649, 20974, 21219, 21492, 21816, 22199, 23119,
            23527, 27053, 31397, 32148)
        },
        {
            AOM_CDF13(17026, 19004, 19997, 20339, 20586, 21103, 21349, 21907,
            22482, 25896, 26541, 31819)
        },
        {
            AOM_CDF13(12124, 13759, 14959, 14992, 15007, 15051, 15078, 15166,
            15255, 15753, 16039, 16606)
        }
    },
    {
        {
            AOM_CDF14(10407, 11208, 12900, 13181, 13823, 14175, 14899, 15656,
            15986, 20086, 20995, 22455, 24212)
        },
        {
            AOM_CDF14(4532, 19780, 20057, 20215, 20428, 21071, 21199, 21451,
            22099, 24228, 24693, 27032, 29472)
        },
        {
            AOM_CDF14(5273, 5379, 20177, 20270, 20385, 20439, 20949, 21695, 21774,
            23138, 24256, 24703, 26679)
        },
        {
            AOM_CDF14(6740, 7167, 7662, 14152, 14536, 14785, 15034, 16741, 18371,
            21520, 22206, 23389, 24182)
        },
        {
            AOM_CDF14(4987, 5368, 5928, 6068, 19114, 20315, 21857, 22253, 22411,
            24911, 25380, 26027, 26376)
        },
        {
            AOM_CDF14(5370, 6889, 7247, 7393, 9498, 21114, 21402, 21753, 21981,
            24780, 25386, 26517, 27176)
        },
        {
            AOM_CDF14(4816, 4961, 7204, 7326, 8765, 8930, 20169, 20682, 20803,
            23188, 23763, 24455, 24940)
        },
        {
            AOM_CDF14(6608, 6740, 8529, 9049, 9257, 9356, 9735, 18827, 19059,
            22336, 23204, 23964, 24793)
        },
        {
            AOM_CDF14(5998, 7419, 7781, 8933, 9255, 9549, 9753, 10417, 18898,
            22494, 23139, 24764, 25989)
        },
        {
            AOM_CDF14(10660, 11298, 12550, 12957, 13322, 13624, 14040, 15004,
            15534, 20714, 21789, 23443, 24861)
        },
        {
            AOM_CDF14(10522, 11530, 12552, 12963, 13378, 13779, 14245, 15235,
            15902, 20102, 22696, 23774, 25838)
        },
        {
            AOM_CDF14(10099, 10691, 12639, 13049, 13386, 13665, 14125, 15163,
            15636, 19676, 20474, 23519, 25208)
        },
        {
            AOM_CDF14(3144, 5087, 7382, 7504, 7593, 7690, 7801, 8064, 8232, 9248,
            9875, 10521, 29048)
        }
    }
};

static const aom_cdf_prob default_partition_cdf[13][16] = {
    {
        AOM_CDF4(19132, 25510, 30392), AOM_CDF4(13928, 19855, 28540),
        AOM_CDF4(12522, 23679, 28629), AOM_CDF4(9896, 18783, 25853),
        AOM_CDF2(11570), AOM_CDF2(16855), AOM_CDF3(9413, 22581)
    },
    {AOM_CDF10(15597, 20929, 24571, 26706, 27664, 28821, 29601, 30571, 31902)},
    {AOM_CDF10(7925, 11043, 16785, 22470, 23971, 25043, 26651, 28701, 29834)},
    {AOM_CDF10(5414, 13269, 15111, 20488, 22360, 24500, 25537, 26336, 32117)},
    {AOM_CDF10(2662, 6362, 8614, 20860, 23053, 24778, 26436, 27829, 31171)},
    {AOM_CDF10(18462, 20920, 23124, 27647, 28227, 29049, 29519, 30178, 31544)},
    {AOM_CDF10(7689, 9060, 12056, 24992, 25660, 26182, 26951, 28041, 29052)},
    {AOM_CDF10(6015, 9009, 10062, 24544, 25409, 26545, 27071, 27526, 32047)},
    {AOM_CDF10(1394, 2208, 2796, 28614, 29061, 29466, 29840, 30185, 31899)},
    {
        AOM_CDF10(20137, 21547, 23078, 29566, 29837, 30261, 30524, 30892, 31724),
        AOM_CDF8(27899, 28219, 28529, 32484, 32539, 32619, 32639)
    },
    {
        AOM_CDF10(6732, 7490, 9497, 27944, 28250, 28515, 28969, 29630, 30104),
        AOM_CDF8(6607, 6990, 8268, 32060, 32219, 32338, 32371)
    },
    {
        AOM_CDF10(5945, 7663, 8348, 28683, 29117, 29749, 30064, 30298, 32238),
        AOM_CDF8(5429, 6676, 7122, 32027, 32227, 32531, 32582)
    },
    {
        AOM_CDF10(870, 1212, 1487, 31198, 31394, 31574, 31743, 31881, 32332),
        AOM_CDF8(711, 966, 1172, 32448, 32538, 32617, 32664)
    },
};

static const aom_cdf_prob
default_intra_ext_tx0_cdf[EXTTX_SIZES][AV1_INTRA_MODES][8] = {
    {
        {AOM_CDF7(1535, 8035, 9461, 12751, 23467, 27825)},
        {AOM_CDF7(564, 3335, 9709, 10870, 18143, 28094)},
        {AOM_CDF7(672, 3247, 3676, 11982, 19415, 23127)},
        {AOM_CDF7(5279, 13885, 15487, 18044, 23527, 30252)},
        {AOM_CDF7(4423, 6074, 7985, 10416, 25693, 29298)},
        {AOM_CDF7(1486, 4241, 9460, 10662, 16456, 27694)},
        {AOM_CDF7(439, 2838, 3522, 6737, 18058, 23754)},
        {AOM_CDF7(1190, 4233, 4855, 11670, 20281, 24377)},
        {AOM_CDF7(1045, 4312, 8647, 10159, 18644, 29335)},
        {AOM_CDF7(202, 3734, 4747, 7298, 17127, 24016)},
        {AOM_CDF7(447, 4312, 6819, 8884, 16010, 23858)},
        {AOM_CDF7(277, 4369, 5255, 8905, 16465, 22271)},
        {AOM_CDF7(3409, 5436, 10599, 15599, 19687, 24040)},
    },
    {
        {AOM_CDF7(1870, 13742, 14530, 16498, 23770, 27698)},
        {AOM_CDF7(326, 8796, 14632, 15079, 19272, 27486)},
        {AOM_CDF7(484, 7576, 7712, 14443, 19159, 22591)},
        {AOM_CDF7(1126, 15340, 15895, 17023, 20896, 30279)},
        {AOM_CDF7(655, 4854, 5249, 5913, 22099, 27138)},
        {AOM_CDF7(1299, 6458, 8885, 9290, 14851, 25497)},
        {AOM_CDF7(311, 5295, 5552, 6885, 16107, 22672)},
        {AOM_CDF7(883, 8059, 8270, 11258, 17289, 21549)},
        {AOM_CDF7(741, 7580, 9318, 10345, 16688, 29046)},
        {AOM_CDF7(110, 7406, 7915, 9195, 16041, 23329)},
        {AOM_CDF7(363, 7974, 9357, 10673, 15629, 24474)},
        {AOM_CDF7(153, 7647, 8112, 9936, 15307, 19996)},
        {AOM_CDF7(3511, 6332, 11165, 15335, 19323, 23594)},
    },
    {
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
    },
    {
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
        {AOM_CDF7(4681, 9362, 14043, 18725, 23406, 28087)},
    },
};

static const aom_cdf_prob default_intra_ext_tx1_cdf[EXTTX_SIZES][AV1_INTRA_MODES][4] = {
    {
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
    },
    {
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
    },
    {
        {AOM_CDF5(1127, 12814, 22772, 27483)},
        {AOM_CDF5(145, 6761, 11980, 26667)},
        {AOM_CDF5(362, 5887, 11678, 16725)},
        {AOM_CDF5(385, 15213, 18587, 30693)},
        {AOM_CDF5(25, 2914, 23134, 27903)},
        {AOM_CDF5(60, 4470, 11749, 23991)},
        {AOM_CDF5(37, 3332, 14511, 21448)},
        {AOM_CDF5(157, 6320, 13036, 17439)},
        {AOM_CDF5(119, 6719, 12906, 29396)},
        {AOM_CDF5(47, 5537, 12576, 21499)},
        {AOM_CDF5(269, 6076, 11258, 23115)},
        {AOM_CDF5(83, 5615, 12001, 17228)},
        {AOM_CDF5(1968, 5556, 12023, 18547)},
    },
    {
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
        {AOM_CDF5(6554, 13107, 19661, 26214)},
    },
};

static const aom_cdf_prob default_inter_ext_tx_cdf[2][EXTTX_SIZES][EXT_TX_TYPES] = {
    {
        {
            AOM_CDF16(4458, 5560, 7695, 9709, 13330, 14789, 17537, 20266,
            21504, 22848, 23934, 25474, 27727, 28915, 30631)
        },
        {
            AOM_CDF16(1645, 2573, 4778, 5711, 7807, 8622, 10522, 15357, 17674,
            20408, 22517, 25010, 27116, 28856, 30749)
        },
        {
            AOM_CDF16(2048, 4096, 6144, 8192, 10240, 12288, 14336, 16384,
            18432, 20480, 22528, 24576, 26624, 28672, 30720)
        },
        {
            AOM_CDF16(2048, 4096, 6144, 8192, 10240, 12288, 14336, 16384,
            18432, 20480, 22528, 24576, 26624, 28672, 30720)
        },
    },
    {
        {
            AOM_CDF12(2731, 5461, 8192, 10923, 13653, 16384, 19115, 21845,
            24576, 27307, 30037),
            AOM_CDF2(16384)
        },
        {
            AOM_CDF12(2731, 5461, 8192, 10923, 13653, 16384, 19115, 21845,
            24576, 27307, 30037),
            AOM_CDF2(4167)
        },
        {
            AOM_CDF12(770, 2421, 5225, 12907, 15819, 18927, 21561, 24089,
            26595, 28526, 30529),
            AOM_CDF2(1998)
        },
        {
            AOM_CDF12(2731, 5461, 8192, 10923, 13653, 16384, 19115, 21845,
            24576, 27307, 30037),
            AOM_CDF2(748)
        },
    }
};

static const aom_cdf_prob default_cfl_sign_cdf[CDF_SIZE(CFL_JOINT_SIGNS)] = {
    AOM_CDF8(1418, 2123, 13340, 18405, 26972, 28343, 32294)
};

static const aom_cdf_prob
default_cfl_alpha_cdf[CFL_ALPHA_CONTEXTS][CDF_SIZE(CFL_ALPHABET_SIZE)] = {
    {
        AOM_CDF16(7637, 20719, 31401, 32481, 32657, 32688, 32692, 32696, 32700,
        32704, 32708, 32712, 32716, 32720, 32724)
    },
    {
        AOM_CDF16(14365, 23603, 28135, 31168, 32167, 32395, 32487, 32573,
        32620, 32647, 32668, 32672, 32676, 32680, 32684)
    },
    {
        AOM_CDF16(11532, 22380, 28445, 31360, 32349, 32523, 32584, 32649,
        32673, 32677, 32681, 32685, 32689, 32693, 32697)
    },
    {
        AOM_CDF16(26990, 31402, 32282, 32571, 32692, 32696, 32700, 32704,
        32708, 32712, 32716, 32720, 32724, 32728, 32732)
    },
    {
        AOM_CDF16(17248, 26058, 28904, 30608, 31305, 31877, 32126, 32321,
        32394, 32464, 32516, 32560, 32576, 32593, 32622)
    },
    {
        AOM_CDF16(14738, 21678, 25779, 27901, 29024, 30302, 30980, 31843,
        32144, 32413, 32520, 32594, 32622, 32656, 32660)
    }
};

static const aom_cdf_prob
default_switchable_interp_cdf[SWITCHABLE_FILTER_CONTEXTS][CDF_SIZE(AV1_SWITCHABLE_FILTERS)] = {
    {AOM_CDF3(31935, 32720)}, {AOM_CDF3(5568, 32719)},
    {AOM_CDF3(422, 2938)},    {AOM_CDF3(28244, 32608)},
    {AOM_CDF3(31206, 31953)}, {AOM_CDF3(4862, 32121)},
    {AOM_CDF3(770, 1152)},    {AOM_CDF3(20889, 25637)},
    {AOM_CDF3(31910, 32724)}, {AOM_CDF3(4120, 32712)},
    {AOM_CDF3(305, 2247)},    {AOM_CDF3(27403, 32636)},
    {AOM_CDF3(31022, 32009)}, {AOM_CDF3(2963, 32093)},
    {AOM_CDF3(601, 943)},     {AOM_CDF3(14969, 21398)}
};

static const aom_cdf_prob default_newmv_cdf[NEWMV_MODE_CONTEXTS][CDF_SIZE(2)] = {
    {AOM_CDF2(24035)}, {AOM_CDF2(16630)}, {AOM_CDF2(15339)},
    {AOM_CDF2(8386)},  {AOM_CDF2(12222)}, {AOM_CDF2(4676)}
};

static const aom_cdf_prob default_zeromv_cdf[GLOBALMV_MODE_CONTEXTS][CDF_SIZE(2)] = {
    {AOM_CDF2(2175)}, {AOM_CDF2(1054)}
};

static const aom_cdf_prob default_refmv_cdf[REFMV_MODE_CONTEXTS][CDF_SIZE(2)] = {
    {AOM_CDF2(23974)}, {AOM_CDF2(24188)}, {AOM_CDF2(17848)},
    {AOM_CDF2(28622)}, {AOM_CDF2(24312)}, {AOM_CDF2(19923)}
};

static const aom_cdf_prob default_drl_cdf[DRL_MODE_CONTEXTS][CDF_SIZE(2)] = {
    {AOM_CDF2(13104)}, {AOM_CDF2(24560)}, {AOM_CDF2(18945)}
};

static const aom_cdf_prob
default_inter_compound_mode_cdf[AV1_INTER_MODE_CONTEXTS][CDF_SIZE(INTER_COMPOUND_MODES)] = {
    {AOM_CDF8(7760, 13823, 15808, 17641, 19156, 20666, 26891)},
    {AOM_CDF8(10730, 19452, 21145, 22749, 24039, 25131, 28724)},
    {AOM_CDF8(10664, 20221, 21588, 22906, 24295, 25387, 28436)},
    {AOM_CDF8(13298, 16984, 20471, 24182, 25067, 25736, 26422)},
    {AOM_CDF8(18904, 23325, 25242, 27432, 27898, 28258, 30758)},
    {AOM_CDF8(10725, 17454, 20124, 22820, 24195, 25168, 26046)},
    {AOM_CDF8(17125, 24273, 25814, 27492, 28214, 28704, 30592)},
    {AOM_CDF8(13046, 23214, 24505, 25942, 27435, 28442, 29330)}
};

static const aom_cdf_prob default_interintra_cdf[BLOCK_SIZE_GROUPS][CDF_SIZE(2)] = {
    {AOM_CDF2(16384)}, {AOM_CDF2(26887)}, {AOM_CDF2(27597)}, {AOM_CDF2(30237)}
};

static const aom_cdf_prob default_interintra_mode_cdf[BLOCK_SIZE_GROUPS][CDF_SIZE(INTERINTRA_MODES)] = {
    {AOM_CDF4(8192, 16384, 24576)},
    {AOM_CDF4(1875, 11082, 27332)},
    {AOM_CDF4(2473, 9996, 26388)},
    {AOM_CDF4(4238, 11537, 25926)}
};

static const aom_cdf_prob default_wedge_interintra_cdf[BLOCK_SIZES_ALL][CDF_SIZE(2)] = {
    {AOM_CDF2(16384)}, {AOM_CDF2(16384)}, {AOM_CDF2(16384)},
    {AOM_CDF2(20036)}, {AOM_CDF2(24957)}, {AOM_CDF2(26704)},
    {AOM_CDF2(27530)}, {AOM_CDF2(29564)}, {AOM_CDF2(29444)},
    {AOM_CDF2(26872)}, {AOM_CDF2(16384)}, {AOM_CDF2(16384)},
    {AOM_CDF2(16384)}, {AOM_CDF2(16384)}, {AOM_CDF2(16384)},
    {AOM_CDF2(16384)}, {AOM_CDF2(16384)}, {AOM_CDF2(16384)},
    {AOM_CDF2(16384)}, {AOM_CDF2(16384)}, {AOM_CDF2(16384)},
    {AOM_CDF2(16384)}
};

static const aom_cdf_prob default_compound_type_cdf[BLOCK_SIZES_ALL][CDF_SIZE(COMPOUND_TYPES - 1)] = {
    {AOM_CDF2(16384)}, {AOM_CDF2(16384)}, {AOM_CDF2(16384)}, {AOM_CDF2(23431)},
    {AOM_CDF2(13171)}, {AOM_CDF2(11470)}, {AOM_CDF2(9770)},  {AOM_CDF2(9100)},
    {AOM_CDF2(8233)},  {AOM_CDF2(6172)},  {AOM_CDF2(16384)}, {AOM_CDF2(16384)},
    {AOM_CDF2(16384)}, {AOM_CDF2(16384)}, {AOM_CDF2(16384)}, {AOM_CDF2(16384)},
    {AOM_CDF2(16384)}, {AOM_CDF2(16384)}, {AOM_CDF2(11820)}, {AOM_CDF2(7701)},
    {AOM_CDF2(16384)}, {AOM_CDF2(16384)}
};

static const aom_cdf_prob default_wedge_idx_cdf[BLOCK_SIZES_ALL][CDF_SIZE(16)] = {
    {
        AOM_CDF16(2048, 4096, 6144, 8192, 10240, 12288, 14336, 16384, 18432,
        20480, 22528, 24576, 26624, 28672, 30720)
    },
    {
        AOM_CDF16(2048, 4096, 6144, 8192, 10240, 12288, 14336, 16384, 18432,
        20480, 22528, 24576, 26624, 28672, 30720)
    },
    {
        AOM_CDF16(2048, 4096, 6144, 8192, 10240, 12288, 14336, 16384, 18432,
        20480, 22528, 24576, 26624, 28672, 30720)
    },
    {
        AOM_CDF16(2438, 4440, 6599, 8663, 11005, 12874, 15751, 18094, 20359,
        22362, 24127, 25702, 27752, 29450, 31171)
    },
    {
        AOM_CDF16(806, 3266, 6005, 6738, 7218, 7367, 7771, 14588, 16323, 17367,
        18452, 19422, 22839, 26127, 29629)
    },
    {
        AOM_CDF16(2779, 3738, 4683, 7213, 7775, 8017, 8655, 14357, 17939, 21332,
        24520, 27470, 29456, 30529, 31656)
    },
    {
        AOM_CDF16(1684, 3625, 5675, 7108, 9302, 11274, 14429, 17144, 19163, 20961,
        22884, 24471, 26719, 28714, 30877)
    },
    {
        AOM_CDF16(1142, 3491, 6277, 7314, 8089, 8355, 9023, 13624, 15369, 16730,
        18114, 19313, 22521, 26012, 29550)
    },
    {
        AOM_CDF16(2742, 4195, 5727, 8035, 8980, 9336, 10146, 14124, 17270, 20533,
        23434, 25972, 27944, 29570, 31416)
    },
    {
        AOM_CDF16(1727, 3948, 6101, 7796, 9841, 12344, 15766, 18944, 20638, 22038,
        23963, 25311, 26988, 28766, 31012)
    },
    {
        AOM_CDF16(2048, 4096, 6144, 8192, 10240, 12288, 14336, 16384, 18432,
        20480, 22528, 24576, 26624, 28672, 30720)
    },
    {
        AOM_CDF16(2048, 4096, 6144, 8192, 10240, 12288, 14336, 16384, 18432,
        20480, 22528, 24576, 26624, 28672, 30720)
    },
    {
        AOM_CDF16(2048, 4096, 6144, 8192, 10240, 12288, 14336, 16384, 18432,
        20480, 22528, 24576, 26624, 28672, 30720)
    },
    {
        AOM_CDF16(2048, 4096, 6144, 8192, 10240, 12288, 14336, 16384, 18432,
        20480, 22528, 24576, 26624, 28672, 30720)
    },
    {
        AOM_CDF16(2048, 4096, 6144, 8192, 10240, 12288, 14336, 16384, 18432,
        20480, 22528, 24576, 26624, 28672, 30720)
    },
    {
        AOM_CDF16(2048, 4096, 6144, 8192, 10240, 12288, 14336, 16384, 18432,
        20480, 22528, 24576, 26624, 28672, 30720)
    },
    {
        AOM_CDF16(2048, 4096, 6144, 8192, 10240, 12288, 14336, 16384, 18432,
        20480, 22528, 24576, 26624, 28672, 30720)
    },
    {
        AOM_CDF16(2048, 4096, 6144, 8192, 10240, 12288, 14336, 16384, 18432,
        20480, 22528, 24576, 26624, 28672, 30720)
    },
    {
        AOM_CDF16(154, 987, 1925, 2051, 2088, 2111, 2151, 23033, 23703, 24284,
        24985, 25684, 27259, 28883, 30911)
    },
    {
        AOM_CDF16(1135, 1322, 1493, 2635, 2696, 2737, 2770, 21016, 22935, 25057,
        27251, 29173, 30089, 30960, 31933)
    },
    {
        AOM_CDF16(2048, 4096, 6144, 8192, 10240, 12288, 14336, 16384, 18432,
        20480, 22528, 24576, 26624, 28672, 30720)
    },
    {
        AOM_CDF16(2048, 4096, 6144, 8192, 10240, 12288, 14336, 16384, 18432,
        20480, 22528, 24576, 26624, 28672, 30720)
    }
};

static const aom_cdf_prob default_motion_mode_cdf[BLOCK_SIZES_ALL][CDF_SIZE(
                                                                       MOTION_MODES)] = {
    {AOM_CDF3(10923, 21845)}, {AOM_CDF3(10923, 21845)},
    {AOM_CDF3(10923, 21845)}, {AOM_CDF3(7651, 24760)},
    {AOM_CDF3(4738, 24765)},  {AOM_CDF3(5391, 25528)},
    {AOM_CDF3(19419, 26810)}, {AOM_CDF3(5123, 23606)},
    {AOM_CDF3(11606, 24308)}, {AOM_CDF3(26260, 29116)},
    {AOM_CDF3(20360, 28062)}, {AOM_CDF3(21679, 26830)},
    {AOM_CDF3(29516, 30701)}, {AOM_CDF3(28898, 30397)},
    {AOM_CDF3(30878, 31335)}, {AOM_CDF3(32507, 32558)},
    {AOM_CDF3(10923, 21845)}, {AOM_CDF3(10923, 21845)},
    {AOM_CDF3(28799, 31390)}, {AOM_CDF3(26431, 30774)},
    {AOM_CDF3(28973, 31594)}, {AOM_CDF3(29742, 31203)}
};

static const aom_cdf_prob default_obmc_cdf[BLOCK_SIZES_ALL][CDF_SIZE(2)] = {
    {AOM_CDF2(16384)}, {AOM_CDF2(16384)}, {AOM_CDF2(16384)}, {AOM_CDF2(10437)},
    {AOM_CDF2(9371)},  {AOM_CDF2(9301)},  {AOM_CDF2(17432)}, {AOM_CDF2(14423)},
    {AOM_CDF2(15142)}, {AOM_CDF2(25817)}, {AOM_CDF2(22823)}, {AOM_CDF2(22083)},
    {AOM_CDF2(30128)}, {AOM_CDF2(31014)}, {AOM_CDF2(31560)}, {AOM_CDF2(32638)},
    {AOM_CDF2(16384)}, {AOM_CDF2(16384)}, {AOM_CDF2(23664)}, {AOM_CDF2(20901)},
    {AOM_CDF2(24008)}, {AOM_CDF2(26879)}
};

static const aom_cdf_prob default_intra_inter_cdf[INTRA_INTER_CONTEXTS][CDF_SIZE(2)] = {
    {AOM_CDF2(806)},
    {AOM_CDF2(16662)},
    {AOM_CDF2(20186)},
    {AOM_CDF2(26538)}
};

static const aom_cdf_prob default_comp_inter_cdf[COMP_INTER_CONTEXTS][CDF_SIZE(2)] = {
    {AOM_CDF2(26828)},
    {AOM_CDF2(24035)},
    {AOM_CDF2(12031)},
    {AOM_CDF2(10640)},
    {AOM_CDF2(2901)}
};

static const aom_cdf_prob default_comp_ref_type_cdf[COMP_REF_TYPE_CONTEXTS][CDF_SIZE(2)] = {
    {AOM_CDF2(1198)},
    {AOM_CDF2(2070)},
    {AOM_CDF2(9166)},
    {AOM_CDF2(7499)},
    {AOM_CDF2(22475)}
};

static const aom_cdf_prob default_uni_comp_ref_cdf[UNI_COMP_REF_CONTEXTS][UNIDIR_COMP_REFS - 1][CDF_SIZE(2)] = {
    {{AOM_CDF2(5284)}, {AOM_CDF2(3865)}, {AOM_CDF2(3128)}},
    {{AOM_CDF2(23152)}, {AOM_CDF2(14173)}, {AOM_CDF2(15270)}},
    {{AOM_CDF2(31774)}, {AOM_CDF2(25120)}, {AOM_CDF2(26710)}}
};

static const aom_cdf_prob default_single_ref_cdf[REF_CONTEXTS][SINGLE_REFS - 1][CDF_SIZE(2)] = {
    {
        {AOM_CDF2(4897)},
        {AOM_CDF2(1555)},
        {AOM_CDF2(4236)},
        {AOM_CDF2(8650)},
        {AOM_CDF2(904)},
        {AOM_CDF2(1444)}
    },
    {
        {AOM_CDF2(16973)},
        {AOM_CDF2(16751)},
        {AOM_CDF2(19647)},
        {AOM_CDF2(24773)},
        {AOM_CDF2(11014)},
        {AOM_CDF2(15087)}
    },
    {
        {AOM_CDF2(29744)},
        {AOM_CDF2(30279)},
        {AOM_CDF2(31194)},
        {AOM_CDF2(31895)},
        {AOM_CDF2(26875)},
        {AOM_CDF2(30304)}
    }
};

static const aom_cdf_prob
default_comp_ref_cdf[REF_CONTEXTS][FWD_REFS - 1][CDF_SIZE(2)] = {
    {{AOM_CDF2(4946)}, {AOM_CDF2(9468)}, {AOM_CDF2(1503)}},
    {{AOM_CDF2(19891)}, {AOM_CDF2(22441)}, {AOM_CDF2(15160)}},
    {{AOM_CDF2(30731)}, {AOM_CDF2(31059)}, {AOM_CDF2(27544)}}
};

static const aom_cdf_prob
default_comp_bwdref_cdf[REF_CONTEXTS][BWD_REFS - 1][CDF_SIZE(2)] = {
    {{AOM_CDF2(2235)}, {AOM_CDF2(1423)}},
    {{AOM_CDF2(17182)}, {AOM_CDF2(15175)}},
    {{AOM_CDF2(30606)}, {AOM_CDF2(30489)}}
};

static const aom_cdf_prob
default_palette_y_size_cdf[PALETTE_BLOCK_SIZES][CDF_SIZE(PALETTE_SIZES)] = {
    {AOM_CDF7(7952, 13000, 18149, 21478, 25527, 29241)},
    {AOM_CDF7(7139, 11421, 16195, 19544, 23666, 28073)},
    {AOM_CDF7(7788, 12741, 17325, 20500, 24315, 28530)},
    {AOM_CDF7(8271, 14064, 18246, 21564, 25071, 28533)},
    {AOM_CDF7(12725, 19180, 21863, 24839, 27535, 30120)},
    {AOM_CDF7(9711, 14888, 16923, 21052, 25661, 27875)},
    {AOM_CDF7(14940, 20797, 21678, 24186, 27033, 28999)}
};

static const aom_cdf_prob
default_palette_uv_size_cdf[PALETTE_BLOCK_SIZES][CDF_SIZE(PALETTE_SIZES)] = {
    {AOM_CDF7(8713, 19979, 27128, 29609, 31331, 32272)},
    {AOM_CDF7(5839, 15573, 23581, 26947, 29848, 31700)},
    {AOM_CDF7(4426, 11260, 17999, 21483, 25863, 29430)},
    {AOM_CDF7(3228, 9464, 14993, 18089, 22523, 27420)},
    {AOM_CDF7(3768, 8886, 13091, 17852, 22495, 27207)},
    {AOM_CDF7(2464, 8451, 12861, 21632, 25525, 28555)},
    {AOM_CDF7(1269, 5435, 10433, 18963, 21700, 25865)}
};

static const aom_cdf_prob default_palette_y_mode_cdf
[PALETTE_BLOCK_SIZES][PALETTE_Y_MODE_CONTEXTS][CDF_SIZE(2)] = {
    {{AOM_CDF2(31676)}, {AOM_CDF2(3419)}, {AOM_CDF2(1261)}},
    {{AOM_CDF2(31912)}, {AOM_CDF2(2859)}, {AOM_CDF2(980)}},
    {{AOM_CDF2(31823)}, {AOM_CDF2(3400)}, {AOM_CDF2(781)}},
    {{AOM_CDF2(32030)}, {AOM_CDF2(3561)}, {AOM_CDF2(904)}},
    {{AOM_CDF2(32309)}, {AOM_CDF2(7337)}, {AOM_CDF2(1462)}},
    {{AOM_CDF2(32265)}, {AOM_CDF2(4015)}, {AOM_CDF2(1521)}},
    {{AOM_CDF2(32450)}, {AOM_CDF2(7946)}, {AOM_CDF2(129)}}
};

static const aom_cdf_prob
default_palette_uv_mode_cdf[PALETTE_UV_MODE_CONTEXTS][CDF_SIZE(2)] = {
    {AOM_CDF2(32461)}, {AOM_CDF2(21488)}
};

static const aom_cdf_prob
default_palette_y_color_index_cdf[PALETTE_IDX_CONTEXTS][8] = {
    // Palette sizes 2 & 8
    {
        AOM_CDF2(28710),
        AOM_CDF8(21689, 23883, 25163, 26352, 27506, 28827, 30195)
    },
    {
        AOM_CDF2(16384),
        AOM_CDF8(6892, 15385, 17840, 21606, 24287, 26753, 29204)
    },
    {
        AOM_CDF2(10553),
        AOM_CDF8(5651, 23182, 25042, 26518, 27982, 29392, 30900)
    },
    {
        AOM_CDF2(27036),
        AOM_CDF8(19349, 22578, 24418, 25994, 27524, 29031, 30448)
    },
    {
        AOM_CDF2(31603),
        AOM_CDF8(31028, 31270, 31504, 31705, 31927, 32153, 32392)
    },
    // Palette sizes 3 & 7
    {
        AOM_CDF3(27877, 30490),
        AOM_CDF7(23105, 25199, 26464, 27684, 28931, 30318)
    },
    {
        AOM_CDF3(11532, 25697),
        AOM_CDF7(6950, 15447, 18952, 22681, 25567, 28563)
    },
    {
        AOM_CDF3(6544, 30234),
        AOM_CDF7(7560, 23474, 25490, 27203, 28921, 30708)
    },
    {
        AOM_CDF3(23018, 28072),
        AOM_CDF7(18544, 22373, 24457, 26195, 28119, 30045)
    },
    {
        AOM_CDF3(31915, 32385),
        AOM_CDF7(31198, 31451, 31670, 31882, 32123, 32391)
    },
    // Palette sizes 4 & 6
    {
        AOM_CDF4(25572, 28046, 30045),
        AOM_CDF6(23132, 25407, 26970, 28435, 30073)
    },
    {
        AOM_CDF4(9478, 21590, 27256),
        AOM_CDF6(7443, 17242, 20717, 24762, 27982)
    },
    {
        AOM_CDF4(7248, 26837, 29824),
        AOM_CDF6(6300, 24862, 26944, 28784, 30671)
    },
    {
        AOM_CDF4(19167, 24486, 28349),
        AOM_CDF6(18916, 22895, 25267, 27435, 29652)
    },
    {
        AOM_CDF4(31400, 31825, 32250),
        AOM_CDF6(31270, 31550, 31808, 32059, 32353)
    },
    // Palette size 5
    {
        AOM_CDF5(24779, 26955, 28576, 30282),
        AOM_CDF5(8669, 20364, 24073, 28093)
    },
    {
        AOM_CDF5(4255, 27565, 29377, 31067),
        AOM_CDF5(19864, 23674, 26716, 29530)
    },
    {
        AOM_CDF5(31646, 31893, 32147, 32426), 0, 0, 0, 0
    }
};

static const aom_cdf_prob
default_palette_uv_color_index_cdf[PALETTE_IDX_CONTEXTS][8] = {
    // Palette sizes 2 & 8
    {
        AOM_CDF2(29089),
        AOM_CDF8(21442, 23288, 24758, 26246, 27649, 28980, 30563)
    },
    {
        AOM_CDF2(16384),
        AOM_CDF8(5863, 14933, 17552, 20668, 23683, 26411, 29273)
    },
    {
        AOM_CDF2(8713),
        AOM_CDF8(3415, 25810, 26877, 27990, 29223, 30394, 31618)
    },
    {
        AOM_CDF2(29257),
        AOM_CDF8(17965, 20084, 22232, 23974, 26274, 28402, 30390)
    },
    {
        AOM_CDF2(31610),
        AOM_CDF8(31190, 31329, 31516, 31679, 31825, 32026, 32322)
    },
    // Palette sizes 3 & 7
    {
        AOM_CDF3(25257, 29145),
        AOM_CDF7(21239, 23168, 25044, 26962, 28705, 30506)
    },
    {
        AOM_CDF3(12287, 27293),
        AOM_CDF7(6545, 15012, 18004, 21817, 25503, 28701)
    },
    {
        AOM_CDF3(7033, 27960),
        AOM_CDF7(3448, 26295, 27437, 28704, 30126, 31442)
    },
    {
        AOM_CDF3(20145, 25405),
        AOM_CDF7(15889, 18323, 21704, 24698, 26976, 29690)
    },
    {
        AOM_CDF3(30608, 31639),
        AOM_CDF7(30988, 31204, 31479, 31734, 31983, 32325)
    },
    // Palette sizes 4 & 6
    {
        AOM_CDF4(24210, 27175, 29903),
        AOM_CDF6(22217, 24567, 26637, 28683, 30548)
    },
    {
        AOM_CDF4(9888, 22386, 27214),
        AOM_CDF6(7307, 16406, 19636, 24632, 28424)
    },
    {
        AOM_CDF4(5901, 26053, 29293),
        AOM_CDF6(4441, 25064, 26879, 28942, 30919)
    },
    {
        AOM_CDF4(18318, 22152, 28333),
        AOM_CDF6(17210, 20528, 23319, 26750, 29582)
    },
    {
        AOM_CDF4(30459, 31136, 31926),
        AOM_CDF6(30674, 30953, 31396, 31735, 32207)
    },
    // Palette size 5
    {
        AOM_CDF5(22980, 25479, 27781, 29986),
        AOM_CDF5(8413, 21408, 24859, 28874)
    },
    {
        AOM_CDF5(2257, 29449, 30594, 31598),
        AOM_CDF5(19189, 21202, 25915, 28620)
    },
    {
        AOM_CDF5(31844, 32044, 32281, 32518), 0, 0, 0, 0
    }
};

static const aom_cdf_prob
default_txfm_partition_cdf[TXFM_PARTITION_CONTEXTS][CDF_SIZE(2)] = {
    {AOM_CDF2(28581)}, {AOM_CDF2(23846)}, {AOM_CDF2(20847)},
    {AOM_CDF2(24315)}, {AOM_CDF2(18196)}, {AOM_CDF2(12133)},
    {AOM_CDF2(18791)}, {AOM_CDF2(10887)}, {AOM_CDF2(11005)},
    {AOM_CDF2(27179)}, {AOM_CDF2(20004)}, {AOM_CDF2(11281)},
    {AOM_CDF2(26549)}, {AOM_CDF2(19308)}, {AOM_CDF2(14224)},
    {AOM_CDF2(28015)}, {AOM_CDF2(21546)}, {AOM_CDF2(14400)},
    {AOM_CDF2(28165)}, {AOM_CDF2(22401)}, {AOM_CDF2(16088)}
};

static const aom_cdf_prob default_skip_cdfs[SKIP_CONTEXTS][CDF_SIZE(2)] = {
    {AOM_CDF2(31671)}, {AOM_CDF2(16515)}, {AOM_CDF2(4576)}
};

static const aom_cdf_prob default_skip_mode_cdfs[SKIP_MODE_CONTEXTS][CDF_SIZE(2)] = {
    {AOM_CDF2(32621)}, {AOM_CDF2(20708)}, {AOM_CDF2(8127)}
};

static const aom_cdf_prob
default_compound_idx_cdfs[COMP_INDEX_CONTEXTS][CDF_SIZE(2)] = {
    {AOM_CDF2(18244)}, {AOM_CDF2(12865)}, {AOM_CDF2(7053)},
    {AOM_CDF2(13259)}, {AOM_CDF2(9334)},  {AOM_CDF2(4644)}
};

static const aom_cdf_prob
default_comp_group_idx_cdfs[COMP_GROUP_IDX_CONTEXTS][CDF_SIZE(2)] = {
    {AOM_CDF2(26607)}, {AOM_CDF2(22891)}, {AOM_CDF2(18840)},
    {AOM_CDF2(24594)}, {AOM_CDF2(19934)}, {AOM_CDF2(22674)}
};

static const aom_cdf_prob default_intrabc_cdf[CDF_SIZE(2)] = {AOM_CDF2(30531)};

static const aom_cdf_prob default_filter_intra_mode_cdf[CDF_SIZE(FILTER_INTRA_MODES)] = {
    AOM_CDF5(8949, 12776, 17211, 29558)
};

static const aom_cdf_prob default_filter_intra_cdfs[BLOCK_SIZES_ALL][CDF_SIZE(2)] = {
    {AOM_CDF2(4621)},  {AOM_CDF2(6743)},  {AOM_CDF2(5893)},  {AOM_CDF2(7866)},
    {AOM_CDF2(12551)}, {AOM_CDF2(9394)},  {AOM_CDF2(12408)}, {AOM_CDF2(14301)},
    {AOM_CDF2(12756)}, {AOM_CDF2(22343)}, {AOM_CDF2(16384)}, {AOM_CDF2(16384)},
    {AOM_CDF2(16384)}, {AOM_CDF2(16384)}, {AOM_CDF2(16384)}, {AOM_CDF2(16384)},
    {AOM_CDF2(12770)}, {AOM_CDF2(10368)}, {AOM_CDF2(20229)}, {AOM_CDF2(18101)},
    {AOM_CDF2(16384)}, {AOM_CDF2(16384)}
};

#if 0
static const aom_cdf_prob default_switchable_restore_cdf[CDF_SIZE(RESTORE_SWITCHABLE_TYPES)] = {
    AOM_CDF3(9413, 22581)
};

static const aom_cdf_prob default_wiener_restore_cdf[CDF_SIZE(2)] = {
    AOM_CDF2(11570)
};

static const aom_cdf_prob default_sgrproj_restore_cdf[CDF_SIZE(2)] = {
    AOM_CDF2(16855)
};
#endif

static const aom_cdf_prob default_delta_q_cdf[CDF_SIZE(DELTA_Q_PROBS + 1)] = {
    AOM_CDF4(28160, 32120, 32677)
};

static const aom_cdf_prob default_delta_lf_multi_cdf[FRAME_LF_COUNT][CDF_SIZE(DELTA_LF_PROBS + 1)] = {
    {AOM_CDF4(28160, 32120, 32677)},
    {AOM_CDF4(28160, 32120, 32677)},
    {AOM_CDF4(28160, 32120, 32677)},
    {AOM_CDF4(28160, 32120, 32677)}
};

static const aom_cdf_prob default_delta_lf_cdf[CDF_SIZE(DELTA_LF_PROBS + 1)] = {
    AOM_CDF4(28160, 32120, 32677)
};

static const aom_cdf_prob
default_segment_pred_cdf[SEG_TEMPORAL_PRED_CTXS][CDF_SIZE(2)] = {
    {AOM_CDF2(128 * 128)}, {AOM_CDF2(128 * 128)}, {AOM_CDF2(128 * 128)}
};

static const aom_cdf_prob default_spatial_pred_seg_tree_cdf[SPATIAL_PREDICTION_PROBS][CDF_SIZE(MAX_SEGMENTS)] = {
    {
        AOM_CDF8(5622, 7893, 16093, 18233, 27809, 28373, 32533),
    },
    {
        AOM_CDF8(14274, 18230, 22557, 24935, 29980, 30851, 32344),
    },
    {
        AOM_CDF8(27527, 28487, 28723, 28890, 32397, 32647, 32679),
    },
};

static const aom_cdf_prob default_tx_size_cdf[MAX_TX_CATS][AV1_TX_SIZE_CONTEXTS][CDF_SIZE(MAX_TX_DEPTH + 1)] = {
    {
        {AOM_CDF2(19968)},
        {AOM_CDF2(19968)},
        {AOM_CDF2(24320)}
    },
    {
        {AOM_CDF3(12272, 30172)},
        {AOM_CDF3(12272, 30172)},
        {AOM_CDF3(18677, 30848)}
    },
    {
        {AOM_CDF3(12986, 15180)},
        {AOM_CDF3(12986, 15180)},
        {AOM_CDF3(24302, 25602)}
    },
    {
        {AOM_CDF3(5782, 11475)},
        {AOM_CDF3(5782, 11475)},
        {AOM_CDF3(16803, 22759)}
    },
};

static const aom_cdf_prob
av1_default_dc_sign_cdfs[TOKEN_CDF_Q_CTXS][PLANE_TYPES][DC_SIGN_CONTEXTS][CDF_SIZE(2)] = {
    {
        {
            {AOM_CDF2(128 * 125)},
            {AOM_CDF2(128 * 102)},
            {AOM_CDF2(128 * 147)},
        },
        {
            {AOM_CDF2(128 * 119)},
            {AOM_CDF2(128 * 101)},
            {AOM_CDF2(128 * 135)},
        }
    },
    {
        {
            {AOM_CDF2(128 * 125)},
            {AOM_CDF2(128 * 102)},
            {AOM_CDF2(128 * 147)},
        },
        {
            {AOM_CDF2(128 * 119)},
            {AOM_CDF2(128 * 101)},
            {AOM_CDF2(128 * 135)},
        }
    },
    {
        {
            {AOM_CDF2(128 * 125)},
            {AOM_CDF2(128 * 102)},
            {AOM_CDF2(128 * 147)},
        },
        {
            {AOM_CDF2(128 * 119)},
            {AOM_CDF2(128 * 101)},
            {AOM_CDF2(128 * 135)},
        }
    },
    {
        {
            {AOM_CDF2(128 * 125)},
            {AOM_CDF2(128 * 102)},
            {AOM_CDF2(128 * 147)},
        },
        {
            {AOM_CDF2(128 * 119)},
            {AOM_CDF2(128 * 101)},
            {AOM_CDF2(128 * 135)},
        }
    },
};

static const aom_cdf_prob
av1_default_txb_skip_cdfs[TOKEN_CDF_Q_CTXS][TX_SIZES][TXB_SKIP_CONTEXTS] [CDF_SIZE(2)] = {
    {
        {
            {AOM_CDF2(31849)},
            {AOM_CDF2(5892)},
            {AOM_CDF2(12112)},
            {AOM_CDF2(21935)},
            {AOM_CDF2(20289)},
            {AOM_CDF2(27473)},
            {AOM_CDF2(32487)},
            {AOM_CDF2(7654)},
            {AOM_CDF2(19473)},
            {AOM_CDF2(29984)},
            {AOM_CDF2(9961)},
            {AOM_CDF2(30242)},
            {AOM_CDF2(32117)}
        },
        {
            {AOM_CDF2(31548)},
            {AOM_CDF2(1549)},
            {AOM_CDF2(10130)},
            {AOM_CDF2(16656)},
            {AOM_CDF2(18591)},
            {AOM_CDF2(26308)},
            {AOM_CDF2(32537)},
            {AOM_CDF2(5403)},
            {AOM_CDF2(18096)},
            {AOM_CDF2(30003)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)}
        },
        {
            {AOM_CDF2(29957)},
            {AOM_CDF2(5391)},
            {AOM_CDF2(18039)},
            {AOM_CDF2(23566)},
            {AOM_CDF2(22431)},
            {AOM_CDF2(25822)},
            {AOM_CDF2(32197)},
            {AOM_CDF2(3778)},
            {AOM_CDF2(15336)},
            {AOM_CDF2(28981)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)}
        },
        {
            {AOM_CDF2(17920)},
            {AOM_CDF2(1818)},
            {AOM_CDF2(7282)},
            {AOM_CDF2(25273)},
            {AOM_CDF2(10923)},
            {AOM_CDF2(31554)},
            {AOM_CDF2(32624)},
            {AOM_CDF2(1366)},
            {AOM_CDF2(15628)},
            {AOM_CDF2(30462)},
            {AOM_CDF2(146)},
            {AOM_CDF2(5132)},
            {AOM_CDF2(31657)}
        },
        {
            {AOM_CDF2(6308)},
            {AOM_CDF2(117)},
            {AOM_CDF2(1638)},
            {AOM_CDF2(2161)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(10923)},
            {AOM_CDF2(30247)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)}
        }
    },
    {
        {
            {AOM_CDF2(30371)},
            {AOM_CDF2(7570)},
            {AOM_CDF2(13155)},
            {AOM_CDF2(20751)},
            {AOM_CDF2(20969)},
            {AOM_CDF2(27067)},
            {AOM_CDF2(32013)},
            {AOM_CDF2(5495)},
            {AOM_CDF2(17942)},
            {AOM_CDF2(28280)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)}
        },
        {
            {AOM_CDF2(31782)},
            {AOM_CDF2(1836)},
            {AOM_CDF2(10689)},
            {AOM_CDF2(17604)},
            {AOM_CDF2(21622)},
            {AOM_CDF2(27518)},
            {AOM_CDF2(32399)},
            {AOM_CDF2(4419)},
            {AOM_CDF2(16294)},
            {AOM_CDF2(28345)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)}
        },
        {
            {AOM_CDF2(31901)},
            {AOM_CDF2(10311)},
            {AOM_CDF2(18047)},
            {AOM_CDF2(24806)},
            {AOM_CDF2(23288)},
            {AOM_CDF2(27914)},
            {AOM_CDF2(32296)},
            {AOM_CDF2(4215)},
            {AOM_CDF2(15756)},
            {AOM_CDF2(28341)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)}
        },
        {
            {AOM_CDF2(26726)},
            {AOM_CDF2(1045)},
            {AOM_CDF2(11703)},
            {AOM_CDF2(20590)},
            {AOM_CDF2(18554)},
            {AOM_CDF2(25970)},
            {AOM_CDF2(31938)},
            {AOM_CDF2(5583)},
            {AOM_CDF2(21313)},
            {AOM_CDF2(29390)},
            {AOM_CDF2(641)},
            {AOM_CDF2(22265)},
            {AOM_CDF2(31452)}
        },
        {
            {AOM_CDF2(26584)},
            {AOM_CDF2(188)},
            {AOM_CDF2(8847)},
            {AOM_CDF2(24519)},
            {AOM_CDF2(22938)},
            {AOM_CDF2(30583)},
            {AOM_CDF2(32608)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)}
        }
    },
    {
        {
            {AOM_CDF2(29614)},
            {AOM_CDF2(9068)},
            {AOM_CDF2(12924)},
            {AOM_CDF2(19538)},
            {AOM_CDF2(17737)},
            {AOM_CDF2(24619)},
            {AOM_CDF2(30642)},
            {AOM_CDF2(4119)},
            {AOM_CDF2(16026)},
            {AOM_CDF2(25657)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)}
        },
        {
            {AOM_CDF2(31957)},
            {AOM_CDF2(3230)},
            {AOM_CDF2(11153)},
            {AOM_CDF2(18123)},
            {AOM_CDF2(20143)},
            {AOM_CDF2(26536)},
            {AOM_CDF2(31986)},
            {AOM_CDF2(3050)},
            {AOM_CDF2(14603)},
            {AOM_CDF2(25155)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)}
        },
        {
            {AOM_CDF2(32363)},
            {AOM_CDF2(10692)},
            {AOM_CDF2(19090)},
            {AOM_CDF2(24357)},
            {AOM_CDF2(24442)},
            {AOM_CDF2(28312)},
            {AOM_CDF2(32169)},
            {AOM_CDF2(3648)},
            {AOM_CDF2(15690)},
            {AOM_CDF2(26815)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)}
        },
        {
            {AOM_CDF2(30669)},
            {AOM_CDF2(3832)},
            {AOM_CDF2(11663)},
            {AOM_CDF2(18889)},
            {AOM_CDF2(19782)},
            {AOM_CDF2(23313)},
            {AOM_CDF2(31330)},
            {AOM_CDF2(5124)},
            {AOM_CDF2(18719)},
            {AOM_CDF2(28468)},
            {AOM_CDF2(3082)},
            {AOM_CDF2(20982)},
            {AOM_CDF2(29443)}
        },
        {
            {AOM_CDF2(28573)},
            {AOM_CDF2(3183)},
            {AOM_CDF2(17802)},
            {AOM_CDF2(25977)},
            {AOM_CDF2(26677)},
            {AOM_CDF2(27832)},
            {AOM_CDF2(32387)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)}
        }
    },
    {
        {
            {AOM_CDF2(26887)},
            {AOM_CDF2(6729)},
            {AOM_CDF2(10361)},
            {AOM_CDF2(17442)},
            {AOM_CDF2(15045)},
            {AOM_CDF2(22478)},
            {AOM_CDF2(29072)},
            {AOM_CDF2(2713)},
            {AOM_CDF2(11861)},
            {AOM_CDF2(20773)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)}
        },
        {
            {AOM_CDF2(31903)},
            {AOM_CDF2(2044)},
            {AOM_CDF2(7528)},
            {AOM_CDF2(14618)},
            {AOM_CDF2(16182)},
            {AOM_CDF2(24168)},
            {AOM_CDF2(31037)},
            {AOM_CDF2(2786)},
            {AOM_CDF2(11194)},
            {AOM_CDF2(20155)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)}
        },
        {
            {AOM_CDF2(32510)},
            {AOM_CDF2(8430)},
            {AOM_CDF2(17318)},
            {AOM_CDF2(24154)},
            {AOM_CDF2(23674)},
            {AOM_CDF2(28789)},
            {AOM_CDF2(32139)},
            {AOM_CDF2(3440)},
            {AOM_CDF2(13117)},
            {AOM_CDF2(22702)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)}
        },
        {
            {AOM_CDF2(31671)},
            {AOM_CDF2(2056)},
            {AOM_CDF2(11746)},
            {AOM_CDF2(16852)},
            {AOM_CDF2(18635)},
            {AOM_CDF2(24715)},
            {AOM_CDF2(31484)},
            {AOM_CDF2(4656)},
            {AOM_CDF2(16074)},
            {AOM_CDF2(24704)},
            {AOM_CDF2(1806)},
            {AOM_CDF2(14645)},
            {AOM_CDF2(25336)}
        },
        {
            {AOM_CDF2(31539)},
            {AOM_CDF2(8433)},
            {AOM_CDF2(20576)},
            {AOM_CDF2(27904)},
            {AOM_CDF2(27852)},
            {AOM_CDF2(30026)},
            {AOM_CDF2(32441)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)},
            {AOM_CDF2(16384)}
        }
    }
};

static const aom_cdf_prob
av1_default_eob_extra_cdfs[TOKEN_CDF_Q_CTXS][TX_SIZES][PLANE_TYPES][EOB_COEF_CONTEXTS][CDF_SIZE(2)] = {
    {
        {
            {
                {AOM_CDF2(16961)},
                {AOM_CDF2(17223)},
                {AOM_CDF2(7621)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            },
            {
                {AOM_CDF2(19069)},
                {AOM_CDF2(22525)},
                {AOM_CDF2(13377)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            }
        },
        {
            {
                {AOM_CDF2(20401)},
                {AOM_CDF2(17025)},
                {AOM_CDF2(12845)},
                {AOM_CDF2(12873)},
                {AOM_CDF2(14094)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            },
            {
                {AOM_CDF2(20681)},
                {AOM_CDF2(20701)},
                {AOM_CDF2(15250)},
                {AOM_CDF2(15017)},
                {AOM_CDF2(14928)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            }
        },
        {
            {
                {AOM_CDF2(23905)},
                {AOM_CDF2(17194)},
                {AOM_CDF2(16170)},
                {AOM_CDF2(17695)},
                {AOM_CDF2(13826)},
                {AOM_CDF2(15810)},
                {AOM_CDF2(12036)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            },
            {
                {AOM_CDF2(23959)},
                {AOM_CDF2(20799)},
                {AOM_CDF2(19021)},
                {AOM_CDF2(16203)},
                {AOM_CDF2(17886)},
                {AOM_CDF2(14144)},
                {AOM_CDF2(12010)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            }
        },
        {
            {
                {AOM_CDF2(27399)},
                {AOM_CDF2(16327)},
                {AOM_CDF2(18071)},
                {AOM_CDF2(19584)},
                {AOM_CDF2(20721)},
                {AOM_CDF2(18432)},
                {AOM_CDF2(19560)},
                {AOM_CDF2(10150)},
                {AOM_CDF2(8805)},
            },
            {
                {AOM_CDF2(24932)},
                {AOM_CDF2(20833)},
                {AOM_CDF2(12027)},
                {AOM_CDF2(16670)},
                {AOM_CDF2(19914)},
                {AOM_CDF2(15106)},
                {AOM_CDF2(17662)},
                {AOM_CDF2(13783)},
                {AOM_CDF2(28756)},
            }
        },
        {
            {
                {AOM_CDF2(23406)},
                {AOM_CDF2(21845)},
                {AOM_CDF2(18432)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(17096)},
                {AOM_CDF2(12561)},
                {AOM_CDF2(17320)},
                {AOM_CDF2(22395)},
                {AOM_CDF2(21370)},
            },
            {
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            }
        }
    },
    {
        {
            {
                {AOM_CDF2(17471)},
                {AOM_CDF2(20223)},
                {AOM_CDF2(11357)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            },
            {
                {AOM_CDF2(20335)},
                {AOM_CDF2(21667)},
                {AOM_CDF2(14818)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            }
        },
        {
            {
                {AOM_CDF2(20430)},
                {AOM_CDF2(20662)},
                {AOM_CDF2(15367)},
                {AOM_CDF2(16970)},
                {AOM_CDF2(14657)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            },
            {
                {AOM_CDF2(22117)},
                {AOM_CDF2(22028)},
                {AOM_CDF2(18650)},
                {AOM_CDF2(16042)},
                {AOM_CDF2(15885)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            }
        },
        {
            {
                {AOM_CDF2(22409)},
                {AOM_CDF2(21012)},
                {AOM_CDF2(15650)},
                {AOM_CDF2(17395)},
                {AOM_CDF2(15469)},
                {AOM_CDF2(20205)},
                {AOM_CDF2(19511)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            },
            {
                {AOM_CDF2(24220)},
                {AOM_CDF2(22480)},
                {AOM_CDF2(17737)},
                {AOM_CDF2(18916)},
                {AOM_CDF2(19268)},
                {AOM_CDF2(18412)},
                {AOM_CDF2(18844)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            }
        },
        {
            {
                {AOM_CDF2(25991)},
                {AOM_CDF2(20314)},
                {AOM_CDF2(17731)},
                {AOM_CDF2(19678)},
                {AOM_CDF2(18649)},
                {AOM_CDF2(17307)},
                {AOM_CDF2(21798)},
                {AOM_CDF2(17549)},
                {AOM_CDF2(15630)},
            },
            {
                {AOM_CDF2(26585)},
                {AOM_CDF2(21469)},
                {AOM_CDF2(20432)},
                {AOM_CDF2(17735)},
                {AOM_CDF2(19280)},
                {AOM_CDF2(15235)},
                {AOM_CDF2(20297)},
                {AOM_CDF2(22471)},
                {AOM_CDF2(28997)},
            }
        },
        {
            {
                {AOM_CDF2(26605)},
                {AOM_CDF2(11304)},
                {AOM_CDF2(16726)},
                {AOM_CDF2(16560)},
                {AOM_CDF2(20866)},
                {AOM_CDF2(23524)},
                {AOM_CDF2(19878)},
                {AOM_CDF2(13469)},
                {AOM_CDF2(23084)},
            },
            {
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            }
        }
    },
    {
        {
            {
                {AOM_CDF2(18983)},
                {AOM_CDF2(20512)},
                {AOM_CDF2(14885)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            },
            {
                {AOM_CDF2(20090)},
                {AOM_CDF2(19444)},
                {AOM_CDF2(17286)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            }
        },
        {
            {
                {AOM_CDF2(19139)},
                {AOM_CDF2(21487)},
                {AOM_CDF2(18959)},
                {AOM_CDF2(20910)},
                {AOM_CDF2(19089)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            },
            {
                {AOM_CDF2(20536)},
                {AOM_CDF2(20664)},
                {AOM_CDF2(20625)},
                {AOM_CDF2(19123)},
                {AOM_CDF2(14862)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            }
        },
        {
            {
                {AOM_CDF2(19833)},
                {AOM_CDF2(21502)},
                {AOM_CDF2(17485)},
                {AOM_CDF2(20267)},
                {AOM_CDF2(18353)},
                {AOM_CDF2(23329)},
                {AOM_CDF2(21478)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            },
            {
                {AOM_CDF2(22041)},
                {AOM_CDF2(23434)},
                {AOM_CDF2(20001)},
                {AOM_CDF2(20554)},
                {AOM_CDF2(20951)},
                {AOM_CDF2(20145)},
                {AOM_CDF2(15562)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            }
        },
        {
            {
                {AOM_CDF2(23312)},
                {AOM_CDF2(21607)},
                {AOM_CDF2(16526)},
                {AOM_CDF2(18957)},
                {AOM_CDF2(18034)},
                {AOM_CDF2(18934)},
                {AOM_CDF2(24247)},
                {AOM_CDF2(16921)},
                {AOM_CDF2(17080)},
            },
            {
                {AOM_CDF2(26579)},
                {AOM_CDF2(24910)},
                {AOM_CDF2(18637)},
                {AOM_CDF2(19800)},
                {AOM_CDF2(20388)},
                {AOM_CDF2(9887)},
                {AOM_CDF2(15642)},
                {AOM_CDF2(30198)},
                {AOM_CDF2(24721)},
            }
        },
        {
            {
                {AOM_CDF2(26998)},
                {AOM_CDF2(16737)},
                {AOM_CDF2(17838)},
                {AOM_CDF2(18922)},
                {AOM_CDF2(19515)},
                {AOM_CDF2(18636)},
                {AOM_CDF2(17333)},
                {AOM_CDF2(15776)},
                {AOM_CDF2(22658)},
            },
            {
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            }
        }
    },
    {
        {
            {
                {AOM_CDF2(20177)},
                {AOM_CDF2(20789)},
                {AOM_CDF2(20262)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            },
            {
                {AOM_CDF2(21416)},
                {AOM_CDF2(20855)},
                {AOM_CDF2(23410)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            }
        },
        {
            {
                {AOM_CDF2(20238)},
                {AOM_CDF2(21057)},
                {AOM_CDF2(19159)},
                {AOM_CDF2(22337)},
                {AOM_CDF2(20159)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            },
            {
                {AOM_CDF2(20125)},
                {AOM_CDF2(20559)},
                {AOM_CDF2(21707)},
                {AOM_CDF2(22296)},
                {AOM_CDF2(17333)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            }
        },
        {
            {
                {AOM_CDF2(19941)},
                {AOM_CDF2(20527)},
                {AOM_CDF2(21470)},
                {AOM_CDF2(22487)},
                {AOM_CDF2(19558)},
                {AOM_CDF2(22354)},
                {AOM_CDF2(20331)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            },
            {
                {AOM_CDF2(22752)},
                {AOM_CDF2(25006)},
                {AOM_CDF2(22075)},
                {AOM_CDF2(21576)},
                {AOM_CDF2(17740)},
                {AOM_CDF2(21690)},
                {AOM_CDF2(19211)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            }
        },
        {
            {
                {AOM_CDF2(21442)},
                {AOM_CDF2(22358)},
                {AOM_CDF2(18503)},
                {AOM_CDF2(20291)},
                {AOM_CDF2(19945)},
                {AOM_CDF2(21294)},
                {AOM_CDF2(21178)},
                {AOM_CDF2(19400)},
                {AOM_CDF2(10556)},
            },
            {
                {AOM_CDF2(24648)},
                {AOM_CDF2(24949)},
                {AOM_CDF2(20708)},
                {AOM_CDF2(23905)},
                {AOM_CDF2(20501)},
                {AOM_CDF2(9558)},
                {AOM_CDF2(9423)},
                {AOM_CDF2(30365)},
                {AOM_CDF2(19253)},
            }
        },
        {
            {
                {AOM_CDF2(26064)},
                {AOM_CDF2(22098)},
                {AOM_CDF2(19613)},
                {AOM_CDF2(20525)},
                {AOM_CDF2(17595)},
                {AOM_CDF2(16618)},
                {AOM_CDF2(20497)},
                {AOM_CDF2(18989)},
                {AOM_CDF2(15513)},
            },
            {
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
                {AOM_CDF2(16384)},
            }
        }
    }
};

static const aom_cdf_prob
av1_default_eob_multi16_cdfs[TOKEN_CDF_Q_CTXS][PLANE_TYPES][2][4] = {
    {
        {
            {AOM_CDF5(840, 1039, 1980, 4895)},
            {AOM_CDF5(370, 671, 1883, 4471)}
        },
        {
            {AOM_CDF5(3247, 4950, 9688, 14563)},
            {AOM_CDF5(1904, 3354, 7763, 14647)}
        }
    },
    {
        {
            {AOM_CDF5(2125, 2551, 5165, 8946)},
            {AOM_CDF5(513, 765, 1859, 6339)}
        },
        {
            {AOM_CDF5(7637, 9498, 14259, 19108)},
            {AOM_CDF5(2497, 4096, 8866, 16993)}
        }
    },
    {
        {
            {AOM_CDF5(4016, 4897, 8881, 14968)},
            {AOM_CDF5(716, 1105, 2646, 10056)}
        },
        {
            {AOM_CDF5(11139, 13270, 18241, 23566)},
            {AOM_CDF5(3192, 5032, 10297, 19755)}
        }
    },
    {
        {
            {AOM_CDF5(6708, 8958, 14746, 22133)},
            {AOM_CDF5(1222, 2074, 4783, 15410)}
        },
        {
            {AOM_CDF5(19575, 21766, 26044, 29709)},
            {AOM_CDF5(7297, 10767, 19273, 28194)}
        }
    }
};

static const aom_cdf_prob
av1_default_eob_multi32_cdfs[TOKEN_CDF_Q_CTXS][PLANE_TYPES][2][8] = {
    {
        {
            {AOM_CDF6(400, 520, 977, 2102, 6542)},
            {AOM_CDF6(210, 405, 1315, 3326, 7537)}
        },
        {
            {AOM_CDF6(2636, 4273, 7588, 11794, 20401)},
            {AOM_CDF6(1786, 3179, 6902, 11357, 19054)}
        }
    },
    {
        {
            {AOM_CDF6(989, 1249, 2019, 4151, 10785)},
            {AOM_CDF6(313, 441, 1099, 2917, 8562)}
        },
        {
            {AOM_CDF6(8394, 10352, 13932, 18855, 26014)},
            {AOM_CDF6(2578, 4124, 8181, 13670, 24234)}
        }
    },
    {
        {
            {AOM_CDF6(2515, 3003, 4452, 8162, 16041)},
            {AOM_CDF6(574, 821, 1836, 5089, 13128)}
        },
        {
            {AOM_CDF6(13468, 16303, 20361, 25105, 29281)},
            {AOM_CDF6(3542, 5502, 10415, 16760, 25644)}
        }
    },
    {
        {
            {AOM_CDF6(4617, 5709, 8446, 13584, 23135)},
            {AOM_CDF6(1156, 1702, 3675, 9274, 20539)}
        },
        {
            {AOM_CDF6(22086, 24282, 27010, 29770, 31743)},
            {AOM_CDF6(7699, 10897, 20891, 26926, 31628)}
        }
    }
};

static const aom_cdf_prob
av1_default_eob_multi64_cdfs[TOKEN_CDF_Q_CTXS][PLANE_TYPES][2][8] = {
    {
        {
            {AOM_CDF7(329, 498, 1101, 1784, 3265, 7758)},
            {AOM_CDF7(335, 730, 1459, 5494, 8755, 12997)}
        },
        {
            {AOM_CDF7(3505, 5304, 10086, 13814, 17684, 23370)},
            {AOM_CDF7(1563, 2700, 4876, 10911, 14706, 22480)}
        }
    },
    {
        {
            {AOM_CDF7(1260, 1446, 2253, 3712, 6652, 13369)},
            {AOM_CDF7(401, 605, 1029, 2563, 5845, 12626)}
        },
        {
            {AOM_CDF7(8609, 10612, 14624, 18714, 22614, 29024)},
            {AOM_CDF7(1923, 3127, 5867, 9703, 14277, 27100)}
        }
    },
    {
        {
            {AOM_CDF7(2374, 2772, 4583, 7276, 12288, 19706)},
            {AOM_CDF7(497, 810, 1315, 3000, 7004, 15641)}
        },
        {
            {AOM_CDF7(15050, 17126, 21410, 24886, 28156, 30726)},
            {AOM_CDF7(4034, 6290, 10235, 14982, 21214, 28491)}
        }
    },
    {
        {
            {AOM_CDF7(6307, 7541, 12060, 16358, 22553, 27865)},
            {AOM_CDF7(1289, 2320, 3971, 7926, 14153, 24291)}
        },
        {
            {AOM_CDF7(24212, 25708, 28268, 30035, 31307, 32049)},
            {AOM_CDF7(8726, 12378, 19409, 26450, 30038, 32462)}
        }
    }
};

static const aom_cdf_prob
av1_default_eob_multi128_cdfs[TOKEN_CDF_Q_CTXS][PLANE_TYPES][2][8] = {
    {
        {
            {AOM_CDF8(219, 482, 1140, 2091, 3680, 6028, 12586)},
            {AOM_CDF8(371, 699, 1254, 4830, 9479, 12562, 17497)}
        },
        {
            {AOM_CDF8(5245, 7456, 12880, 15852, 20033, 23932, 27608)},
            {AOM_CDF8(2054, 3472, 5869, 14232, 18242, 20590, 26752)}
        }
    },
    {
        {
            {AOM_CDF8(685, 933, 1488, 2714, 4766, 8562, 19254)},
            {AOM_CDF8(217, 352, 618, 2303, 5261, 9969, 17472)}
        },
        {
            {AOM_CDF8(8045, 11200, 15497, 19595, 23948, 27408, 30938)},
            {AOM_CDF8(2310, 4160, 7471, 14997, 17931, 20768, 30240)}
        }
    },
    {
        {
            {AOM_CDF8(1366, 1738, 2527, 5016, 9355, 15797, 24643)},
            {AOM_CDF8(354, 558, 944, 2760, 7287, 14037, 21779)}
        },
        {
            {AOM_CDF8(13627, 16246, 20173, 24429, 27948, 30415, 31863)},
            {AOM_CDF8(6275, 9889, 14769, 23164, 27988, 30493, 32272)}
        }
    },
    {
        {
            {AOM_CDF8(3472, 4885, 7489, 12481, 18517, 24536, 29635)},
            {AOM_CDF8(886, 1731, 3271, 8469, 15569, 22126, 28383)}
        },
        {
            {AOM_CDF8(24313, 26062, 28385, 30107, 31217, 31898, 32345)},
            {AOM_CDF8(9165, 13282, 21150, 30286, 31894, 32571, 32712)}
        }
    }
};

static const aom_cdf_prob
av1_default_eob_multi256_cdfs[TOKEN_CDF_Q_CTXS][PLANE_TYPES][2][8] = {
    {
        {
            {AOM_CDF9(310, 584, 1887, 3589, 6168, 8611, 11352, 15652)},
            {AOM_CDF9(998, 1850, 2998, 5604, 17341, 19888, 22899, 25583)}
        },
        {
            {AOM_CDF9(2520, 3240, 5952, 8870, 12577, 17558, 19954, 24168)},
            {AOM_CDF9(2203, 4130, 7435, 10739, 20652, 23681, 25609, 27261)}
        }
    },
    {
        {
            {AOM_CDF9(1448, 2109, 4151, 6263, 9329, 13260, 17944, 23300)},
            {AOM_CDF9(399, 1019, 1749, 3038, 10444, 15546, 22739, 27294)}
        },
        {
            {AOM_CDF9(6402, 8148, 12623, 15072, 18728, 22847, 26447, 29377)},
            {AOM_CDF9(1674, 3252, 5734, 10159, 22397, 23802, 24821, 30940)}
        }
    },
    {
        {
            {AOM_CDF9(3089, 3920, 6038, 9460, 14266, 19881, 25766, 29176)},
            {AOM_CDF9(1084, 2358, 3488, 5122, 11483, 18103, 26023, 29799)}
        },
        {
            {AOM_CDF9(11514, 13794, 17480, 20754, 24361, 27378, 29492, 31277)},
            {AOM_CDF9(6571, 9610, 15516, 21826, 29092, 30829, 31842, 32708)}
        }
    },
    {
        {
            {AOM_CDF9(5348, 7113, 11820, 15924, 22106, 26777, 30334, 31757)},
            {AOM_CDF9(2453, 4474, 6307, 8777, 16474, 22975, 29000, 31547)}
        },
        {
            {AOM_CDF9(23110, 24597, 27140, 28894, 30167, 30927, 31392, 32094)},
            {AOM_CDF9(9998, 17661, 25178, 28097, 31308, 32038, 32403, 32695)}
        }
    }
};

static const aom_cdf_prob
av1_default_eob_multi512_cdfs[TOKEN_CDF_Q_CTXS][PLANE_TYPES][2][16] = {
    {
        {
            {AOM_CDF10(641, 983, 3707, 5430, 10234, 14958, 18788, 23412, 26061)},
            {AOM_CDF10(3277, 6554, 9830, 13107, 16384, 19661, 22938, 26214, 29491)}
        },
        {
            {AOM_CDF10(5095, 6446, 9996, 13354, 16017, 17986, 20919, 26129, 29140)},
            {
                AOM_CDF10(3277, 6554, 9830, 13107, 16384, 19661, 22938, 26214,
                29491)
            }
        }
    },
    {
        {
            {AOM_CDF10(1230, 2278, 5035, 7776, 11871, 15346, 19590, 24584, 28749)},
            {AOM_CDF10(3277, 6554, 9830, 13107, 16384, 19661, 22938, 26214, 29491)}
        },
        {
            {AOM_CDF10(7265, 9979, 15819, 19250, 21780, 23846, 26478, 28396, 31811)},
            {AOM_CDF10(3277, 6554, 9830, 13107, 16384, 19661, 22938, 26214, 29491)}
        }
    },
    {
        {
            {AOM_CDF10(2624, 3936, 6480, 9686, 13979, 17726, 23267, 28410, 31078)},
            {AOM_CDF10(3277, 6554, 9830, 13107, 16384, 19661, 22938, 26214, 29491)}
        },
        {
            {AOM_CDF10(12015, 14769, 19588, 22052, 24222, 25812, 27300, 29219, 32114)},
            {AOM_CDF10(3277, 6554, 9830, 13107, 16384, 19661, 22938, 26214, 29491)}
        }
    },
    {
        {
            {AOM_CDF10(5927, 7809, 10923, 14597, 19439, 24135, 28456, 31142, 32060)},
            {AOM_CDF10(3277, 6554, 9830, 13107, 16384, 19661, 22938, 26214, 29491)}
        },
        {
            {AOM_CDF10(21093, 23043, 25742, 27658, 29097, 29716, 30073, 30820, 31956)},
            {AOM_CDF10(3277, 6554, 9830, 13107, 16384, 19661, 22938, 26214, 29491)}
        }
    }
};

static const aom_cdf_prob
av1_default_eob_multi1024_cdfs[TOKEN_CDF_Q_CTXS][PLANE_TYPES][2][16] = {
    {
        {
            {AOM_CDF11(393, 421, 751, 1623, 3160, 6352, 13345, 18047, 22571, 25830)},
            {AOM_CDF11(2979, 5958, 8937, 11916, 14895, 17873, 20852, 23831, 26810, 29789)}
        },
        {
            {AOM_CDF11(1865, 1988, 2930, 4242, 10533, 16538, 21354, 27255, 28546, 31784)},
            {AOM_CDF11(2979, 5958, 8937, 11916, 14895, 17873, 20852, 23831, 26810, 29789)}
        }
    },
    {
        {
            {AOM_CDF11(696, 948, 3145, 5702, 9706, 13217, 17851, 21856, 25692, 28034)},
            {AOM_CDF11(2979, 5958, 8937, 11916, 14895, 17873, 20852, 23831, 26810, 29789)}
        },
        {
            {AOM_CDF11(2672, 3591, 9330, 17084, 22725, 24284, 26527, 28027, 28377, 30876)},
            {AOM_CDF11(2979, 5958, 8937, 11916, 14895, 17873, 20852, 23831, 26810, 29789)}
        }
    },
    {
        {
            {AOM_CDF11(2784, 3831, 7041, 10521, 14847, 18844, 23155, 26682, 29229, 31045)},
            {AOM_CDF11(2979, 5958, 8937, 11916, 14895, 17873, 20852, 23831, 26810, 29789)}
        },
        {
            {AOM_CDF11(9577, 12466, 17739, 20750, 22061, 23215, 24601, 25483, 25843, 32056)},
            {AOM_CDF11(2979, 5958, 8937, 11916, 14895, 17873, 20852, 23831, 26810, 29789)}
        }
    },
    {
        {
            {AOM_CDF11(6698, 8334, 11961, 15762, 20186, 23862, 27434, 29326, 31082, 32050)},
            {AOM_CDF11(2979, 5958, 8937, 11916, 14895, 17873, 20852, 23831, 26810, 29789)}
        },
        {
            {AOM_CDF11(20569, 22426, 25569, 26859, 28053, 28913, 29486, 29724, 29807, 32570)},
            {AOM_CDF11(2979, 5958, 8937, 11916, 14895, 17873, 20852, 23831, 26810, 29789)}
        }
    }
};

static const aom_cdf_prob av1_default_coeff_lps_multi_cdfs
[TOKEN_CDF_Q_CTXS][TX_SIZES][PLANE_TYPES][LEVEL_CONTEXTS][CDF_SIZE(BR_CDF_SIZE) + 1] = {
    {
        {
            {
                {AOM_CDF4(14298, 20718, 24174)}, {AOM_CDF4(12536, 19601, 23789)},
                {AOM_CDF4(8712, 15051, 19503)},  {AOM_CDF4(6170, 11327, 15434)},
                {AOM_CDF4(4742, 8926, 12538)},   {AOM_CDF4(3803, 7317, 10546)},
                {AOM_CDF4(1696, 3317, 4871)},    {AOM_CDF4(14392, 19951, 22756)},
                {AOM_CDF4(15978, 23218, 26818)}, {AOM_CDF4(12187, 19474, 23889)},
                {AOM_CDF4(9176, 15640, 20259)},  {AOM_CDF4(7068, 12655, 17028)},
                {AOM_CDF4(5656, 10442, 14472)},  {AOM_CDF4(2580, 4992, 7244)},
                {AOM_CDF4(12136, 18049, 21426)}, {AOM_CDF4(13784, 20721, 24481)},
                {AOM_CDF4(10836, 17621, 21900)}, {AOM_CDF4(8372, 14444, 18847)},
                {AOM_CDF4(6523, 11779, 16000)},  {AOM_CDF4(5337, 9898, 13760)},
                {AOM_CDF4(3034, 5860, 8462)}
            },
            {
                {AOM_CDF4(15967, 22905, 26286)}, {AOM_CDF4(13534, 20654, 24579)},
                {AOM_CDF4(9504, 16092, 20535)},  {AOM_CDF4(6975, 12568, 16903)},
                {AOM_CDF4(5364, 10091, 14020)},  {AOM_CDF4(4357, 8370, 11857)},
                {AOM_CDF4(2506, 4934, 7218)},    {AOM_CDF4(23032, 28815, 30936)},
                {AOM_CDF4(19540, 26704, 29719)}, {AOM_CDF4(15158, 22969, 27097)},
                {AOM_CDF4(11408, 18865, 23650)}, {AOM_CDF4(8885, 15448, 20250)},
                {AOM_CDF4(7108, 12853, 17416)},  {AOM_CDF4(4231, 8041, 11480)},
                {AOM_CDF4(19823, 26490, 29156)}, {AOM_CDF4(18890, 25929, 28932)},
                {AOM_CDF4(15660, 23491, 27433)}, {AOM_CDF4(12147, 19776, 24488)},
                {AOM_CDF4(9728, 16774, 21649)},  {AOM_CDF4(7919, 14277, 19066)},
                {AOM_CDF4(5440, 10170, 14185)}
            }
        },
        {
            {
                {AOM_CDF4(14406, 20862, 24414)}, {AOM_CDF4(11824, 18907, 23109)},
                {AOM_CDF4(8257, 14393, 18803)},  {AOM_CDF4(5860, 10747, 14778)},
                {AOM_CDF4(4475, 8486, 11984)},   {AOM_CDF4(3606, 6954, 10043)},
                {AOM_CDF4(1736, 3410, 5048)},    {AOM_CDF4(14430, 20046, 22882)},
                {AOM_CDF4(15593, 22899, 26709)}, {AOM_CDF4(12102, 19368, 23811)},
                {AOM_CDF4(9059, 15584, 20262)},  {AOM_CDF4(6999, 12603, 17048)},
                {AOM_CDF4(5684, 10497, 14553)},  {AOM_CDF4(2822, 5438, 7862)},
                {AOM_CDF4(15785, 21585, 24359)}, {AOM_CDF4(18347, 25229, 28266)},
                {AOM_CDF4(14974, 22487, 26389)}, {AOM_CDF4(11423, 18681, 23271)},
                {AOM_CDF4(8863, 15350, 20008)},  {AOM_CDF4(7153, 12852, 17278)},
                {AOM_CDF4(3707, 7036, 9982)}
            },
            {
                {AOM_CDF4(15460, 21696, 25469)}, {AOM_CDF4(12170, 19249, 23191)},
                {AOM_CDF4(8723, 15027, 19332)},  {AOM_CDF4(6428, 11704, 15874)},
                {AOM_CDF4(4922, 9292, 13052)},   {AOM_CDF4(4139, 7695, 11010)},
                {AOM_CDF4(2291, 4508, 6598)},    {AOM_CDF4(19856, 26920, 29828)},
                {AOM_CDF4(17923, 25289, 28792)}, {AOM_CDF4(14278, 21968, 26297)},
                {AOM_CDF4(10910, 18136, 22950)}, {AOM_CDF4(8423, 14815, 19627)},
                {AOM_CDF4(6771, 12283, 16774)},  {AOM_CDF4(4074, 7750, 11081)},
                {AOM_CDF4(19852, 26074, 28672)}, {AOM_CDF4(19371, 26110, 28989)},
                {AOM_CDF4(16265, 23873, 27663)}, {AOM_CDF4(12758, 20378, 24952)},
                {AOM_CDF4(10095, 17098, 21961)}, {AOM_CDF4(8250, 14628, 19451)},
                {AOM_CDF4(5205, 9745, 13622)}
            }
        },
        {
            {
                {AOM_CDF4(10563, 16233, 19763)}, {AOM_CDF4(9794, 16022, 19804)},
                {AOM_CDF4(6750, 11945, 15759)},  {AOM_CDF4(4963, 9186, 12752)},
                {AOM_CDF4(3845, 7435, 10627)},   {AOM_CDF4(3051, 6085, 8834)},
                {AOM_CDF4(1311, 2596, 3830)},    {AOM_CDF4(11246, 16404, 19689)},
                {AOM_CDF4(12315, 18911, 22731)}, {AOM_CDF4(10557, 17095, 21289)},
                {AOM_CDF4(8136, 14006, 18249)},  {AOM_CDF4(6348, 11474, 15565)},
                {AOM_CDF4(5196, 9655, 13400)},   {AOM_CDF4(2349, 4526, 6587)},
                {AOM_CDF4(13337, 18730, 21569)}, {AOM_CDF4(19306, 26071, 28882)},
                {AOM_CDF4(15952, 23540, 27254)}, {AOM_CDF4(12409, 19934, 24430)},
                {AOM_CDF4(9760, 16706, 21389)},  {AOM_CDF4(8004, 14220, 18818)},
                {AOM_CDF4(4138, 7794, 10961)}
            },
            {
                {AOM_CDF4(10870, 16684, 20949)}, {AOM_CDF4(9664, 15230, 18680)},
                {AOM_CDF4(6886, 12109, 15408)},  {AOM_CDF4(4825, 8900, 12305)},
                {AOM_CDF4(3630, 7162, 10314)},   {AOM_CDF4(3036, 6429, 9387)},
                {AOM_CDF4(1671, 3296, 4940)},    {AOM_CDF4(13819, 19159, 23026)},
                {AOM_CDF4(11984, 19108, 23120)}, {AOM_CDF4(10690, 17210, 21663)},
                {AOM_CDF4(7984, 14154, 18333)},  {AOM_CDF4(6868, 12294, 16124)},
                {AOM_CDF4(5274, 8994, 12868)},   {AOM_CDF4(2988, 5771, 8424)},
                {AOM_CDF4(19736, 26647, 29141)}, {AOM_CDF4(18933, 26070, 28984)},
                {AOM_CDF4(15779, 23048, 27200)}, {AOM_CDF4(12638, 20061, 24532)},
                {AOM_CDF4(10692, 17545, 22220)}, {AOM_CDF4(9217, 15251, 20054)},
                {AOM_CDF4(5078, 9284, 12594)}
            }
        },
        {
            {
                {AOM_CDF4(2331, 3662, 5244)},    {AOM_CDF4(2891, 4771, 6145)},
                {AOM_CDF4(4598, 7623, 9729)},    {AOM_CDF4(3520, 6845, 9199)},
                {AOM_CDF4(3417, 6119, 9324)},    {AOM_CDF4(2601, 5412, 7385)},
                {AOM_CDF4(600, 1173, 1744)},     {AOM_CDF4(7672, 13286, 17469)},
                {AOM_CDF4(4232, 7792, 10793)},   {AOM_CDF4(2915, 5317, 7397)},
                {AOM_CDF4(2318, 4356, 6152)},    {AOM_CDF4(2127, 4000, 5554)},
                {AOM_CDF4(1850, 3478, 5275)},    {AOM_CDF4(977, 1933, 2843)},
                {AOM_CDF4(18280, 24387, 27989)}, {AOM_CDF4(15852, 22671, 26185)},
                {AOM_CDF4(13845, 20951, 24789)}, {AOM_CDF4(11055, 17966, 22129)},
                {AOM_CDF4(9138, 15422, 19801)},  {AOM_CDF4(7454, 13145, 17456)},
                {AOM_CDF4(3370, 6393, 9013)}
            },
            {
                {AOM_CDF4(5842, 9229, 10838)},   {AOM_CDF4(2313, 3491, 4276)},
                {AOM_CDF4(2998, 6104, 7496)},    {AOM_CDF4(2420, 7447, 9868)},
                {AOM_CDF4(3034, 8495, 10923)},   {AOM_CDF4(4076, 8937, 10975)},
                {AOM_CDF4(1086, 2370, 3299)},    {AOM_CDF4(9714, 17254, 20444)},
                {AOM_CDF4(8543, 13698, 17123)},  {AOM_CDF4(4918, 9007, 11910)},
                {AOM_CDF4(4129, 7532, 10553)},   {AOM_CDF4(2364, 5533, 8058)},
                {AOM_CDF4(1834, 3546, 5563)},    {AOM_CDF4(1473, 2908, 4133)},
                {AOM_CDF4(15405, 21193, 25619)}, {AOM_CDF4(15691, 21952, 26561)},
                {AOM_CDF4(12962, 19194, 24165)}, {AOM_CDF4(10272, 17855, 22129)},
                {AOM_CDF4(8588, 15270, 20718)},  {AOM_CDF4(8682, 14669, 19500)},
                {AOM_CDF4(4870, 9636, 13205)}
            }
        },
        {
            {
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}
            },
            {
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}
            }
        }
    },
    {
        {
            {
                {AOM_CDF4(14995, 21341, 24749)}, {AOM_CDF4(13158, 20289, 24601)},
                {AOM_CDF4(8941, 15326, 19876)},  {AOM_CDF4(6297, 11541, 15807)},
                {AOM_CDF4(4817, 9029, 12776)},   {AOM_CDF4(3731, 7273, 10627)},
                {AOM_CDF4(1847, 3617, 5354)},    {AOM_CDF4(14472, 19659, 22343)},
                {AOM_CDF4(16806, 24162, 27533)}, {AOM_CDF4(12900, 20404, 24713)},
                {AOM_CDF4(9411, 16112, 20797)},  {AOM_CDF4(7056, 12697, 17148)},
                {AOM_CDF4(5544, 10339, 14460)},  {AOM_CDF4(2954, 5704, 8319)},
                {AOM_CDF4(12464, 18071, 21354)}, {AOM_CDF4(15482, 22528, 26034)},
                {AOM_CDF4(12070, 19269, 23624)}, {AOM_CDF4(8953, 15406, 20106)},
                {AOM_CDF4(7027, 12730, 17220)},  {AOM_CDF4(5887, 10913, 15140)},
                {AOM_CDF4(3793, 7278, 10447)}
            },
            {
                {AOM_CDF4(15571, 22232, 25749)}, {AOM_CDF4(14506, 21575, 25374)},
                {AOM_CDF4(10189, 17089, 21569)}, {AOM_CDF4(7316, 13301, 17915)},
                {AOM_CDF4(5783, 10912, 15190)},  {AOM_CDF4(4760, 9155, 13088)},
                {AOM_CDF4(2993, 5966, 8774)},    {AOM_CDF4(23424, 28903, 30778)},
                {AOM_CDF4(20775, 27666, 30290)}, {AOM_CDF4(16474, 24410, 28299)},
                {AOM_CDF4(12471, 20180, 24987)}, {AOM_CDF4(9410, 16487, 21439)},
                {AOM_CDF4(7536, 13614, 18529)},  {AOM_CDF4(5048, 9586, 13549)},
                {AOM_CDF4(21090, 27290, 29756)}, {AOM_CDF4(20796, 27402, 30026)},
                {AOM_CDF4(17819, 25485, 28969)}, {AOM_CDF4(13860, 21909, 26462)},
                {AOM_CDF4(11002, 18494, 23529)}, {AOM_CDF4(8953, 15929, 20897)},
                {AOM_CDF4(6448, 11918, 16454)}
            }
        },
        {
            {
                {AOM_CDF4(15999, 22208, 25449)}, {AOM_CDF4(13050, 19988, 24122)},
                {AOM_CDF4(8594, 14864, 19378)},  {AOM_CDF4(6033, 11079, 15238)},
                {AOM_CDF4(4554, 8683, 12347)},   {AOM_CDF4(3672, 7139, 10337)},
                {AOM_CDF4(1900, 3771, 5576)},    {AOM_CDF4(15788, 21340, 23949)},
                {AOM_CDF4(16825, 24235, 27758)}, {AOM_CDF4(12873, 20402, 24810)},
                {AOM_CDF4(9590, 16363, 21094)},  {AOM_CDF4(7352, 13209, 17733)},
                {AOM_CDF4(5960, 10989, 15184)},  {AOM_CDF4(3232, 6234, 9007)},
                {AOM_CDF4(15761, 20716, 23224)}, {AOM_CDF4(19318, 25989, 28759)},
                {AOM_CDF4(15529, 23094, 26929)}, {AOM_CDF4(11662, 18989, 23641)},
                {AOM_CDF4(8955, 15568, 20366)},  {AOM_CDF4(7281, 13106, 17708)},
                {AOM_CDF4(4248, 8059, 11440)}
            },
            {
                {AOM_CDF4(14899, 21217, 24503)}, {AOM_CDF4(13519, 20283, 24047)},
                {AOM_CDF4(9429, 15966, 20365)},  {AOM_CDF4(6700, 12355, 16652)},
                {AOM_CDF4(5088, 9704, 13716)},   {AOM_CDF4(4243, 8154, 11731)},
                {AOM_CDF4(2702, 5364, 7861)},    {AOM_CDF4(22745, 28388, 30454)},
                {AOM_CDF4(20235, 27146, 29922)}, {AOM_CDF4(15896, 23715, 27637)},
                {AOM_CDF4(11840, 19350, 24131)}, {AOM_CDF4(9122, 15932, 20880)},
                {AOM_CDF4(7488, 13581, 18362)},  {AOM_CDF4(5114, 9568, 13370)},
                {AOM_CDF4(20845, 26553, 28932)}, {AOM_CDF4(20981, 27372, 29884)},
                {AOM_CDF4(17781, 25335, 28785)}, {AOM_CDF4(13760, 21708, 26297)},
                {AOM_CDF4(10975, 18415, 23365)}, {AOM_CDF4(9045, 15789, 20686)},
                {AOM_CDF4(6130, 11199, 15423)}
            }
        },
        {
            {
                {AOM_CDF4(13549, 19724, 23158)}, {AOM_CDF4(11844, 18382, 22246)},
                {AOM_CDF4(7919, 13619, 17773)},  {AOM_CDF4(5486, 10143, 13946)},
                {AOM_CDF4(4166, 7983, 11324)},   {AOM_CDF4(3364, 6506, 9427)},
                {AOM_CDF4(1598, 3160, 4674)},    {AOM_CDF4(15281, 20979, 23781)},
                {AOM_CDF4(14939, 22119, 25952)}, {AOM_CDF4(11363, 18407, 22812)},
                {AOM_CDF4(8609, 14857, 19370)},  {AOM_CDF4(6737, 12184, 16480)},
                {AOM_CDF4(5506, 10263, 14262)},  {AOM_CDF4(2990, 5786, 8380)},
                {AOM_CDF4(20249, 25253, 27417)}, {AOM_CDF4(21070, 27518, 30001)},
                {AOM_CDF4(16854, 24469, 28074)}, {AOM_CDF4(12864, 20486, 25000)},
                {AOM_CDF4(9962, 16978, 21778)},  {AOM_CDF4(8074, 14338, 19048)},
                {AOM_CDF4(4494, 8479, 11906)}
            },
            {
                {AOM_CDF4(13960, 19617, 22829)}, {AOM_CDF4(11150, 17341, 21228)},
                {AOM_CDF4(7150, 12964, 17190)},  {AOM_CDF4(5331, 10002, 13867)},
                {AOM_CDF4(4167, 7744, 11057)},   {AOM_CDF4(3480, 6629, 9646)},
                {AOM_CDF4(1883, 3784, 5686)},    {AOM_CDF4(18752, 25660, 28912)},
                {AOM_CDF4(16968, 24586, 28030)}, {AOM_CDF4(13520, 21055, 25313)},
                {AOM_CDF4(10453, 17626, 22280)}, {AOM_CDF4(8386, 14505, 19116)},
                {AOM_CDF4(6742, 12595, 17008)},  {AOM_CDF4(4273, 8140, 11499)},
                {AOM_CDF4(22120, 27827, 30233)}, {AOM_CDF4(20563, 27358, 29895)},
                {AOM_CDF4(17076, 24644, 28153)}, {AOM_CDF4(13362, 20942, 25309)},
                {AOM_CDF4(10794, 17965, 22695)}, {AOM_CDF4(9014, 15652, 20319)},
                {AOM_CDF4(5708, 10512, 14497)}
            }
        },
        {
            {
                {AOM_CDF4(5705, 10930, 15725)},  {AOM_CDF4(7946, 12765, 16115)},
                {AOM_CDF4(6801, 12123, 16226)},  {AOM_CDF4(5462, 10135, 14200)},
                {AOM_CDF4(4189, 8011, 11507)},   {AOM_CDF4(3191, 6229, 9408)},
                {AOM_CDF4(1057, 2137, 3212)},    {AOM_CDF4(10018, 17067, 21491)},
                {AOM_CDF4(7380, 12582, 16453)},  {AOM_CDF4(6068, 10845, 14339)},
                {AOM_CDF4(5098, 9198, 12555)},   {AOM_CDF4(4312, 8010, 11119)},
                {AOM_CDF4(3700, 6966, 9781)},    {AOM_CDF4(1693, 3326, 4887)},
                {AOM_CDF4(18757, 24930, 27774)}, {AOM_CDF4(17648, 24596, 27817)},
                {AOM_CDF4(14707, 22052, 26026)}, {AOM_CDF4(11720, 18852, 23292)},
                {AOM_CDF4(9357, 15952, 20525)},  {AOM_CDF4(7810, 13753, 18210)},
                {AOM_CDF4(3879, 7333, 10328)}
            },
            {
                {AOM_CDF4(8278, 13242, 15922)},  {AOM_CDF4(10547, 15867, 18919)},
                {AOM_CDF4(9106, 15842, 20609)},  {AOM_CDF4(6833, 13007, 17218)},
                {AOM_CDF4(4811, 9712, 13923)},   {AOM_CDF4(3985, 7352, 11128)},
                {AOM_CDF4(1688, 3458, 5262)},    {AOM_CDF4(12951, 21861, 26510)},
                {AOM_CDF4(9788, 16044, 20276)},  {AOM_CDF4(6309, 11244, 14870)},
                {AOM_CDF4(5183, 9349, 12566)},   {AOM_CDF4(4389, 8229, 11492)},
                {AOM_CDF4(3633, 6945, 10620)},   {AOM_CDF4(3600, 6847, 9907)},
                {AOM_CDF4(21748, 28137, 30255)}, {AOM_CDF4(19436, 26581, 29560)},
                {AOM_CDF4(16359, 24201, 27953)}, {AOM_CDF4(13961, 21693, 25871)},
                {AOM_CDF4(11544, 18686, 23322)}, {AOM_CDF4(9372, 16462, 20952)},
                {AOM_CDF4(6138, 11210, 15390)}
            }
        },
        {
            {
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}
            },
            {
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}
            }
        }
    },
    {
        {
            {
                {AOM_CDF4(16138, 22223, 25509)}, {AOM_CDF4(15347, 22430, 26332)},
                {AOM_CDF4(9614, 16736, 21332)},  {AOM_CDF4(6600, 12275, 16907)},
                {AOM_CDF4(4811, 9424, 13547)},   {AOM_CDF4(3748, 7809, 11420)},
                {AOM_CDF4(2254, 4587, 6890)},    {AOM_CDF4(15196, 20284, 23177)},
                {AOM_CDF4(18317, 25469, 28451)}, {AOM_CDF4(13918, 21651, 25842)},
                {AOM_CDF4(10052, 17150, 21995)}, {AOM_CDF4(7499, 13630, 18587)},
                {AOM_CDF4(6158, 11417, 16003)},  {AOM_CDF4(4014, 7785, 11252)},
                {AOM_CDF4(15048, 21067, 24384)}, {AOM_CDF4(18202, 25346, 28553)},
                {AOM_CDF4(14302, 22019, 26356)}, {AOM_CDF4(10839, 18139, 23166)},
                {AOM_CDF4(8715, 15744, 20806)},  {AOM_CDF4(7536, 13576, 18544)},
                {AOM_CDF4(5413, 10335, 14498)}
            },
            {
                {AOM_CDF4(17394, 24501, 27895)}, {AOM_CDF4(15889, 23420, 27185)},
                {AOM_CDF4(11561, 19133, 23870)}, {AOM_CDF4(8285, 14812, 19844)},
                {AOM_CDF4(6496, 12043, 16550)},  {AOM_CDF4(4771, 9574, 13677)},
                {AOM_CDF4(3603, 6830, 10144)},   {AOM_CDF4(21656, 27704, 30200)},
                {AOM_CDF4(21324, 27915, 30511)}, {AOM_CDF4(17327, 25336, 28997)},
                {AOM_CDF4(13417, 21381, 26033)}, {AOM_CDF4(10132, 17425, 22338)},
                {AOM_CDF4(8580, 15016, 19633)},  {AOM_CDF4(5694, 11477, 16411)},
                {AOM_CDF4(24116, 29780, 31450)}, {AOM_CDF4(23853, 29695, 31591)},
                {AOM_CDF4(20085, 27614, 30428)}, {AOM_CDF4(15326, 24335, 28575)},
                {AOM_CDF4(11814, 19472, 24810)}, {AOM_CDF4(10221, 18611, 24767)},
                {AOM_CDF4(7689, 14558, 20321)}
            }
        },
        {
            {
                {AOM_CDF4(16214, 22380, 25770)}, {AOM_CDF4(14213, 21304, 25295)},
                {AOM_CDF4(9213, 15823, 20455)},  {AOM_CDF4(6395, 11758, 16139)},
                {AOM_CDF4(4779, 9187, 13066)},   {AOM_CDF4(3821, 7501, 10953)},
                {AOM_CDF4(2293, 4567, 6795)},    {AOM_CDF4(15859, 21283, 23820)},
                {AOM_CDF4(18404, 25602, 28726)}, {AOM_CDF4(14325, 21980, 26206)},
                {AOM_CDF4(10669, 17937, 22720)}, {AOM_CDF4(8297, 14642, 19447)},
                {AOM_CDF4(6746, 12389, 16893)},  {AOM_CDF4(4324, 8251, 11770)},
                {AOM_CDF4(16532, 21631, 24475)}, {AOM_CDF4(20667, 27150, 29668)},
                {AOM_CDF4(16728, 24510, 28175)}, {AOM_CDF4(12861, 20645, 25332)},
                {AOM_CDF4(10076, 17361, 22417)}, {AOM_CDF4(8395, 14940, 19963)},
                {AOM_CDF4(5731, 10683, 14912)}
            },
            {
                {AOM_CDF4(14433, 21155, 24938)}, {AOM_CDF4(14658, 21716, 25545)},
                {AOM_CDF4(9923, 16824, 21557)},  {AOM_CDF4(6982, 13052, 17721)},
                {AOM_CDF4(5419, 10503, 15050)},  {AOM_CDF4(4852, 9162, 13014)},
                {AOM_CDF4(3271, 6395, 9630)},    {AOM_CDF4(22210, 27833, 30109)},
                {AOM_CDF4(20750, 27368, 29821)}, {AOM_CDF4(16894, 24828, 28573)},
                {AOM_CDF4(13247, 21276, 25757)}, {AOM_CDF4(10038, 17265, 22563)},
                {AOM_CDF4(8587, 14947, 20327)},  {AOM_CDF4(5645, 11371, 15252)},
                {AOM_CDF4(22027, 27526, 29714)}, {AOM_CDF4(23098, 29146, 31221)},
                {AOM_CDF4(19886, 27341, 30272)}, {AOM_CDF4(15609, 23747, 28046)},
                {AOM_CDF4(11993, 20065, 24939)}, {AOM_CDF4(9637, 18267, 23671)},
                {AOM_CDF4(7625, 13801, 19144)}
            }
        },
        {
            {
                {AOM_CDF4(14438, 20798, 24089)}, {AOM_CDF4(12621, 19203, 23097)},
                {AOM_CDF4(8177, 14125, 18402)},  {AOM_CDF4(5674, 10501, 14456)},
                {AOM_CDF4(4236, 8239, 11733)},   {AOM_CDF4(3447, 6750, 9806)},
                {AOM_CDF4(1986, 3950, 5864)},    {AOM_CDF4(16208, 22099, 24930)},
                {AOM_CDF4(16537, 24025, 27585)}, {AOM_CDF4(12780, 20381, 24867)},
                {AOM_CDF4(9767, 16612, 21416)},  {AOM_CDF4(7686, 13738, 18398)},
                {AOM_CDF4(6333, 11614, 15964)},  {AOM_CDF4(3941, 7571, 10836)},
                {AOM_CDF4(22819, 27422, 29202)}, {AOM_CDF4(22224, 28514, 30721)},
                {AOM_CDF4(17660, 25433, 28913)}, {AOM_CDF4(13574, 21482, 26002)},
                {AOM_CDF4(10629, 17977, 22938)}, {AOM_CDF4(8612, 15298, 20265)},
                {AOM_CDF4(5607, 10491, 14596)}
            },
            {
                {AOM_CDF4(13569, 19800, 23206)}, {AOM_CDF4(13128, 19924, 23869)},
                {AOM_CDF4(8329, 14841, 19403)},  {AOM_CDF4(6130, 10976, 15057)},
                {AOM_CDF4(4682, 8839, 12518)},   {AOM_CDF4(3656, 7409, 10588)},
                {AOM_CDF4(2577, 5099, 7412)},    {AOM_CDF4(22427, 28684, 30585)},
                {AOM_CDF4(20913, 27750, 30139)}, {AOM_CDF4(15840, 24109, 27834)},
                {AOM_CDF4(12308, 20029, 24569)}, {AOM_CDF4(10216, 16785, 21458)},
                {AOM_CDF4(8309, 14203, 19113)},  {AOM_CDF4(6043, 11168, 15307)},
                {AOM_CDF4(23166, 28901, 30998)}, {AOM_CDF4(21899, 28405, 30751)},
                {AOM_CDF4(18413, 26091, 29443)}, {AOM_CDF4(15233, 23114, 27352)},
                {AOM_CDF4(12683, 20472, 25288)}, {AOM_CDF4(10702, 18259, 23409)},
                {AOM_CDF4(8125, 14464, 19226)}
            }
        },
        {
            {
                {AOM_CDF4(9040, 14786, 18360)},  {AOM_CDF4(9979, 15718, 19415)},
                {AOM_CDF4(7913, 13918, 18311)},  {AOM_CDF4(5859, 10889, 15184)},
                {AOM_CDF4(4593, 8677, 12510)},   {AOM_CDF4(3820, 7396, 10791)},
                {AOM_CDF4(1730, 3471, 5192)},    {AOM_CDF4(11803, 18365, 22709)},
                {AOM_CDF4(11419, 18058, 22225)}, {AOM_CDF4(9418, 15774, 20243)},
                {AOM_CDF4(7539, 13325, 17657)},  {AOM_CDF4(6233, 11317, 15384)},
                {AOM_CDF4(5137, 9656, 13545)},   {AOM_CDF4(2977, 5774, 8349)},
                {AOM_CDF4(21207, 27246, 29640)}, {AOM_CDF4(19547, 26578, 29497)},
                {AOM_CDF4(16169, 23871, 27690)}, {AOM_CDF4(12820, 20458, 25018)},
                {AOM_CDF4(10224, 17332, 22214)}, {AOM_CDF4(8526, 15048, 19884)},
                {AOM_CDF4(5037, 9410, 13118)}
            },
            {
                {AOM_CDF4(12339, 17329, 20140)}, {AOM_CDF4(13505, 19895, 23225)},
                {AOM_CDF4(9847, 16944, 21564)},  {AOM_CDF4(7280, 13256, 18348)},
                {AOM_CDF4(4712, 10009, 14454)},  {AOM_CDF4(4361, 7914, 12477)},
                {AOM_CDF4(2870, 5628, 7995)},    {AOM_CDF4(20061, 25504, 28526)},
                {AOM_CDF4(15235, 22878, 26145)}, {AOM_CDF4(12985, 19958, 24155)},
                {AOM_CDF4(9782, 16641, 21403)},  {AOM_CDF4(9456, 16360, 20760)},
                {AOM_CDF4(6855, 12940, 18557)},  {AOM_CDF4(5661, 10564, 15002)},
                {AOM_CDF4(25656, 30602, 31894)}, {AOM_CDF4(22570, 29107, 31092)},
                {AOM_CDF4(18917, 26423, 29541)}, {AOM_CDF4(15940, 23649, 27754)},
                {AOM_CDF4(12803, 20581, 25219)}, {AOM_CDF4(11082, 18695, 23376)},
                {AOM_CDF4(7939, 14373, 19005)}
            }
        },
        {
            {
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}
            },
            {
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}
            }
        }
    },
    {
        {
            {
                {AOM_CDF4(18315, 24289, 27551)}, {AOM_CDF4(16854, 24068, 27835)},
                {AOM_CDF4(10140, 17927, 23173)}, {AOM_CDF4(6722, 12982, 18267)},
                {AOM_CDF4(4661, 9826, 14706)},   {AOM_CDF4(3832, 8165, 12294)},
                {AOM_CDF4(2795, 6098, 9245)},    {AOM_CDF4(17145, 23326, 26672)},
                {AOM_CDF4(20733, 27680, 30308)}, {AOM_CDF4(16032, 24461, 28546)},
                {AOM_CDF4(11653, 20093, 25081)}, {AOM_CDF4(9290, 16429, 22086)},
                {AOM_CDF4(7796, 14598, 19982)},  {AOM_CDF4(6502, 12378, 17441)},
                {AOM_CDF4(21681, 27732, 30320)}, {AOM_CDF4(22389, 29044, 31261)},
                {AOM_CDF4(19027, 26731, 30087)}, {AOM_CDF4(14739, 23755, 28624)},
                {AOM_CDF4(11358, 20778, 25511)}, {AOM_CDF4(10995, 18073, 24190)},
                {AOM_CDF4(9162, 14990, 20617)}
            },
            {
                {AOM_CDF4(21425, 27952, 30388)}, {AOM_CDF4(18062, 25838, 29034)},
                {AOM_CDF4(11956, 19881, 24808)}, {AOM_CDF4(7718, 15000, 20980)},
                {AOM_CDF4(5702, 11254, 16143)},  {AOM_CDF4(4898, 9088, 16864)},
                {AOM_CDF4(3679, 6776, 11907)},   {AOM_CDF4(23294, 30160, 31663)},
                {AOM_CDF4(24397, 29896, 31836)}, {AOM_CDF4(19245, 27128, 30593)},
                {AOM_CDF4(13202, 19825, 26404)}, {AOM_CDF4(11578, 19297, 23957)},
                {AOM_CDF4(8073, 13297, 21370)},  {AOM_CDF4(5461, 10923, 19745)},
                {AOM_CDF4(27367, 30521, 31934)}, {AOM_CDF4(24904, 30671, 31940)},
                {AOM_CDF4(23075, 28460, 31299)}, {AOM_CDF4(14400, 23658, 30417)},
                {AOM_CDF4(13885, 23882, 28325)}, {AOM_CDF4(14746, 22938, 27853)},
                {AOM_CDF4(5461, 16384, 27307)}
            }
        },
        {
            {
                {AOM_CDF4(18274, 24813, 27890)}, {AOM_CDF4(15537, 23149, 27003)},
                {AOM_CDF4(9449, 16740, 21827)},  {AOM_CDF4(6700, 12498, 17261)},
                {AOM_CDF4(4988, 9866, 14198)},   {AOM_CDF4(4236, 8147, 11902)},
                {AOM_CDF4(2867, 5860, 8654)},    {AOM_CDF4(17124, 23171, 26101)},
                {AOM_CDF4(20396, 27477, 30148)}, {AOM_CDF4(16573, 24629, 28492)},
                {AOM_CDF4(12749, 20846, 25674)}, {AOM_CDF4(10233, 17878, 22818)},
                {AOM_CDF4(8525, 15332, 20363)},  {AOM_CDF4(6283, 11632, 16255)},
                {AOM_CDF4(20466, 26511, 29286)}, {AOM_CDF4(23059, 29174, 31191)},
                {AOM_CDF4(19481, 27263, 30241)}, {AOM_CDF4(15458, 23631, 28137)},
                {AOM_CDF4(12416, 20608, 25693)}, {AOM_CDF4(10261, 18011, 23261)},
                {AOM_CDF4(8016, 14655, 19666)}
            },
            {
                {AOM_CDF4(17616, 24586, 28112)}, {AOM_CDF4(15809, 23299, 27155)},
                {AOM_CDF4(10767, 18890, 23793)}, {AOM_CDF4(7727, 14255, 18865)},
                {AOM_CDF4(6129, 11926, 16882)},  {AOM_CDF4(4482, 9704, 14861)},
                {AOM_CDF4(3277, 7452, 11522)},   {AOM_CDF4(22956, 28551, 30730)},
                {AOM_CDF4(22724, 28937, 30961)}, {AOM_CDF4(18467, 26324, 29580)},
                {AOM_CDF4(13234, 20713, 25649)}, {AOM_CDF4(11181, 17592, 22481)},
                {AOM_CDF4(8291, 18358, 24576)},  {AOM_CDF4(7568, 11881, 14984)},
                {AOM_CDF4(24948, 29001, 31147)}, {AOM_CDF4(25674, 30619, 32151)},
                {AOM_CDF4(20841, 26793, 29603)}, {AOM_CDF4(14669, 24356, 28666)},
                {AOM_CDF4(11334, 23593, 28219)}, {AOM_CDF4(8922, 14762, 22873)},
                {AOM_CDF4(8301, 13544, 20535)}
            }
        },
        {
            {
                {AOM_CDF4(17113, 23733, 27081)}, {AOM_CDF4(14139, 21406, 25452)},
                {AOM_CDF4(8552, 15002, 19776)},  {AOM_CDF4(5871, 11120, 15378)},
                {AOM_CDF4(4455, 8616, 12253)},   {AOM_CDF4(3469, 6910, 10386)},
                {AOM_CDF4(2255, 4553, 6782)},    {AOM_CDF4(18224, 24376, 27053)},
                {AOM_CDF4(19290, 26710, 29614)}, {AOM_CDF4(14936, 22991, 27184)},
                {AOM_CDF4(11238, 18951, 23762)}, {AOM_CDF4(8786, 15617, 20588)},
                {AOM_CDF4(7317, 13228, 18003)},  {AOM_CDF4(5101, 9512, 13493)},
                {AOM_CDF4(22639, 28222, 30210)}, {AOM_CDF4(23216, 29331, 31307)},
                {AOM_CDF4(19075, 26762, 29895)}, {AOM_CDF4(15014, 23113, 27457)},
                {AOM_CDF4(11938, 19857, 24752)}, {AOM_CDF4(9942, 17280, 22282)},
                {AOM_CDF4(7167, 13144, 17752)}
            },
            {
                {AOM_CDF4(15820, 22738, 26488)}, {AOM_CDF4(13530, 20885, 25216)},
                {AOM_CDF4(8395, 15530, 20452)},  {AOM_CDF4(6574, 12321, 16380)},
                {AOM_CDF4(5353, 10419, 14568)},  {AOM_CDF4(4613, 8446, 12381)},
                {AOM_CDF4(3440, 7158, 9903)},    {AOM_CDF4(24247, 29051, 31224)},
                {AOM_CDF4(22118, 28058, 30369)}, {AOM_CDF4(16498, 24768, 28389)},
                {AOM_CDF4(12920, 21175, 26137)}, {AOM_CDF4(10730, 18619, 25352)},
                {AOM_CDF4(10187, 16279, 22791)}, {AOM_CDF4(9310, 14631, 22127)},
                {AOM_CDF4(24970, 30558, 32057)}, {AOM_CDF4(24801, 29942, 31698)},
                {AOM_CDF4(22432, 28453, 30855)}, {AOM_CDF4(19054, 25680, 29580)},
                {AOM_CDF4(14392, 23036, 28109)}, {AOM_CDF4(12495, 20947, 26650)},
                {AOM_CDF4(12442, 20326, 26214)}
            }
        },
        {
            {
                {AOM_CDF4(12162, 18785, 22648)}, {AOM_CDF4(12749, 19697, 23806)},
                {AOM_CDF4(8580, 15297, 20346)},  {AOM_CDF4(6169, 11749, 16543)},
                {AOM_CDF4(4836, 9391, 13448)},   {AOM_CDF4(3821, 7711, 11613)},
                {AOM_CDF4(2228, 4601, 7070)},    {AOM_CDF4(16319, 24725, 28280)},
                {AOM_CDF4(15698, 23277, 27168)}, {AOM_CDF4(12726, 20368, 25047)},
                {AOM_CDF4(9912, 17015, 21976)},  {AOM_CDF4(7888, 14220, 19179)},
                {AOM_CDF4(6777, 12284, 17018)},  {AOM_CDF4(4492, 8590, 12252)},
                {AOM_CDF4(23249, 28904, 30947)}, {AOM_CDF4(21050, 27908, 30512)},
                {AOM_CDF4(17440, 25340, 28949)}, {AOM_CDF4(14059, 22018, 26541)},
                {AOM_CDF4(11288, 18903, 23898)}, {AOM_CDF4(9411, 16342, 21428)},
                {AOM_CDF4(6278, 11588, 15944)}
            },
            {
                {AOM_CDF4(13981, 20067, 23226)}, {AOM_CDF4(16922, 23580, 26783)},
                {AOM_CDF4(11005, 19039, 24487)}, {AOM_CDF4(7389, 14218, 19798)},
                {AOM_CDF4(5598, 11505, 17206)},  {AOM_CDF4(6090, 11213, 15659)},
                {AOM_CDF4(3820, 7371, 10119)},   {AOM_CDF4(21082, 26925, 29675)},
                {AOM_CDF4(21262, 28627, 31128)}, {AOM_CDF4(18392, 26454, 30437)},
                {AOM_CDF4(14870, 22910, 27096)}, {AOM_CDF4(12620, 19484, 24908)},
                {AOM_CDF4(9290, 16553, 22802)},  {AOM_CDF4(6668, 14288, 20004)},
                {AOM_CDF4(27704, 31055, 31949)}, {AOM_CDF4(24709, 29978, 31788)},
                {AOM_CDF4(21668, 29264, 31657)}, {AOM_CDF4(18295, 26968, 30074)},
                {AOM_CDF4(16399, 24422, 29313)}, {AOM_CDF4(14347, 23026, 28104)},
                {AOM_CDF4(12370, 19806, 24477)}
            }
        },
        {
            {
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}
            },
            {
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}
            }
        }
    }
};

static const aom_cdf_prob av1_default_coeff_base_multi_cdfs
[TOKEN_CDF_Q_CTXS][TX_SIZES][PLANE_TYPES][SIG_COEF_CONTEXTS] [CDF_SIZE(NUM_BASE_LEVELS + 2) + 1] = {
    {
        {
            {
                {AOM_CDF4(4034, 8930, 12727)},   {AOM_CDF4(18082, 29741, 31877)},
                {AOM_CDF4(12596, 26124, 30493)}, {AOM_CDF4(9446, 21118, 27005)},
                {AOM_CDF4(6308, 15141, 21279)},  {AOM_CDF4(2463, 6357, 9783)},
                {AOM_CDF4(20667, 30546, 31929)}, {AOM_CDF4(13043, 26123, 30134)},
                {AOM_CDF4(8151, 18757, 24778)},  {AOM_CDF4(5255, 12839, 18632)},
                {AOM_CDF4(2820, 7206, 11161)},   {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(15736, 27553, 30604)},
                {AOM_CDF4(11210, 23794, 28787)}, {AOM_CDF4(5947, 13874, 19701)},
                {AOM_CDF4(4215, 9323, 13891)},   {AOM_CDF4(2833, 6462, 10059)},
                {AOM_CDF4(19605, 30393, 31582)}, {AOM_CDF4(13523, 26252, 30248)},
                {AOM_CDF4(8446, 18622, 24512)},  {AOM_CDF4(3818, 10343, 15974)},
                {AOM_CDF4(1481, 4117, 6796)},    {AOM_CDF4(22649, 31302, 32190)},
                {AOM_CDF4(14829, 27127, 30449)}, {AOM_CDF4(8313, 17702, 23304)},
                {AOM_CDF4(3022, 8301, 12786)},   {AOM_CDF4(1536, 4412, 7184)},
                {AOM_CDF4(22354, 29774, 31372)}, {AOM_CDF4(14723, 25472, 29214)},
                {AOM_CDF4(6673, 13745, 18662)},  {AOM_CDF4(2068, 5766, 9322)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)}
            },
            {
                {AOM_CDF4(6302, 16444, 21761)},  {AOM_CDF4(23040, 31538, 32475)},
                {AOM_CDF4(15196, 28452, 31496)}, {AOM_CDF4(10020, 22946, 28514)},
                {AOM_CDF4(6533, 16862, 23501)},  {AOM_CDF4(3538, 9816, 15076)},
                {AOM_CDF4(24444, 31875, 32525)}, {AOM_CDF4(15881, 28924, 31635)},
                {AOM_CDF4(9922, 22873, 28466)},  {AOM_CDF4(6527, 16966, 23691)},
                {AOM_CDF4(4114, 11303, 17220)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(20201, 30770, 32209)},
                {AOM_CDF4(14754, 28071, 31258)}, {AOM_CDF4(8378, 20186, 26517)},
                {AOM_CDF4(5916, 15299, 21978)},  {AOM_CDF4(4268, 11583, 17901)},
                {AOM_CDF4(24361, 32025, 32581)}, {AOM_CDF4(18673, 30105, 31943)},
                {AOM_CDF4(10196, 22244, 27576)}, {AOM_CDF4(5495, 14349, 20417)},
                {AOM_CDF4(2676, 7415, 11498)},   {AOM_CDF4(24678, 31958, 32585)},
                {AOM_CDF4(18629, 29906, 31831)}, {AOM_CDF4(9364, 20724, 26315)},
                {AOM_CDF4(4641, 12318, 18094)},  {AOM_CDF4(2758, 7387, 11579)},
                {AOM_CDF4(25433, 31842, 32469)}, {AOM_CDF4(18795, 29289, 31411)},
                {AOM_CDF4(7644, 17584, 23592)},  {AOM_CDF4(3408, 9014, 15047)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)}
            }
        },
        {
            {
                {AOM_CDF4(4536, 10072, 14001)},  {AOM_CDF4(25459, 31416, 32206)},
                {AOM_CDF4(16605, 28048, 30818)}, {AOM_CDF4(11008, 22857, 27719)},
                {AOM_CDF4(6915, 16268, 22315)},  {AOM_CDF4(2625, 6812, 10537)},
                {AOM_CDF4(24257, 31788, 32499)}, {AOM_CDF4(16880, 29454, 31879)},
                {AOM_CDF4(11958, 25054, 29778)}, {AOM_CDF4(7916, 18718, 25084)},
                {AOM_CDF4(3383, 8777, 13446)},   {AOM_CDF4(22720, 31603, 32393)},
                {AOM_CDF4(14960, 28125, 31335)}, {AOM_CDF4(9731, 22210, 27928)},
                {AOM_CDF4(6304, 15832, 22277)},  {AOM_CDF4(2910, 7818, 12166)},
                {AOM_CDF4(20375, 30627, 32131)}, {AOM_CDF4(13904, 27284, 30887)},
                {AOM_CDF4(9368, 21558, 27144)},  {AOM_CDF4(5937, 14966, 21119)},
                {AOM_CDF4(2667, 7225, 11319)},   {AOM_CDF4(23970, 31470, 32378)},
                {AOM_CDF4(17173, 29734, 32018)}, {AOM_CDF4(12795, 25441, 29965)},
                {AOM_CDF4(8981, 19680, 25893)},  {AOM_CDF4(4728, 11372, 16902)},
                {AOM_CDF4(24287, 31797, 32439)}, {AOM_CDF4(16703, 29145, 31696)},
                {AOM_CDF4(10833, 23554, 28725)}, {AOM_CDF4(6468, 16566, 23057)},
                {AOM_CDF4(2415, 6562, 10278)},   {AOM_CDF4(26610, 32395, 32659)},
                {AOM_CDF4(18590, 30498, 32117)}, {AOM_CDF4(12420, 25756, 29950)},
                {AOM_CDF4(7639, 18746, 24710)},  {AOM_CDF4(3001, 8086, 12347)},
                {AOM_CDF4(25076, 32064, 32580)}, {AOM_CDF4(17946, 30128, 32028)},
                {AOM_CDF4(12024, 24985, 29378)}, {AOM_CDF4(7517, 18390, 24304)},
                {AOM_CDF4(3243, 8781, 13331)},   {AOM_CDF4(8192, 16384, 24576)}
            },
            {
                {AOM_CDF4(6037, 16771, 21957)},  {AOM_CDF4(24774, 31704, 32426)},
                {AOM_CDF4(16830, 28589, 31056)}, {AOM_CDF4(10602, 22828, 27760)},
                {AOM_CDF4(6733, 16829, 23071)},  {AOM_CDF4(3250, 8914, 13556)},
                {AOM_CDF4(25582, 32220, 32668)}, {AOM_CDF4(18659, 30342, 32223)},
                {AOM_CDF4(12546, 26149, 30515)}, {AOM_CDF4(8420, 20451, 26801)},
                {AOM_CDF4(4636, 12420, 18344)},  {AOM_CDF4(27581, 32362, 32639)},
                {AOM_CDF4(18987, 30083, 31978)}, {AOM_CDF4(11327, 24248, 29084)},
                {AOM_CDF4(7264, 17719, 24120)},  {AOM_CDF4(3995, 10768, 16169)},
                {AOM_CDF4(25893, 31831, 32487)}, {AOM_CDF4(16577, 28587, 31379)},
                {AOM_CDF4(10189, 22748, 28182)}, {AOM_CDF4(6832, 17094, 23556)},
                {AOM_CDF4(3708, 10110, 15334)},  {AOM_CDF4(25904, 32282, 32656)},
                {AOM_CDF4(19721, 30792, 32276)}, {AOM_CDF4(12819, 26243, 30411)},
                {AOM_CDF4(8572, 20614, 26891)},  {AOM_CDF4(5364, 14059, 20467)},
                {AOM_CDF4(26580, 32438, 32677)}, {AOM_CDF4(20852, 31225, 32340)},
                {AOM_CDF4(12435, 25700, 29967)}, {AOM_CDF4(8691, 20825, 26976)},
                {AOM_CDF4(4446, 12209, 17269)},  {AOM_CDF4(27350, 32429, 32696)},
                {AOM_CDF4(21372, 30977, 32272)}, {AOM_CDF4(12673, 25270, 29853)},
                {AOM_CDF4(9208, 20925, 26640)},  {AOM_CDF4(5018, 13351, 18732)},
                {AOM_CDF4(27351, 32479, 32713)}, {AOM_CDF4(21398, 31209, 32387)},
                {AOM_CDF4(12162, 25047, 29842)}, {AOM_CDF4(7896, 18691, 25319)},
                {AOM_CDF4(4670, 12882, 18881)},  {AOM_CDF4(8192, 16384, 24576)}
            }
        },
        {
            {
                {AOM_CDF4(5487, 10460, 13708)},  {AOM_CDF4(21597, 28303, 30674)},
                {AOM_CDF4(11037, 21953, 26476)}, {AOM_CDF4(8147, 17962, 22952)},
                {AOM_CDF4(5242, 13061, 18532)},  {AOM_CDF4(1889, 5208, 8182)},
                {AOM_CDF4(26774, 32133, 32590)}, {AOM_CDF4(17844, 29564, 31767)},
                {AOM_CDF4(11690, 24438, 29171)}, {AOM_CDF4(7542, 18215, 24459)},
                {AOM_CDF4(2993, 8050, 12319)},   {AOM_CDF4(28023, 32328, 32591)},
                {AOM_CDF4(18651, 30126, 31954)}, {AOM_CDF4(12164, 25146, 29589)},
                {AOM_CDF4(7762, 18530, 24771)},  {AOM_CDF4(3492, 9183, 13920)},
                {AOM_CDF4(27591, 32008, 32491)}, {AOM_CDF4(17149, 28853, 31510)},
                {AOM_CDF4(11485, 24003, 28860)}, {AOM_CDF4(7697, 18086, 24210)},
                {AOM_CDF4(3075, 7999, 12218)},   {AOM_CDF4(28268, 32482, 32654)},
                {AOM_CDF4(19631, 31051, 32404)}, {AOM_CDF4(13860, 27260, 31020)},
                {AOM_CDF4(9605, 21613, 27594)},  {AOM_CDF4(4876, 12162, 17908)},
                {AOM_CDF4(27248, 32316, 32576)}, {AOM_CDF4(18955, 30457, 32075)},
                {AOM_CDF4(11824, 23997, 28795)}, {AOM_CDF4(7346, 18196, 24647)},
                {AOM_CDF4(3403, 9247, 14111)},   {AOM_CDF4(29711, 32655, 32735)},
                {AOM_CDF4(21169, 31394, 32417)}, {AOM_CDF4(13487, 27198, 30957)},
                {AOM_CDF4(8828, 21683, 27614)},  {AOM_CDF4(4270, 11451, 17038)},
                {AOM_CDF4(28708, 32578, 32731)}, {AOM_CDF4(20120, 31241, 32482)},
                {AOM_CDF4(13692, 27550, 31321)}, {AOM_CDF4(9418, 22514, 28439)},
                {AOM_CDF4(4999, 13283, 19462)},  {AOM_CDF4(8192, 16384, 24576)}
            },
            {
                {AOM_CDF4(5673, 14302, 19711)},  {AOM_CDF4(26251, 30701, 31834)},
                {AOM_CDF4(12782, 23783, 27803)}, {AOM_CDF4(9127, 20657, 25808)},
                {AOM_CDF4(6368, 16208, 21462)},  {AOM_CDF4(2465, 7177, 10822)},
                {AOM_CDF4(29961, 32563, 32719)}, {AOM_CDF4(18318, 29891, 31949)},
                {AOM_CDF4(11361, 24514, 29357)}, {AOM_CDF4(7900, 19603, 25607)},
                {AOM_CDF4(4002, 10590, 15546)},  {AOM_CDF4(29637, 32310, 32595)},
                {AOM_CDF4(18296, 29913, 31809)}, {AOM_CDF4(10144, 21515, 26871)},
                {AOM_CDF4(5358, 14322, 20394)},  {AOM_CDF4(3067, 8362, 13346)},
                {AOM_CDF4(28652, 32470, 32676)}, {AOM_CDF4(17538, 30771, 32209)},
                {AOM_CDF4(13924, 26882, 30494)}, {AOM_CDF4(10496, 22837, 27869)},
                {AOM_CDF4(7236, 16396, 21621)},  {AOM_CDF4(30743, 32687, 32746)},
                {AOM_CDF4(23006, 31676, 32489)}, {AOM_CDF4(14494, 27828, 31120)},
                {AOM_CDF4(10174, 22801, 28352)}, {AOM_CDF4(6242, 15281, 21043)},
                {AOM_CDF4(25817, 32243, 32720)}, {AOM_CDF4(18618, 31367, 32325)},
                {AOM_CDF4(13997, 28318, 31878)}, {AOM_CDF4(12255, 26534, 31383)},
                {AOM_CDF4(9561, 21588, 28450)},  {AOM_CDF4(28188, 32635, 32724)},
                {AOM_CDF4(22060, 32365, 32728)}, {AOM_CDF4(18102, 30690, 32528)},
                {AOM_CDF4(14196, 28864, 31999)}, {AOM_CDF4(12262, 25792, 30865)},
                {AOM_CDF4(24176, 32109, 32628)}, {AOM_CDF4(18280, 29681, 31963)},
                {AOM_CDF4(10205, 23703, 29664)}, {AOM_CDF4(7889, 20025, 27676)},
                {AOM_CDF4(6060, 16743, 23970)},  {AOM_CDF4(8192, 16384, 24576)}
            }
        },
        {
            {
                {AOM_CDF4(5141, 7096, 8260)},    {AOM_CDF4(27186, 29022, 29789)},
                {AOM_CDF4(6668, 12568, 15682)},  {AOM_CDF4(2172, 6181, 8638)},
                {AOM_CDF4(1126, 3379, 4531)},    {AOM_CDF4(443, 1361, 2254)},
                {AOM_CDF4(26083, 31153, 32436)}, {AOM_CDF4(13486, 24603, 28483)},
                {AOM_CDF4(6508, 14840, 19910)},  {AOM_CDF4(3386, 8800, 13286)},
                {AOM_CDF4(1530, 4322, 7054)},    {AOM_CDF4(29639, 32080, 32548)},
                {AOM_CDF4(15897, 27552, 30290)}, {AOM_CDF4(8588, 20047, 25383)},
                {AOM_CDF4(4889, 13339, 19269)},  {AOM_CDF4(2240, 6871, 10498)},
                {AOM_CDF4(28165, 32197, 32517)}, {AOM_CDF4(20735, 30427, 31568)},
                {AOM_CDF4(14325, 24671, 27692)}, {AOM_CDF4(5119, 12554, 17805)},
                {AOM_CDF4(1810, 5441, 8261)},    {AOM_CDF4(31212, 32724, 32748)},
                {AOM_CDF4(23352, 31766, 32545)}, {AOM_CDF4(14669, 27570, 31059)},
                {AOM_CDF4(8492, 20894, 27272)},  {AOM_CDF4(3644, 10194, 15204)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)}
            },
            {
                {AOM_CDF4(2461, 7013, 9371)},    {AOM_CDF4(24749, 29600, 30986)},
                {AOM_CDF4(9466, 19037, 22417)},  {AOM_CDF4(3584, 9280, 14400)},
                {AOM_CDF4(1505, 3929, 5433)},    {AOM_CDF4(677, 1500, 2736)},
                {AOM_CDF4(23987, 30702, 32117)}, {AOM_CDF4(13554, 24571, 29263)},
                {AOM_CDF4(6211, 14556, 21155)},  {AOM_CDF4(3135, 10972, 15625)},
                {AOM_CDF4(2435, 7127, 11427)},   {AOM_CDF4(31300, 32532, 32550)},
                {AOM_CDF4(14757, 30365, 31954)}, {AOM_CDF4(4405, 11612, 18553)},
                {AOM_CDF4(580, 4132, 7322)},     {AOM_CDF4(1695, 10169, 14124)},
                {AOM_CDF4(30008, 32282, 32591)}, {AOM_CDF4(19244, 30108, 31748)},
                {AOM_CDF4(11180, 24158, 29555)}, {AOM_CDF4(5650, 14972, 19209)},
                {AOM_CDF4(2114, 5109, 8456)},    {AOM_CDF4(31856, 32716, 32748)},
                {AOM_CDF4(23012, 31664, 32572)}, {AOM_CDF4(13694, 26656, 30636)},
                {AOM_CDF4(8142, 19508, 26093)},  {AOM_CDF4(4253, 10955, 16724)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)}
            }
        },
        {
            {
                {AOM_CDF4(601, 983, 1311)},      {AOM_CDF4(18725, 23406, 28087)},
                {AOM_CDF4(5461, 8192, 10923)},   {AOM_CDF4(3781, 15124, 21425)},
                {AOM_CDF4(2587, 7761, 12072)},   {AOM_CDF4(106, 458, 810)},
                {AOM_CDF4(22282, 29710, 31894)}, {AOM_CDF4(8508, 20926, 25984)},
                {AOM_CDF4(3726, 12713, 18083)},  {AOM_CDF4(1620, 7112, 10893)},
                {AOM_CDF4(729, 2236, 3495)},     {AOM_CDF4(30163, 32474, 32684)},
                {AOM_CDF4(18304, 30464, 32000)}, {AOM_CDF4(11443, 26526, 29647)},
                {AOM_CDF4(6007, 15292, 21299)},  {AOM_CDF4(2234, 6703, 8937)},
                {AOM_CDF4(30954, 32177, 32571)}, {AOM_CDF4(17363, 29562, 31076)},
                {AOM_CDF4(9686, 22464, 27410)},  {AOM_CDF4(8192, 16384, 21390)},
                {AOM_CDF4(1755, 8046, 11264)},   {AOM_CDF4(31168, 32734, 32748)},
                {AOM_CDF4(22486, 31441, 32471)}, {AOM_CDF4(12833, 25627, 29738)},
                {AOM_CDF4(6980, 17379, 23122)},  {AOM_CDF4(3111, 8887, 13479)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)}
            },
            {
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)}
            }
        }
    },
    {
        {
            {
                {AOM_CDF4(6041, 11854, 15927)},  {AOM_CDF4(20326, 30905, 32251)},
                {AOM_CDF4(14164, 26831, 30725)}, {AOM_CDF4(9760, 20647, 26585)},
                {AOM_CDF4(6416, 14953, 21219)},  {AOM_CDF4(2966, 7151, 10891)},
                {AOM_CDF4(23567, 31374, 32254)}, {AOM_CDF4(14978, 27416, 30946)},
                {AOM_CDF4(9434, 20225, 26254)},  {AOM_CDF4(6658, 14558, 20535)},
                {AOM_CDF4(3916, 8677, 12989)},   {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(18088, 29545, 31587)},
                {AOM_CDF4(13062, 25843, 30073)}, {AOM_CDF4(8940, 16827, 22251)},
                {AOM_CDF4(7654, 13220, 17973)},  {AOM_CDF4(5733, 10316, 14456)},
                {AOM_CDF4(22879, 31388, 32114)}, {AOM_CDF4(15215, 27993, 30955)},
                {AOM_CDF4(9397, 19445, 24978)},  {AOM_CDF4(3442, 9813, 15344)},
                {AOM_CDF4(1368, 3936, 6532)},    {AOM_CDF4(25494, 32033, 32406)},
                {AOM_CDF4(16772, 27963, 30718)}, {AOM_CDF4(9419, 18165, 23260)},
                {AOM_CDF4(2677, 7501, 11797)},   {AOM_CDF4(1516, 4344, 7170)},
                {AOM_CDF4(26556, 31454, 32101)}, {AOM_CDF4(17128, 27035, 30108)},
                {AOM_CDF4(8324, 15344, 20249)},  {AOM_CDF4(1903, 5696, 9469)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)}
            },
            {
                {AOM_CDF4(8455, 19003, 24368)},  {AOM_CDF4(23563, 32021, 32604)},
                {AOM_CDF4(16237, 29446, 31935)}, {AOM_CDF4(10724, 23999, 29358)},
                {AOM_CDF4(6725, 17528, 24416)},  {AOM_CDF4(3927, 10927, 16825)},
                {AOM_CDF4(26313, 32288, 32634)}, {AOM_CDF4(17430, 30095, 32095)},
                {AOM_CDF4(11116, 24606, 29679)}, {AOM_CDF4(7195, 18384, 25269)},
                {AOM_CDF4(4726, 12852, 19315)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(22822, 31648, 32483)},
                {AOM_CDF4(16724, 29633, 31929)}, {AOM_CDF4(10261, 23033, 28725)},
                {AOM_CDF4(7029, 17840, 24528)},  {AOM_CDF4(4867, 13886, 21502)},
                {AOM_CDF4(25298, 31892, 32491)}, {AOM_CDF4(17809, 29330, 31512)},
                {AOM_CDF4(9668, 21329, 26579)},  {AOM_CDF4(4774, 12956, 18976)},
                {AOM_CDF4(2322, 7030, 11540)},   {AOM_CDF4(25472, 31920, 32543)},
                {AOM_CDF4(17957, 29387, 31632)}, {AOM_CDF4(9196, 20593, 26400)},
                {AOM_CDF4(4680, 12705, 19202)},  {AOM_CDF4(2917, 8456, 13436)},
                {AOM_CDF4(26471, 32059, 32574)}, {AOM_CDF4(18458, 29783, 31909)},
                {AOM_CDF4(8400, 19464, 25956)},  {AOM_CDF4(3812, 10973, 17206)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)}
            }
        },
        {
            {
                {AOM_CDF4(6779, 13743, 17678)},  {AOM_CDF4(24806, 31797, 32457)},
                {AOM_CDF4(17616, 29047, 31372)}, {AOM_CDF4(11063, 23175, 28003)},
                {AOM_CDF4(6521, 16110, 22324)},  {AOM_CDF4(2764, 7504, 11654)},
                {AOM_CDF4(25266, 32367, 32637)}, {AOM_CDF4(19054, 30553, 32175)},
                {AOM_CDF4(12139, 25212, 29807)}, {AOM_CDF4(7311, 18162, 24704)},
                {AOM_CDF4(3397, 9164, 14074)},   {AOM_CDF4(25988, 32208, 32522)},
                {AOM_CDF4(16253, 28912, 31526)}, {AOM_CDF4(9151, 21387, 27372)},
                {AOM_CDF4(5688, 14915, 21496)},  {AOM_CDF4(2717, 7627, 12004)},
                {AOM_CDF4(23144, 31855, 32443)}, {AOM_CDF4(16070, 28491, 31325)},
                {AOM_CDF4(8702, 20467, 26517)},  {AOM_CDF4(5243, 13956, 20367)},
                {AOM_CDF4(2621, 7335, 11567)},   {AOM_CDF4(26636, 32340, 32630)},
                {AOM_CDF4(19990, 31050, 32341)}, {AOM_CDF4(13243, 26105, 30315)},
                {AOM_CDF4(8588, 19521, 25918)},  {AOM_CDF4(4717, 11585, 17304)},
                {AOM_CDF4(25844, 32292, 32582)}, {AOM_CDF4(19090, 30635, 32097)},
                {AOM_CDF4(11963, 24546, 28939)}, {AOM_CDF4(6218, 16087, 22354)},
                {AOM_CDF4(2340, 6608, 10426)},   {AOM_CDF4(28046, 32576, 32694)},
                {AOM_CDF4(21178, 31313, 32296)}, {AOM_CDF4(13486, 26184, 29870)},
                {AOM_CDF4(7149, 17871, 23723)},  {AOM_CDF4(2833, 7958, 12259)},
                {AOM_CDF4(27710, 32528, 32686)}, {AOM_CDF4(20674, 31076, 32268)},
                {AOM_CDF4(12413, 24955, 29243)}, {AOM_CDF4(6676, 16927, 23097)},
                {AOM_CDF4(2966, 8333, 12919)},   {AOM_CDF4(8192, 16384, 24576)}
            },
            {
                {AOM_CDF4(8639, 19339, 24429)},  {AOM_CDF4(24404, 31837, 32525)},
                {AOM_CDF4(16997, 29425, 31784)}, {AOM_CDF4(11253, 24234, 29149)},
                {AOM_CDF4(6751, 17394, 24028)},  {AOM_CDF4(3490, 9830, 15191)},
                {AOM_CDF4(26283, 32471, 32714)}, {AOM_CDF4(19599, 31168, 32442)},
                {AOM_CDF4(13146, 26954, 30893)}, {AOM_CDF4(8214, 20588, 26890)},
                {AOM_CDF4(4699, 13081, 19300)},  {AOM_CDF4(28212, 32458, 32669)},
                {AOM_CDF4(18594, 30316, 32100)}, {AOM_CDF4(11219, 24408, 29234)},
                {AOM_CDF4(6865, 17656, 24149)},  {AOM_CDF4(3678, 10362, 16006)},
                {AOM_CDF4(25825, 32136, 32616)}, {AOM_CDF4(17313, 29853, 32021)},
                {AOM_CDF4(11197, 24471, 29472)}, {AOM_CDF4(6947, 17781, 24405)},
                {AOM_CDF4(3768, 10660, 16261)},  {AOM_CDF4(27352, 32500, 32706)},
                {AOM_CDF4(20850, 31468, 32469)}, {AOM_CDF4(14021, 27707, 31133)},
                {AOM_CDF4(8964, 21748, 27838)},  {AOM_CDF4(5437, 14665, 21187)},
                {AOM_CDF4(26304, 32492, 32698)}, {AOM_CDF4(20409, 31380, 32385)},
                {AOM_CDF4(13682, 27222, 30632)}, {AOM_CDF4(8974, 21236, 26685)},
                {AOM_CDF4(4234, 11665, 16934)},  {AOM_CDF4(26273, 32357, 32711)},
                {AOM_CDF4(20672, 31242, 32441)}, {AOM_CDF4(14172, 27254, 30902)},
                {AOM_CDF4(9870, 21898, 27275)},  {AOM_CDF4(5164, 13506, 19270)},
                {AOM_CDF4(26725, 32459, 32728)}, {AOM_CDF4(20991, 31442, 32527)},
                {AOM_CDF4(13071, 26434, 30811)}, {AOM_CDF4(8184, 20090, 26742)},
                {AOM_CDF4(4803, 13255, 19895)},  {AOM_CDF4(8192, 16384, 24576)}
            }
        },
        {
            {
                {AOM_CDF4(7555, 14942, 18501)},  {AOM_CDF4(24410, 31178, 32287)},
                {AOM_CDF4(14394, 26738, 30253)}, {AOM_CDF4(8413, 19554, 25195)},
                {AOM_CDF4(4766, 12924, 18785)},  {AOM_CDF4(2029, 5806, 9207)},
                {AOM_CDF4(26776, 32364, 32663)}, {AOM_CDF4(18732, 29967, 31931)},
                {AOM_CDF4(11005, 23786, 28852)}, {AOM_CDF4(6466, 16909, 23510)},
                {AOM_CDF4(3044, 8638, 13419)},   {AOM_CDF4(29208, 32582, 32704)},
                {AOM_CDF4(20068, 30857, 32208)}, {AOM_CDF4(12003, 25085, 29595)},
                {AOM_CDF4(6947, 17750, 24189)},  {AOM_CDF4(3245, 9103, 14007)},
                {AOM_CDF4(27359, 32465, 32669)}, {AOM_CDF4(19421, 30614, 32174)},
                {AOM_CDF4(11915, 25010, 29579)}, {AOM_CDF4(6950, 17676, 24074)},
                {AOM_CDF4(3007, 8473, 13096)},   {AOM_CDF4(29002, 32676, 32735)},
                {AOM_CDF4(22102, 31849, 32576)}, {AOM_CDF4(14408, 28009, 31405)},
                {AOM_CDF4(9027, 21679, 27931)},  {AOM_CDF4(4694, 12678, 18748)},
                {AOM_CDF4(28216, 32528, 32682)}, {AOM_CDF4(20849, 31264, 32318)},
                {AOM_CDF4(12756, 25815, 29751)}, {AOM_CDF4(7565, 18801, 24923)},
                {AOM_CDF4(3509, 9533, 14477)},   {AOM_CDF4(30133, 32687, 32739)},
                {AOM_CDF4(23063, 31910, 32515)}, {AOM_CDF4(14588, 28051, 31132)},
                {AOM_CDF4(9085, 21649, 27457)},  {AOM_CDF4(4261, 11654, 17264)},
                {AOM_CDF4(29518, 32691, 32748)}, {AOM_CDF4(22451, 31959, 32613)},
                {AOM_CDF4(14864, 28722, 31700)}, {AOM_CDF4(9695, 22964, 28716)},
                {AOM_CDF4(4932, 13358, 19502)},  {AOM_CDF4(8192, 16384, 24576)}
            },
            {
                {AOM_CDF4(6465, 16958, 21688)},  {AOM_CDF4(25199, 31514, 32360)},
                {AOM_CDF4(14774, 27149, 30607)}, {AOM_CDF4(9257, 21438, 26972)},
                {AOM_CDF4(5723, 15183, 21882)},  {AOM_CDF4(3150, 8879, 13731)},
                {AOM_CDF4(26989, 32262, 32682)}, {AOM_CDF4(17396, 29937, 32085)},
                {AOM_CDF4(11387, 24901, 29784)}, {AOM_CDF4(7289, 18821, 25548)},
                {AOM_CDF4(3734, 10577, 16086)},  {AOM_CDF4(29728, 32501, 32695)},
                {AOM_CDF4(17431, 29701, 31903)}, {AOM_CDF4(9921, 22826, 28300)},
                {AOM_CDF4(5896, 15434, 22068)},  {AOM_CDF4(3430, 9646, 14757)},
                {AOM_CDF4(28614, 32511, 32705)}, {AOM_CDF4(19364, 30638, 32263)},
                {AOM_CDF4(13129, 26254, 30402)}, {AOM_CDF4(8754, 20484, 26440)},
                {AOM_CDF4(4378, 11607, 17110)},  {AOM_CDF4(30292, 32671, 32744)},
                {AOM_CDF4(21780, 31603, 32501)}, {AOM_CDF4(14314, 27829, 31291)},
                {AOM_CDF4(9611, 22327, 28263)},  {AOM_CDF4(4890, 13087, 19065)},
                {AOM_CDF4(25862, 32567, 32733)}, {AOM_CDF4(20794, 32050, 32567)},
                {AOM_CDF4(17243, 30625, 32254)}, {AOM_CDF4(13283, 27628, 31474)},
                {AOM_CDF4(9669, 22532, 28918)},  {AOM_CDF4(27435, 32697, 32748)},
                {AOM_CDF4(24922, 32390, 32714)}, {AOM_CDF4(21449, 31504, 32536)},
                {AOM_CDF4(16392, 29729, 31832)}, {AOM_CDF4(11692, 24884, 29076)},
                {AOM_CDF4(24193, 32290, 32735)}, {AOM_CDF4(18909, 31104, 32563)},
                {AOM_CDF4(12236, 26841, 31403)}, {AOM_CDF4(8171, 21840, 29082)},
                {AOM_CDF4(7224, 17280, 25275)},  {AOM_CDF4(8192, 16384, 24576)}
            }
        },
        {
            {
                {AOM_CDF4(3078, 6839, 9890)},    {AOM_CDF4(13837, 20450, 24479)},
                {AOM_CDF4(5914, 14222, 19328)},  {AOM_CDF4(3866, 10267, 14762)},
                {AOM_CDF4(2612, 7208, 11042)},   {AOM_CDF4(1067, 2991, 4776)},
                {AOM_CDF4(25817, 31646, 32529)}, {AOM_CDF4(13708, 26338, 30385)},
                {AOM_CDF4(7328, 18585, 24870)},  {AOM_CDF4(4691, 13080, 19276)},
                {AOM_CDF4(1825, 5253, 8352)},    {AOM_CDF4(29386, 32315, 32624)},
                {AOM_CDF4(17160, 29001, 31360)}, {AOM_CDF4(9602, 21862, 27396)},
                {AOM_CDF4(5915, 15772, 22148)},  {AOM_CDF4(2786, 7779, 12047)},
                {AOM_CDF4(29246, 32450, 32663)}, {AOM_CDF4(18696, 29929, 31818)},
                {AOM_CDF4(10510, 23369, 28560)}, {AOM_CDF4(6229, 16499, 23125)},
                {AOM_CDF4(2608, 7448, 11705)},   {AOM_CDF4(30753, 32710, 32748)},
                {AOM_CDF4(21638, 31487, 32503)}, {AOM_CDF4(12937, 26854, 30870)},
                {AOM_CDF4(8182, 20596, 26970)},  {AOM_CDF4(3637, 10269, 15497)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)}
            },
            {
                {AOM_CDF4(5244, 12150, 16906)},  {AOM_CDF4(20486, 26858, 29701)},
                {AOM_CDF4(7756, 18317, 23735)},  {AOM_CDF4(3452, 9256, 13146)},
                {AOM_CDF4(2020, 5206, 8229)},    {AOM_CDF4(1801, 4993, 7903)},
                {AOM_CDF4(27051, 31858, 32531)}, {AOM_CDF4(15988, 27531, 30619)},
                {AOM_CDF4(9188, 21484, 26719)},  {AOM_CDF4(6273, 17186, 23800)},
                {AOM_CDF4(3108, 9355, 14764)},   {AOM_CDF4(31076, 32520, 32680)},
                {AOM_CDF4(18119, 30037, 31850)}, {AOM_CDF4(10244, 22969, 27472)},
                {AOM_CDF4(4692, 14077, 19273)},  {AOM_CDF4(3694, 11677, 17556)},
                {AOM_CDF4(30060, 32581, 32720)}, {AOM_CDF4(21011, 30775, 32120)},
                {AOM_CDF4(11931, 24820, 29289)}, {AOM_CDF4(7119, 17662, 24356)},
                {AOM_CDF4(3833, 10706, 16304)},  {AOM_CDF4(31954, 32731, 32748)},
                {AOM_CDF4(23913, 31724, 32489)}, {AOM_CDF4(15520, 28060, 31286)},
                {AOM_CDF4(11517, 23008, 28571)}, {AOM_CDF4(6193, 14508, 20629)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)}
            }
        },
        {
            {
                {AOM_CDF4(1035, 2807, 4156)},    {AOM_CDF4(13162, 18138, 20939)},
                {AOM_CDF4(2696, 6633, 8755)},    {AOM_CDF4(1373, 4161, 6853)},
                {AOM_CDF4(1099, 2746, 4716)},    {AOM_CDF4(340, 1021, 1599)},
                {AOM_CDF4(22826, 30419, 32135)}, {AOM_CDF4(10395, 21762, 26942)},
                {AOM_CDF4(4726, 12407, 17361)},  {AOM_CDF4(2447, 7080, 10593)},
                {AOM_CDF4(1227, 3717, 6011)},    {AOM_CDF4(28156, 31424, 31934)},
                {AOM_CDF4(16915, 27754, 30373)}, {AOM_CDF4(9148, 20990, 26431)},
                {AOM_CDF4(5950, 15515, 21148)},  {AOM_CDF4(2492, 7327, 11526)},
                {AOM_CDF4(30602, 32477, 32670)}, {AOM_CDF4(20026, 29955, 31568)},
                {AOM_CDF4(11220, 23628, 28105)}, {AOM_CDF4(6652, 17019, 22973)},
                {AOM_CDF4(3064, 8536, 13043)},   {AOM_CDF4(31769, 32724, 32748)},
                {AOM_CDF4(22230, 30887, 32373)}, {AOM_CDF4(12234, 25079, 29731)},
                {AOM_CDF4(7326, 18816, 25353)},  {AOM_CDF4(3933, 10907, 16616)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)}
            },
            {
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)}
            }
        }
    },
    {
        {
            {
                {AOM_CDF4(8896, 16227, 20630)},  {AOM_CDF4(23629, 31782, 32527)},
                {AOM_CDF4(15173, 27755, 31321)}, {AOM_CDF4(10158, 21233, 27382)},
                {AOM_CDF4(6420, 14857, 21558)},  {AOM_CDF4(3269, 8155, 12646)},
                {AOM_CDF4(24835, 32009, 32496)}, {AOM_CDF4(16509, 28421, 31579)},
                {AOM_CDF4(10957, 21514, 27418)}, {AOM_CDF4(7881, 15930, 22096)},
                {AOM_CDF4(5388, 10960, 15918)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(20745, 30773, 32093)},
                {AOM_CDF4(15200, 27221, 30861)}, {AOM_CDF4(13032, 20873, 25667)},
                {AOM_CDF4(12285, 18663, 23494)}, {AOM_CDF4(11563, 17481, 21489)},
                {AOM_CDF4(26260, 31982, 32320)}, {AOM_CDF4(15397, 28083, 31100)},
                {AOM_CDF4(9742, 19217, 24824)},  {AOM_CDF4(3261, 9629, 15362)},
                {AOM_CDF4(1480, 4322, 7499)},    {AOM_CDF4(27599, 32256, 32460)},
                {AOM_CDF4(16857, 27659, 30774)}, {AOM_CDF4(9551, 18290, 23748)},
                {AOM_CDF4(3052, 8933, 14103)},   {AOM_CDF4(2021, 5910, 9787)},
                {AOM_CDF4(29005, 32015, 32392)}, {AOM_CDF4(17677, 27694, 30863)},
                {AOM_CDF4(9204, 17356, 23219)},  {AOM_CDF4(2403, 7516, 12814)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)}
            },
            {
                {AOM_CDF4(10808, 22056, 26896)}, {AOM_CDF4(25739, 32313, 32676)},
                {AOM_CDF4(17288, 30203, 32221)}, {AOM_CDF4(11359, 24878, 29896)},
                {AOM_CDF4(6949, 17767, 24893)},  {AOM_CDF4(4287, 11796, 18071)},
                {AOM_CDF4(27880, 32521, 32705)}, {AOM_CDF4(19038, 31004, 32414)},
                {AOM_CDF4(12564, 26345, 30768)}, {AOM_CDF4(8269, 19947, 26779)},
                {AOM_CDF4(5674, 14657, 21674)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(25742, 32319, 32671)},
                {AOM_CDF4(19557, 31164, 32454)}, {AOM_CDF4(13381, 26381, 30755)},
                {AOM_CDF4(10101, 21466, 26722)}, {AOM_CDF4(9209, 19650, 26825)},
                {AOM_CDF4(27107, 31917, 32432)}, {AOM_CDF4(18056, 28893, 31203)},
                {AOM_CDF4(10200, 21434, 26764)}, {AOM_CDF4(4660, 12913, 19502)},
                {AOM_CDF4(2368, 6930, 12504)},   {AOM_CDF4(26960, 32158, 32613)},
                {AOM_CDF4(18628, 30005, 32031)}, {AOM_CDF4(10233, 22442, 28232)},
                {AOM_CDF4(5471, 14630, 21516)},  {AOM_CDF4(3235, 10767, 17109)},
                {AOM_CDF4(27696, 32440, 32692)}, {AOM_CDF4(20032, 31167, 32438)},
                {AOM_CDF4(8700, 21341, 28442)},  {AOM_CDF4(5662, 14831, 21795)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)}
            }
        },
        {
            {
                {AOM_CDF4(9704, 17294, 21132)},  {AOM_CDF4(26762, 32278, 32633)},
                {AOM_CDF4(18382, 29620, 31819)}, {AOM_CDF4(10891, 23475, 28723)},
                {AOM_CDF4(6358, 16583, 23309)},  {AOM_CDF4(3248, 9118, 14141)},
                {AOM_CDF4(27204, 32573, 32699)}, {AOM_CDF4(19818, 30824, 32329)},
                {AOM_CDF4(11772, 25120, 30041)}, {AOM_CDF4(6995, 18033, 25039)},
                {AOM_CDF4(3752, 10442, 16098)},  {AOM_CDF4(27222, 32256, 32559)},
                {AOM_CDF4(15356, 28399, 31475)}, {AOM_CDF4(8821, 20635, 27057)},
                {AOM_CDF4(5511, 14404, 21239)},  {AOM_CDF4(2935, 8222, 13051)},
                {AOM_CDF4(24875, 32120, 32529)}, {AOM_CDF4(15233, 28265, 31445)},
                {AOM_CDF4(8605, 20570, 26932)},  {AOM_CDF4(5431, 14413, 21196)},
                {AOM_CDF4(2994, 8341, 13223)},   {AOM_CDF4(28201, 32604, 32700)},
                {AOM_CDF4(21041, 31446, 32456)}, {AOM_CDF4(13221, 26213, 30475)},
                {AOM_CDF4(8255, 19385, 26037)},  {AOM_CDF4(4930, 12585, 18830)},
                {AOM_CDF4(28768, 32448, 32627)}, {AOM_CDF4(19705, 30561, 32021)},
                {AOM_CDF4(11572, 23589, 28220)}, {AOM_CDF4(5532, 15034, 21446)},
                {AOM_CDF4(2460, 7150, 11456)},   {AOM_CDF4(29874, 32619, 32699)},
                {AOM_CDF4(21621, 31071, 32201)}, {AOM_CDF4(12511, 24747, 28992)},
                {AOM_CDF4(6281, 16395, 22748)},  {AOM_CDF4(3246, 9278, 14497)},
                {AOM_CDF4(29715, 32625, 32712)}, {AOM_CDF4(20958, 31011, 32283)},
                {AOM_CDF4(11233, 23671, 28806)}, {AOM_CDF4(6012, 16128, 22868)},
                {AOM_CDF4(3427, 9851, 15414)},   {AOM_CDF4(8192, 16384, 24576)}
            },
            {
                {AOM_CDF4(11016, 22111, 26794)}, {AOM_CDF4(25946, 32357, 32677)},
                {AOM_CDF4(17890, 30452, 32252)}, {AOM_CDF4(11678, 25142, 29816)},
                {AOM_CDF4(6720, 17534, 24584)},  {AOM_CDF4(4230, 11665, 17820)},
                {AOM_CDF4(28400, 32623, 32747)}, {AOM_CDF4(21164, 31668, 32575)},
                {AOM_CDF4(13572, 27388, 31182)}, {AOM_CDF4(8234, 20750, 27358)},
                {AOM_CDF4(5065, 14055, 20897)},  {AOM_CDF4(28981, 32547, 32705)},
                {AOM_CDF4(18681, 30543, 32239)}, {AOM_CDF4(10919, 24075, 29286)},
                {AOM_CDF4(6431, 17199, 24077)},  {AOM_CDF4(3819, 10464, 16618)},
                {AOM_CDF4(26870, 32467, 32693)}, {AOM_CDF4(19041, 30831, 32347)},
                {AOM_CDF4(11794, 25211, 30016)}, {AOM_CDF4(6888, 18019, 24970)},
                {AOM_CDF4(4370, 12363, 18992)},  {AOM_CDF4(29578, 32670, 32744)},
                {AOM_CDF4(23159, 32007, 32613)}, {AOM_CDF4(15315, 28669, 31676)},
                {AOM_CDF4(9298, 22607, 28782)},  {AOM_CDF4(6144, 15913, 22968)},
                {AOM_CDF4(28110, 32499, 32669)}, {AOM_CDF4(21574, 30937, 32015)},
                {AOM_CDF4(12759, 24818, 28727)}, {AOM_CDF4(6545, 16761, 23042)},
                {AOM_CDF4(3649, 10597, 16833)},  {AOM_CDF4(28163, 32552, 32728)},
                {AOM_CDF4(22101, 31469, 32464)}, {AOM_CDF4(13160, 25472, 30143)},
                {AOM_CDF4(7303, 18684, 25468)},  {AOM_CDF4(5241, 13975, 20955)},
                {AOM_CDF4(28400, 32631, 32744)}, {AOM_CDF4(22104, 31793, 32603)},
                {AOM_CDF4(13557, 26571, 30846)}, {AOM_CDF4(7749, 19861, 26675)},
                {AOM_CDF4(4873, 14030, 21234)},  {AOM_CDF4(8192, 16384, 24576)}
            }
        },
        {
            {
                {AOM_CDF4(9800, 17635, 21073)},  {AOM_CDF4(26153, 31885, 32527)},
                {AOM_CDF4(15038, 27852, 31006)}, {AOM_CDF4(8718, 20564, 26486)},
                {AOM_CDF4(5128, 14076, 20514)},  {AOM_CDF4(2636, 7566, 11925)},
                {AOM_CDF4(27551, 32504, 32701)}, {AOM_CDF4(18310, 30054, 32100)},
                {AOM_CDF4(10211, 23420, 29082)}, {AOM_CDF4(6222, 16876, 23916)},
                {AOM_CDF4(3462, 9954, 15498)},   {AOM_CDF4(29991, 32633, 32721)},
                {AOM_CDF4(19883, 30751, 32201)}, {AOM_CDF4(11141, 24184, 29285)},
                {AOM_CDF4(6420, 16940, 23774)},  {AOM_CDF4(3392, 9753, 15118)},
                {AOM_CDF4(28465, 32616, 32712)}, {AOM_CDF4(19850, 30702, 32244)},
                {AOM_CDF4(10983, 24024, 29223)}, {AOM_CDF4(6294, 16770, 23582)},
                {AOM_CDF4(3244, 9283, 14509)},   {AOM_CDF4(30023, 32717, 32748)},
                {AOM_CDF4(22940, 32032, 32626)}, {AOM_CDF4(14282, 27928, 31473)},
                {AOM_CDF4(8562, 21327, 27914)},  {AOM_CDF4(4846, 13393, 19919)},
                {AOM_CDF4(29981, 32590, 32695)}, {AOM_CDF4(20465, 30963, 32166)},
                {AOM_CDF4(11479, 23579, 28195)}, {AOM_CDF4(5916, 15648, 22073)},
                {AOM_CDF4(3031, 8605, 13398)},   {AOM_CDF4(31146, 32691, 32739)},
                {AOM_CDF4(23106, 31724, 32444)}, {AOM_CDF4(13783, 26738, 30439)},
                {AOM_CDF4(7852, 19468, 25807)},  {AOM_CDF4(3860, 11124, 16853)},
                {AOM_CDF4(31014, 32724, 32748)}, {AOM_CDF4(23629, 32109, 32628)},
                {AOM_CDF4(14747, 28115, 31403)}, {AOM_CDF4(8545, 21242, 27478)},
                {AOM_CDF4(4574, 12781, 19067)},  {AOM_CDF4(8192, 16384, 24576)}
            },
            {
                {AOM_CDF4(9185, 19694, 24688)},  {AOM_CDF4(26081, 31985, 32621)},
                {AOM_CDF4(16015, 29000, 31787)}, {AOM_CDF4(10542, 23690, 29206)},
                {AOM_CDF4(6732, 17945, 24677)},  {AOM_CDF4(3916, 11039, 16722)},
                {AOM_CDF4(28224, 32566, 32744)}, {AOM_CDF4(19100, 31138, 32485)},
                {AOM_CDF4(12528, 26620, 30879)}, {AOM_CDF4(7741, 20277, 26885)},
                {AOM_CDF4(4566, 12845, 18990)},  {AOM_CDF4(29933, 32593, 32718)},
                {AOM_CDF4(17670, 30333, 32155)}, {AOM_CDF4(10385, 23600, 28909)},
                {AOM_CDF4(6243, 16236, 22407)},  {AOM_CDF4(3976, 10389, 16017)},
                {AOM_CDF4(28377, 32561, 32738)}, {AOM_CDF4(19366, 31175, 32482)},
                {AOM_CDF4(13327, 27175, 31094)}, {AOM_CDF4(8258, 20769, 27143)},
                {AOM_CDF4(4703, 13198, 19527)},  {AOM_CDF4(31086, 32706, 32748)},
                {AOM_CDF4(22853, 31902, 32583)}, {AOM_CDF4(14759, 28186, 31419)},
                {AOM_CDF4(9284, 22382, 28348)},  {AOM_CDF4(5585, 15192, 21868)},
                {AOM_CDF4(28291, 32652, 32746)}, {AOM_CDF4(19849, 32107, 32571)},
                {AOM_CDF4(14834, 26818, 29214)}, {AOM_CDF4(10306, 22594, 28672)},
                {AOM_CDF4(6615, 17384, 23384)},  {AOM_CDF4(28947, 32604, 32745)},
                {AOM_CDF4(25625, 32289, 32646)}, {AOM_CDF4(18758, 28672, 31403)},
                {AOM_CDF4(10017, 23430, 28523)}, {AOM_CDF4(6862, 15269, 22131)},
                {AOM_CDF4(23933, 32509, 32739)}, {AOM_CDF4(19927, 31495, 32631)},
                {AOM_CDF4(11903, 26023, 30621)}, {AOM_CDF4(7026, 20094, 27252)},
                {AOM_CDF4(5998, 18106, 24437)},  {AOM_CDF4(8192, 16384, 24576)}
            }
        },
        {
            {
                {AOM_CDF4(4456, 11274, 15533)},  {AOM_CDF4(21219, 29079, 31616)},
                {AOM_CDF4(11173, 23774, 28567)}, {AOM_CDF4(7282, 18293, 24263)},
                {AOM_CDF4(4890, 13286, 19115)},  {AOM_CDF4(1890, 5508, 8659)},
                {AOM_CDF4(26651, 32136, 32647)}, {AOM_CDF4(14630, 28254, 31455)},
                {AOM_CDF4(8716, 21287, 27395)},  {AOM_CDF4(5615, 15331, 22008)},
                {AOM_CDF4(2675, 7700, 12150)},   {AOM_CDF4(29954, 32526, 32690)},
                {AOM_CDF4(16126, 28982, 31633)}, {AOM_CDF4(9030, 21361, 27352)},
                {AOM_CDF4(5411, 14793, 21271)},  {AOM_CDF4(2943, 8422, 13163)},
                {AOM_CDF4(29539, 32601, 32730)}, {AOM_CDF4(18125, 30385, 32201)},
                {AOM_CDF4(10422, 24090, 29468)}, {AOM_CDF4(6468, 17487, 24438)},
                {AOM_CDF4(2970, 8653, 13531)},   {AOM_CDF4(30912, 32715, 32748)},
                {AOM_CDF4(20666, 31373, 32497)}, {AOM_CDF4(12509, 26640, 30917)},
                {AOM_CDF4(8058, 20629, 27290)},  {AOM_CDF4(4231, 12006, 18052)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)}
            },
            {
                {AOM_CDF4(10202, 20633, 25484)}, {AOM_CDF4(27336, 31445, 32352)},
                {AOM_CDF4(12420, 24384, 28552)}, {AOM_CDF4(7648, 18115, 23856)},
                {AOM_CDF4(5662, 14341, 19902)},  {AOM_CDF4(3611, 10328, 15390)},
                {AOM_CDF4(30945, 32616, 32736)}, {AOM_CDF4(18682, 30505, 32253)},
                {AOM_CDF4(11513, 25336, 30203)}, {AOM_CDF4(7449, 19452, 26148)},
                {AOM_CDF4(4482, 13051, 18886)},  {AOM_CDF4(32022, 32690, 32747)},
                {AOM_CDF4(18578, 30501, 32146)}, {AOM_CDF4(11249, 23368, 28631)},
                {AOM_CDF4(5645, 16958, 22158)},  {AOM_CDF4(5009, 11444, 16637)},
                {AOM_CDF4(31357, 32710, 32748)}, {AOM_CDF4(21552, 31494, 32504)},
                {AOM_CDF4(13891, 27677, 31340)}, {AOM_CDF4(9051, 22098, 28172)},
                {AOM_CDF4(5190, 13377, 19486)},  {AOM_CDF4(32364, 32740, 32748)},
                {AOM_CDF4(24839, 31907, 32551)}, {AOM_CDF4(17160, 28779, 31696)},
                {AOM_CDF4(12452, 24137, 29602)}, {AOM_CDF4(6165, 15389, 22477)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)}
            }
        },
        {
            {
                {AOM_CDF4(2575, 7281, 11077)},   {AOM_CDF4(14002, 20866, 25402)},
                {AOM_CDF4(6343, 15056, 19658)},  {AOM_CDF4(4474, 11858, 17041)},
                {AOM_CDF4(2865, 8299, 12534)},   {AOM_CDF4(1344, 3949, 6391)},
                {AOM_CDF4(24720, 31239, 32459)}, {AOM_CDF4(12585, 25356, 29968)},
                {AOM_CDF4(7181, 18246, 24444)},  {AOM_CDF4(5025, 13667, 19885)},
                {AOM_CDF4(2521, 7304, 11605)},   {AOM_CDF4(29908, 32252, 32584)},
                {AOM_CDF4(17421, 29156, 31575)}, {AOM_CDF4(9889, 22188, 27782)},
                {AOM_CDF4(5878, 15647, 22123)},  {AOM_CDF4(2814, 8665, 13323)},
                {AOM_CDF4(30183, 32568, 32713)}, {AOM_CDF4(18528, 30195, 32049)},
                {AOM_CDF4(10982, 24606, 29657)}, {AOM_CDF4(6957, 18165, 25231)},
                {AOM_CDF4(3508, 10118, 15468)},  {AOM_CDF4(31761, 32736, 32748)},
                {AOM_CDF4(21041, 31328, 32546)}, {AOM_CDF4(12568, 26732, 31166)},
                {AOM_CDF4(8052, 20720, 27733)},  {AOM_CDF4(4336, 12192, 18396)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)}
            },
            {
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)}
            }
        }
    },
    {
        {
            {
                {AOM_CDF4(7062, 16472, 22319)},  {AOM_CDF4(24538, 32261, 32674)},
                {AOM_CDF4(13675, 28041, 31779)}, {AOM_CDF4(8590, 20674, 27631)},
                {AOM_CDF4(5685, 14675, 22013)},  {AOM_CDF4(3655, 9898, 15731)},
                {AOM_CDF4(26493, 32418, 32658)}, {AOM_CDF4(16376, 29342, 32090)},
                {AOM_CDF4(10594, 22649, 28970)}, {AOM_CDF4(8176, 17170, 24303)},
                {AOM_CDF4(5605, 12694, 19139)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(23888, 31902, 32542)},
                {AOM_CDF4(18612, 29687, 31987)}, {AOM_CDF4(16245, 24852, 29249)},
                {AOM_CDF4(15765, 22608, 27559)}, {AOM_CDF4(19895, 24699, 27510)},
                {AOM_CDF4(28401, 32212, 32457)}, {AOM_CDF4(15274, 27825, 30980)},
                {AOM_CDF4(9364, 18128, 24332)},  {AOM_CDF4(2283, 8193, 15082)},
                {AOM_CDF4(1228, 3972, 7881)},    {AOM_CDF4(29455, 32469, 32620)},
                {AOM_CDF4(17981, 28245, 31388)}, {AOM_CDF4(10921, 20098, 26240)},
                {AOM_CDF4(3743, 11829, 18657)},  {AOM_CDF4(2374, 9593, 15715)},
                {AOM_CDF4(31068, 32466, 32635)}, {AOM_CDF4(20321, 29572, 31971)},
                {AOM_CDF4(10771, 20255, 27119)}, {AOM_CDF4(2795, 10410, 17361)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)}
            },
            {
                {AOM_CDF4(9320, 22102, 27840)},  {AOM_CDF4(27057, 32464, 32724)},
                {AOM_CDF4(16331, 30268, 32309)}, {AOM_CDF4(10319, 23935, 29720)},
                {AOM_CDF4(6189, 16448, 24106)},  {AOM_CDF4(3589, 10884, 18808)},
                {AOM_CDF4(29026, 32624, 32748)}, {AOM_CDF4(19226, 31507, 32587)},
                {AOM_CDF4(12692, 26921, 31203)}, {AOM_CDF4(7049, 19532, 27635)},
                {AOM_CDF4(7727, 15669, 23252)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(28056, 32625, 32748)},
                {AOM_CDF4(22383, 32075, 32669)}, {AOM_CDF4(15417, 27098, 31749)},
                {AOM_CDF4(18127, 26493, 27190)}, {AOM_CDF4(5461, 16384, 21845)},
                {AOM_CDF4(27982, 32091, 32584)}, {AOM_CDF4(19045, 29868, 31972)},
                {AOM_CDF4(10397, 22266, 27932)}, {AOM_CDF4(5990, 13697, 21500)},
                {AOM_CDF4(1792, 6912, 15104)},   {AOM_CDF4(28198, 32501, 32718)},
                {AOM_CDF4(21534, 31521, 32569)}, {AOM_CDF4(11109, 25217, 30017)},
                {AOM_CDF4(5671, 15124, 26151)},  {AOM_CDF4(4681, 14043, 18725)},
                {AOM_CDF4(28688, 32580, 32741)}, {AOM_CDF4(22576, 32079, 32661)},
                {AOM_CDF4(10627, 22141, 28340)}, {AOM_CDF4(9362, 14043, 28087)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)}
            }
        },
        {
            {
                {AOM_CDF4(7754, 16948, 22142)},  {AOM_CDF4(25670, 32330, 32691)},
                {AOM_CDF4(15663, 29225, 31994)}, {AOM_CDF4(9878, 23288, 29158)},
                {AOM_CDF4(6419, 17088, 24336)},  {AOM_CDF4(3859, 11003, 17039)},
                {AOM_CDF4(27562, 32595, 32725)}, {AOM_CDF4(17575, 30588, 32399)},
                {AOM_CDF4(10819, 24838, 30309)}, {AOM_CDF4(7124, 18686, 25916)},
                {AOM_CDF4(4479, 12688, 19340)},  {AOM_CDF4(28385, 32476, 32673)},
                {AOM_CDF4(15306, 29005, 31938)}, {AOM_CDF4(8937, 21615, 28322)},
                {AOM_CDF4(5982, 15603, 22786)},  {AOM_CDF4(3620, 10267, 16136)},
                {AOM_CDF4(27280, 32464, 32667)}, {AOM_CDF4(15607, 29160, 32004)},
                {AOM_CDF4(9091, 22135, 28740)},  {AOM_CDF4(6232, 16632, 24020)},
                {AOM_CDF4(4047, 11377, 17672)},  {AOM_CDF4(29220, 32630, 32718)},
                {AOM_CDF4(19650, 31220, 32462)}, {AOM_CDF4(13050, 26312, 30827)},
                {AOM_CDF4(9228, 20870, 27468)},  {AOM_CDF4(6146, 15149, 21971)},
                {AOM_CDF4(30169, 32481, 32623)}, {AOM_CDF4(17212, 29311, 31554)},
                {AOM_CDF4(9911, 21311, 26882)},  {AOM_CDF4(4487, 13314, 20372)},
                {AOM_CDF4(2570, 7772, 12889)},   {AOM_CDF4(30924, 32613, 32708)},
                {AOM_CDF4(19490, 30206, 32107)}, {AOM_CDF4(11232, 23998, 29276)},
                {AOM_CDF4(6769, 17955, 25035)},  {AOM_CDF4(4398, 12623, 19214)},
                {AOM_CDF4(30609, 32627, 32722)}, {AOM_CDF4(19370, 30582, 32287)},
                {AOM_CDF4(10457, 23619, 29409)}, {AOM_CDF4(6443, 17637, 24834)},
                {AOM_CDF4(4645, 13236, 20106)},  {AOM_CDF4(8192, 16384, 24576)}
            },
            {
                {AOM_CDF4(8626, 20271, 26216)},  {AOM_CDF4(26707, 32406, 32711)},
                {AOM_CDF4(16999, 30329, 32286)}, {AOM_CDF4(11445, 25123, 30286)},
                {AOM_CDF4(6411, 18828, 25601)},  {AOM_CDF4(6801, 12458, 20248)},
                {AOM_CDF4(29918, 32682, 32748)}, {AOM_CDF4(20649, 31739, 32618)},
                {AOM_CDF4(12879, 27773, 31581)}, {AOM_CDF4(7896, 21751, 28244)},
                {AOM_CDF4(5260, 14870, 23698)},  {AOM_CDF4(29252, 32593, 32731)},
                {AOM_CDF4(17072, 30460, 32294)}, {AOM_CDF4(10653, 24143, 29365)},
                {AOM_CDF4(6536, 17490, 23983)},  {AOM_CDF4(4929, 13170, 20085)},
                {AOM_CDF4(28137, 32518, 32715)}, {AOM_CDF4(18171, 30784, 32407)},
                {AOM_CDF4(11437, 25436, 30459)}, {AOM_CDF4(7252, 18534, 26176)},
                {AOM_CDF4(4126, 13353, 20978)},  {AOM_CDF4(31162, 32726, 32748)},
                {AOM_CDF4(23017, 32222, 32701)}, {AOM_CDF4(15629, 29233, 32046)},
                {AOM_CDF4(9387, 22621, 29480)},  {AOM_CDF4(6922, 17616, 25010)},
                {AOM_CDF4(28838, 32265, 32614)}, {AOM_CDF4(19701, 30206, 31920)},
                {AOM_CDF4(11214, 22410, 27933)}, {AOM_CDF4(5320, 14177, 23034)},
                {AOM_CDF4(5049, 12881, 17827)},  {AOM_CDF4(27484, 32471, 32734)},
                {AOM_CDF4(21076, 31526, 32561)}, {AOM_CDF4(12707, 26303, 31211)},
                {AOM_CDF4(8169, 21722, 28219)},  {AOM_CDF4(6045, 19406, 27042)},
                {AOM_CDF4(27753, 32572, 32745)}, {AOM_CDF4(20832, 31878, 32653)},
                {AOM_CDF4(13250, 27356, 31674)}, {AOM_CDF4(7718, 21508, 29858)},
                {AOM_CDF4(7209, 18350, 25559)},  {AOM_CDF4(8192, 16384, 24576)}
            }
        },
        {
            {
                {AOM_CDF4(7876, 16901, 21741)},  {AOM_CDF4(24001, 31898, 32625)},
                {AOM_CDF4(14529, 27959, 31451)}, {AOM_CDF4(8273, 20818, 27258)},
                {AOM_CDF4(5278, 14673, 21510)},  {AOM_CDF4(2983, 8843, 14039)},
                {AOM_CDF4(28016, 32574, 32732)}, {AOM_CDF4(17471, 30306, 32301)},
                {AOM_CDF4(10224, 24063, 29728)}, {AOM_CDF4(6602, 17954, 25052)},
                {AOM_CDF4(4002, 11585, 17759)},  {AOM_CDF4(30190, 32634, 32739)},
                {AOM_CDF4(17497, 30282, 32270)}, {AOM_CDF4(10229, 23729, 29538)},
                {AOM_CDF4(6344, 17211, 24440)},  {AOM_CDF4(3849, 11189, 17108)},
                {AOM_CDF4(28570, 32583, 32726)}, {AOM_CDF4(17521, 30161, 32238)},
                {AOM_CDF4(10153, 23565, 29378)}, {AOM_CDF4(6455, 17341, 24443)},
                {AOM_CDF4(3907, 11042, 17024)},  {AOM_CDF4(30689, 32715, 32748)},
                {AOM_CDF4(21546, 31840, 32610)}, {AOM_CDF4(13547, 27581, 31459)},
                {AOM_CDF4(8912, 21757, 28309)},  {AOM_CDF4(5548, 15080, 22046)},
                {AOM_CDF4(30783, 32540, 32685)}, {AOM_CDF4(17540, 29528, 31668)},
                {AOM_CDF4(10160, 21468, 26783)}, {AOM_CDF4(4724, 13393, 20054)},
                {AOM_CDF4(2702, 8174, 13102)},   {AOM_CDF4(31648, 32686, 32742)},
                {AOM_CDF4(20954, 31094, 32337)}, {AOM_CDF4(12420, 25698, 30179)},
                {AOM_CDF4(7304, 19320, 26248)},  {AOM_CDF4(4366, 12261, 18864)},
                {AOM_CDF4(31581, 32723, 32748)}, {AOM_CDF4(21373, 31586, 32525)},
                {AOM_CDF4(12744, 26625, 30885)}, {AOM_CDF4(7431, 20322, 26950)},
                {AOM_CDF4(4692, 13323, 20111)},  {AOM_CDF4(8192, 16384, 24576)}
            },
            {
                {AOM_CDF4(7833, 18369, 24095)},  {AOM_CDF4(26650, 32273, 32702)},
                {AOM_CDF4(16371, 29961, 32191)}, {AOM_CDF4(11055, 24082, 29629)},
                {AOM_CDF4(6892, 18644, 25400)},  {AOM_CDF4(5006, 13057, 19240)},
                {AOM_CDF4(29834, 32666, 32748)}, {AOM_CDF4(19577, 31335, 32570)},
                {AOM_CDF4(12253, 26509, 31122)}, {AOM_CDF4(7991, 20772, 27711)},
                {AOM_CDF4(5677, 15910, 23059)},  {AOM_CDF4(30109, 32532, 32720)},
                {AOM_CDF4(16747, 30166, 32252)}, {AOM_CDF4(10134, 23542, 29184)},
                {AOM_CDF4(5791, 16176, 23556)},  {AOM_CDF4(4362, 10414, 17284)},
                {AOM_CDF4(29492, 32626, 32748)}, {AOM_CDF4(19894, 31402, 32525)},
                {AOM_CDF4(12942, 27071, 30869)}, {AOM_CDF4(8346, 21216, 27405)},
                {AOM_CDF4(6572, 17087, 23859)},  {AOM_CDF4(32035, 32735, 32748)},
                {AOM_CDF4(22957, 31838, 32618)}, {AOM_CDF4(14724, 28572, 31772)},
                {AOM_CDF4(10364, 23999, 29553)}, {AOM_CDF4(7004, 18433, 25655)},
                {AOM_CDF4(27528, 32277, 32681)}, {AOM_CDF4(16959, 31171, 32096)},
                {AOM_CDF4(10486, 23593, 27962)}, {AOM_CDF4(8192, 16384, 23211)},
                {AOM_CDF4(8937, 17873, 20852)},  {AOM_CDF4(27715, 32002, 32615)},
                {AOM_CDF4(15073, 29491, 31676)}, {AOM_CDF4(11264, 24576, 28672)},
                {AOM_CDF4(2341, 18725, 23406)},  {AOM_CDF4(7282, 18204, 25486)},
                {AOM_CDF4(28547, 32213, 32657)}, {AOM_CDF4(20788, 29773, 32239)},
                {AOM_CDF4(6780, 21469, 30508)},  {AOM_CDF4(5958, 14895, 23831)},
                {AOM_CDF4(16384, 21845, 27307)}, {AOM_CDF4(8192, 16384, 24576)}
            }
        },
        {
            {
                {AOM_CDF4(5992, 14304, 19765)},  {AOM_CDF4(22612, 31238, 32456)},
                {AOM_CDF4(13456, 27162, 31087)}, {AOM_CDF4(8001, 20062, 26504)},
                {AOM_CDF4(5168, 14105, 20764)},  {AOM_CDF4(2632, 7771, 12385)},
                {AOM_CDF4(27034, 32344, 32709)}, {AOM_CDF4(15850, 29415, 31997)},
                {AOM_CDF4(9494, 22776, 28841)},  {AOM_CDF4(6151, 16830, 23969)},
                {AOM_CDF4(3461, 10039, 15722)},  {AOM_CDF4(30134, 32569, 32731)},
                {AOM_CDF4(15638, 29422, 31945)}, {AOM_CDF4(9150, 21865, 28218)},
                {AOM_CDF4(5647, 15719, 22676)},  {AOM_CDF4(3402, 9772, 15477)},
                {AOM_CDF4(28530, 32586, 32735)}, {AOM_CDF4(17139, 30298, 32292)},
                {AOM_CDF4(10200, 24039, 29685)}, {AOM_CDF4(6419, 17674, 24786)},
                {AOM_CDF4(3544, 10225, 15824)},  {AOM_CDF4(31333, 32726, 32748)},
                {AOM_CDF4(20618, 31487, 32544)}, {AOM_CDF4(12901, 27217, 31232)},
                {AOM_CDF4(8624, 21734, 28171)},  {AOM_CDF4(5104, 14191, 20748)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)}
            },
            {
                {AOM_CDF4(11206, 21090, 26561)}, {AOM_CDF4(28759, 32279, 32671)},
                {AOM_CDF4(14171, 27952, 31569)}, {AOM_CDF4(9743, 22907, 29141)},
                {AOM_CDF4(6871, 17886, 24868)},  {AOM_CDF4(4960, 13152, 19315)},
                {AOM_CDF4(31077, 32661, 32748)}, {AOM_CDF4(19400, 31195, 32515)},
                {AOM_CDF4(12752, 26858, 31040)}, {AOM_CDF4(8370, 22098, 28591)},
                {AOM_CDF4(5457, 15373, 22298)},  {AOM_CDF4(31697, 32706, 32748)},
                {AOM_CDF4(17860, 30657, 32333)}, {AOM_CDF4(12510, 24812, 29261)},
                {AOM_CDF4(6180, 19124, 24722)},  {AOM_CDF4(5041, 13548, 17959)},
                {AOM_CDF4(31552, 32716, 32748)}, {AOM_CDF4(21908, 31769, 32623)},
                {AOM_CDF4(14470, 28201, 31565)}, {AOM_CDF4(9493, 22982, 28608)},
                {AOM_CDF4(6858, 17240, 24137)},  {AOM_CDF4(32543, 32752, 32756)},
                {AOM_CDF4(24286, 32097, 32666)}, {AOM_CDF4(15958, 29217, 32024)},
                {AOM_CDF4(10207, 24234, 29958)}, {AOM_CDF4(6929, 18305, 25652)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)}
            }
        },
        {
            {
                {AOM_CDF4(4137, 10847, 15682)},  {AOM_CDF4(17824, 27001, 30058)},
                {AOM_CDF4(10204, 22796, 28291)}, {AOM_CDF4(6076, 15935, 22125)},
                {AOM_CDF4(3852, 10937, 16816)},  {AOM_CDF4(2252, 6324, 10131)},
                {AOM_CDF4(25840, 32016, 32662)}, {AOM_CDF4(15109, 28268, 31531)},
                {AOM_CDF4(9385, 22231, 28340)},  {AOM_CDF4(6082, 16672, 23479)},
                {AOM_CDF4(3318, 9427, 14681)},   {AOM_CDF4(30594, 32574, 32718)},
                {AOM_CDF4(16836, 29552, 31859)}, {AOM_CDF4(9556, 22542, 28356)},
                {AOM_CDF4(6305, 16725, 23540)},  {AOM_CDF4(3376, 9895, 15184)},
                {AOM_CDF4(29383, 32617, 32745)}, {AOM_CDF4(18891, 30809, 32401)},
                {AOM_CDF4(11688, 25942, 30687)}, {AOM_CDF4(7468, 19469, 26651)},
                {AOM_CDF4(3909, 11358, 17012)},  {AOM_CDF4(31564, 32736, 32748)},
                {AOM_CDF4(20906, 31611, 32600)}, {AOM_CDF4(13191, 27621, 31537)},
                {AOM_CDF4(8768, 22029, 28676)},  {AOM_CDF4(5079, 14109, 20906)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)},  {AOM_CDF4(8192, 16384, 24576)}
            },
            {
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)},
                {AOM_CDF4(8192, 16384, 24576)}, {AOM_CDF4(8192, 16384, 24576)}
            }
        }
    }
};

static const aom_cdf_prob
av1_default_coeff_base_eob_multi_cdfs[TOKEN_CDF_Q_CTXS][TX_SIZES]
[PLANE_TYPES][SIG_COEF_CONTEXTS_EOB][CDF_SIZE(NUM_BASE_LEVELS + 1)] = {
    {
        {
            {
                {AOM_CDF3(17837, 29055)},
                {AOM_CDF3(29600, 31446)},
                {AOM_CDF3(30844, 31878)},
                {AOM_CDF3(24926, 28948)}
            },
            {
                {AOM_CDF3(21365, 30026)},
                {AOM_CDF3(30512, 32423)},
                {AOM_CDF3(31658, 32621)},
                {AOM_CDF3(29630, 31881)}
            }
        },
        {
            {
                {AOM_CDF3(5717, 26477)},
                {AOM_CDF3(30491, 31703)},
                {AOM_CDF3(31550, 32158)},
                {AOM_CDF3(29648, 31491)}
            },
            {
                {AOM_CDF3(12608, 27820)},
                {AOM_CDF3(30680, 32225)},
                {AOM_CDF3(30809, 32335)},
                {AOM_CDF3(31299, 32423)}
            }
        },
        {
            {
                {AOM_CDF3(1786, 12612)},
                {AOM_CDF3(30663, 31625)},
                {AOM_CDF3(32339, 32468)},
                {AOM_CDF3(31148, 31833)}
            },
            {
                {AOM_CDF3(18857, 23865)},
                {AOM_CDF3(31428, 32428)},
                {AOM_CDF3(31744, 32373)},
                {AOM_CDF3(31775, 32526)}
            }
        },
        {
            {
                {AOM_CDF3(1787, 2532)},
                {AOM_CDF3(30832, 31662)},
                {AOM_CDF3(31824, 32682)},
                {AOM_CDF3(32133, 32569)}
            },
            {
                {AOM_CDF3(13751, 22235)},
                {AOM_CDF3(32089, 32409)},
                {AOM_CDF3(27084, 27920)},
                {AOM_CDF3(29291, 32594)}
            }
        },
        {
            {
                {AOM_CDF3(1725, 3449)},
                {AOM_CDF3(31102, 31935)},
                {AOM_CDF3(32457, 32613)},
                {AOM_CDF3(32412, 32649)}
            },
            {
                {AOM_CDF3(10923, 21845)},
                {AOM_CDF3(10923, 21845)},
                {AOM_CDF3(10923, 21845)},
                {AOM_CDF3(10923, 21845)}
            }
        }
    },
    {
        {
            {
                {AOM_CDF3(17560, 29888)},
                {AOM_CDF3(29671, 31549)},
                {AOM_CDF3(31007, 32056)},
                {AOM_CDF3(27286, 30006)}
            },
            {
                {AOM_CDF3(26594, 31212)},
                {AOM_CDF3(31208, 32582)},
                {AOM_CDF3(31835, 32637)},
                {AOM_CDF3(30595, 32206)}
            }
        },
        {
            {
                {AOM_CDF3(15239, 29932)},
                {AOM_CDF3(31315, 32095)},
                {AOM_CDF3(32130, 32434)},
                {AOM_CDF3(30864, 31996)}
            },
            {
                {AOM_CDF3(26279, 30968)},
                {AOM_CDF3(31142, 32495)},
                {AOM_CDF3(31713, 32540)},
                {AOM_CDF3(31929, 32594)}
            }
        },
        {
            {
                {AOM_CDF3(2644, 25198)},
                {AOM_CDF3(32038, 32451)},
                {AOM_CDF3(32639, 32695)},
                {AOM_CDF3(32166, 32518)}
            },
            {
                {AOM_CDF3(17187, 27668)},
                {AOM_CDF3(31714, 32550)},
                {AOM_CDF3(32283, 32678)},
                {AOM_CDF3(31930, 32563)}
            }
        },
        {
            {
                {AOM_CDF3(1044, 2257)},
                {AOM_CDF3(30755, 31923)},
                {AOM_CDF3(32208, 32693)},
                {AOM_CDF3(32244, 32615)}
            },
            {
                {AOM_CDF3(21317, 26207)},
                {AOM_CDF3(29133, 30868)},
                {AOM_CDF3(29311, 31231)},
                {AOM_CDF3(29657, 31087)}
            }
        },
        {
            {
                {AOM_CDF3(478, 1834)},
                {AOM_CDF3(31005, 31987)},
                {AOM_CDF3(32317, 32724)},
                {AOM_CDF3(30865, 32648)}
            },
            {
                {AOM_CDF3(10923, 21845)},
                {AOM_CDF3(10923, 21845)},
                {AOM_CDF3(10923, 21845)},
                {AOM_CDF3(10923, 21845)}
            }
        }
    },
    {
        {
            {
                {AOM_CDF3(20092, 30774)},
                {AOM_CDF3(30695, 32020)},
                {AOM_CDF3(31131, 32103)},
                {AOM_CDF3(28666, 30870)}
            },
            {
                {AOM_CDF3(27258, 31095)},
                {AOM_CDF3(31804, 32623)},
                {AOM_CDF3(31763, 32528)},
                {AOM_CDF3(31438, 32506)}
            }
        },
        {
            {
                {AOM_CDF3(18049, 30489)},
                {AOM_CDF3(31706, 32286)},
                {AOM_CDF3(32163, 32473)},
                {AOM_CDF3(31550, 32184)}
            },
            {
                {AOM_CDF3(27116, 30842)},
                {AOM_CDF3(31971, 32598)},
                {AOM_CDF3(32088, 32576)},
                {AOM_CDF3(32067, 32664)}
            }
        },
        {
            {
                {AOM_CDF3(12854, 29093)},
                {AOM_CDF3(32272, 32558)},
                {AOM_CDF3(32667, 32729)},
                {AOM_CDF3(32306, 32585)}
            },
            {
                {AOM_CDF3(25476, 30366)},
                {AOM_CDF3(32169, 32687)},
                {AOM_CDF3(32479, 32689)},
                {AOM_CDF3(31673, 32634)}
            }
        },
        {
            {
                {AOM_CDF3(2809, 19301)},
                {AOM_CDF3(32205, 32622)},
                {AOM_CDF3(32338, 32730)},
                {AOM_CDF3(31786, 32616)}
            },
            {
                {AOM_CDF3(22737, 29105)},
                {AOM_CDF3(30810, 32362)},
                {AOM_CDF3(30014, 32627)},
                {AOM_CDF3(30528, 32574)}
            }
        },
        {
            {
                {AOM_CDF3(935, 3382)},
                {AOM_CDF3(30789, 31909)},
                {AOM_CDF3(32466, 32756)},
                {AOM_CDF3(30860, 32513)}
            },
            {
                {AOM_CDF3(10923, 21845)},
                {AOM_CDF3(10923, 21845)},
                {AOM_CDF3(10923, 21845)},
                {AOM_CDF3(10923, 21845)}
            }
        }
    },
    {
        {
            {
                {AOM_CDF3(22497, 31198)},
                {AOM_CDF3(31715, 32495)},
                {AOM_CDF3(31606, 32337)},
                {AOM_CDF3(30388, 31990)}
            },
            {
                {AOM_CDF3(27877, 31584)},
                {AOM_CDF3(32170, 32728)},
                {AOM_CDF3(32155, 32688)},
                {AOM_CDF3(32219, 32702)}
            }
        },
        {
            {
                {AOM_CDF3(21457, 31043)},
                {AOM_CDF3(31951, 32483)},
                {AOM_CDF3(32153, 32562)},
                {AOM_CDF3(31473, 32215)}
            },
            {
                {AOM_CDF3(27558, 31151)},
                {AOM_CDF3(32020, 32640)},
                {AOM_CDF3(32097, 32575)},
                {AOM_CDF3(32242, 32719)}
            }
        },
        {
            {
                {AOM_CDF3(19980, 30591)},
                {AOM_CDF3(32219, 32597)},
                {AOM_CDF3(32581, 32706)},
                {AOM_CDF3(31803, 32287)}
            },
            {
                {AOM_CDF3(26473, 30507)},
                {AOM_CDF3(32431, 32723)},
                {AOM_CDF3(32196, 32611)},
                {AOM_CDF3(31588, 32528)}
            }
        },
        {
            {
                {AOM_CDF3(24647, 30463)},
                {AOM_CDF3(32412, 32695)},
                {AOM_CDF3(32468, 32720)},
                {AOM_CDF3(31269, 32523)}
            },
            {
                {AOM_CDF3(28482, 31505)},
                {AOM_CDF3(32152, 32701)},
                {AOM_CDF3(31732, 32598)},
                {AOM_CDF3(31767, 32712)}
            }
        },
        {
            {
                {AOM_CDF3(12358, 24977)},
                {AOM_CDF3(31331, 32385)},
                {AOM_CDF3(32634, 32756)},
                {AOM_CDF3(30411, 32548)}
            },
            {
                {AOM_CDF3(10923, 21845)},
                {AOM_CDF3(10923, 21845)},
                {AOM_CDF3(10923, 21845)},
                {AOM_CDF3(10923, 21845)}
            }
        }
    }
};

static const RK_U16 default_joint_cdf[] = {ICDF(4096), ICDF(11264), ICDF(19328)};
static const RK_U16 default_clsss_cdf[][10] = {
    // Vertical component
    {
        ICDF(28672), ICDF(30976), ICDF(31858), ICDF(32320), ICDF(32551),
        ICDF(32656), ICDF(32740), ICDF(32757), ICDF(32762), ICDF(32767)
    },
    // Horizontal component
    {
        ICDF(28672), ICDF(30976), ICDF(31858), ICDF(32320), ICDF(32551),
        ICDF(32656), ICDF(32740), ICDF(32757), ICDF(32762), ICDF(32767)
    }
};

static const RK_U16 default_clsss0_fp_cdf[][2][3] = {
    // Vertical component
    {
        {ICDF(16384), ICDF(24576), ICDF(26624)},
        {ICDF(12288), ICDF(21248), ICDF(24128)}
    },
    // Horizontal component
    {
        {ICDF(16384), ICDF(24576), ICDF(26624)},
        {ICDF(12288), ICDF(21248), ICDF(24128)}
    }
};

static const RK_U16 default_fp_cdf[][3] = {
    // Vertical component
    {ICDF(8192), ICDF(17408), ICDF(21248)},
    // Horizontal component
    {ICDF(8192), ICDF(17408), ICDF(21248)}
};

static const RK_U16 default_sign_cdf[] = {ICDF(128 * 128), ICDF(128 * 128)};
static const RK_U16 default_class0_hp_cdf[] = {ICDF(160 * 128), ICDF(160 * 128)};
static const RK_U16 default_hp_cdf[] = {ICDF(128 * 128), ICDF(128 * 128)};
static const RK_U16 default_class0_cdf[] = {ICDF(216 * 128), ICDF(216 * 128)};
static const RK_U16 default_bits_cdf[][10] = {
    {
        ICDF(128 * 136), ICDF(128 * 140), ICDF(128 * 148), ICDF(128 * 160),
        ICDF(128 * 176), ICDF(128 * 192), ICDF(128 * 224), ICDF(128 * 234),
        ICDF(128 * 234), ICDF(128 * 240)
    },
    {
        ICDF(128 * 136), ICDF(128 * 140), ICDF(128 * 148), ICDF(128 * 160),
        ICDF(128 * 176), ICDF(128 * 192), ICDF(128 * 224), ICDF(128 * 234),
        ICDF(128 * 234), ICDF(128 * 240)
    }
};

int get_q_ctx(int q)
{
    if (q <= 20) return 0;
    if (q <= 60) return 1;
    if (q <= 120) return 2;
    return 3;
}

void Av1DefaultCoeffProbs(RK_U32 base_qindex, void *ptr)
{

    AV1CDFs *cdfs = (AV1CDFs*)ptr;
    const int index = get_q_ctx(base_qindex);

    memcpy(cdfs->txb_skip_cdf, av1_default_txb_skip_cdfs[index],
           sizeof(av1_default_txb_skip_cdfs[0]));
    memcpy(cdfs->eob_extra_cdf, av1_default_eob_extra_cdfs[index],
           sizeof(av1_default_eob_extra_cdfs[0]));
    memcpy(cdfs->dc_sign_cdf, av1_default_dc_sign_cdfs[index],
           sizeof(av1_default_dc_sign_cdfs[0]));
    memcpy(cdfs->coeff_br_cdf, av1_default_coeff_lps_multi_cdfs[index],
           sizeof(av1_default_coeff_lps_multi_cdfs[0]));
    memcpy(cdfs->coeff_base_cdf, av1_default_coeff_base_multi_cdfs[index],
           sizeof(av1_default_coeff_base_multi_cdfs[0]));
    memcpy(cdfs->coeff_base_eob_cdf,
           av1_default_coeff_base_eob_multi_cdfs[index],
           sizeof(av1_default_coeff_base_eob_multi_cdfs[0]));
    memcpy(cdfs->eob_flag_cdf16, av1_default_eob_multi16_cdfs[index],
           sizeof(av1_default_eob_multi16_cdfs[0]));
    memcpy(cdfs->eob_flag_cdf32, av1_default_eob_multi32_cdfs[index],
           sizeof(av1_default_eob_multi32_cdfs[0]));
    memcpy(cdfs->eob_flag_cdf64, av1_default_eob_multi64_cdfs[index],
           sizeof(av1_default_eob_multi64_cdfs[0]));
    memcpy(cdfs->eob_flag_cdf128, av1_default_eob_multi128_cdfs[index],
           sizeof(av1_default_eob_multi128_cdfs[0]));
    memcpy(cdfs->eob_flag_cdf256, av1_default_eob_multi256_cdfs[index],
           sizeof(av1_default_eob_multi256_cdfs[0]));
    memcpy(cdfs->eob_flag_cdf512, av1_default_eob_multi512_cdfs[index],
           sizeof(av1_default_eob_multi512_cdfs[0]));
    memcpy(cdfs->eob_flag_cdf1024, av1_default_eob_multi1024_cdfs[index],
           sizeof(av1_default_eob_multi1024_cdfs[0]));
}

void AV1InitCDFs(RK_U32 base_qindex, void *ptr)
{
    Av1DefaultCoeffProbs(base_qindex, ptr);
}

void AV1SetDefaultCDFs(AV1CDFs *cdfs, MvCDFs *cdfs_ndvc)
{
    // AV1CDFs *cdfs = (AV1CDFs*)ptr;
    // MvCDFs *cdfs_ndvc = &cdfs->mv_cdf;

    memcpy(cdfs->partition_cdf, default_partition_cdf,
           sizeof(cdfs->partition_cdf));

    memcpy(cdfs->tx_type_intra0_cdf, default_intra_ext_tx0_cdf,
           sizeof(cdfs->tx_type_intra0_cdf));
    memcpy(cdfs->tx_type_intra1_cdf, default_intra_ext_tx1_cdf,
           sizeof(cdfs->tx_type_intra1_cdf));
    memcpy(cdfs->tx_type_inter_cdf, default_inter_ext_tx_cdf,
           sizeof(cdfs->tx_type_inter_cdf));

    memcpy(cdfs->vartx_part_cdf, default_txfm_partition_cdf,
           sizeof(cdfs->vartx_part_cdf));
    memcpy(cdfs->mbskip_cdf, default_skip_cdfs, sizeof(cdfs->mbskip_cdf));
    memcpy(cdfs->delta_q_cdf, default_delta_q_cdf,
           sizeof(cdfs->delta_q_cdf));
    memcpy(cdfs->delta_lf_multi_cdf, default_delta_lf_multi_cdf,
           sizeof(cdfs->delta_lf_multi_cdf));
    memcpy(cdfs->delta_lf_cdf, default_delta_lf_cdf,
           sizeof(cdfs->delta_lf_cdf));

    memcpy(cdfs->segment_pred_cdf, default_segment_pred_cdf,
           sizeof(cdfs->segment_pred_cdf));

    memcpy(cdfs->spatial_pred_seg_tree_cdf, default_spatial_pred_seg_tree_cdf,
           sizeof(cdfs->spatial_pred_seg_tree_cdf));

    memcpy(cdfs->skip_mode_cdf, default_skip_mode_cdfs,
           sizeof(cdfs->skip_mode_cdf));

    memcpy(cdfs->tx_size_cdf, default_tx_size_cdf,
           sizeof(cdfs->tx_size_cdf));

    memcpy(cdfs->kf_ymode_cdf, default_kf_y_mode_cdf,
           sizeof(cdfs->kf_ymode_cdf));
    memcpy(cdfs->uv_mode_cdf, default_uv_mode_cdf,
           sizeof(cdfs->uv_mode_cdf));
    memcpy(cdfs->if_ymode_cdf, default_if_y_mode_cdf,
           sizeof(cdfs->if_ymode_cdf));

    memcpy(cdfs->intra_inter_cdf, default_intra_inter_cdf,
           sizeof(cdfs->intra_inter_cdf));

    memcpy(cdfs->comp_ref_cdf, default_comp_ref_cdf,
           sizeof(cdfs->comp_ref_cdf));
    memcpy(cdfs->comp_bwdref_cdf, default_comp_bwdref_cdf,
           sizeof(cdfs->comp_bwdref_cdf));

    memcpy(cdfs->comp_inter_cdf, default_comp_inter_cdf,
           sizeof(cdfs->comp_inter_cdf));

    memcpy(cdfs->single_ref_cdf, default_single_ref_cdf,
           sizeof(cdfs->single_ref_cdf));
    memcpy(cdfs->comp_ref_type_cdf, default_comp_ref_type_cdf,
           sizeof(cdfs->comp_ref_type_cdf));
    memcpy(cdfs->uni_comp_ref_cdf, default_uni_comp_ref_cdf,
           sizeof(cdfs->uni_comp_ref_cdf));

    memcpy(cdfs->newmv_cdf, default_newmv_cdf, sizeof(cdfs->newmv_cdf));
    memcpy(cdfs->zeromv_cdf, default_zeromv_cdf, sizeof(cdfs->zeromv_cdf));
    memcpy(cdfs->refmv_cdf, default_refmv_cdf, sizeof(cdfs->refmv_cdf));
    memcpy(cdfs->drl_cdf, default_drl_cdf, sizeof(cdfs->drl_cdf));

    memcpy(cdfs->interp_filter_cdf, default_switchable_interp_cdf,
           sizeof(cdfs->interp_filter_cdf));

    // Regular MV cdfs
    memcpy(cdfs->mv_cdf.joint_cdf, default_joint_cdf,
           sizeof(cdfs->mv_cdf.joint_cdf));
    memcpy(cdfs->mv_cdf.sign_cdf, default_sign_cdf,
           sizeof(cdfs->mv_cdf.sign_cdf));
    memcpy(cdfs->mv_cdf.clsss_cdf, default_clsss_cdf,
           sizeof(cdfs->mv_cdf.clsss_cdf));
    memcpy(cdfs->mv_cdf.clsss0_fp_cdf, default_clsss0_fp_cdf,
           sizeof(cdfs->mv_cdf.clsss0_fp_cdf));
    memcpy(cdfs->mv_cdf.fp_cdf, default_fp_cdf,
           sizeof(cdfs->mv_cdf.fp_cdf));
    memcpy(cdfs->mv_cdf.class0_hp_cdf, default_class0_hp_cdf,
           sizeof(cdfs->mv_cdf.class0_hp_cdf));
    memcpy(cdfs->mv_cdf.hp_cdf, default_hp_cdf,
           sizeof(cdfs->mv_cdf.hp_cdf));
    memcpy(cdfs->mv_cdf.class0_cdf, default_class0_cdf,
           sizeof(cdfs->mv_cdf.class0_cdf));
    memcpy(cdfs->mv_cdf.bits_cdf, default_bits_cdf,
           sizeof(cdfs->mv_cdf.bits_cdf));

    // Intrabc cdfs
    memcpy(cdfs_ndvc->joint_cdf, default_joint_cdf,
           sizeof(cdfs_ndvc->joint_cdf));
    memcpy(cdfs_ndvc->sign_cdf, default_sign_cdf,
           sizeof(cdfs_ndvc->sign_cdf));
    memcpy(cdfs_ndvc->clsss_cdf, default_clsss_cdf,
           sizeof(cdfs_ndvc->clsss_cdf));
    memcpy(cdfs_ndvc->clsss0_fp_cdf, default_clsss0_fp_cdf,
           sizeof(cdfs_ndvc->clsss0_fp_cdf));
    memcpy(cdfs_ndvc->fp_cdf, default_fp_cdf, sizeof(cdfs_ndvc->fp_cdf));
    memcpy(cdfs_ndvc->class0_hp_cdf, default_class0_hp_cdf,
           sizeof(cdfs_ndvc->class0_hp_cdf));
    memcpy(cdfs_ndvc->hp_cdf, default_hp_cdf, sizeof(cdfs_ndvc->hp_cdf));
    memcpy(cdfs_ndvc->class0_cdf, default_class0_cdf,
           sizeof(cdfs_ndvc->class0_cdf));
    memcpy(cdfs_ndvc->bits_cdf, default_bits_cdf,
           sizeof(cdfs_ndvc->bits_cdf));

    memcpy(cdfs->obmc_cdf, default_obmc_cdf, sizeof(cdfs->obmc_cdf));
    memcpy(cdfs->motion_mode_cdf, default_motion_mode_cdf,
           sizeof(cdfs->motion_mode_cdf));

    memcpy(cdfs->inter_compound_mode_cdf, default_inter_compound_mode_cdf,
           sizeof(cdfs->inter_compound_mode_cdf));
    memcpy(cdfs->compound_type_cdf, default_compound_type_cdf,
           sizeof(cdfs->compound_type_cdf));
    memcpy(cdfs->interintra_cdf, default_interintra_cdf,
           sizeof(cdfs->interintra_cdf));
    memcpy(cdfs->interintra_mode_cdf, default_interintra_mode_cdf,
           sizeof(cdfs->interintra_mode_cdf));
    memcpy(cdfs->wedge_interintra_cdf, default_wedge_interintra_cdf,
           sizeof(cdfs->wedge_interintra_cdf));
    memcpy(cdfs->wedge_idx_cdf, default_wedge_idx_cdf,
           sizeof(cdfs->wedge_idx_cdf));

    memcpy(cdfs->palette_y_mode_cdf, default_palette_y_mode_cdf,
           sizeof(cdfs->palette_y_mode_cdf));
    memcpy(cdfs->palette_uv_mode_cdf, default_palette_uv_mode_cdf,
           sizeof(cdfs->palette_uv_mode_cdf));
    memcpy(cdfs->palette_y_size_cdf, default_palette_y_size_cdf,
           sizeof(cdfs->palette_y_size_cdf));
    memcpy(cdfs->palette_uv_size_cdf, default_palette_uv_size_cdf,
           sizeof(cdfs->palette_uv_size_cdf));
    memcpy(cdfs->palette_y_color_index_cdf, default_palette_y_color_index_cdf,
           sizeof(cdfs->palette_y_color_index_cdf));
    memcpy(cdfs->palette_uv_color_index_cdf,
           default_palette_uv_color_index_cdf,
           sizeof(cdfs->palette_uv_color_index_cdf));

    memcpy(cdfs->cfl_sign_cdf, default_cfl_sign_cdf,
           sizeof(cdfs->cfl_sign_cdf));
    memcpy(cdfs->cfl_alpha_cdf, default_cfl_alpha_cdf,
           sizeof(cdfs->cfl_alpha_cdf));

    memcpy(cdfs->intrabc_cdf, default_intrabc_cdf,
           sizeof(cdfs->intrabc_cdf));
    memcpy(cdfs->angle_delta_cdf, default_angle_delta_cdf,
           sizeof(cdfs->angle_delta_cdf));
    memcpy(cdfs->filter_intra_mode_cdf, default_filter_intra_mode_cdf,
           sizeof(cdfs->filter_intra_mode_cdf));
    memcpy(cdfs->filter_intra_cdf, default_filter_intra_cdfs,
           sizeof(cdfs->filter_intra_cdf));
    memcpy(cdfs->comp_group_idx_cdf, default_comp_group_idx_cdfs,
           sizeof(cdfs->comp_group_idx_cdf));
    memcpy(cdfs->compound_idx_cdf, default_compound_idx_cdfs,
           sizeof(cdfs->compound_idx_cdf));

    // Av1DefaultCoeffProbs(x);
}
