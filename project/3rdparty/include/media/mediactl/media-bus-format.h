#ifndef LIBDRM_MEDIA_BUS_FORMAT_H
#define LIBDRM_MEDIA_BUS_FORMAT_H

#define MEDIA_BUS_FMT_FIXED                             0x0001

#define MEDIA_BUS_FMT_RGB444_1X12                       0x1016
#define MEDIA_BUS_FMT_RGB444_2X8_PADHI_BE               0x1001
#define MEDIA_BUS_FMT_RGB444_2X8_PADHI_LE               0x1002
#define MEDIA_BUS_FMT_RGB555_2X8_PADHI_BE               0x1003
#define MEDIA_BUS_FMT_RGB555_2X8_PADHI_LE               0x1004
#define MEDIA_BUS_FMT_RGB565_1X16                       0x1017
#define MEDIA_BUS_FMT_BGR565_2X8_BE                     0x1005
#define MEDIA_BUS_FMT_BGR565_2X8_LE                     0x1006
#define MEDIA_BUS_FMT_RGB565_2X8_BE                     0x1007
#define MEDIA_BUS_FMT_RGB565_2X8_LE                     0x1008
#define MEDIA_BUS_FMT_RGB666_1X18                       0x1009
#define MEDIA_BUS_FMT_RBG888_1X24                       0x100e
#define MEDIA_BUS_FMT_RGB666_1X24_CPADHI                0x1015
#define MEDIA_BUS_FMT_RGB666_1X7X3_SPWG                 0x1010
#define MEDIA_BUS_FMT_BGR888_1X24                       0x1013
#define MEDIA_BUS_FMT_BGR888_3X8                        0x101b
#define MEDIA_BUS_FMT_GBR888_1X24                       0x1014
#define MEDIA_BUS_FMT_RGB888_1X24                       0x100a
#define MEDIA_BUS_FMT_RGB888_2X12_BE                    0x100b
#define MEDIA_BUS_FMT_RGB888_2X12_LE                    0x100c
#define MEDIA_BUS_FMT_RGB888_3X8                        0x101c
#define MEDIA_BUS_FMT_RGB888_3X8_DELTA                  0x101d
#define MEDIA_BUS_FMT_RGB888_1X7X4_SPWG                 0x1011
#define MEDIA_BUS_FMT_RGB888_1X7X4_JEIDA                0x1012
#define MEDIA_BUS_FMT_RGB666_1X30_CPADLO                0x101e
#define MEDIA_BUS_FMT_RGB888_1X30_CPADLO                0x101f
#define MEDIA_BUS_FMT_ARGB8888_1X32                     0x100d
#define MEDIA_BUS_FMT_RGB888_1X32_PADHI                 0x100f
#define MEDIA_BUS_FMT_RGB101010_1X30                    0x1018
#define MEDIA_BUS_FMT_RGB666_1X36_CPADLO                0x1020
#define MEDIA_BUS_FMT_RGB888_1X36_CPADLO                0x1021
#define MEDIA_BUS_FMT_RGB121212_1X36                    0x1019
#define MEDIA_BUS_FMT_RGB161616_1X48                    0x101a

#define MEDIA_BUS_FMT_Y8_1X8                            0x2001
#define MEDIA_BUS_FMT_UV8_1X8                           0x2015
#define MEDIA_BUS_FMT_UYVY8_1_5X8                       0x2002
#define MEDIA_BUS_FMT_VYUY8_1_5X8                       0x2003
#define MEDIA_BUS_FMT_YUYV8_1_5X8                       0x2004
#define MEDIA_BUS_FMT_YVYU8_1_5X8                       0x2005
#define MEDIA_BUS_FMT_UYVY8_2X8                         0x2006
#define MEDIA_BUS_FMT_VYUY8_2X8                         0x2007
#define MEDIA_BUS_FMT_YUYV8_2X8                         0x2008
#define MEDIA_BUS_FMT_YVYU8_2X8                         0x2009
#define MEDIA_BUS_FMT_Y10_1X10                          0x200a
#define MEDIA_BUS_FMT_Y10_2X8_PADHI_LE                  0x202c
#define MEDIA_BUS_FMT_UYVY10_2X10                       0x2018
#define MEDIA_BUS_FMT_VYUY10_2X10                       0x2019
#define MEDIA_BUS_FMT_YUYV10_2X10                       0x200b
#define MEDIA_BUS_FMT_YVYU10_2X10                       0x200c
#define MEDIA_BUS_FMT_Y12_1X12                          0x2013
#define MEDIA_BUS_FMT_UYVY12_2X12                       0x201c
#define MEDIA_BUS_FMT_VYUY12_2X12                       0x201d
#define MEDIA_BUS_FMT_YUYV12_2X12                       0x201e
#define MEDIA_BUS_FMT_YVYU12_2X12                       0x201f
#define MEDIA_BUS_FMT_Y14_1X14                          0x202d
#define MEDIA_BUS_FMT_UYVY8_1X16                        0x200f
#define MEDIA_BUS_FMT_VYUY8_1X16                        0x2010
#define MEDIA_BUS_FMT_YUYV8_1X16                        0x2011
#define MEDIA_BUS_FMT_YVYU8_1X16                        0x2012
#define MEDIA_BUS_FMT_YDYUYDYV8_1X16                    0x2014
#define MEDIA_BUS_FMT_UYVY10_1X20                       0x201a
#define MEDIA_BUS_FMT_VYUY10_1X20                       0x201b
#define MEDIA_BUS_FMT_YUYV10_1X20                       0x200d
#define MEDIA_BUS_FMT_YVYU10_1X20                       0x200e
#define MEDIA_BUS_FMT_VUY8_1X24                         0x2024
#define MEDIA_BUS_FMT_YUV8_1X24                         0x2025
#define MEDIA_BUS_FMT_UYYVYY8_0_5X24                    0x2026
#define MEDIA_BUS_FMT_UYVY12_1X24                       0x2020
#define MEDIA_BUS_FMT_VYUY12_1X24                       0x2021
#define MEDIA_BUS_FMT_YUYV12_1X24                       0x2022
#define MEDIA_BUS_FMT_YVYU12_1X24                       0x2023
#define MEDIA_BUS_FMT_YUV10_1X30                        0x2016
#define MEDIA_BUS_FMT_UYYVYY10_0_5X30                   0x2027
#define MEDIA_BUS_FMT_AYUV8_1X32                        0x2017
#define MEDIA_BUS_FMT_UYYVYY12_0_5X36                   0x2028
#define MEDIA_BUS_FMT_YUV12_1X36                        0x2029
#define MEDIA_BUS_FMT_YUV16_1X48                        0x202a
#define MEDIA_BUS_FMT_UYYVYY16_0_5X48                   0x202b

