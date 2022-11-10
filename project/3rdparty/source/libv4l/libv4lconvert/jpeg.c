#include <errno.h>
#include <stdlib.h>

#include "../config.h"
#include "libv4lconvert-priv.h"

#if defined(CONFIG_JPEG)
#include "jpeg_memsrcdest.h"
#endif

int v4lconvert_decode_jpeg_tinyjpeg(struct v4lconvert_data *data, unsigned char *src, int src_size, unsigned char *dest, struct v4l2_format *fmt, unsigned int dest_pix_fmt, int flags)
{
    int result = 0;
    unsigned char *components[3];
    unsigned int header_width, header_height;
    unsigned int width = fmt->fmt.pix.width;
    unsigned int height = fmt->fmt.pix.height;

    if (!data->tinyjpeg) {
        data->tinyjpeg = tinyjpeg_init();
        if (!data->tinyjpeg) {
            return v4lconvert_oom_error(data);
        }
    }

    flags |= TINYJPEG_FLAGS_MJPEG_TABLE;
    tinyjpeg_set_flags(data->tinyjpeg, flags);
    if (tinyjpeg_parse_header(data->tinyjpeg, src, src_size)) {
        V4LCONVERT_ERR("parsing JPEG header: %s", tinyjpeg_get_errorstring(data->tinyjpeg));
        errno = EAGAIN;
        return -1;
    }

    tinyjpeg_get_size(data->tinyjpeg, &header_width, &header_height);

    if (data->control_flags & V4LCONTROL_ROTATED_90_JPEG) {
        unsigned int tmp = width;
        width = height;
        height = tmp;
    }

    if (header_width != width || header_height != height) {
        V4LCONVERT_ERR("unexpected width / height in JPEG header: expected: %ux%u, header: %ux%u\n", width, height, header_width, header_height);
        errno = EIO;
        return -1;
    }

    fmt->fmt.pix.width = header_width;
    fmt->fmt.pix.height = header_height;

    components[0] = dest;

    switch (dest_pix_fmt) {
        case V4L2_PIX_FMT_RGB24:
            tinyjpeg_set_components(data->tinyjpeg, components, 1);
            result = tinyjpeg_decode(data->tinyjpeg, TINYJPEG_FMT_RGB24);
            break;

        case V4L2_PIX_FMT_BGR24:
            tinyjpeg_set_components(data->tinyjpeg, components, 1);
            result = tinyjpeg_decode(data->tinyjpeg, TINYJPEG_FMT_BGR24);
            break;

        case V4L2_PIX_FMT_YUV420:
            components[1] = components[0] + width * height;
            components[2] = components[1] + width * height / 4;
            tinyjpeg_set_components(data->tinyjpeg, components, 3);
            result = tinyjpeg_decode(data->tinyjpeg, TINYJPEG_FMT_YUV420P);
            break;

        case V4L2_PIX_FMT_YVU420:
            components[2] = components[0] + width * height;
            components[1] = components[2] + width * height / 4;
            tinyjpeg_set_components(data->tinyjpeg, components, 3);
            result = tinyjpeg_decode(data->tinyjpeg, TINYJPEG_FMT_YUV420P);
            break;
    }

    if (result) {
        V4LCONVERT_ERR("decompressing JPEG: %s", tinyjpeg_get_errorstring(data->tinyjpeg));
        errno = EPIPE;
        return -1;
    }
    return 0;
}

#if defined(CONFIG_JPEG)
static void jerr_error_exit(j_common_ptr cinfo)
{
    struct v4lconvert_data *data = cinfo->client_data;
    longjmp(data->jerr_jmp_state, data->jerr_errno);
}

static void jerr_emit_message(j_common_ptr cinfo, int msg_level)
{
    char buffer[JMSG_LENGTH_MAX];
    struct v4lconvert_data *data = cinfo->client_data;

    if (msg_level < -1) {
        return;
    }

    cinfo->err->format_message(cinfo, buffer);
    snprintf(data->error_msg, V4LCONVERT_ERROR_MSG_SIZE, "v4l-convert: libjpeg error: %s\n", buffer);
}

