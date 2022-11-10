#include <stdio.h>
#include <stdlib.h>

#include "../config.h"

#if defined(CONFIG_JPEG)
#include <jerror.h>
#include <jpeglib.h>

#include "jpeg_memsrcdest.h"

#if (JPEG_LIB_VERSION < 80) && !defined(MEM_SRCDST_SUPPORTED)

typedef struct {
    struct jpeg_source_mgr pub;
    JOCTET                 eoi_buffer[2];
} my_source_mgr;

typedef my_source_mgr * my_src_ptr;

METHODDEF(void) init_source(j_decompress_ptr cinfo)
{

}

METHODDEF(boolean) fill_input_buffer(j_decompress_ptr cinfo)
{
    my_src_ptr src = (my_src_ptr) cinfo->src;

    WARNMS(cinfo, JWRN_JPEG_EOF);

    src->eoi_buffer[0] = (JOCTET) 0xFF;
    src->eoi_buffer[1] = (JOCTET) JPEG_EOI;
    src->pub.next_input_byte = src->eoi_buffer;
    src->pub.bytes_in_buffer = 2;

    return TRUE;
}

METHODDEF(void) skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
    my_src_ptr src = (my_src_ptr) cinfo->src;

    if (num_bytes > 0) {
        while (num_bytes > (long)src->pub.bytes_in_buffer) {
            num_bytes -= (long)src->pub.bytes_in_buffer;
            (void)fill_input_buffer(cinfo);
        }

        src->pub.next_input_byte += (size_t) num_bytes;
        src->pub.bytes_in_buffer -= (size_t) num_bytes;
    }
}

METHODDEF(void) term_source(j_decompress_ptr cinfo)
{

}

GLOBAL(void) jpeg_mem_src(j_decompress_ptr cinfo, unsigned char *buffer, unsigned long bufsize)
{
    my_src_ptr src;

    if (cinfo->src == NULL) {
        cinfo->src = (struct jpeg_source_mgr *)(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT, sizeof(my_source_mgr));
    }

    src = (my_src_ptr) cinfo->src;
    src->pub.init_source = init_source;
    src->pub.fill_input_buffer = fill_input_buffer;
    src->pub.skip_input_data = skip_input_data;
    src->pub.resync_to_restart = jpeg_resync_to_restart;
    src->pub.term_source = term_source;

    src->pub.next_input_byte = buffer;
    src->pub.bytes_in_buffer = bufsize;
}

typedef struct {
    struct jpeg_destination_mgr pub;
    JOCTET                      **buffer;
    unsigned long               buf_size, *outsize;
} my_destination_mgr;

typedef my_destination_mgr * my_dest_ptr;

#define OUTPUT_BUF_SIZE         32768

METHODDEF(void) init_destination(j_compress_ptr cinfo)
{

}

METHODDEF(boolean) empty_output_buffer(j_compress_ptr cinfo)
{
    my_dest_ptr dest = (my_dest_ptr) cinfo->dest;
    *dest->buffer = realloc(*dest->buffer, dest->buf_size + OUTPUT_BUF_SIZE);
    if (!*dest->buffer) {
        ERREXIT1(cinfo, JERR_OUT_OF_MEMORY, 0);
    }

    dest->pub.next_output_byte = *dest->buffer + dest->buf_size;
    dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
    dest->buf_size += OUTPUT_BUF_SIZE;

    return TRUE;
}

METHODDEF(void) term_destination(j_compress_ptr cinfo)
{
    my_dest_ptr dest = (my_dest_ptr)cinfo->dest;
    *dest->outsize = dest->buf_size - dest->pub.free_in_buffer;
}

GLOBAL(void) jpeg_mem_dest(j_compress_ptr cinfo, unsigned char ** outbuffer, unsigned long * outsize)
{
    my_dest_ptr dest;

    if (cinfo->dest == NULL) {
        cinfo->dest = (struct jpeg_destination_mgr *)(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT, sizeof(my_destination_mgr));
    }

    dest = (my_dest_ptr) cinfo->dest;
    dest->pub.init_destination = init_destination;
    dest->pub.empty_output_buffer = empty_output_buffer;
    dest->pub.term_destination = term_destination;
    dest->buffer = outbuffer;
    dest->buf_size = *outsize;
    dest->outsize = outsize;

    if ((*dest->buffer == NULL) || (dest->buf_size == 0)) {
        *dest->buffer = malloc(OUTPUT_BUF_SIZE);
        if (*dest->buffer == NULL) {
            ERREXIT1(cinfo, JERR_OUT_OF_MEMORY, 10);
        }

        dest->buf_size = OUTPUT_BUF_SIZE;
    }

    dest->pub.next_output_byte = *dest->buffer;
    dest->pub.free_in_buffer = dest->buf_size;
}

#endif

#endif
