#include <rtsp/internal/JPEGRTPSource.h>
#include <rtsp/internal/RTSPCommonEnv.h>

namespace rtsp {
JPEGRTPSource::JPEGRTPSource(int connType, MediaSubsession &subsession, TaskScheduler &task)
    : RTPSource(connType, subsession, task)
{
    fDefaultWidth = fDefaultHeight = 0;
}

JPEGRTPSource::~JPEGRTPSource()
{

}

enum {
    MARKER_SOF0      = 0xc0,
    MARKER_SOI       = 0xd8,
    MARKER_EOI       = 0xd9,
    MARKER_SOS       = 0xda,
    MARKER_DRI       = 0xdd,
    MARKER_DQT       = 0xdb,
    MARKER_DHT       = 0xc4,
    MARKER_APP_FIRST = 0xe0,
    MARKER_APP_LAST  = 0xef,
    MARKER_COMMENT   = 0xfe,
};

static unsigned char const lum_dc_codelens[] = {
    0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
};

static unsigned char const lum_dc_symbols[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
};

static unsigned char const lum_ac_codelens[] = {
    0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d,
};

static unsigned char const lum_ac_symbols[] = {
    0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
    0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
    0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
    0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
    0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
    0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
    0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
    0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
    0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
    0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
    0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
    0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
    0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
    0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
    0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa,
};

static unsigned char const chm_dc_codelens[] = {
    0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
};

static unsigned char const chm_dc_symbols[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
};

static unsigned char const chm_ac_codelens[] = {
    0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77,
};

static unsigned char const chm_ac_symbols[] = {
    0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
    0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
    0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
    0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
    0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
    0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
    0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
    0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
    0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
    0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
    0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
    0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
    0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
    0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
    0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
    0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa,
};

static void createHuffmanHeader(unsigned char *&p, unsigned char const *codelens, int ncodes, unsigned char const *symbols, int nsymbols, int tableNo, int tableClass) 
{
    *p++ = 0xff; *p++ = MARKER_DHT;
    *p++ = 0;
    *p++ = 3 + ncodes + nsymbols;
    *p++ = (tableClass << 4) | tableNo;
    memcpy(p, codelens, ncodes);
    p += ncodes;
    memcpy(p, symbols, nsymbols);
    p += nsymbols;
}

static unsigned computeJPEGHeaderSize(unsigned qtlen, unsigned dri) 
{
    unsigned qtlen_half = qtlen / 2;
    return 495 + qtlen_half * 2 + (dri > 0 ? 6 : 0);
}

static void createJPEGHeader(unsigned char *buf, unsigned type, unsigned w, unsigned h, unsigned char const *qtables, unsigned qtlen, unsigned dri) 
{
    unsigned char *ptr = buf;
    unsigned numQtables = qtlen > 64 ? 2 : 1;

    *ptr++ = 0xFF; *ptr++ = MARKER_SOI;

    *ptr++ = 0xFF; *ptr++ = MARKER_APP_FIRST;
    *ptr++ = 0x00; *ptr++ = 0x10;
    *ptr++ = 'J'; *ptr++ = 'F'; *ptr++ = 'I'; *ptr++ = 'F'; *ptr++ = 0x00;
    *ptr++ = 0x01; *ptr++ = 0x01;
    *ptr++ = 0x00;
    *ptr++ = 0x00; *ptr++ = 0x01;
    *ptr++ = 0x00; *ptr++ = 0x01;
    *ptr++ = 0x00; *ptr++ = 0x00;

    if (dri > 0) {
        *ptr++ = 0xFF; *ptr++ = MARKER_DRI;
        *ptr++ = 0x00; *ptr++ = 0x04;
        *ptr++ = (unsigned char)(dri >> 8); *ptr++ = (unsigned char)(dri);
    }

    unsigned tableSize = numQtables == 1 ? qtlen : qtlen / 2;
    *ptr++ = 0xFF; *ptr++ = MARKER_DQT;
    *ptr++ = 0x00; *ptr++ = tableSize + 3;
    *ptr++ = 0x00;
    memcpy(ptr, qtables, tableSize);
    qtables += tableSize;
    ptr += tableSize;

    if (numQtables > 1) {
        unsigned tableSize = qtlen - qtlen / 2;
        *ptr++ = 0xFF; *ptr++ = MARKER_DQT;
        *ptr++ = 0x00; *ptr++ = tableSize + 3;
        *ptr++ = 0x01;
        memcpy(ptr, qtables, tableSize);
        qtables += tableSize;
        ptr += tableSize;
    }

    *ptr++ = 0xFF; *ptr++ = MARKER_SOF0;
    *ptr++ = 0x00; *ptr++ = 0x11;
    *ptr++ = 0x08;
    *ptr++ = (unsigned char)(h >> 8);
    *ptr++ = (unsigned char)(h);
    *ptr++ = (unsigned char)(w >> 8);
    *ptr++ = (unsigned char)(w);
    *ptr++ = 0x03;
    *ptr++ = 0x01;
    *ptr++ = type ? 0x22 : 0x21;
    *ptr++ = 0x00;
    *ptr++ = 0x02;
    *ptr++ = 0x11;
    *ptr++ = numQtables == 1 ? 0x00 : 0x01;
    *ptr++ = 0x03;
    *ptr++ = 0x11;
    *ptr++ = 0x01;

    createHuffmanHeader(ptr, lum_dc_codelens, sizeof(lum_dc_codelens), lum_dc_symbols, sizeof(lum_dc_symbols), 0, 0);
    createHuffmanHeader(ptr, lum_ac_codelens, sizeof(lum_ac_codelens), lum_ac_symbols, sizeof(lum_ac_symbols), 0, 1);
    createHuffmanHeader(ptr, chm_dc_codelens, sizeof(chm_dc_codelens), chm_dc_symbols, sizeof(chm_dc_symbols), 1, 0);
    createHuffmanHeader(ptr, chm_ac_codelens, sizeof(chm_ac_codelens), chm_ac_symbols, sizeof(chm_ac_symbols), 1, 1);

    *ptr++ = 0xFF; *ptr++ = MARKER_SOS;
    *ptr++ = 0x00; *ptr++ = 0x0C;
    *ptr++ = 0x03;
    *ptr++ = 0x01;
    *ptr++ = 0x00;
    *ptr++ = 0x02;
    *ptr++ = 0x11;
    *ptr++ = 0x03;
    *ptr++ = 0x11;
    *ptr++ = 0x00;
    *ptr++ = 0x3F;
    *ptr++ = 0x00;
}

static unsigned char const defaultQuantizers[128] = {
    // luma table:
    16, 11, 12, 14, 12, 10, 16, 14,
    13, 14, 18, 17, 16, 19, 24, 40,
    26, 24, 22, 22, 24, 49, 35, 37,
    29, 40, 58, 51, 61, 60, 57, 51,
    56, 55, 64, 72, 92, 78, 64, 68,
    87, 69, 55, 56, 80, 109, 81, 87,
    95, 98, 103, 104, 103, 62, 77, 113,
    121, 112, 100, 120, 92, 101, 103, 99,
    // chroma table:
    17, 18, 18, 24, 21, 24, 47, 26,
    26, 47, 99, 66, 56, 66, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99
};

static void makeDefaultQtables(unsigned char *resultTables, unsigned Q) 
{
    int q;
    int factor = Q;

    if (Q < 1) {
        factor = 1;
    } else if (Q > 99) {
        factor = 99;
    }

    if (Q < 50) {
        q = 5000 / factor;
    } else {
        q = 200 - factor*2;
    }

    for (int i = 0; i < 128; ++i) {
        int newVal = (defaultQuantizers[i]*q + 50)/100;
        if (newVal < 1) {
            newVal = 1;
        } else if (newVal > 255) {
            newVal = 255;
        }
        resultTables[i] = newVal;
    }
}

void JPEGRTPSource::processFrame(RTPPacketBuffer *packet)
{
    int len = packet->payloadLen();
    uint8_t *buf = packet->payload();
    uint8_t *jpeg_buf = new uint8_t[MAX_JPEG_HEADER_SIZE + len];

    memcpy(&jpeg_buf[MAX_JPEG_HEADER_SIZE], buf, len);

    uint8_t *buf_ptr = &jpeg_buf[MAX_JPEG_HEADER_SIZE];
    int64_t media_timestamp = packet->extTimestamp() == 0 ? getMediaTimestamp(packet->timestamp()) : packet->extTimestamp();

    unsigned dri = 0;
    unsigned qtlen = 0;
    unsigned char *qtables = NULL;

    if (len < 8) {
        delete[] jpeg_buf;
        return;
    }

    unsigned Offset = (unsigned)((uint32_t)buf_ptr[1] << 16 | (uint32_t)buf_ptr[2] << 8 | (uint32_t)buf_ptr[3]);
    unsigned Type = (unsigned)buf_ptr[4];
    unsigned type = Type & 1;
    unsigned Q = (unsigned)buf_ptr[5];
    unsigned width = (unsigned)buf_ptr[6] * 8;
    unsigned height = (unsigned)buf_ptr[7] * 8;

    if (((width == 0) || (height == 0)) && (fDefaultWidth != 0) && (fDefaultHeight != 0)) {
        width = fDefaultWidth;
        height = fDefaultHeight;
    }

    if (width == 0) {
        width = 256*8;
    }

    if (height == 0) {
        height = 256*8;
    }

    buf_ptr += 8;
    len -= 8;

    if (Type > 63) {
        if (len < 4) {
            delete[] jpeg_buf;
            return;
        }

        unsigned RestartInterval = (unsigned)((uint16_t)buf_ptr[0] << 8 | (uint16_t)buf_ptr[1]);
        dri = RestartInterval;
        buf_ptr += 4;
        len -= 4;
    }

    if (Offset == 0) {
        if (Q > 127) {
            if (len < 4) {
                delete[] jpeg_buf;
                return;
            }

            unsigned MBZ = (unsigned)buf_ptr[0];
            if (MBZ == 0) {
                unsigned Length = (unsigned)((uint16_t)buf_ptr[2] << 8 | (uint16_t)buf_ptr[3]);

                buf_ptr += 4;
                len -= 4;

                if (len < Length) {
                    delete[] jpeg_buf;
                    return;
                }

                qtlen = Length;
                qtables = &buf_ptr[0];
                buf_ptr += Length;
                len -= Length;
            }
        }
    }

    if (Offset == 0) {
        unsigned char newQtables[128];
        if (qtlen == 0) {
            makeDefaultQtables(newQtables, Q);
            qtables = newQtables;
            qtlen = sizeof(newQtables);
        }

        unsigned hdrlen = computeJPEGHeaderSize(qtlen, dri);
        buf_ptr -= hdrlen;
        len += hdrlen;
        createJPEGHeader(buf_ptr, type, width, height, qtables, qtlen, dri);
    }

    copyToFrameBuffer(buf_ptr, len);

    if (packet->markerBit()) {
        if (fFrameHandlerFunc) {
            fFrameHandlerFunc(fFrameHandlerFuncData, fFrameType, media_timestamp, fFrameBuf, fFrameBufPos);
        }

        resetFrameBuf();
    }

    delete[] jpeg_buf;
}
}