static void init_libjpeg_cinfo(struct v4lconvert_data *data)
{
    struct jpeg_compress_struct cinfo;
    unsigned char *jpeg_header = NULL;
    unsigned long jpeg_header_size = 0;

    if (data->cinfo_initialized) {
        return;
    }

    jpeg_std_error(&data->jerr);
    data->jerr.error_exit = jerr_error_exit;
    data->jerr.emit_message = jerr_emit_message;

    cinfo.err = &data->jerr;
    cinfo.client_data = data;
    jpeg_create_compress(&cinfo);
    jpeg_mem_dest(&cinfo, &jpeg_header, &jpeg_header_size);
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);
    jpeg_write_tables(&cinfo);
    jpeg_destroy_compress(&cinfo);

    data->cinfo.err = &data->jerr;
    data->cinfo.client_data = data;
    jpeg_create_decompress(&data->cinfo);
    jpeg_mem_src(&data->cinfo, jpeg_header, jpeg_header_size);
    jpeg_read_header(&data->cinfo, FALSE);

    free(jpeg_header);
    data->cinfo_initialized = 1;
}

static int decode_libjpeg_h_samp1(struct v4lconvert_data *data, unsigned char *ydest, unsigned char *udest, unsigned char *vdest, int v_samp)
{
    struct jpeg_decompress_struct *cinfo = &data->cinfo;

    int x, y;
    unsigned char *uv_buf;
    unsigned int width = cinfo->image_width;
    JSAMPROW y_rows[16], u_rows[8], v_rows[8];
    JSAMPARRAY rows[3] = { y_rows, u_rows, v_rows };

    uv_buf = v4lconvert_alloc_buffer(width * 16, &data->convert_pixfmt_buf, &data->convert_pixfmt_buf_size);
    if (!uv_buf) {
        return v4lconvert_oom_error(data);
    }

    for (y = 0; y < 8; y++) {
        u_rows[y] = uv_buf;
        uv_buf += width;
        v_rows[y] = uv_buf;
        uv_buf += width;
    }
    uv_buf -= width * 16;

    while (cinfo->output_scanline < cinfo->image_height) {
        for (y = 0; y < 8 * v_samp; y++) {
            y_rows[y] = ydest;
            ydest += cinfo->image_width;
        }

        y = jpeg_read_raw_data(cinfo, rows, 8 * v_samp);
        if (y != 8 * v_samp) {
            return -1;
        }

        if (cinfo->output_scanline % 16) {
            continue;
        }

        for (y = 0; y < 8; y++) {
            for (x = 0; x < width; x += 2) {
                *udest++ = *uv_buf++;
                uv_buf++;
            }

            for (x = 0; x < width; x += 2) {
                *vdest++ = *uv_buf++;
                uv_buf++;
            }
        }

        uv_buf -= width * 16;
    }

    return 0;
}

static int decode_libjpeg_h_samp2(struct v4lconvert_data *data, unsigned char *ydest, unsigned char *udest, unsigned char *vdest, int v_samp)
{
    struct jpeg_decompress_struct *cinfo = &data->cinfo;

    int y;
    unsigned int width = cinfo->image_width;
    JSAMPROW y_rows[16], u_rows[8], v_rows[8];
    JSAMPARRAY rows[3] = { y_rows, u_rows, v_rows };

    while (cinfo->output_scanline < cinfo->image_height) {
        for (y = 0; y < 8 * v_samp; y++) {
            y_rows[y] = ydest;
            ydest += width;
        }

        if (v_samp == 1) {
            for (y = 0; y < 8; y++) {
                u_rows[y] = udest;
                v_rows[y] = vdest;
                y++;
                u_rows[y] = udest;
                v_rows[y] = vdest;
                udest += width / 2;
                vdest += width / 2;
            }
        } else {
            for (y = 0; y < 8; y++) {
                u_rows[y] = udest;
                v_rows[y] = vdest;
                udest += width / 2;
                vdest += width / 2;
            }
        }

        y = jpeg_read_raw_data(cinfo, rows, 8 * v_samp);
        if (y != 8 * v_samp) {
            return -1;
        }
    }

    return 0;
}

