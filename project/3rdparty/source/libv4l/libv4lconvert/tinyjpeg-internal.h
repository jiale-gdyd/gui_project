#ifndef LIBV4L_TINYJPEG_INTERNAL_H
#define LIBV4L_TINYJPEG_INTERNAL_H

#include <setjmp.h>

#define SANITY_CHECK            1

struct jdec_private;

#define HUFFMAN_HASH_NBITS      9
#define HUFFMAN_HASH_SIZE       (1UL<<HUFFMAN_HASH_NBITS)
#define HUFFMAN_HASH_MASK       (HUFFMAN_HASH_SIZE-1)

#define HUFFMAN_TABLES          4
#define COMPONENTS              3
#define JPEG_MAX_WIDTH          2048
#define JPEG_MAX_HEIGHT         2048

struct huffman_table {
    short int     lookup[HUFFMAN_HASH_SIZE];
    unsigned char code_size[HUFFMAN_HASH_SIZE];
    uint16_t      slowtable[16 - HUFFMAN_HASH_NBITS][256];
};

struct component {
    unsigned int         Hfactor;
    unsigned int         Vfactor;
    float                *Q_table;
    struct huffman_table *AC_table;
    struct huffman_table *DC_table;
    short int            previous_DC;
    short int            DCT[64];
#if SANITY_CHECK
    unsigned int         cid;
#endif
};

typedef void (*decode_MCU_fct)(struct jdec_private *priv);
typedef void (*convert_colorspace_fct)(struct jdec_private *priv);

struct jdec_private {
    uint8_t              *components[COMPONENTS];
    unsigned int         width, height;
    unsigned int         flags;

    const unsigned char  *stream_end;
    const unsigned char  *stream;
    unsigned char        *stream_filtered;
    int                  stream_filtered_bufsize;
    unsigned int         reservoir, nbits_in_reservoir;

    struct component     component_infos[COMPONENTS];
    float                Q_tables[COMPONENTS][64];
    struct huffman_table HTDC[HUFFMAN_TABLES];
    struct huffman_table HTAC[HUFFMAN_TABLES];
    int                  default_huffman_table_initialized;
    int                  restart_interval;
    int                  restarts_to_go;
    int                  last_rst_marker_seen;
#if SANITY_CHECK
    unsigned int         current_cid;
#endif
    unsigned char        marker;
    unsigned char        first_marker;

    uint8_t              Y[64 * 4], Cr[64], Cb[64];

    jmp_buf              jump_state;
    uint8_t              *plane[COMPONENTS];

    char                 error_string[256];

    int                  tmp_buf_y_size;
    uint8_t              *tmp_buf[COMPONENTS];
};

#define IDCT tinyjpeg_idct_float
void tinyjpeg_idct_float (struct component *compptr, uint8_t *output_buf, int stride);

#endif