#define MEDIA_BUS_FMT_SBGGR8_1X8                        0x3001
#define MEDIA_BUS_FMT_SGBRG8_1X8                        0x3013
#define MEDIA_BUS_FMT_SGRBG8_1X8                        0x3002
#define MEDIA_BUS_FMT_SRGGB8_1X8                        0x3014
#define MEDIA_BUS_FMT_SBGGR10_ALAW8_1X8                 0x3015
#define MEDIA_BUS_FMT_SGBRG10_ALAW8_1X8                 0x3016
#define MEDIA_BUS_FMT_SGRBG10_ALAW8_1X8                 0x3017
#define MEDIA_BUS_FMT_SRGGB10_ALAW8_1X8                 0x3018
#define MEDIA_BUS_FMT_SBGGR10_DPCM8_1X8                 0x300b
#define MEDIA_BUS_FMT_SGBRG10_DPCM8_1X8                 0x300c
#define MEDIA_BUS_FMT_SGRBG10_DPCM8_1X8                 0x3009
#define MEDIA_BUS_FMT_SRGGB10_DPCM8_1X8                 0x300d
#define MEDIA_BUS_FMT_SBGGR10_2X8_PADHI_BE              0x3003
#define MEDIA_BUS_FMT_SBGGR10_2X8_PADHI_LE              0x3004
#define MEDIA_BUS_FMT_SBGGR10_2X8_PADLO_BE              0x3005
#define MEDIA_BUS_FMT_SBGGR10_2X8_PADLO_LE              0x3006
#define MEDIA_BUS_FMT_SBGGR10_1X10                      0x3007
#define MEDIA_BUS_FMT_SGBRG10_1X10                      0x300e
#define MEDIA_BUS_FMT_SGRBG10_1X10                      0x300a
#define MEDIA_BUS_FMT_SRGGB10_1X10                      0x300f
#define MEDIA_BUS_FMT_SBGGR12_1X12                      0x3008
#define MEDIA_BUS_FMT_SGBRG12_1X12                      0x3010
#define MEDIA_BUS_FMT_SGRBG12_1X12                      0x3011
#define MEDIA_BUS_FMT_SRGGB12_1X12                      0x3012
#define MEDIA_BUS_FMT_SBGGR14_1X14                      0x3019
#define MEDIA_BUS_FMT_SGBRG14_1X14                      0x301a
#define MEDIA_BUS_FMT_SGRBG14_1X14                      0x301b
#define MEDIA_BUS_FMT_SRGGB14_1X14                      0x301c
#define MEDIA_BUS_FMT_SBGGR16_1X16                      0x301d
#define MEDIA_BUS_FMT_SGBRG16_1X16                      0x301e
#define MEDIA_BUS_FMT_SGRBG16_1X16                      0x301f
#define MEDIA_BUS_FMT_SRGGB16_1X16                      0x3020

#define MEDIA_BUS_FMT_JPEG_1X8                          0x4001

#define MEDIA_BUS_FMT_S5C_UYVY_JPEG_1X8                 0x5001

#define MEDIA_BUS_FMT_AHSV8888_1X32                     0x6001

#define MEDIA_BUS_FMT_METADATA_FIXED                    0x7001

#endif
