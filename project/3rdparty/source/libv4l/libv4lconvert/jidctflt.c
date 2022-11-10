#include <stdint.h>

#include "tinyjpeg-internal.h"

#define FAST_FLOAT                      float
#define DCTSIZE                         8
#define DCTSIZE2                        (DCTSIZE * DCTSIZE)

#define DEQUANTIZE(coef, quantval)      (((FAST_FLOAT) (coef)) * (quantval))

#if defined(__GNUC__) && (defined(__i686__) || defined(__x86_64__))
static inline unsigned char descale_and_clamp(int x, int shift)
{
    __asm__ (
        "add %3,%1\n"
        "\tsar %2,%1\n"
        "\tsub $-128,%1\n"
        "\tcmovl %5,%1\n"
        "\tcmpl %4,%1\n"
        "\tcmovg %4,%1\n"
        : "=r"(x)
        : "0"(x), "Ic"((unsigned char)shift), "ir" (1U << (shift - 1)), "r" (0xff), "r" (0)
    );

    return x;
}
#else
static inline unsigned char descale_and_clamp(int x, int shift)
{
    x += 1UL << (shift - 1);
    if (x < 0) {
        x = (x >> shift) | ((~(0UL)) << (32 - (shift)));
    } else {
        x >>= shift;
    }

    x += 128;
    if (x > 255) {
        return 255;
    }

    if (x < 0) {
        return 0;
    }

    return x;
}
#endif

void tinyjpeg_idct_float(struct component *compptr, uint8_t *output_buf, int stride)
{
    int ctr;
    int16_t *inptr;
    uint8_t *outptr;
    FAST_FLOAT *wsptr;
    FAST_FLOAT *quantptr;
    FAST_FLOAT workspace[DCTSIZE2];
    FAST_FLOAT z5, z10, z11, z12, z13;
    FAST_FLOAT tmp10, tmp11, tmp12, tmp13;
    FAST_FLOAT tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

    inptr = compptr->DCT;
    quantptr = compptr->Q_table;
    wsptr = workspace;

    for (ctr = DCTSIZE; ctr > 0; ctr--) {
        if (inptr[DCTSIZE * 1] == 0
            && inptr[DCTSIZE * 2] == 0
            && inptr[DCTSIZE * 3] == 0
            && inptr[DCTSIZE * 4] == 0
            && inptr[DCTSIZE * 5] == 0
            && inptr[DCTSIZE * 6] == 0
            && inptr[DCTSIZE * 7] == 0)
        {
            FAST_FLOAT dcval = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);

            wsptr[DCTSIZE*0] = dcval;
            wsptr[DCTSIZE*1] = dcval;
            wsptr[DCTSIZE*2] = dcval;
            wsptr[DCTSIZE*3] = dcval;
            wsptr[DCTSIZE*4] = dcval;
            wsptr[DCTSIZE*5] = dcval;
            wsptr[DCTSIZE*6] = dcval;
            wsptr[DCTSIZE*7] = dcval;

            inptr++;
            quantptr++;
            wsptr++;
            continue;
        }

        tmp0 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
        tmp1 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
        tmp2 = DEQUANTIZE(inptr[DCTSIZE*4], quantptr[DCTSIZE*4]);
        tmp3 = DEQUANTIZE(inptr[DCTSIZE*6], quantptr[DCTSIZE*6]);

        tmp10 = tmp0 + tmp2;
        tmp11 = tmp0 - tmp2;

        tmp13 = tmp1 + tmp3;
        tmp12 = (tmp1 - tmp3) * ((FAST_FLOAT) 1.414213562) - tmp13;

        tmp0 = tmp10 + tmp13;
        tmp3 = tmp10 - tmp13;
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;

        tmp4 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
        tmp5 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
        tmp6 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);
        tmp7 = DEQUANTIZE(inptr[DCTSIZE*7], quantptr[DCTSIZE*7]);

        z13 = tmp6 + tmp5;
        z10 = tmp6 - tmp5;
        z11 = tmp4 + tmp7;
        z12 = tmp4 - tmp7;

        tmp7 = z11 + z13;
        tmp11 = (z11 - z13) * ((FAST_FLOAT) 1.414213562);

        z5 = (z10 + z12) * ((FAST_FLOAT) 1.847759065);
        tmp10 = ((FAST_FLOAT) 1.082392200) * z12 - z5;
        tmp12 = ((FAST_FLOAT) -2.613125930) * z10 + z5;

        tmp6 = tmp12 - tmp7;
        tmp5 = tmp11 - tmp6;
        tmp4 = tmp10 + tmp5;

        wsptr[DCTSIZE*0] = tmp0 + tmp7;
        wsptr[DCTSIZE*7] = tmp0 - tmp7;
        wsptr[DCTSIZE*1] = tmp1 + tmp6;
        wsptr[DCTSIZE*6] = tmp1 - tmp6;
        wsptr[DCTSIZE*2] = tmp2 + tmp5;
        wsptr[DCTSIZE*5] = tmp2 - tmp5;
        wsptr[DCTSIZE*4] = tmp3 + tmp4;
        wsptr[DCTSIZE*3] = tmp3 - tmp4;

        inptr++;
        quantptr++;
        wsptr++;
    }

    wsptr = workspace;
    outptr = output_buf;

    for (ctr = 0; ctr < DCTSIZE; ctr++) {
        tmp10 = wsptr[0] + wsptr[4];
        tmp11 = wsptr[0] - wsptr[4];

        tmp13 = wsptr[2] + wsptr[6];
        tmp12 = (wsptr[2] - wsptr[6]) * ((FAST_FLOAT) 1.414213562) - tmp13;

        tmp0 = tmp10 + tmp13;
        tmp3 = tmp10 - tmp13;
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;

        z13 = wsptr[5] + wsptr[3];
        z10 = wsptr[5] - wsptr[3];
        z11 = wsptr[1] + wsptr[7];
        z12 = wsptr[1] - wsptr[7];

        tmp7 = z11 + z13;
        tmp11 = (z11 - z13) * ((FAST_FLOAT) 1.414213562);

        z5 = (z10 + z12) * ((FAST_FLOAT) 1.847759065);
        tmp10 = ((FAST_FLOAT) 1.082392200) * z12 - z5;
        tmp12 = ((FAST_FLOAT) -2.613125930) * z10 + z5;

        tmp6 = tmp12 - tmp7;
        tmp5 = tmp11 - tmp6;
        tmp4 = tmp10 + tmp5;

        outptr[0] = descale_and_clamp((int)(tmp0 + tmp7), 3);
        outptr[7] = descale_and_clamp((int)(tmp0 - tmp7), 3);
        outptr[1] = descale_and_clamp((int)(tmp1 + tmp6), 3);
        outptr[6] = descale_and_clamp((int)(tmp1 - tmp6), 3);
        outptr[2] = descale_and_clamp((int)(tmp2 + tmp5), 3);
        outptr[5] = descale_and_clamp((int)(tmp2 - tmp5), 3);
        outptr[4] = descale_and_clamp((int)(tmp3 + tmp4), 3);
        outptr[3] = descale_and_clamp((int)(tmp3 - tmp4), 3);

        wsptr += DCTSIZE;
        outptr += stride;
    }
}