int v4lconvert_decode_jpeg_libjpeg(struct v4lconvert_data *data, unsigned char *src, int src_size, unsigned char *dest, struct v4l2_format *fmt, unsigned int dest_pix_fmt)
{
    int result = 0;
    unsigned int width  = fmt->fmt.pix.width;
    unsigned int height = fmt->fmt.pix.height;

    data->jerr_errno = EAGAIN;
    result = setjmp(data->jerr_jmp_state);
    if (result) {
        if (data->cinfo_initialized) {
            jpeg_abort_decompress(&data->cinfo);
        }

        errno = result;
        return -1;
    }

    init_libjpeg_cinfo(data);

    jpeg_mem_src(&data->cinfo, src, src_size);
    jpeg_read_header(&data->cinfo, TRUE);

    if ((data->cinfo.image_width != width) || (data->cinfo.image_height != height)) {
        V4LCONVERT_ERR("unexpected width / height in JPEG header: expected: %ux%u, header: %ux%u\n", width, height, data->cinfo.image_width, data->cinfo.image_height);
        errno = EIO;
        return -1;
    }

    if (data->cinfo.num_components != 3) {
        V4LCONVERT_ERR("unexpected no components in JPEG: %d\n", data->cinfo.num_components);
        errno = EIO;
        return -1;
    }

    if ((dest_pix_fmt == V4L2_PIX_FMT_RGB24) || (dest_pix_fmt == V4L2_PIX_FMT_BGR24)) {
        JSAMPROW row_pointer[1];

#ifdef JCS_EXTENSIONS
        if (dest_pix_fmt == V4L2_PIX_FMT_BGR24) {
            data->cinfo.out_color_space = JCS_EXT_BGR;
        }
#endif
        row_pointer[0] = dest;
        jpeg_start_decompress(&data->cinfo);
        data->jerr_errno = EPIPE;
        while (data->cinfo.output_scanline < height) {
            jpeg_read_scanlines(&data->cinfo, row_pointer, 1);
            row_pointer[0] += 3 * width;
        }

        jpeg_finish_decompress(&data->cinfo);
#ifndef JCS_EXTENSIONS
        if (dest_pix_fmt == V4L2_PIX_FMT_BGR24) {
            v4lconvert_swap_rgb(dest, dest, width, height);
        }
#endif
    } else {
        int h_samp, v_samp;
        unsigned char *udest, *vdest;

        if (data->cinfo.max_h_samp_factor == 2
            && data->cinfo.cur_comp_info[0]->h_samp_factor == 2
            && data->cinfo.cur_comp_info[1]->h_samp_factor == 1
            && data->cinfo.cur_comp_info[2]->h_samp_factor == 1)
        {
            h_samp = 2;
        } else {
            fprintf(stderr,
                "libv4lconvert: unsupported jpeg h-sampling "
                "factors %d:%d:%d, please report this to "
                "hdegoede@redhat.com\n",
                data->cinfo.cur_comp_info[0]->h_samp_factor,
                data->cinfo.cur_comp_info[1]->h_samp_factor,
                data->cinfo.cur_comp_info[2]->h_samp_factor);
            errno = EOPNOTSUPP;
            return -1;
        }

        if (data->cinfo.max_v_samp_factor == 2
            && data->cinfo.cur_comp_info[0]->v_samp_factor == 2
            && data->cinfo.cur_comp_info[1]->v_samp_factor == 1
            && data->cinfo.cur_comp_info[2]->v_samp_factor == 1)
        {
            v_samp = 2;
        } else if (data->cinfo.max_v_samp_factor == 1
            && data->cinfo.cur_comp_info[0]->v_samp_factor == 1
            && data->cinfo.cur_comp_info[1]->v_samp_factor == 1
            && data->cinfo.cur_comp_info[2]->v_samp_factor == 1)
        {
            v_samp = 1;
        } else {
            fprintf(stderr,
                "libv4lconvert: unsupported jpeg v-sampling "
                "factors %d:%d:%d, please report this to "
                "hdegoede@redhat.com\n",
                data->cinfo.cur_comp_info[0]->v_samp_factor,
                data->cinfo.cur_comp_info[1]->v_samp_factor,
                data->cinfo.cur_comp_info[2]->v_samp_factor);
            errno = EOPNOTSUPP;
            return -1;
        }

        if (width % (8 * h_samp) || height % (8 * v_samp)) {
            V4LCONVERT_ERR( "resolution is not a multiple of dctsize");
            errno = EIO;
            return -1;
        }

        if (dest_pix_fmt == V4L2_PIX_FMT_YVU420) {
            vdest = dest + width * height;
            udest = vdest + (width * height) / 4;
        } else {
            udest = dest + width * height;
            vdest = udest + (width * height) / 4;
        }

        data->cinfo.raw_data_out = TRUE;
        data->cinfo.do_fancy_upsampling = FALSE;
        jpeg_start_decompress(&data->cinfo);
        data->jerr_errno = EPIPE;

        if (h_samp == 1) {
            result = decode_libjpeg_h_samp1(data, dest, udest, vdest, v_samp);
        } else {
            result = decode_libjpeg_h_samp2(data, dest, udest, vdest, v_samp);
        }

        if (result) {
            jpeg_abort_decompress(&data->cinfo);
        } else {
            jpeg_finish_decompress(&data->cinfo);
        }
    }

    return result;
}

#endif
