#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <libdrm/xf86drm.h>
#include <libdrm/util/common.h>
#include <libdrm/drm/amdgpu_drm.h>

#include "amdgpu_internal.h"

static const char *amdgpu_ids_array[] = {
    "# List of AMDGPU IDs",
    "#",
    "# Syntax:",
    "# device_id,    revision_id,    product_name        <-- single tab after comma",
    "1.0.0",
    "1309,   00, AMD Radeon R7 Graphics",
    "130A,   00, AMD Radeon R6 Graphics",
    "130B,   00, AMD Radeon R4 Graphics",
    "130C,   00, AMD Radeon R7 Graphics",
    "130D,   00, AMD Radeon R6 Graphics",
    "130E,   00, AMD Radeon R5 Graphics",
    "130F,   00, AMD Radeon R7 Graphics",
    "130F,   D4, AMD Radeon R7 Graphics",
    "130F,   D5, AMD Radeon R7 Graphics",
    "130F,   D6, AMD Radeon R7 Graphics",
    "130F,   D7, AMD Radeon R7 Graphics",
    "1313,   00, AMD Radeon R7 Graphics",
    "1313,   D4, AMD Radeon R7 Graphics",
    "1313,   D5, AMD Radeon R7 Graphics",
    "1313,   D6, AMD Radeon R7 Graphics",
    "1315,   00, AMD Radeon R5 Graphics",
    "1315,   D4, AMD Radeon R5 Graphics",
    "1315,   D5, AMD Radeon R5 Graphics",
    "1315,   D6, AMD Radeon R5 Graphics",
    "1315,   D7, AMD Radeon R5 Graphics",
    "1316,   00, AMD Radeon R5 Graphics",
    "1318,   00, AMD Radeon R5 Graphics",
    "131B,   00, AMD Radeon R4 Graphics",
    "131C,   00, AMD Radeon R7 Graphics",
    "131D,   00, AMD Radeon R6 Graphics",
    "15D8,   00, AMD Radeon RX Vega 8 Graphics WS",
    "15D8,   91, AMD Radeon Vega 3 Graphics",
    "15D8,   91, AMD Ryzen Embedded R1606G with Radeon Vega Gfx",
    "15D8,   92, AMD Radeon Vega 3 Graphics",
    "15D8,   92, AMD Ryzen Embedded R1505G with Radeon Vega Gfx",
    "15D8,   93, AMD Radeon Vega 1 Graphics",
    "15D8,   A1, AMD Radeon Vega 10 Graphics",
    "15D8,   A2, AMD Radeon Vega 8 Graphics",
    "15D8,   A3, AMD Radeon Vega 6 Graphics",
    "15D8,   A4, AMD Radeon Vega 3 Graphics",
    "15D8,   B1, AMD Radeon Vega 10 Graphics",
    "15D8,   B2, AMD Radeon Vega 8 Graphics",
    "15D8,   B3, AMD Radeon Vega 6 Graphics",
    "15D8,   B4, AMD Radeon Vega 3 Graphics",
    "15D8,   C1, AMD Radeon Vega 10 Graphics",
    "15D8,   C2, AMD Radeon Vega 8 Graphics",
    "15D8,   C3, AMD Radeon Vega 6 Graphics",
    "15D8,   C4, AMD Radeon Vega 3 Graphics",
    "15D8,   C5, AMD Radeon Vega 3 Graphics",
    "15D8,   C8, AMD Radeon Vega 11 Graphics",
    "15D8,   C9, AMD Radeon Vega 8 Graphics",
    "15D8,   CA, AMD Radeon Vega 11 Graphics",
    "15D8,   CB, AMD Radeon Vega 8 Graphics",
    "15D8,   CC, AMD Radeon Vega 3 Graphics",
    "15D8,   CE, AMD Radeon Vega 3 Graphics",
    "15D8,   CF, AMD Ryzen Embedded R1305G with Radeon Vega Gfx",
    "15D8,   D1, AMD Radeon Vega 10 Graphics",
    "15D8,   D2, AMD Radeon Vega 8 Graphics",
    "15D8,   D3, AMD Radeon Vega 6 Graphics",
    "15D8,   D4, AMD Radeon Vega 3 Graphics",
    "15D8,   D8, AMD Radeon Vega 11 Graphics",
    "15D8,   D9, AMD Radeon Vega 8 Graphics",
    "15D8,   DA, AMD Radeon Vega 11 Graphics",
    "15D8,   DB, AMD Radeon Vega 3 Graphics",
    "15D8,   DB, AMD Radeon Vega 8 Graphics",
    "15D8,   DC, AMD Radeon Vega 3 Graphics",
    "15D8,   DD, AMD Radeon Vega 3 Graphics",
    "15D8,   DE, AMD Radeon Vega 3 Graphics",
    "15D8,   DF, AMD Radeon Vega 3 Graphics",
    "15D8,   E3, AMD Radeon Vega 3 Graphics",
    "15D8,   E4, AMD Ryzen Embedded R1102G with Radeon Vega Gfx",
    "15DD,   81, AMD Ryzen Embedded V1807B with Radeon Vega Gfx",
    "15DD,   82, AMD Ryzen Embedded V1756B with Radeon Vega Gfx",
    "15DD,   83, AMD Ryzen Embedded V1605B with Radeon Vega Gfx",
    "15DD,   84, AMD Radeon Vega 6 Graphics",
    "15DD,   85, AMD Ryzen Embedded V1202B with Radeon Vega Gfx",
    "15DD,   86, AMD Radeon Vega 11 Graphics",
    "15DD,   88, AMD Radeon Vega 8 Graphics",
    "15DD,   C1, AMD Radeon Vega 11 Graphics",
    "15DD,   C2, AMD Radeon Vega 8 Graphics",
    "15DD,   C3, AMD Radeon Vega 3 / 10 Graphics",
    "15DD,   C4, AMD Radeon Vega 8 Graphics",
    "15DD,   C5, AMD Radeon Vega 3 Graphics",
    "15DD,   C6, AMD Radeon Vega 11 Graphics",
    "15DD,   C8, AMD Radeon Vega 8 Graphics",
    "15DD,   C9, AMD Radeon Vega 11 Graphics",
    "15DD,   CA, AMD Radeon Vega 8 Graphics",
    "15DD,   CB, AMD Radeon Vega 3 Graphics",
    "15DD,   CC, AMD Radeon Vega 6 Graphics",
    "15DD,   CE, AMD Radeon Vega 3 Graphics",
    "15DD,   CF, AMD Radeon Vega 3 Graphics",
    "15DD,   D0, AMD Radeon Vega 10 Graphics",
    "15DD,   D1, AMD Radeon Vega 8 Graphics",
    "15DD,   D3, AMD Radeon Vega 11 Graphics",
    "15DD,   D5, AMD Radeon Vega 8 Graphics",
    "15DD,   D6, AMD Radeon Vega 11 Graphics",
    "15DD,   D7, AMD Radeon Vega 8 Graphics",
    "15DD,   D8, AMD Radeon Vega 3 Graphics",
    "15DD,   D9, AMD Radeon Vega 6 Graphics",
    "15DD,   E1, AMD Radeon Vega 3 Graphics",
    "15DD,   E2, AMD Radeon Vega 3 Graphics",
    "163F,   AE, AMD Custom GPU 0405",
    "6600,   00, AMD Radeon HD 8600 / 8700M",
    "6600,   81, AMD Radeon R7 M370",
    "6601,   00, AMD Radeon HD 8500M / 8700M",
    "6604,   00, AMD Radeon R7 M265 Series",
    "6604,   81, AMD Radeon R7 M350",
    "6605,   00, AMD Radeon R7 M260 Series",
    "6605,   81, AMD Radeon R7 M340",
    "6606,   00, AMD Radeon HD 8790M",
    "6607,   00, AMD Radeon R5 M240",
    "6608,   00, AMD FirePro W2100",
    "6610,   00, AMD Radeon R7 200 Series",
    "6610,   81, AMD Radeon R7 350",
    "6610,   83, AMD Radeon R5 340",
    "6610,   87, AMD Radeon R7 200 Series",
    "6611,   00, AMD Radeon R7 200 Series",
    "6611,   87, AMD Radeon R7 200 Series",
    "6613,   00, AMD Radeon R7 200 Series",
    "6617,   00, AMD Radeon R7 240 Series",
    "6617,   87, AMD Radeon R7 200 Series",
    "6617,   C7, AMD Radeon R7 240 Series",
    "6640,   00, AMD Radeon HD 8950",
    "6640,   80, AMD Radeon R9 M380",
    "6646,   00, AMD Radeon R9 M280X",
    "6646,   80, AMD Radeon R9 M385",
    "6646,   80, AMD Radeon R9 M470X",
    "6647,   00, AMD Radeon R9 M200X Series",
    "6647,   80, AMD Radeon R9 M380",
    "6649,   00, AMD FirePro W5100",
    "6658,   00, AMD Radeon R7 200 Series",
    "665C,   00, AMD Radeon HD 7700 Series",
    "665D,   00, AMD Radeon R7 200 Series",
    "665F,   81, AMD Radeon R7 360 Series",
    "6660,   00, AMD Radeon HD 8600M Series",
    "6660,   81, AMD Radeon R5 M335",
    "6660,   83, AMD Radeon R5 M330",
    "6663,   00, AMD Radeon HD 8500M Series",
    "6663,   83, AMD Radeon R5 M320",
    "6664,   00, AMD Radeon R5 M200 Series",
    "6665,   00, AMD Radeon R5 M230 Series",
    "6665,   83, AMD Radeon R5 M320",
    "6665,   C3, AMD Radeon R5 M435",
    "6666,   00, AMD Radeon R5 M200 Series",
    "6667,   00, AMD Radeon R5 M200 Series",
    "666F,   00, AMD Radeon HD 8500M",
    "66A1,   02, AMD Instinct MI60 / MI50",
    "66A1,   06, AMD Radeon Pro VII",
    "66AF,   C1, AMD Radeon VII",
    "6780,   00, AMD FirePro W9000",
    "6784,   00, ATI FirePro V (FireGL V) Graphics Adapter",
    "6788,   00, ATI FirePro V (FireGL V) Graphics Adapter",
    "678A,   00, AMD FirePro W8000",
    "6798,   00, AMD Radeon R9 200 / HD 7900 Series",
    "6799,   00, AMD Radeon HD 7900 Series",
    "679A,   00, AMD Radeon HD 7900 Series",
    "679B,   00, AMD Radeon HD 7900 Series",
    "679E,   00, AMD Radeon HD 7800 Series",
    "67A0,   00, AMD Radeon FirePro W9100",
    "67A1,   00, AMD Radeon FirePro W8100",
    "67B0,   00, AMD Radeon R9 200 Series",
    "67B0,   80, AMD Radeon R9 390 Series",
    "67B1,   00, AMD Radeon R9 200 Series",
    "67B1,   80, AMD Radeon R9 390 Series",
    "67B9,   00, AMD Radeon R9 200 Series",
    "67C0,   00, AMD Radeon Pro WX 7100 Graphics",
    "67C0,   80, AMD Radeon E9550",
    "67C2,   01, AMD Radeon Pro V7350x2",
    "67C2,   02, AMD Radeon Pro V7300X",
    "67C4,   00, AMD Radeon Pro WX 7100 Graphics",
    "67C4,   80, AMD Radeon E9560 / E9565 Graphics",
    "67C7,   00, AMD Radeon Pro WX 5100 Graphics",
    "67C7,   80, AMD Radeon E9390 Graphics",
    "67D0,   01, AMD Radeon Pro V7350x2",
    "67D0,   02, AMD Radeon Pro V7300X",
    "67DF,   C0, AMD Radeon Pro 580X",
    "67DF,   C1, AMD Radeon RX 580 Series",
    "67DF,   C2, AMD Radeon RX 570 Series",
    "67DF,   C3, AMD Radeon RX 580 Series",
    "67DF,   C4, AMD Radeon RX 480 Graphics",
    "67DF,   C5, AMD Radeon RX 470 Graphics",
    "67DF,   C6, AMD Radeon RX 570 Series",
    "67DF,   C7, AMD Radeon RX 480 Graphics",
    "67DF,   CF, AMD Radeon RX 470 Graphics",
    "67DF,   D7, AMD Radeon RX 470 Graphics",
    "67DF,   E0, AMD Radeon RX 470 Series",
    "67DF,   E1, AMD Radeon RX 590 Series",
    "67DF,   E3, AMD Radeon RX Series",
    "67DF,   E7, AMD Radeon RX 580 Series",
    "67DF,   EB, AMD Radeon Pro 580X",
    "67DF,   EF, AMD Radeon RX 570 Series",
    "67DF,   F7, AMD Radeon RX P30PH",
    "67DF,   FF, AMD Radeon RX 470 Series",
    "67E0,   00, AMD Radeon Pro WX Series",
    "67E3,   00, AMD Radeon Pro WX 4100",
    "67E8,   00, AMD Radeon Pro WX Series",
    "67E8,   01, AMD Radeon Pro WX Series",
    "67E8,   80, AMD Radeon E9260 Graphics",
    "67EB,   00, AMD Radeon Pro V5300X",
    "67EF,   C0, AMD Radeon RX Graphics",
    "67EF,   C1, AMD Radeon RX 460 Graphics",
    "67EF,   C2, AMD Radeon Pro Series",
    "67EF,   C3, AMD Radeon RX Series",
    "67EF,   C5, AMD Radeon RX 460 Graphics",
    "67EF,   C7, AMD Radeon RX Graphics",
    "67EF,   CF, AMD Radeon RX 460 Graphics",
    "67EF,   E0, AMD Radeon RX 560 Series",
    "67EF,   E1, AMD Radeon RX Series",
    "67EF,   E2, AMD Radeon RX 560X",
    "67EF,   E3, AMD Radeon RX Series",
    "67EF,   E5, AMD Radeon RX 560 Series",
    "67EF,   E7, AMD Radeon RX 560 Series",
    "67EF,   EF, AMD Radeon 550 Series",
    "67EF,   FF, AMD Radeon RX 460 Graphics",
    "67FF,   C0, AMD Radeon Pro 465",
    "67FF,   C1, AMD Radeon RX 560 Series",
    "67FF,   CF, AMD Radeon RX 560 Series",
    "67FF,   EF, AMD Radeon RX 560 Series",
    "67FF,   FF, AMD Radeon RX 550 Series",
    "6800,   00, AMD Radeon HD 7970M",
    "6801,   00, AMD Radeon HD 8970M",
    "6806,   00, AMD Radeon R9 M290X",
    "6808,   00, AMD FirePro W7000",
    "6808,   00, ATI FirePro V (FireGL V) Graphics Adapter",
    "6809,   00, ATI FirePro W5000",
    "6810,   00, AMD Radeon R9 200 Series",
    "6810,   81, AMD Radeon R9 370 Series",
    "6811,   00, AMD Radeon R9 200 Series",
    "6811,   81, AMD Radeon R7 370 Series",
    "6818,   00, AMD Radeon HD 7800 Series",
    "6819,   00, AMD Radeon HD 7800 Series",
    "6820,   00, AMD Radeon R9 M275X",
    "6820,   81, AMD Radeon R9 M375",
    "6820,   83, AMD Radeon R9 M375X",
    "6821,   00, AMD Radeon R9 M200X Series",
    "6821,   83, AMD Radeon R9 M370X",
    "6821,   87, AMD Radeon R7 M380",
    "6822,   00, AMD Radeon E8860",
    "6823,   00, AMD Radeon R9 M200X Series",
    "6825,   00, AMD Radeon HD 7800M Series",
    "6826,   00, AMD Radeon HD 7700M Series",
    "6827,   00, AMD Radeon HD 7800M Series",
    "6828,   00, AMD FirePro W600",
    "682B,   00, AMD Radeon HD 8800M Series",
    "682B,   87, AMD Radeon R9 M360",
    "682C,   00, AMD FirePro W4100",
    "682D,   00, AMD Radeon HD 7700M Series",
    "682F,   00, AMD Radeon HD 7700M Series",
    "6830,   00, AMD Radeon 7800M Series",
    "6831,   00, AMD Radeon 7700M Series",
    "6835,   00, AMD Radeon R7 Series / HD 9000 Series",
    "6837,   00, AMD Radeon HD 7700 Series",
    "683D,   00, AMD Radeon HD 7700 Series",
    "683F,   00, AMD Radeon HD 7700 Series",
    "684C,   00, ATI FirePro V (FireGL V) Graphics Adapter",
    "6860,   00, AMD Radeon Instinct MI25",
    "6860,   01, AMD Radeon Instinct MI25",
    "6860,   02, AMD Radeon Instinct MI25",
    "6860,   03, AMD Radeon Pro V340",
    "6860,   04, AMD Radeon Instinct MI25x2",
    "6860,   07, AMD Radeon Pro V320",
    "6861,   00, AMD Radeon Pro WX 9100",
    "6862,   00, AMD Radeon Pro SSG",
    "6863,   00, AMD Radeon Vega Frontier Edition",
    "6864,   03, AMD Radeon Pro V340",
    "6864,   04, AMD Radeon Instinct MI25x2",
    "6864,   05, AMD Radeon Pro V340",
    "6868,   00, AMD Radeon Pro WX 8200",
    "686C,   00, AMD Radeon Instinct MI25 MxGPU",
    "686C,   01, AMD Radeon Instinct MI25 MxGPU",
    "686C,   02, AMD Radeon Instinct MI25 MxGPU",
    "686C,   03, AMD Radeon Pro V340 MxGPU",
    "686C,   04, AMD Radeon Instinct MI25x2 MxGPU",
    "686C,   05, AMD Radeon Pro V340L MxGPU",
    "686C,   06, AMD Radeon Instinct MI25 MxGPU",
    "687F,   01, AMD Radeon RX Vega",
    "687F,   C0, AMD Radeon RX Vega",
    "687F,   C1, AMD Radeon RX Vega",
    "687F,   C3, AMD Radeon RX Vega",
    "687F,   C7, AMD Radeon RX Vega",
    "6900,   00, AMD Radeon R7 M260",
    "6900,   81, AMD Radeon R7 M360",
    "6900,   83, AMD Radeon R7 M340",
    "6900,   C1, AMD Radeon R5 M465 Series",
    "6900,   C3, AMD Radeon R5 M445 Series",
    "6900,   D1, AMD Radeon 530 Series",
    "6900,   D3, AMD Radeon 530 Series",
    "6901,   00, AMD Radeon R5 M255",
    "6902,   00, AMD Radeon Series",
    "6907,   00, AMD Radeon R5 M255",
    "6907,   87, AMD Radeon R5 M315",
    "6920,   00, AMD Radeon R9 M395X",
    "6920,   01, AMD Radeon R9 M390X",
    "6921,   00, AMD Radeon R9 M390X",
    "6929,   00, AMD FirePro S7150",
    "6929,   01, AMD FirePro S7100X",
    "692B,   00, AMD FirePro W7100",
    "6938,   00, AMD Radeon R9 200 Series",
    "6938,   F0, AMD Radeon R9 200 Series",
    "6938,   F1, AMD Radeon R9 380 Series",
    "6939,   00, AMD Radeon R9 200 Series",
    "6939,   F0, AMD Radeon R9 200 Series",
    "6939,   F1, AMD Radeon R9 380 Series",
    "694C,   C0, AMD Radeon RX Vega M GH Graphics",
    "694E,   C0, AMD Radeon RX Vega M GL Graphics",
    "6980,   00, AMD Radeon Pro WX 3100",
    "6981,   00, AMD Radeon Pro WX 3200 Series",
    "6981,   01, AMD Radeon Pro WX 3200 Series",
    "6981,   10, AMD Radeon Pro WX 3200 Series",
    "6985,   00, AMD Radeon Pro WX 3100",
    "6986,   00, AMD Radeon Pro WX 2100",
    "6987,   80, AMD Embedded Radeon E9171",
    "6987,   C0, AMD Radeon 550X Series",
    "6987,   C1, AMD Radeon RX 640",
    "6987,   C3, AMD Radeon 540X Series",
    "6987,   C7, AMD Radeon 540",
    "6995,   00, AMD Radeon Pro WX 2100",
    "6997,   00, AMD Radeon Pro WX 2100",
    "699F,   81, AMD Embedded Radeon E9170 Series",
    "699F,   C0, AMD Radeon 500 Series",
    "699F,   C1, AMD Radeon 540 Series",
    "699F,   C3, AMD Radeon 500 Series",
    "699F,   C7, AMD Radeon RX 550 / 550 Series",
    "699F,   C9, AMD Radeon 540",
    "6FDF,   E7, AMD Radeon RX 590 GME",
    "6FDF,   EF, AMD Radeon RX 580 2048SP",
    "7300,   C1, AMD FirePro S9300 x2",
    "7300,   C8, AMD Radeon R9 Fury Series",
    "7300,   C9, AMD Radeon Pro Duo",
    "7300,   CA, AMD Radeon R9 Fury Series",
    "7300,   CB, AMD Radeon R9 Fury Series",
    "7312,   00, AMD Radeon Pro W5700",
    "731E,   C6, AMD Radeon RX 5700XTB",
    "731E,   C7, AMD Radeon RX 5700B",
    "731F,   C0, AMD Radeon RX 5700 XT 50th Anniversary",
    "731F,   C1, AMD Radeon RX 5700 XT",
    "731F,   C2, AMD Radeon RX 5600M",
    "731F,   C3, AMD Radeon RX 5700M",
    "731F,   C4, AMD Radeon RX 5700",
    "731F,   C5, AMD Radeon RX 5700 XT",
    "731F,   CA, AMD Radeon RX 5600 XT",
    "731F,   CB, AMD Radeon RX 5600 OEM",
    "7340,   C1, AMD Radeon RX 5500M",
    "7340,   C3, AMD Radeon RX 5300M",
    "7340,   C5, AMD Radeon RX 5500 XT",
    "7340,   C7, AMD Radeon RX 5500",
    "7340,   C9, AMD Radeon RX 5500XTB",
    "7340,   CF, AMD Radeon RX 5300",
    "7341,   00, AMD Radeon Pro W5500",
    "7347,   00, AMD Radeon Pro W5500M",
    "7360,   41, AMD Radeon Pro 5600M",
    "7360,   C3, AMD Radeon Pro V520",
    "738C,   01, AMD Instinct MI100",
    "73A3,   00, AMD Radeon Pro W6800",
    "73A5,   C0, AMD Radeon RX 6950 XT",
    "73AF,   C0, AMD Radeon RX 6900 XT",
    "73BF,   C0, AMD Radeon RX 6900 XT",
    "73BF,   C1, AMD Radeon RX 6800 XT",
    "73BF,   C3, AMD Radeon RX 6800",
    "73DF,   C0, AMD Radeon RX 6750 XT",
    "73DF,   C1, AMD Radeon RX 6700 XT",
    "73DF,   C2, AMD Radeon RX 6800M",
    "73DF,   C3, AMD Radeon RX 6800M",
    "73DF,   C5, AMD Radeon RX 6700 XT",
    "73DF,   CF, AMD Radeon RX 6700M",
    "73DF,   D7, AMD TDC-235",
    "73E1,   00, AMD Radeon Pro W6600M",
    "73E3,   00, AMD Radeon Pro W6600",
    "73EF,   C0, AMD Radeon RX 6800S",
    "73EF,   C1, AMD Radeon RX 6650 XT",
    "73EF,   C2, AMD Radeon RX 6700S",
    "73EF,   C3, AMD Radeon RX 6650M",
    "73EF,   C4, AMD Radeon RX 6650M XT",
    "73FF,   C1, AMD Radeon RX 6600 XT",
    "73FF,   C3, AMD Radeon RX 6600M",
    "73FF,   C7, AMD Radeon RX 6600",
    "73FF,   CB, AMD Radeon RX 6600S",
    "7408,   00, AMD Instinct MI250X",
    "740C,   01, AMD Instinct MI250X / MI250",
    "740F,   02, AMD Instinct MI210",
    "7421,   00, AMD Radeon Pro W6500M",
    "7422,   00, AMD Radeon Pro W6400",
    "7423,   00, AMD Radeon Pro W6300M",
    "7423,   01, AMD Radeon Pro W6300",
    "7424,   00, AMD Radeon RX 6300",
    "743F,   C1, AMD Radeon RX 6500 XT",
    "743F,   C3, AMD Radeon RX 6500",
    "743F,   C3, AMD Radeon RX 6500M",
    "743F,   C7, AMD Radeon RX 6400",
    "743F,   CF, AMD Radeon RX 6300M",
    "9830,   00, AMD Radeon HD 8400 / R3 Series",
    "9831,   00, AMD Radeon HD 8400E",
    "9832,   00, AMD Radeon HD 8330",
    "9833,   00, AMD Radeon HD 8330E",
    "9834,   00, AMD Radeon HD 8210",
    "9835,   00, AMD Radeon HD 8210E",
    "9836,   00, AMD Radeon HD 8200 / R3 Series",
    "9837,   00, AMD Radeon HD 8280E",
    "9838,   00, AMD Radeon HD 8200 / R3 series",
    "9839,   00, AMD Radeon HD 8180",
    "983D,   00, AMD Radeon HD 8250",
    "9850,   00, AMD Radeon R3 Graphics",
    "9850,   03, AMD Radeon R3 Graphics",
    "9850,   40, AMD Radeon R2 Graphics",
    "9850,   45, AMD Radeon R3 Graphics",
    "9851,   00, AMD Radeon R4 Graphics",
    "9851,   01, AMD Radeon R5E Graphics",
    "9851,   05, AMD Radeon R5 Graphics",
    "9851,   06, AMD Radeon R5E Graphics",
    "9851,   40, AMD Radeon R4 Graphics",
    "9851,   45, AMD Radeon R5 Graphics",
    "9852,   00, AMD Radeon R2 Graphics",
    "9852,   40, AMD Radeon E1 Graphics",
    "9853,   00, AMD Radeon R2 Graphics",
    "9853,   01, AMD Radeon R4E Graphics",
    "9853,   03, AMD Radeon R2 Graphics",
    "9853,   05, AMD Radeon R1E Graphics",
    "9853,   06, AMD Radeon R1E Graphics",
    "9853,   07, AMD Radeon R1E Graphics",
    "9853,   08, AMD Radeon R1E Graphics",
    "9853,   40, AMD Radeon R2 Graphics",
    "9854,   00, AMD Radeon R3 Graphics",
    "9854,   01, AMD Radeon R3E Graphics",
    "9854,   02, AMD Radeon R3 Graphics",
    "9854,   05, AMD Radeon R2 Graphics",
    "9854,   06, AMD Radeon R4 Graphics",
    "9854,   07, AMD Radeon R3 Graphics",
    "9855,   02, AMD Radeon R6 Graphics",
    "9855,   05, AMD Radeon R4 Graphics",
    "9856,   00, AMD Radeon R2 Graphics",
    "9856,   01, AMD Radeon R2E Graphics",
    "9856,   02, AMD Radeon R2 Graphics",
    "9856,   05, AMD Radeon R1E Graphics",
    "9856,   06, AMD Radeon R2 Graphics",
    "9856,   07, AMD Radeon R1E Graphics",
    "9856,   08, AMD Radeon R1E Graphics",
    "9856,   13, AMD Radeon R1E Graphics",
    "9874,   81, AMD Radeon R6 Graphics",
    "9874,   84, AMD Radeon R7 Graphics",
    "9874,   85, AMD Radeon R6 Graphics",
    "9874,   87, AMD Radeon R5 Graphics",
    "9874,   88, AMD Radeon R7E Graphics",
    "9874,   89, AMD Radeon R6E Graphics",
    "9874,   C4, AMD Radeon R7 Graphics",
    "9874,   C5, AMD Radeon R6 Graphics",
    "9874,   C6, AMD Radeon R6 Graphics",
    "9874,   C7, AMD Radeon R5 Graphics",
    "9874,   C8, AMD Radeon R7 Graphics",
    "9874,   C9, AMD Radeon R7 Graphics",
    "9874,   CA, AMD Radeon R5 Graphics",
    "9874,   CB, AMD Radeon R5 Graphics",
    "9874,   CC, AMD Radeon R7 Graphics",
    "9874,   CD, AMD Radeon R7 Graphics",
    "9874,   CE, AMD Radeon R5 Graphics",
    "9874,   E1, AMD Radeon R7 Graphics",
    "9874,   E2, AMD Radeon R7 Graphics",
    "9874,   E3, AMD Radeon R7 Graphics",
    "9874,   E4, AMD Radeon R7 Graphics",
    "9874,   E5, AMD Radeon R5 Graphics",
    "9874,   E6, AMD Radeon R5 Graphics",
    "98E4,   80, AMD Radeon R5E Graphics",
    "98E4,   81, AMD Radeon R4E Graphics",
    "98E4,   83, AMD Radeon R2E Graphics",
    "98E4,   84, AMD Radeon R2E Graphics",
    "98E4,   86, AMD Radeon R1E Graphics",
    "98E4,   C0, AMD Radeon R4 Graphics",
    "98E4,   C1, AMD Radeon R5 Graphics",
    "98E4,   C2, AMD Radeon R4 Graphics",
    "98E4,   C4, AMD Radeon R5 Graphics",
    "98E4,   C6, AMD Radeon R5 Graphics",
    "98E4,   C8, AMD Radeon R4 Graphics",
    "98E4,   C9, AMD Radeon R4 Graphics",
    "98E4,   CA, AMD Radeon R5 Graphics",
    "98E4,   D0, AMD Radeon R2 Graphics",
    "98E4,   D1, AMD Radeon R2 Graphics",
    "98E4,   D2, AMD Radeon R2 Graphics",
    "98E4,   D4, AMD Radeon R2 Graphics",
    "98E4,   D9, AMD Radeon R5 Graphics",
    "98E4,   DA, AMD Radeon R5 Graphics",
    "98E4,   DB, AMD Radeon R3 Graphics",
    "98E4,   E1, AMD Radeon R3 Graphics",
    "98E4,   E2, AMD Radeon R3 Graphics",
    "98E4,   E9, AMD Radeon R4 Graphics",
    "98E4,   EA, AMD Radeon R4 Graphics",
    "98E4,   EB, AMD Radeon R3 Graphics",
    "98E4,   EB, AMD Radeon R4 Graphics",
};

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

#if 1
void amdgpu_parse_asic_ids(struct amdgpu_device *dev)
{
    for (size_t i = 0; i < ARRAY_SIZE(amdgpu_ids_array); ++i) {
        char *line = (char *)amdgpu_ids_array[i];

        size_t n = strlen(line);
        if (line[n - 1] == '\n') {
            line[n - 1] = '\0';
        }

        if ((strlen(line) == 0) || (line[0] == '#')) {
            continue;
        }

        int r = parse_one_line(dev, line);
        if (r != -EAGAIN) {
            break;
        }
    }
}
#else
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
#endif
