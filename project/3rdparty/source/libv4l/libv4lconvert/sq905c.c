#include <stdlib.h>
#include "libv4lconvert-priv.h"

#define CLIP(x)         ((x) < 0 ? 0 : ((x) > 0xff) ? 0xff : (x))

static int sq905c_first_decompress(unsigned char *output, const unsigned char *input, unsigned int outputsize)
{
    unsigned int i = 0;
    unsigned int cycles = 0;
    unsigned char parity = 0;
    unsigned char input_byte;
    unsigned char lookup = 0;
    unsigned int bytes_used = 0;
    unsigned int bytes_done = 0;
    unsigned int bit_counter = 8;
    unsigned char nibble_to_keep[2];
    unsigned char temp1 = 0, temp2 = 0;
    int table[9] = {-1, 0, 2, 6, 0x0e, 0x0e, 0x0e, 0x0e, 0xfb};
    unsigned char translator[16] = {8, 7, 9, 6, 10, 11, 12, 13, 14, 15, 5, 4, 3, 2, 1, 0};
    unsigned char lookup_table[16] = {0, 2, 6, 0x0e, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb};

    nibble_to_keep[0] = 0;
    nibble_to_keep[1] = 0;

    while (bytes_done < outputsize) {
        while (parity < 2) {
            while (lookup > table[cycles]) {
                if (bit_counter == 8) {
                    input_byte = input[bytes_used];
                    bytes_used++;
                    temp1 = input_byte;
                    bit_counter = 0;
                }

                input_byte = temp1;
                temp2 = (temp2 << 1) & 0xFF;
                input_byte = input_byte >> 7;
                temp2 = temp2 | input_byte;
                temp1 = (temp1 << 1) & 0xFF;
                bit_counter++;
                cycles++;

                if (cycles > 8) {
                    return -1;
                }
                lookup = temp2 & 0xff;
            }

            temp2 = 0;
            for (i = 0; i < 17; i++) {
                if (i == 16) {
                    return -1;
                }

                if (lookup == lookup_table[i]) {
                    nibble_to_keep[parity] = translator[i];
                    break;
                }
            }

            cycles = 0;
            parity++;
        }

        output[bytes_done] = (nibble_to_keep[0]<<4)|nibble_to_keep[1];
        bytes_done++;
        parity = 0;
    }

    return 0;
}

static int sq905c_second_decompress(unsigned char *uncomp, unsigned char *in, int width, int height)
{
    int i, m;
    int diff = 0;
    int tempval = 0;
    int input_counter = 0;
    unsigned char *templine_red;
    unsigned char delta_left = 0;
    unsigned char *templine_blue;
    unsigned char delta_right = 0;
    unsigned char *templine_green;
    int delta_table[] = {-144, -110, -77, -53, -35, -21, -11, -3, 2, 10, 20, 34, 52, 76, 110, 144 };

    templine_red = malloc(width);
    if (!templine_red) {
        return -1;
    }

    for (i = 0; i < width; i++) {
        templine_red[i] = 0x80;
    }

    templine_green = malloc(width);
    if (!templine_green) {
        free(templine_red);
        return -1;
    }

    for (i = 0; i < width; i++) {
        templine_green[i] = 0x80;
    }

    templine_blue = malloc(width);
    if (!templine_blue) {
        free(templine_red);
        free(templine_green);
        return -1;
    }

    for (i = 0; i < width; i++) {
        templine_blue[i] = 0x80;
    }

    for (m = 0; m < height / 2; m++) {
        for (i = 0; i < width / 2; i++) {
            delta_right = in[input_counter] & 0x0f;
            delta_left = (in[input_counter] >> 4) & 0xff;
            input_counter++;

            diff = delta_table[delta_left];
            if (!i) {
                tempval = templine_red[0] + diff;
            } else {
                tempval = (templine_red[i] + uncomp[2 * m * width + 2 * i - 2]) / 2 + diff;
            }

            tempval = CLIP(tempval);
            uncomp[2 * m * width + 2 * i] = tempval;
            templine_red[i] = tempval;

            diff = delta_table[delta_right];
            if (!i) {
                tempval = templine_green[1] + diff;
            } else if (2 * i == width - 2) {
                tempval = (templine_green[i] + uncomp[2 * m * width + 2 * i - 1]) / 2 + diff;
            } else {
                tempval = (templine_green[i + 1] + uncomp[2 * m * width + 2 * i - 1]) / 2 + diff;
            }

            tempval = CLIP(tempval);
            uncomp[2 * m * width + 2 * i + 1] = tempval;
            templine_green[i] = tempval;
        }

        for (i = 0; i < width/2; i++) {
            delta_right = in[input_counter] & 0x0f;
            delta_left = (in[input_counter] >> 4) & 0xff;
            input_counter++;

            diff = delta_table[delta_left];
            if (!i) {
                tempval = templine_green[0] + diff;
            } else {
                tempval = (templine_green[i] + uncomp[(2 * m + 1) * width + 2 * i - 2]) / 2 + diff;
            }

            tempval = CLIP(tempval);
            uncomp[(2*m+1)*width+2*i] = tempval;
            templine_green[i] = tempval;

            diff = delta_table[delta_right];
            if (!i) {
                tempval = templine_blue[0] + diff;
            } else {
                tempval = (templine_blue[i] + uncomp[(2 * m + 1) * width + 2 * i - 1]) / 2 + diff;
            }

            tempval = CLIP(tempval);
            uncomp[(2 * m + 1) * width + 2 * i + 1] = tempval;
            templine_blue[i] = tempval;
        }
    }

    free(templine_green);
    free(templine_red);
    free(templine_blue);

    return 0;
}

void v4lconvert_decode_sq905c(const unsigned char *src, unsigned char *dst, int width, int height)
{
    int size;
    unsigned char *temp_data;
    const unsigned char *raw;

    raw = src + 0x50;
    size = width * height / 2;
    temp_data = malloc(size);
    if (!temp_data) {
        goto out;
    }

    sq905c_first_decompress(temp_data, raw, size);
    sq905c_second_decompress(dst, temp_data, width, height);

out:
    free(temp_data);
}
