#ifndef RTSP_SIMPLE_RTSP_SERVER_RTP_ENC_H
#define RTSP_SIMPLE_RTSP_SERVER_RTP_ENC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rtRTPEnc {
    uint8_t  pt;
    uint16_t seq;
    uint32_t ssrc;
    uint32_t sample_rate;
} rtp_enc_t;

int rtp_enc_h264(rtp_enc_t *e, const uint8_t *frame, int len, uint64_t ts, uint8_t *packets[], int pktsizs[]);
int rtp_enc_h265(rtp_enc_t *e, const uint8_t *frame, int len, uint64_t ts, uint8_t *packets[], int pktsizs[]);

int rtp_enc_aac(rtp_enc_t *e, const uint8_t *frame, int len, uint64_t ts, uint8_t *packets[], int pktsizs[]);
int rtp_enc_g711(rtp_enc_t *e, const uint8_t *frame, int len, uint64_t ts, uint8_t *packets[], int pktsizs[]);
int rtp_enc_g726(rtp_enc_t *e, const uint8_t *frame, int len, uint64_t ts, uint8_t *packets[], int pktsizs[]);

#ifdef __cplusplus
}
#endif

#endif
