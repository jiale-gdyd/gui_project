#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <netinet/tcp.h>

#include "utils.h"
#include "queue.h"
#include "common.h"
#include "rtp_enc.h"
#include "rtsp_msg.h"
#include "stream_queue.h"
#include "simpleRtspServer.h"

#define SOCKET_ERROR            (-1)
#define INVALID_SOCKET          (-1)
#define SK_EAGAIN               (EAGAIN)
#define SK_EINTR                (EINTR)
#define closesocket(x)          close(x)

#define RTP_MAX_PKTSIZ          ((1500 - 42) / 4 * 4)
#define VRTP_MAX_NBPKTS         (300)
#define ARTP_MAX_NBPKTS         (10)
#define VRTP_PT_ID              (96)
#define ARTP_PT_ID              (97)
#define VRTSP_SUBPATH           "video"
#define ARTSP_SUBPATH           "audio"

typedef int SOCKET;
typedef socklen_t SOCKLEN;

static int sk_errno(void)
{
    return (errno);
}

static const char *sk_strerror(int simple_rtsp_server_error)
{
    return strerror(simple_rtsp_server_error);
}

struct rtsp_session;
struct rtsp_client_connection;

TAILQ_HEAD(rtsp_session_queue_head, rtsp_session);
TAILQ_HEAD(rtsp_client_connection_queue_head, rtsp_client_connection);

struct rtsp_session {
    char                                     path[64];
    int                                      vcodec_id;
    int                                      acodec_id;

    union {
        struct codec_data_h264               h264;
        struct codec_data_h265               h265;
    } vcodec_data;

    union {
        struct codec_data_g726               g726;
        struct codec_data_aac                aac;
    } acodec_data;

    rtp_enc_t                                vrtpe;
    rtp_enc_t                                artpe;
    struct stream_queue                      *vstreamq;
    struct stream_queue                      *astreamq;

    uint64_t                                 video_ntptime_of_zero_ts;
    uint64_t                                 audio_ntptime_of_zero_ts;

    struct rtsp_demo                         *demo;
    struct rtsp_client_connection_queue_head connections_qhead;
    TAILQ_ENTRY(rtsp_session)
    demo_entry;
};

struct rtp_connection {
    int            is_over_tcp;
    SOCKET         tcp_sockfd;
    int            tcp_interleaved[2];
    SOCKET         udp_sockfd[2];
    uint16_t       udp_localport[2];
    uint16_t       udp_peerport[2];
    struct in_addr peer_addr;
    int            streamq_index;
    uint32_t       ssrc;
    uint32_t       rtcp_packet_count;
    uint32_t       rtcp_octet_count;
    uint64_t       rtcp_last_ts;
};

#define RTSP_CC_STATE_INIT      0
#define RTSP_CC_STATE_READY     1
#define RTSP_CC_STATE_PLAYING   2
#define RTSP_CC_STATE_RECORDING 3

struct rtsp_client_connection
{
    int                   state;
#define RTSP_CC_STATE_INIT      0
#define RTSP_CC_STATE_READY     1
#define RTSP_CC_STATE_PLAYING   2
#define RTSP_CC_STATE_RECORDING 3

    SOCKET                sockfd;
    struct in_addr        peer_addr;
    unsigned long         session_id;

    char                  reqbuf[1024];
    int                   reqlen;

    struct rtp_connection *vrtp;
    struct rtp_connection *artp;

    struct rtsp_demo      *demo;
    struct rtsp_session   *session;
    TAILQ_ENTRY(rtsp_client_connection)
    demo_entry;
    TAILQ_ENTRY(rtsp_client_connection)
    session_entry;
};

struct rtsp_demo {
    SOCKET                                   sockfd;
    struct rtsp_session_queue_head           sessions_qhead;
    struct rtsp_client_connection_queue_head connections_qhead;
};

static void rtsp_del_rtp_connection(struct rtsp_client_connection *cc, int isaudio);
static int rtcp_try_tx_sr(struct rtp_connection *c, uint64_t ntptime_of_zero_ts, uint64_t ts, uint32_t sample_rate);

static struct rtsp_demo *__alloc_demo(void)
{
    struct rtsp_demo *d = (struct rtsp_demo *)calloc(1, sizeof(struct rtsp_demo));
    if (NULL == d) {
        simple_rtsp_server_error("alloc memory for rtsp_demo failed");
        return NULL;
    }

    TAILQ_INIT(&d->sessions_qhead);
    TAILQ_INIT(&d->connections_qhead);

    return d;
}

static void __free_demo(struct rtsp_demo *d)
{
    if (d) {
        free(d);
    }
}

static struct rtsp_session *__alloc_session(struct rtsp_demo *d)
{
    struct rtsp_session *s = (struct rtsp_session *)calloc(1, sizeof(struct rtsp_session));
    if (NULL == s) {
        simple_rtsp_server_error("alloc memory for rtsp_session failed");
        return NULL;
    }

    s->demo = d;
    TAILQ_INIT(&s->connections_qhead);
    TAILQ_INSERT_TAIL(&d->sessions_qhead, s, demo_entry);

    return s;
}

static void __free_session(struct rtsp_session *s)
{
    if (s) {
        struct rtsp_demo *d = s->demo;
        TAILQ_REMOVE(&d->sessions_qhead, s, demo_entry);
        free(s);
    }
}

static struct rtsp_client_connection *__alloc_client_connection(struct rtsp_demo *d)
{
    struct rtsp_client_connection *cc = (struct rtsp_client_connection *)calloc(1, sizeof(struct rtsp_client_connection));
    if (NULL == cc) {
        simple_rtsp_server_error("alloc memory for rtsp_session failed\n");
        return NULL;
    }

    cc->demo = d;
    TAILQ_INSERT_TAIL(&d->connections_qhead, cc, demo_entry);

    return cc;
}

static void __free_client_connection(struct rtsp_client_connection *cc)
{
    if (cc) {
        struct rtsp_demo *d = cc->demo;
        TAILQ_REMOVE(&d->connections_qhead, cc, demo_entry);
        free(cc);
    }
}

static void __client_connection_bind_session(struct rtsp_client_connection *cc, struct rtsp_session *s)
{
    if (cc->session == NULL) {
        cc->session = s;
        TAILQ_INSERT_TAIL(&s->connections_qhead, cc, session_entry);
    }
}

static void __client_connection_unbind_session(struct rtsp_client_connection *cc)
{
    struct rtsp_session *s = cc->session;
    if (s) {
        TAILQ_REMOVE(&s->connections_qhead, cc, session_entry);
        cc->session = NULL;
    }
}

static simple_rtsp_handle_t simple_rtsp_new(uint16_t port)
{
    int ret;
    SOCKET sockfd;
    struct sockaddr_in inaddr;
    struct rtsp_demo *d = NULL;

    d = __alloc_demo();
    if (NULL == d) {
        return NULL;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        simple_rtsp_server_error("create socket failed, errstr:[%s]", sk_strerror(sk_errno()));
        __free_demo(d);
        return NULL;
    }

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

    if (port <= 0) {
        port = 554;
    }

    memset(&inaddr, 0, sizeof(inaddr));
    inaddr.sin_family = AF_INET;
    inaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    inaddr.sin_port = htons(port);

    ret = bind(sockfd, (struct sockaddr *)&inaddr, sizeof(inaddr));
    if (ret == SOCKET_ERROR) {
        simple_rtsp_server_error("bind socket to address failed, errstr:[%s]", sk_strerror(sk_errno()));
        closesocket(sockfd);
        __free_demo(d);
        return NULL;
    }

    ret = listen(sockfd, 128);
    if (ret == SOCKET_ERROR) {
        simple_rtsp_server_error("listen socket failed, errstr:[%s]", sk_strerror(sk_errno()));
        closesocket(sockfd);
        __free_demo(d);
        return NULL;
    }

    d->sockfd = sockfd;

    simple_rtsp_server_info("rtsp server demo starting on %d", port);
    return (simple_rtsp_handle_t)d;
}

static int rtsp_set_client_socket(SOCKET sockfd)
{
    int ret;
    int keepalive = 1;
    int keepidle = 60;
    int keepcount = 5;
    struct linger ling;
    int keepinterval = 3;
    int sndbufsiz = 1024 * 512;

    ret = fcntl(sockfd, F_GETFL, 0);
    if (ret < 0) {
        simple_rtsp_server_warn("fcntl F_GETFL failed, errstr:[%s]", strerror(errno));
    } else {
        ret |= O_NONBLOCK;
        ret = fcntl(sockfd, F_SETFL, ret);
        if (ret < 0) {
            simple_rtsp_server_warn("fcntl F_SETFL failed, errstr:[%s]", strerror(errno));
        }
    }

    ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (const char *)&sndbufsiz, sizeof(sndbufsiz));
    if (ret == SOCKET_ERROR) {
        simple_rtsp_server_warn("setsockopt SO_SNDBUF failed, errstr:[%s]", sk_strerror(sk_errno()));
    }

    ret = setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (const char *)&keepalive, sizeof(keepalive));
    if (ret == SOCKET_ERROR) {
        simple_rtsp_server_warn("setsockopt SO_KEEPALIVE failed, errstr:[%s]", sk_strerror(sk_errno()));
    }

    ret = setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE, (const char *)&keepidle, sizeof(keepidle));
    if (ret == SOCKET_ERROR) {
        simple_rtsp_server_warn("setsockopt TCP_KEEPIDLE failed, errstr:[%s]", sk_strerror(sk_errno()));
    }

    ret = setsockopt(sockfd, SOL_TCP, TCP_KEEPINTVL, (const char *)&keepinterval, sizeof(keepinterval));
    if (ret == SOCKET_ERROR) {
        simple_rtsp_server_warn("setsockopt TCP_KEEPINTVL failed, errstr:[%s]", sk_strerror(sk_errno()));
    }

    ret = setsockopt(sockfd, SOL_TCP, TCP_KEEPCNT, (const char *)&keepcount, sizeof(keepcount));
    if (ret == SOCKET_ERROR) {
        simple_rtsp_server_warn("setsockopt TCP_KEEPCNT failed, errstr:[%s]", sk_strerror(sk_errno()));
    }

    memset(&ling, 0, sizeof(ling));
    ling.l_onoff = 1;
    ling.l_linger = 0;
    ret = setsockopt(sockfd, SOL_SOCKET, SO_LINGER, (const char *)&ling, sizeof(ling));
    if (ret == SOCKET_ERROR) {
        simple_rtsp_server_warn("setsockopt SO_LINGER failed, errstr:[%s]", sk_strerror(sk_errno()));
    }

    return 0;
}

static struct rtsp_client_connection *rtsp_new_client_connection(struct rtsp_demo *d)
{
    SOCKET sockfd;
    struct sockaddr_in inaddr;
    SOCKLEN addrlen = sizeof(inaddr);
    struct rtsp_client_connection *cc = NULL;

    sockfd = accept(d->sockfd, (struct sockaddr *)&inaddr, &addrlen);
    if (sockfd == INVALID_SOCKET) {
        simple_rtsp_server_error("accept failed, errstr:[%s]", sk_strerror(sk_errno()));
        return NULL;
    }

    rtsp_set_client_socket(sockfd);

    simple_rtsp_server_info("new rtsp client:[%s:%u] comming", inet_ntoa(inaddr.sin_addr), ntohs(inaddr.sin_port));

    cc = __alloc_client_connection(d);
    if (cc == NULL) {
        closesocket(sockfd);
        return NULL;
    }

    cc->state = RTSP_CC_STATE_INIT;
    cc->sockfd = sockfd;
    cc->peer_addr = inaddr.sin_addr;

    return cc;
}

static void rtsp_del_client_connection(struct rtsp_client_connection *cc)
{
    if (cc) {
        simple_rtsp_server_info("delete client:[%d] from:[%s]", cc->sockfd, inet_ntoa(cc->peer_addr));

        __client_connection_unbind_session(cc);
        rtsp_del_rtp_connection(cc, 0);
        rtsp_del_rtp_connection(cc, 1);
        closesocket(cc->sockfd);
        __free_client_connection(cc);
    }
}

static int rtsp_path_match(const char *main_path, const char *full_path)
{
    char path0[64] = {0};
    char path1[64] = {0};

    strncpy(path0, main_path, sizeof(path0) - 2);
    strncpy(path1, full_path, sizeof(path1) - 2);

    if (path0[strlen(path0) - 1] != '/') {
        strcat(path0, "/");
    }

    if (path1[strlen(path1) - 1] != '/') {
        strcat(path1, "/");
    }

    if (strncmp(path0, path1, strlen(path0))) {
        return 0;
    }

    return 1;
}

static simple_rtsp_session_t simple_rtsp_new_session(simple_rtsp_handle_t handle, const char *path)
{
    struct rtsp_session *s = NULL;
    struct rtsp_demo *d = (struct rtsp_demo *)handle;

    if (!d || !path || (strlen(path) == 0)) {
        simple_rtsp_server_error("param invalid\n");
        goto fail;
    }

    TAILQ_FOREACH(s, &d->sessions_qhead, demo_entry) {
        if (rtsp_path_match(s->path, path) || rtsp_path_match(path, s->path)) {
            simple_rtsp_server_error("path:%s (%s) is exist!!!", s->path, path);
            goto fail;
        }
    }

    s = __alloc_session(d);
    if (NULL == s) {
        goto fail;
    }

    strncpy(s->path, path, sizeof(s->path) - 1);
    s->vcodec_id = RTSP_CODEC_ID_NONE;
    s->acodec_id = RTSP_CODEC_ID_NONE;

    simple_rtsp_server_debug("add session path:[%s]", s->path);
    return (simple_rtsp_session_t)s;

fail:
    if (s) {
        free(s);
    }

    return NULL;
}

simple_rtsp_handle_t simple_create_rtsp_server(uint16_t port)
{
    return simple_rtsp_new(port);
}

simple_rtsp_session_t simple_create_rtsp_session(simple_rtsp_handle_t handle, const char *path, enum rtspCodecFlags encoder_flags)
{
    simple_rtsp_session_t session;

    session = simple_rtsp_new_session(handle, path);
    if (encoder_flags == RTSP_CODEC_FLAGS_H264) {
        simple_rtsp_set_video(session, RTSP_CODEC_ID_VIDEO_H264, NULL, 0);
    } else if (encoder_flags == RTSP_CODEC_FLAGS_H265) {
        simple_rtsp_set_video(session, RTSP_CODEC_ID_VIDEO_H265, NULL, 0);
    }

    return session;
}

int simple_rtsp_set_video(simple_rtsp_session_t session, enum rtspCodecIdEnum codec_id, const uint8_t *codec_data, int data_len)
{
    struct rtsp_session *s = (struct rtsp_session *)session;

    if (!s || ((s->vcodec_id != RTSP_CODEC_ID_NONE) && (s->vcodec_id != codec_id))) {
        return -1;
    }

    switch (codec_id) {
        case RTSP_CODEC_ID_VIDEO_H264:
        case RTSP_CODEC_ID_VIDEO_H265:
            break;

        default:
            simple_rtsp_server_error("not supported codec_id:[%d] for video", codec_id);
            return -1;
    }

    s->vcodec_id = codec_id;
    s->vrtpe.pt = VRTP_PT_ID;
    s->vrtpe.seq = 0;
    s->vrtpe.ssrc = 0;
    s->vrtpe.sample_rate = 9000;
    memset(&s->vcodec_data, 0, sizeof(s->vcodec_data));

    if (codec_data && (data_len > 0)) {
        switch ((int)codec_id) {
            case RTSP_CODEC_ID_VIDEO_H264:
                if (rtsp_codec_data_parse_from_user_h264(codec_data, data_len, &s->vcodec_data.h264) <= 0) {
                    simple_rtsp_server_warn("parse codec_data failed");
                    break;
                }
                break;

            case RTSP_CODEC_ID_VIDEO_H265:
                if (rtsp_codec_data_parse_from_user_h265(codec_data, data_len, &s->vcodec_data.h265) <= 0) {
                    simple_rtsp_server_warn("parse codec_data failed");
                    break;
                }
                break;
        }
    }

    if (!s->vstreamq) {
        s->vstreamq = streamq_alloc(RTP_MAX_PKTSIZ, VRTP_MAX_NBPKTS * 2 + 1);
        if (!s->vstreamq) {
            simple_rtsp_server_error("alloc memory for video rtp queue failed");
            s->vcodec_id = RTSP_CODEC_ID_NONE;
            return -1;
        }
    }

    return 0;
}

int simple_rtsp_set_audio(simple_rtsp_session_t session, enum rtspCodecIdEnum codec_id, const uint8_t *codec_data, int data_len)
{
    struct rtsp_session *s = (struct rtsp_session *)session;

    if (!s || ((s->acodec_id != RTSP_CODEC_ID_NONE) && (s->acodec_id != codec_id))) {
        return -1;
    }

    switch (codec_id) {
        case RTSP_CODEC_ID_AUDIO_G711A:
        case RTSP_CODEC_ID_AUDIO_G711U:
        case RTSP_CODEC_ID_AUDIO_G726:
        case RTSP_CODEC_ID_AUDIO_AAC:
            break;

        default:
            simple_rtsp_server_error("not supported codec_id:[%d] for audio", codec_id);
            return -1;
    }

    s->acodec_id = codec_id;
    s->artpe.pt = ARTP_PT_ID;
    s->artpe.seq = 0;
    s->artpe.ssrc = 0;
    s->artpe.sample_rate = 8000;
    memset(&s->acodec_data, 0, sizeof(s->acodec_data));

    if (codec_data && (data_len > 0)) {
        switch ((int)codec_id) {
            case RTSP_CODEC_ID_AUDIO_G726:
                if (rtsp_codec_data_parse_from_user_g726(codec_data, data_len, &s->acodec_data.g726) <= 0) {
                    simple_rtsp_server_warn("parse codec_data failed");
                    break;
                }
                break;

            case RTSP_CODEC_ID_AUDIO_AAC:
                if (rtsp_codec_data_parse_from_user_aac(codec_data, data_len, &s->acodec_data.aac) <= 0) {
                    simple_rtsp_server_warn("parse codec_data failed");
                    break;
                }
                s->artpe.sample_rate = s->acodec_data.aac.sample_rate;
                break;
        }
    }

    if (!s->astreamq) {
        s->astreamq = streamq_alloc(RTP_MAX_PKTSIZ, ARTP_MAX_NBPKTS * 2 + 1);
        if (!s->astreamq) {
            simple_rtsp_server_error("alloc memory for audio rtp queue failed");
            s->acodec_id = RTSP_CODEC_ID_NONE;
            return -1;
        }
    }

    return 0;
}

void simple_rtsp_delete_session(simple_rtsp_session_t session)
{
    struct rtsp_session *s = (struct rtsp_session *)session;
    if (s) {
        struct rtsp_client_connection *cc;
        while ((cc = TAILQ_FIRST(&s->connections_qhead))) {
            rtsp_del_client_connection(cc);
        }

        simple_rtsp_server_debug("del session path:[%s]", s->path);

        if (s->vstreamq) {
            streamq_free(s->vstreamq);
        }

        if (s->astreamq) {
            streamq_free(s->astreamq);
        }

        __free_session(s);
    }
}

void simple_rtsp_delete_handle(simple_rtsp_handle_t handle)
{
    struct rtsp_demo *d = (struct rtsp_demo *)handle;
    if (d) {
        struct rtsp_session *s;
        struct rtsp_client_connection *cc;

        while ((cc = TAILQ_FIRST(&d->connections_qhead))) {
            rtsp_del_client_connection(cc);
        }

        while ((s = TAILQ_FIRST(&d->sessions_qhead))) {
            simple_rtsp_delete_session(s);
        }

        closesocket(d->sockfd);
        __free_demo(d);
    }
}

static int build_simple_sdp(struct rtsp_session *s, const char *uri, char *sdpbuf, int maxlen)
{
    char *p = sdpbuf;

    p += sprintf(p, "v=0\r\n");
    p += sprintf(p, "o=- 0 0 IN IP4 0.0.0.0\r\n");
    p += sprintf(p, "s=rtsp_demo\r\n");
    p += sprintf(p, "t=0 0\r\n");
    p += sprintf(p, "a=control:%s\r\n", uri ? uri : "*");
    p += sprintf(p, "a=range:npt=0-\r\n");

    if (s->vcodec_id != RTSP_CODEC_ID_NONE) {
        switch (s->vcodec_id) {
            case RTSP_CODEC_ID_VIDEO_H264:
                p += rtsp_build_sdp_media_attr_h264(VRTP_PT_ID, s->vrtpe.sample_rate, &s->vcodec_data.h264, p, maxlen - (p - sdpbuf));
                break;

            case RTSP_CODEC_ID_VIDEO_H265:
                p += rtsp_build_sdp_media_attr_h265(VRTP_PT_ID, s->vrtpe.sample_rate, &s->vcodec_data.h265, p, maxlen - (p - sdpbuf));
                break;
        }

        if (uri) {
            p += sprintf(p, "a=control:%s/%s\r\n", uri, VRTSP_SUBPATH);
        } else {
            p += sprintf(p, "a=control:%s\r\n", VRTSP_SUBPATH);
        }
    }

    if (s->acodec_id != RTSP_CODEC_ID_NONE) {
        switch (s->acodec_id) {
            case RTSP_CODEC_ID_AUDIO_G711A:
                p += rtsp_build_sdp_media_attr_g711a(ARTP_PT_ID, s->artpe.sample_rate, p, maxlen - (p - sdpbuf));
                break;

            case RTSP_CODEC_ID_AUDIO_G711U:
                p += rtsp_build_sdp_media_attr_g711u(ARTP_PT_ID, s->artpe.sample_rate, p, maxlen - (p - sdpbuf));
                break;

            case RTSP_CODEC_ID_AUDIO_G726:
                p += rtsp_build_sdp_media_attr_g726(ARTP_PT_ID, s->artpe.sample_rate, &s->acodec_data.g726, p, maxlen - (p - sdpbuf));
                break;

            case RTSP_CODEC_ID_AUDIO_AAC:
                p += rtsp_build_sdp_media_attr_aac(ARTP_PT_ID, s->artpe.sample_rate, &s->acodec_data.aac, p, maxlen - (p - sdpbuf));
                break;
        }

        if (uri) {
            p += sprintf(p, "a=control:%s/%s\r\n", uri, ARTSP_SUBPATH);
        } else {
            p += sprintf(p, "a=control:%s\r\n", ARTSP_SUBPATH);
        }
    }

    return (p - sdpbuf);
}

static int rtsp_handle_OPTIONS(struct rtsp_client_connection *cc, const rtsp_msg_t *reqmsg, rtsp_msg_t *resmsg)
{
    uint32_t public_ = 0;

    public_ |= RTSP_MSG_PUBLIC_OPTIONS;
    public_ |= RTSP_MSG_PUBLIC_DESCRIBE;
    public_ |= RTSP_MSG_PUBLIC_SETUP;
    public_ |= RTSP_MSG_PUBLIC_PAUSE;
    public_ |= RTSP_MSG_PUBLIC_PLAY;
    public_ |= RTSP_MSG_PUBLIC_TEARDOWN;

    rtsp_msg_set_public(resmsg, public_);
    return 0;
}

static int rtsp_handle_DESCRIBE(struct rtsp_client_connection *cc, const rtsp_msg_t *reqmsg, rtsp_msg_t *resmsg)
{
    int sdplen = 0;
    char uri[128] = "";
    uint32_t accept = 0;
    char sdpbuf[1024] = "";
    struct rtsp_session *s = cc->session;
    const rtsp_msg_uri_s *puri = &reqmsg->hdrs.startline.reqline.uri;

    if ((rtsp_msg_get_accept(reqmsg, &accept) < 0) && !(accept & RTSP_MSG_ACCEPT_SDP)) {
        rtsp_msg_set_response(resmsg, 406);
        simple_rtsp_server_warn("client not support accept SDP");
        return 0;
    }

    if (puri->scheme == RTSP_MSG_URI_SCHEME_RTSPU) {
        strcat(uri, "rtspu://");
    } else {
        strcat(uri, "rtsp://");
    }

    strcat(uri, puri->ipaddr);
    if (puri->port != 0) {
        sprintf(uri + strlen(uri), ":%u", puri->port);
    }
    strcat(uri, s->path);

    sdplen = build_simple_sdp(s, uri, sdpbuf, sizeof(sdpbuf));

    rtsp_msg_set_content_type(resmsg, RTSP_MSG_CONTENT_TYPE_SDP);
    rtsp_msg_set_content_length(resmsg, sdplen);
    resmsg->body.body = rtsp_mem_dup(sdpbuf, sdplen);

    return 0;
}

static unsigned long __rtp_gen_ssrc(void)
{
    static unsigned long ssrc = 0x22345678;
    return ssrc++;
}

static int __rtp_udp_local_setup(struct rtp_connection *rtp)
{
    int i, ret;

    for (i = 65536 / 4 * 3 / 2 * 2; i < 65536; i += 2) {
        uint16_t port;
        SOCKET rtpsock, rtcpsock;
        struct sockaddr_in inaddr;

        rtpsock = socket(AF_INET, SOCK_DGRAM, 0);
        if (rtpsock == INVALID_SOCKET) {
            simple_rtsp_server_error("create rtp socket failed, errstr:[%s]", sk_strerror(sk_errno()));
            return -1;
        }

        rtcpsock = socket(AF_INET, SOCK_DGRAM, 0);
        if (rtcpsock == INVALID_SOCKET) {
            simple_rtsp_server_error("create rtcp socket failed, errstr:[%s]", sk_strerror(sk_errno()));
            closesocket(rtpsock);
            return -1;
        }

        port = i;
        memset(&inaddr, 0, sizeof(inaddr));
        inaddr.sin_family = AF_INET;
        inaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        inaddr.sin_port = htons(port);

        ret = bind(rtpsock, (struct sockaddr *)&inaddr, sizeof(inaddr));
        if (ret == SOCKET_ERROR) {
            closesocket(rtpsock);
            closesocket(rtcpsock);
            continue;
        }

        port = i + 1;
        memset(&inaddr, 0, sizeof(inaddr));
        inaddr.sin_family = AF_INET;
        inaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        inaddr.sin_port = htons(port);

        ret = bind(rtcpsock, (struct sockaddr *)&inaddr, sizeof(inaddr));
        if (ret == SOCKET_ERROR) {
            closesocket(rtpsock);
            closesocket(rtcpsock);
            continue;
        }

        ret = fcntl(rtpsock, F_GETFL, 0);
        if (ret < 0) {
            simple_rtsp_server_warn("fcntl F_GETFL failed, errstr:[%s]", strerror(errno));
        } else {
            ret |= O_NONBLOCK;
            ret = fcntl(rtpsock, F_SETFL, ret);
            if (ret < 0) {
                simple_rtsp_server_warn("fcntl F_SETFL failed, errstr:[%s]", strerror(errno));
            }
        }

        ret = fcntl(rtcpsock, F_GETFL, 0);
        if (ret < 0) {
            simple_rtsp_server_warn("fcntl F_GETFL failed, errstr:[%s]", strerror(errno));
        } else {
            ret |= O_NONBLOCK;
            ret = fcntl(rtcpsock, F_SETFL, ret);
            if (ret < 0) {
                simple_rtsp_server_warn("fcntl F_SETFL failed, errstr:[%s]", strerror(errno));
            }
        }

        rtp->is_over_tcp = 0;
        rtp->udp_sockfd[0] = rtpsock;
        rtp->udp_sockfd[1] = rtcpsock;
        rtp->udp_localport[0] = i;
        rtp->udp_localport[1] = i + 1;

        return 0;
    }

    simple_rtsp_server_error("not found free local port for rtp/rtcp");
    return -1;
}

static int rtsp_new_rtp_connection(struct rtsp_client_connection *cc, int isaudio, int istcp, int peer_port, int peer_interleaved)
{
    struct rtp_connection *rtp;
    struct in_addr peer_addr = cc->peer_addr;

    rtp = (struct rtp_connection *)calloc(1, sizeof(struct rtp_connection));
    if (rtp == NULL) {
        simple_rtsp_server_error("alloc mem for rtp session failed, errstr:[%s]", strerror(errno));
        return -1;
    }

    rtp->is_over_tcp = !!istcp;
    rtp->peer_addr = peer_addr;
    rtp->ssrc = __rtp_gen_ssrc();

    if (istcp) {
        rtp->tcp_sockfd = cc->sockfd;
        rtp->tcp_interleaved[0] = peer_interleaved;
        rtp->tcp_interleaved[1] = peer_interleaved + 1;

        simple_rtsp_server_info("new rtp over tcp for %s ssrc:[%08X] peer_addr:[%s] interleaved:[%u-%u]", (isaudio ? "audio" : "video"), rtp->ssrc, inet_ntoa(peer_addr), rtp->tcp_interleaved[0], rtp->tcp_interleaved[1]);
    } else {
        if (__rtp_udp_local_setup(rtp) < 0) {
            free(rtp);
            return -1;
        }

        rtp->udp_peerport[0] = peer_port;
        rtp->udp_peerport[1] = peer_port + 1;
        simple_rtsp_server_info("new rtp over udp for %s ssrc:[%08X] local_port:[%u-%u] peer_addr:[%s] peer_port:[%u-%u]", (isaudio ? "audio" : "video"), rtp->ssrc, rtp->udp_localport[0], rtp->udp_localport[1], inet_ntoa(peer_addr), rtp->udp_peerport[0], rtp->udp_peerport[1]);
    }

    if (isaudio)  {
        cc->artp = rtp;
    } else {
        cc->vrtp = rtp;
    }

    return 0;
}

static void rtsp_del_rtp_connection(struct rtsp_client_connection *cc, int isaudio)
{
    struct rtp_connection *rtp;

    if (isaudio) {
        rtp = cc->artp;
        cc->artp = NULL;
    } else {
        rtp = cc->vrtp;
        cc->vrtp = NULL;
    }

    if (rtp) {
        if (!(rtp->is_over_tcp)) {
            closesocket(rtp->udp_sockfd[0]);
            closesocket(rtp->udp_sockfd[1]);
        }

        free(rtp);
    }
}

static int rtsp_handle_SETUP(struct rtsp_client_connection *cc, const rtsp_msg_t *reqmsg, rtsp_msg_t *resmsg)
{
    int ret;
    char vpath[64] = "";
    char apath[64] = "";
    int istcp = 0, isaudio = 0;
    struct rtp_connection *rtp = NULL;
    struct rtsp_session *s = cc->session;

    if (!reqmsg->hdrs.transport) {
        rtsp_msg_set_response(resmsg, 461);
        simple_rtsp_server_warn("rtsp no transport simple_rtsp_server_error");
        return 0;
    }

    if (reqmsg->hdrs.transport->type == RTSP_MSG_TRANSPORT_TYPE_RTP_AVP_TCP) {
        istcp = 1;
        if (!(reqmsg->hdrs.transport->flags & RTSP_MSG_TRANSPORT_FLAG_INTERLEAVED)) {
            simple_rtsp_server_warn("rtsp no interleaved simple_rtsp_server_error");
            rtsp_msg_set_response(resmsg, 461);
            return 0;
        }
    } else {
        if (!(reqmsg->hdrs.transport->flags & RTSP_MSG_TRANSPORT_FLAG_CLIENT_PORT)) {
            simple_rtsp_server_warn("rtsp no client_port simple_rtsp_server_error");
            rtsp_msg_set_response(resmsg, 461);
            return 0;
        }
    }

    snprintf(vpath, sizeof(vpath) - 1, "%s/%s", s->path, VRTSP_SUBPATH);
    snprintf(apath, sizeof(vpath) - 1, "%s/%s", s->path, ARTSP_SUBPATH);

    if ((s->vcodec_id != RTSP_CODEC_ID_NONE) && rtsp_path_match(vpath, reqmsg->hdrs.startline.reqline.uri.abspath)) {
        isaudio = 0;
    } else if ((s->acodec_id != RTSP_CODEC_ID_NONE) && rtsp_path_match(apath, reqmsg->hdrs.startline.reqline.uri.abspath)) {
        isaudio = 1;
    } else {
        simple_rtsp_server_warn("rtsp urlpath:[%s] simple_rtsp_server_error", reqmsg->hdrs.startline.reqline.uri.abspath);

        rtsp_msg_set_response(resmsg, 461);
        return 0;
    }

    rtsp_del_rtp_connection(cc, isaudio);

    ret = rtsp_new_rtp_connection(cc, isaudio, istcp, reqmsg->hdrs.transport->client_port, reqmsg->hdrs.transport->interleaved);
    if (ret < 0) {
        rtsp_msg_set_response(resmsg, 500);
        return 0;
    }

    rtp = isaudio ? cc->artp : cc->vrtp;
    if (istcp) {
        rtsp_msg_set_transport_tcp(resmsg, rtp->ssrc, rtp->tcp_interleaved[0]);
    } else {
        rtsp_msg_set_transport_udp(resmsg, rtp->ssrc, rtp->udp_peerport[0], rtp->udp_localport[0]);
    }

    if (cc->state == RTSP_CC_STATE_PLAYING) {
        rtp->streamq_index = streamq_tail((isaudio ? s->astreamq : s->vstreamq));
    }

    if (cc->state == RTSP_CC_STATE_INIT) {
        cc->state = RTSP_CC_STATE_READY;
        cc->session_id = rtsp_msg_gen_session_id();
        rtsp_msg_set_session(resmsg, cc->session_id);
    }

    return 0;
}

static int rtsp_handle_PAUSE(struct rtsp_client_connection *cc, const rtsp_msg_t *reqmsg, rtsp_msg_t *resmsg)
{
    if ((cc->state != RTSP_CC_STATE_READY) && (cc->state != RTSP_CC_STATE_PLAYING)) {
        rtsp_msg_set_response(resmsg, 455);
        simple_rtsp_server_warn("rtsp status error");
        return 0;
    }

    if (cc->state != RTSP_CC_STATE_READY) {
        cc->state = RTSP_CC_STATE_READY;
    }

    return 0;
}

static int rtsp_handle_PLAY(struct rtsp_client_connection *cc, const rtsp_msg_t *reqmsg, rtsp_msg_t *resmsg)
{
    struct rtsp_session *s = cc->session;

    if ((cc->state != RTSP_CC_STATE_READY) && (cc->state != RTSP_CC_STATE_PLAYING)) {
        rtsp_msg_set_response(resmsg, 455);
        simple_rtsp_server_warn("rtsp status error");
        return 0;
    }

    if (cc->state != RTSP_CC_STATE_PLAYING) {
        if (cc->vrtp && s->vstreamq) {
            cc->vrtp->streamq_index = streamq_tail(s->vstreamq);
        }

        if (cc->artp && s->astreamq) {
            cc->artp->streamq_index = streamq_tail(s->astreamq);
        }
        cc->state = RTSP_CC_STATE_PLAYING;
    }

    return 0;
}

static int rtsp_handle_TEARDOWN(struct rtsp_client_connection *cc, const rtsp_msg_t *reqmsg, rtsp_msg_t *resmsg)
{
    char vpath[64] = "";
    char apath[64] = "";
    struct rtsp_session *s = cc->session;

    snprintf(vpath, sizeof(vpath) - 1, "%s/%s", s->path, VRTSP_SUBPATH);
    snprintf(apath, sizeof(vpath) - 1, "%s/%s", s->path, ARTSP_SUBPATH);

    if (rtsp_path_match(vpath, reqmsg->hdrs.startline.reqline.uri.abspath)) {
        rtsp_del_rtp_connection(cc, 0);
    } else if (rtsp_path_match(apath, reqmsg->hdrs.startline.reqline.uri.abspath)) {
        rtsp_del_rtp_connection(cc, 1);
    } else {
        rtsp_del_rtp_connection(cc, 0);
        rtsp_del_rtp_connection(cc, 1);
    }

    if (!cc->vrtp && !cc->artp) {
        cc->state = RTSP_CC_STATE_INIT;
    }

    return 0;
}

static int rtsp_process_request(struct rtsp_client_connection *cc, const rtsp_msg_t *reqmsg, rtsp_msg_t *resmsg)
{
    struct rtsp_demo *d = cc->demo;
    uint32_t cseq = 0, session = 0;
    struct rtsp_session *s = cc->session;
    const char *path = reqmsg->hdrs.startline.reqline.uri.abspath;

    rtsp_msg_set_response(resmsg, 200);
    rtsp_msg_set_date(resmsg, NULL);
    rtsp_msg_set_server(resmsg, "rtsp_demo");

    if (rtsp_msg_get_cseq(reqmsg, &cseq) < 0) {
        rtsp_msg_set_response(resmsg, 400);
        simple_rtsp_server_warn("No CSeq field");
        return 0;
    }

    rtsp_msg_set_cseq(resmsg, cseq);

    if (cc->state != RTSP_CC_STATE_INIT) {
        if ((rtsp_msg_get_session(reqmsg, &session) < 0) || (session != cc->session_id)) {
            simple_rtsp_server_warn("Invalid Session field");
            rtsp_msg_set_response(resmsg, 454);
            return 0;
        }

        rtsp_msg_set_session(resmsg, session);
    }

    if (s) {
        if (rtsp_path_match(s->path, path) == 0) {
            simple_rtsp_server_warn("path is not matched %s (old:%s)", path, s->path);
            rtsp_msg_set_response(resmsg, 451);
            return 0;
        }
    } else if (reqmsg->hdrs.startline.reqline.method != RTSP_MSG_METHOD_OPTIONS) {
        TAILQ_FOREACH(s, &d->sessions_qhead, demo_entry) {
            if (rtsp_path_match(s->path, path)) {
                break;
            }
        }

        if (NULL == s) {
            simple_rtsp_server_warn("Not found session path:[%s]", path);
            rtsp_msg_set_response(resmsg, 454);
            return 0;
        }

        __client_connection_bind_session(cc, s);
    }

    switch (reqmsg->hdrs.startline.reqline.method) {
        case RTSP_MSG_METHOD_OPTIONS:
            return rtsp_handle_OPTIONS(cc, reqmsg, resmsg);

        case RTSP_MSG_METHOD_DESCRIBE:
            return rtsp_handle_DESCRIBE(cc, reqmsg, resmsg);

        case RTSP_MSG_METHOD_SETUP:
            return rtsp_handle_SETUP(cc, reqmsg, resmsg);

        case RTSP_MSG_METHOD_PAUSE:
            return rtsp_handle_PAUSE(cc, reqmsg, resmsg);

        case RTSP_MSG_METHOD_PLAY:
            return rtsp_handle_PLAY(cc, reqmsg, resmsg);

        case RTSP_MSG_METHOD_TEARDOWN:
            return rtsp_handle_TEARDOWN(cc, reqmsg, resmsg);

        default:
            break;
    }

    rtsp_msg_set_response(resmsg, 501);
    return 0;
}

static int rtsp_recv_msg(struct rtsp_client_connection *cc, rtsp_msg_t *msg)
{
    int ret;

    if ((sizeof(cc->reqbuf) - cc->reqlen - 1) > 0) {
        ret = recv(cc->sockfd, cc->reqbuf + cc->reqlen, sizeof(cc->reqbuf) - cc->reqlen - 1, MSG_DONTWAIT);
        if (ret == 0) {
            simple_rtsp_server_debug("peer closed");
            return -1;
        }

        if (ret == SOCKET_ERROR) {
            if ((sk_errno() != SK_EAGAIN) && (sk_errno() != SK_EINTR)) {
                simple_rtsp_server_error("recv data failed, errstr:[%s]", sk_strerror(sk_errno()));
                return -1;
            }

            ret = 0;
        }

        cc->reqlen += ret;
        cc->reqbuf[cc->reqlen] = 0;
    }

    if (cc->reqlen == 0) {
        return 0;
    }

    ret = rtsp_msg_parse_from_array(msg, cc->reqbuf, cc->reqlen);
    if (ret < 0) {
        simple_rtsp_server_error("Invalid frame");
        return -1;
    }

    if (ret == 0) {
        return 0;
    }

    memmove(cc->reqbuf, cc->reqbuf + ret, cc->reqlen - ret);
    cc->reqlen -= ret;

    return ret;
}

static int rtsp_send_msg(struct rtsp_client_connection *cc, rtsp_msg_t *msg)
{
    int ret;
    char szbuf[1024] = "";

    ret = rtsp_msg_build_to_array(msg, szbuf, sizeof(szbuf));
    if (ret < 0) {
        simple_rtsp_server_error("rtsp_msg_build_to_array failed");
        return -1;
    }

    ret = send(cc->sockfd, szbuf, ret, MSG_NOSIGNAL);
    if (ret == SOCKET_ERROR) {
        simple_rtsp_server_error("rtsp response send failed, errstr:[%s]", sk_strerror(sk_errno()));
        return -1;
    }

    return ret;
}

static int rtsp_recv_rtp_over_udp(struct rtsp_client_connection *cc, int isaudio)
{
    int len;
    char szbuf[128];
    struct sockaddr_in inaddr;
    SOCKLEN addrlen = sizeof(inaddr);
    struct rtp_connection *rtp = isaudio ? cc->artp : cc->vrtp;

    len = recvfrom(rtp->udp_sockfd[0], szbuf, sizeof(szbuf), MSG_DONTWAIT, (struct sockaddr *)&inaddr, &addrlen);
    if (len == SOCKET_ERROR) {
        if ((sk_errno() != SK_EAGAIN) && (sk_errno() != SK_EINTR)) {
            simple_rtsp_server_warn("rtp over udp recv failed, errstr:[%s]", sk_strerror(sk_errno()));
            return -1;
        }

        return 0;
    }

    if (rtp->udp_peerport[0] != ntohs(inaddr.sin_port)) {
        simple_rtsp_server_info("rtp over udp peer:[%s] port change:[%u] to:[%u]", inet_ntoa(rtp->peer_addr), rtp->udp_peerport[0], ntohs(inaddr.sin_port));
        rtp->udp_peerport[0] = ntohs(inaddr.sin_port);
    }

    return len;
}

static int rtsp_recv_rtcp_over_udp(struct rtsp_client_connection *cc, int isaudio)
{
    int len;
    char szbuf[128];
    struct sockaddr_in inaddr;
    SOCKLEN addrlen = sizeof(inaddr);
    struct rtp_connection *rtp = isaudio ? cc->artp : cc->vrtp;

    len = recvfrom(rtp->udp_sockfd[1], szbuf, sizeof(szbuf), MSG_DONTWAIT, (struct sockaddr *)&inaddr, &addrlen);
    if (len == SOCKET_ERROR) {
        if ((sk_errno() != SK_EAGAIN) && (sk_errno() != SK_EINTR)) {
            simple_rtsp_server_warn("rtcp over udp recv failed, errstr:[%s]", sk_strerror(sk_errno()));
            return -1;
        }

        return 0;
    }

    if (rtp->udp_peerport[1] != ntohs(inaddr.sin_port)) {
        simple_rtsp_server_info("rtcp over udp peer:[%s] port change:[%u] to:[%u]", inet_ntoa(rtp->peer_addr), rtp->udp_peerport[1], ntohs(inaddr.sin_port));
        rtp->udp_peerport[1] = ntohs(inaddr.sin_port);
    }

    return len;
}

static int rtp_tx_data(struct rtp_connection *c, const uint8_t *data, int size)
{
    if (c->is_over_tcp) {
        int ret = -1;
        uint8_t szbuf[4];
        SOCKET sockfd = c->tcp_sockfd;

        sockfd = c->tcp_sockfd;
        szbuf[0] = '$';
        szbuf[1] = c->tcp_interleaved[0];
        *((uint16_t *)&szbuf[2]) = htons(size);

        ret = send(sockfd, (const char *)szbuf, 4, MSG_NOSIGNAL);
        if (ret == SOCKET_ERROR) {
            if ((sk_errno() != SK_EAGAIN) && (sk_errno() != SK_EINTR)) {
                simple_rtsp_server_warn("rtp over tcp send interlaced frame to:[%s] failed, errstr:[%s]", inet_ntoa(c->peer_addr), sk_strerror(sk_errno()));
                return -1;
            }

            return 0;
        }

        ret = send(sockfd, (const char *)data, size, MSG_NOSIGNAL);
        if (ret == SOCKET_ERROR) {
            if ((sk_errno() != SK_EAGAIN) && (sk_errno() != SK_EINTR)) {
                simple_rtsp_server_warn("rtp over tcp send %d bytes to %s failed, errstr:[%s]", size, inet_ntoa(c->peer_addr), sk_strerror(sk_errno()));
                return -1;
            }

            return 0;
        }
    } else {
        int ret = -1;
        struct sockaddr_in inaddr;
        SOCKET sockfd = c->udp_sockfd[0];

        memset(&inaddr, 0, sizeof(inaddr));
        inaddr.sin_family = AF_INET;
        inaddr.sin_addr = c->peer_addr;
        inaddr.sin_port = htons(c->udp_peerport[0]);

        ret = sendto(sockfd, (const char *)data, size, 0, (struct sockaddr *)&inaddr, sizeof(inaddr));
        if (ret == SOCKET_ERROR) {
            if ((sk_errno() != SK_EAGAIN) && (sk_errno() != SK_EINTR)) {
                simple_rtsp_server_warn("rtp over udp send %d bytes to %s failed, errstr:[%s]", size, inet_ntoa(c->peer_addr), sk_strerror(sk_errno()));
                return -1;
            }

            return 0;
        }
    }

    return size;
}

static int rtsp_tx_video_packet(struct rtsp_client_connection *cc)
{
    int count = 0;
    int *ppktlen = NULL;
    uint8_t *ppacket = NULL;
    struct rtsp_session *s = cc->session;
    struct stream_queue *q = s->vstreamq;
    struct rtp_connection *rtp = cc->vrtp;

    while (streamq_inused(q, rtp->streamq_index) > 0) {
        streamq_query(q, rtp->streamq_index, (char **)&ppacket, &ppktlen);

        if (*ppktlen > 0) {
            *((uint32_t *)(&ppacket[8])) = htonl(rtp->ssrc);
            if (rtp_tx_data(rtp, ppacket, *ppktlen) != *ppktlen) {
                break;
            }

            rtp->rtcp_packet_count++;
            rtp->rtcp_octet_count += *ppktlen - 12;
        }

        rtp->streamq_index = streamq_next(q, rtp->streamq_index);
        count++;
    }

    return count;
}

static int rtsp_tx_audio_packet(struct rtsp_client_connection *cc)
{
    int count = 0;
    int *ppktlen = NULL;
    uint8_t *ppacket = NULL;
    struct rtsp_session *s = cc->session;
    struct stream_queue *q = s->astreamq;
    struct rtp_connection *rtp = cc->artp;

    while (streamq_inused(q, rtp->streamq_index) > 0) {
        streamq_query(q, rtp->streamq_index, (char **)&ppacket, &ppktlen);

        if (*ppktlen > 0) {
            *((uint32_t *)(&ppacket[8])) = htonl(rtp->ssrc);
            if (rtp_tx_data(rtp, ppacket, *ppktlen) != *ppktlen) {
                break;
            }

            rtp->rtcp_packet_count++;
            rtp->rtcp_octet_count += *ppktlen - 12;
        }

        rtp->streamq_index = streamq_next(q, rtp->streamq_index);
        count++;
    }

    return count;
}

int simple_rtsp_wait_frame_finished(simple_rtsp_handle_t handle)
{
    int ret;
    fd_set rfds;
    fd_set wfds;
    SOCKET maxfd;
    struct timeval tv;
    struct rtsp_client_connection *cc = NULL;
    struct rtsp_demo *d = (struct rtsp_demo *)handle;

    if (NULL == d) {
        return -1;
    }

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);

    FD_SET(d->sockfd, &rfds);

    maxfd = d->sockfd;
    TAILQ_FOREACH(cc, &d->connections_qhead, demo_entry) {
        struct rtsp_session *s = cc->session;
        struct rtp_connection *vrtp = cc->vrtp;
        struct rtp_connection *artp = cc->artp;

        FD_SET(cc->sockfd, &rfds);
        if (cc->sockfd > maxfd) {
            maxfd = cc->sockfd;
        }

        if (cc->state != RTSP_CC_STATE_PLAYING) {
            continue;
        }

        if (vrtp && (streamq_inused(s->vstreamq, vrtp->streamq_index) > 0)) {
            if (vrtp->is_over_tcp) {
                FD_SET(vrtp->tcp_sockfd, &wfds);
                if (vrtp->tcp_sockfd > maxfd)
                    maxfd = vrtp->tcp_sockfd;
            } else {
                FD_SET(vrtp->udp_sockfd[0], &wfds);
                if (vrtp->udp_sockfd[0] > maxfd) {
                    maxfd = vrtp->udp_sockfd[0];
                }
            }
        }

        if (artp && (streamq_inused(s->astreamq, artp->streamq_index) > 0)) {
            if (artp->is_over_tcp) {
                FD_SET(artp->tcp_sockfd, &wfds);
                if (artp->tcp_sockfd > maxfd) {
                    maxfd = artp->tcp_sockfd;
                }
            } else {
                FD_SET(artp->udp_sockfd[0], &wfds);
                if (artp->udp_sockfd[0] > maxfd) {
                    maxfd = artp->udp_sockfd[0];
                }
            }
        }

        if (vrtp && (!vrtp->is_over_tcp)) {
            FD_SET(vrtp->udp_sockfd[0], &rfds);
            FD_SET(vrtp->udp_sockfd[1], &rfds);

            if (vrtp->udp_sockfd[0] > maxfd) {
                maxfd = vrtp->udp_sockfd[0];
            }

            if (vrtp->udp_sockfd[1] > maxfd) {
                maxfd = vrtp->udp_sockfd[1];
            }
        }

        if (artp && (!artp->is_over_tcp)) {
            FD_SET(artp->udp_sockfd[0], &rfds);
            FD_SET(artp->udp_sockfd[1], &rfds);

            if (artp->udp_sockfd[0] > maxfd) {
                maxfd = artp->udp_sockfd[0];
            }

            if (artp->udp_sockfd[1] > maxfd) {
                maxfd = artp->udp_sockfd[1];
            }
        }
    }

    memset(&tv, 0, sizeof(tv));
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    ret = select(maxfd + 1, &rfds, &wfds, NULL, &tv);
    if (ret < 0) {
        simple_rtsp_server_error("select failed, errstr:[%s]", strerror(errno));
        return -1;
    }

    if (ret == 0) {
        simple_rtsp_server_warn("select timeout, errstr:[%s]", strerror(errno));
        return 0;
    }

    if (FD_ISSET(d->sockfd, &rfds)) {
        rtsp_new_client_connection(d);
    }

    cc = TAILQ_FIRST(&d->connections_qhead);
    while (cc) {
        struct rtsp_client_connection *cc1 = cc;
        struct rtsp_session *s = cc1->session;
        struct rtp_connection *vrtp = cc1->vrtp;
        struct rtp_connection *artp = cc1->artp;

        cc = TAILQ_NEXT(cc, demo_entry);

        if (FD_ISSET(cc1->sockfd, &rfds)) {
            do {
                rtsp_msg_t reqmsg, resmsg;

                rtsp_msg_init(&reqmsg);
                rtsp_msg_init(&resmsg);

                ret = rtsp_recv_msg(cc1, &reqmsg);
                if (ret == 0) {
                    break;
                }

                if (ret < 0) {
                    rtsp_del_client_connection(cc1);
                    cc1 = NULL;
                    break;
                }

                if (reqmsg.type == RTSP_MSG_TYPE_INTERLEAVED) {
                    rtsp_msg_free(&reqmsg);
                    continue;
                }

                if (reqmsg.type != RTSP_MSG_TYPE_REQUEST) {
                    simple_rtsp_server_error("not request frame");
                    rtsp_msg_free(&reqmsg);
                    continue;
                }

                ret = rtsp_process_request(cc1, &reqmsg, &resmsg);
                if (ret < 0) {
                    simple_rtsp_server_error("request internal simple_rtsp_server_error");
                } else {
                    rtsp_send_msg(cc1, &resmsg);
                }

                rtsp_msg_free(&reqmsg);
                rtsp_msg_free(&resmsg);
            } while (cc1);

            if (cc1 == NULL) {
                continue;
            }
        }

        if (cc1->state != RTSP_CC_STATE_PLAYING) {
            continue;
        }

        if (vrtp && (streamq_inused(s->vstreamq, vrtp->streamq_index) > 0)) {
            if (vrtp->is_over_tcp && FD_ISSET(vrtp->tcp_sockfd, &wfds)) {
                rtsp_tx_video_packet(cc1);
            } else if ((!vrtp->is_over_tcp) && FD_ISSET(vrtp->udp_sockfd[0], &wfds)) {
                rtsp_tx_video_packet(cc1);
            }
        }

        if (artp && (streamq_inused(s->astreamq, artp->streamq_index) > 0)) {
            if (artp->is_over_tcp && FD_ISSET(artp->tcp_sockfd, &wfds)) {
                rtsp_tx_audio_packet(cc1);
            } else if ((0 == artp->is_over_tcp) && FD_ISSET(artp->udp_sockfd[0], &wfds)) {
                rtsp_tx_audio_packet(cc1);
            }
        }

        if (vrtp && (!vrtp->is_over_tcp)) {
            if (FD_ISSET(vrtp->udp_sockfd[0], &rfds)) {
                rtsp_recv_rtp_over_udp(cc1, 0);
            }

            if (FD_ISSET(vrtp->udp_sockfd[1], &rfds)) {
                rtsp_recv_rtcp_over_udp(cc1, 0);
            }
        }

        if (artp && (!artp->is_over_tcp)) {
            if (FD_ISSET(artp->udp_sockfd[0], &rfds)) {
                rtsp_recv_rtp_over_udp(cc1, 1);
            }

            if (FD_ISSET(artp->udp_sockfd[1], &rfds)) {
                rtsp_recv_rtcp_over_udp(cc1, 1);
            }
        }
    }

    return 1;
}

int simple_rtsp_send_video(simple_rtsp_session_t session, const uint8_t *frame, int len, uint64_t ts)
{
    int i, index, count, start;
    struct stream_queue *q = NULL;
    int pktsizs[VRTP_MAX_NBPKTS + 1] = {0};
    int *pktlens[VRTP_MAX_NBPKTS] = {NULL};
    struct rtsp_client_connection *cc = NULL;
    uint8_t *packets[VRTP_MAX_NBPKTS + 1] = {NULL};
    struct rtsp_session *s = (struct rtsp_session *)session;

    if (!s || !frame || (s->vcodec_id == RTSP_CODEC_ID_NONE)) {
        return -1;
    }

    q = s->vstreamq;
    index = streamq_tail(q);
    for (i = 0; i < VRTP_MAX_NBPKTS; i++) {
        if (streamq_next(q, index) == streamq_head(q)) {
            streamq_pop(q);
        }

        streamq_query(q, index, (char **)&packets[i], &pktlens[i]);
        pktsizs[i] = RTP_MAX_PKTSIZ;
        index = streamq_next(q, index);
    }

    packets[i] = NULL;
    pktsizs[i] = 0;

    TAILQ_FOREACH(cc, &s->connections_qhead, session_entry) {
        struct rtp_connection *rtp = cc->vrtp;
        if ((cc->state != RTSP_CC_STATE_PLAYING) || !rtp) {
            continue;
        }

        if (!streamq_inused(q, rtp->streamq_index) && (rtp->streamq_index != streamq_tail(q))) {
            rtp->streamq_index = streamq_head(q);
        }
    }

    switch (s->vcodec_id) {
        case RTSP_CODEC_ID_VIDEO_H264:
            if ((s->vcodec_data.h264.pps_len == 0) || (s->vcodec_data.h264.pps_len == 0)) {
                if (rtsp_codec_data_parse_from_frame_h264(frame, len, &s->vcodec_data.h264) < 0) {
                    simple_rtsp_server_warn("rtsp_codec_data_parse_from_frame_h264 failed");
                }
            }
            break;

        case RTSP_CODEC_ID_VIDEO_H265:
            if ((s->vcodec_data.h265.pps_len == 0) || (s->vcodec_data.h265.pps_len == 0) || (s->vcodec_data.h265.vps_len == 0)) {
                if (rtsp_codec_data_parse_from_frame_h265(frame, len, &s->vcodec_data.h265) < 0) {
                    simple_rtsp_server_warn("rtsp_codec_data_parse_from_frame_h265 failed");
                }
            }
            break;
    }

    start = 0;
    count = 0;

    while ((start < len) && packets[count] && (pktsizs[count] > 0)) {
        int ret;
        int size = 0;
        const uint8_t *p = NULL;

        p = rtsp_find_h264_h265_nalu(frame + start, len - start, &size);
        if (!p) {
            simple_rtsp_server_warn("not found nal header");
            break;
        }

        switch (s->vcodec_id) {
            case RTSP_CODEC_ID_VIDEO_H264:
                ret = rtp_enc_h264(&s->vrtpe, p, size, ts, &packets[count], &pktsizs[count]);
                if (ret <= 0) {
                    simple_rtsp_server_error("rtp_enc_h264 return:[%d]", ret);
                    return -1;
                }
                break;

            case RTSP_CODEC_ID_VIDEO_H265:
                ret = rtp_enc_h265(&s->vrtpe, p, size, ts, &packets[count], &pktsizs[count]);
                if (ret <= 0) {
                    simple_rtsp_server_error("rtp_enc_h265 return:[%d]", ret);
                    return -1;
                }
                break;
        }

        count += ret;
        start = p - frame + size;
    }

    for (i = 0; i < count; i++) {
        *pktlens[i] = pktsizs[i];
        streamq_push(q);
    }

    TAILQ_FOREACH(cc, &s->connections_qhead, session_entry) {
        struct rtp_connection *rtp = cc->vrtp;
        if ((cc->state != RTSP_CC_STATE_PLAYING) || !rtp) {
            continue;
        }

        rtcp_try_tx_sr(rtp, s->video_ntptime_of_zero_ts, ts, s->vrtpe.sample_rate);
        rtsp_tx_video_packet(cc);
    }

    return len;
}

int simple_rtsp_server_send_video(simple_rtsp_handle_t handle, simple_rtsp_session_t session, const uint8_t *frame, int len, uint64_t ts)
{
    int i, index, count, start;
    struct stream_queue *q = NULL;
    int pktsizs[VRTP_MAX_NBPKTS + 1] = {0};
    int *pktlens[VRTP_MAX_NBPKTS] = {NULL};
    struct rtsp_client_connection *cc = NULL;
    uint8_t *packets[VRTP_MAX_NBPKTS + 1] = {NULL};
    struct rtsp_session *s = (struct rtsp_session *)session;

    if (!s || !frame || (s->vcodec_id == RTSP_CODEC_ID_NONE)) {
        return -1;
    }

    q = s->vstreamq;
    index = streamq_tail(q);
    for (i = 0; i < VRTP_MAX_NBPKTS; i++) {
        if (streamq_next(q, index) == streamq_head(q)) {
            streamq_pop(q);
        }

        streamq_query(q, index, (char **)&packets[i], &pktlens[i]);
        pktsizs[i] = RTP_MAX_PKTSIZ;
        index = streamq_next(q, index);
    }

    packets[i] = NULL;
    pktsizs[i] = 0;

    TAILQ_FOREACH(cc, &s->connections_qhead, session_entry) {
        struct rtp_connection *rtp = cc->vrtp;
        if ((cc->state != RTSP_CC_STATE_PLAYING) || !rtp) {
            continue;
        }

        if (!streamq_inused(q, rtp->streamq_index) && (rtp->streamq_index != streamq_tail(q))) {
            rtp->streamq_index = streamq_head(q);
        }
    }

    switch (s->vcodec_id) {
        case RTSP_CODEC_ID_VIDEO_H264:
            if ((s->vcodec_data.h264.pps_len == 0) || (s->vcodec_data.h264.pps_len == 0)) {
                if (rtsp_codec_data_parse_from_frame_h264(frame, len, &s->vcodec_data.h264) < 0) {
                    simple_rtsp_server_warn("rtsp_codec_data_parse_from_frame_h264 failed");
                }
            }
            break;

        case RTSP_CODEC_ID_VIDEO_H265:
            if ((s->vcodec_data.h265.pps_len == 0) || (s->vcodec_data.h265.pps_len == 0) || (s->vcodec_data.h265.vps_len == 0)) {
                if (rtsp_codec_data_parse_from_frame_h265(frame, len, &s->vcodec_data.h265) < 0) {
                    simple_rtsp_server_warn("rtsp_codec_data_parse_from_frame_h265 failed");
                }
            }
            break;
    }

    start = 0;
    count = 0;

    while ((start < len) && packets[count] && (pktsizs[count] > 0)) {
        int ret;
        int size = 0;
        const uint8_t *p = NULL;

        p = rtsp_find_h264_h265_nalu(frame + start, len - start, &size);
        if (!p) {
            simple_rtsp_server_warn("not found nal header");
            break;
        }

        switch (s->vcodec_id) {
            case RTSP_CODEC_ID_VIDEO_H264:
                ret = rtp_enc_h264(&s->vrtpe, p, size, ts, &packets[count], &pktsizs[count]);
                if (ret <= 0) {
                    simple_rtsp_server_error("rtp_enc_h264 return:[%d]", ret);
                    return -1;
                }
                break;

            case RTSP_CODEC_ID_VIDEO_H265:
                ret = rtp_enc_h265(&s->vrtpe, p, size, ts, &packets[count], &pktsizs[count]);
                if (ret <= 0) {
                    simple_rtsp_server_error("rtp_enc_h265 return:[%d]", ret);
                    return -1;
                }
                break;
        }

        count += ret;
        start = p - frame + size;
    }

    for (i = 0; i < count; i++) {
        *pktlens[i] = pktsizs[i];
        streamq_push(q);
    }

    TAILQ_FOREACH(cc, &s->connections_qhead, session_entry) {
        struct rtp_connection *rtp = cc->vrtp;
        if ((cc->state != RTSP_CC_STATE_PLAYING) || !rtp) {
            continue;
        }

        rtcp_try_tx_sr(rtp, s->video_ntptime_of_zero_ts, ts, s->vrtpe.sample_rate);
        rtsp_tx_video_packet(cc);
    }

    simple_rtsp_wait_frame_finished(handle);
    return len;
}

int simple_rtsp_send_audio(simple_rtsp_session_t session, const uint8_t *frame, int len, uint64_t ts)
{
    int i, index, count;
    struct stream_queue *q = NULL;
    int pktsizs[ARTP_MAX_NBPKTS + 1] = {0};
    int *pktlens[ARTP_MAX_NBPKTS] = {NULL};
    struct rtsp_client_connection *cc = NULL;
    uint8_t *packets[ARTP_MAX_NBPKTS + 1] = {NULL};
    struct rtsp_session *s = (struct rtsp_session *)session;

    if (!s || !frame || (s->acodec_id == RTSP_CODEC_ID_NONE)) {
        return -1;
    }

    q = s->astreamq;
    index = streamq_tail(q);
    for (i = 0; i < ARTP_MAX_NBPKTS; i++) {
        if (streamq_next(q, index) == streamq_head(q)) {
            streamq_pop(q);
        }

        streamq_query(q, index, (char **)&packets[i], &pktlens[i]);
        pktsizs[i] = RTP_MAX_PKTSIZ;
        index = streamq_next(q, index);
    }

    packets[i] = NULL;
    pktsizs[i] = 0;

    TAILQ_FOREACH(cc, &s->connections_qhead, session_entry) {
        struct rtp_connection *rtp = cc->artp;
        if ((cc->state != RTSP_CC_STATE_PLAYING) || !rtp) {
            continue;
        }

        if (!streamq_inused(q, rtp->streamq_index) && (rtp->streamq_index != streamq_tail(q))) {
            rtp->streamq_index = streamq_head(q);
            simple_rtsp_server_warn("client:[%s] will lost audio packet", inet_ntoa(cc->peer_addr));
        }
    }

    switch (s->acodec_id) {
        case RTSP_CODEC_ID_AUDIO_G711A:
        case RTSP_CODEC_ID_AUDIO_G711U:
            count = rtp_enc_g711(&s->artpe, frame, len, ts, packets, pktsizs);
            if (count <= 0) {
                simple_rtsp_server_error("rtp_enc_g711 return:[%d]", count);
                return -1;
            }
            break;

        case RTSP_CODEC_ID_AUDIO_G726:
            count = rtp_enc_g726(&s->artpe, frame, len, ts, packets, pktsizs);
            if (count <= 0) {
                simple_rtsp_server_error("rtp_enc_g726 return:[%d]", count);
                return -1;
            }
            break;

        case RTSP_CODEC_ID_AUDIO_AAC:
            if (s->acodec_data.aac.audio_specific_config_len == 0) {
                if (rtsp_codec_data_parse_from_frame_aac(frame, len, &s->acodec_data.aac) < 0) {
                    // simple_rtsp_server_warn("rtsp_codec_data_parse_from_frame_aac failed");
                } else {
                    // simple_rtsp_server_debug("rtsp_codec_data_parse_from_frame_aac success");
                    s->artpe.sample_rate = s->acodec_data.aac.sample_rate;
                }
            }

            count = rtp_enc_aac(&s->artpe, frame, len, ts, packets, pktsizs);
            if (count <= 0) {
                simple_rtsp_server_error("rtp_enc_aac return:[%d]", count);
                return -1;
            }
            break;
    }

    for (i = 0; i < count; i++) {
        *pktlens[i] = pktsizs[i];
        streamq_push(q);
    }

    TAILQ_FOREACH(cc, &s->connections_qhead, session_entry) {
        struct rtp_connection *rtp = cc->artp;
        if ((cc->state != RTSP_CC_STATE_PLAYING) || !rtp) {
            continue;
        }

        rtcp_try_tx_sr(rtp, s->audio_ntptime_of_zero_ts, ts, s->artpe.sample_rate);
        rtsp_tx_audio_packet(cc);
    }

    return len;
}

uint64_t simple_rtsp_get_reltime(void)
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return (tp.tv_sec * 1000000ULL + tp.tv_nsec / 1000ULL);
}

static uint64_t rtsp_get_abstime(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000000ULL + tv.tv_usec);
}

uint64_t simple_rtsp_get_ntptime(void)
{
#define NTP_OFFSET_US (2208988800000000ULL)
    return (rtsp_get_abstime() + NTP_OFFSET_US);
}

int simple_rtsp_sync_video_timestamp(simple_rtsp_session_t session, uint64_t ts, uint64_t ntptime)
{
    struct rtsp_session *s = (struct rtsp_session *)session;

    if (!s || (s->vcodec_id == RTSP_CODEC_ID_NONE)) {
        return -1;
    }

    s->video_ntptime_of_zero_ts = ntptime - ts;
    return 0;
}

int simple_rtsp_sync_audio_timestamp(simple_rtsp_session_t session, uint64_t ts, uint64_t ntptime)
{
    struct rtsp_session *s = (struct rtsp_session *)session;

    if (!s || (s->acodec_id == RTSP_CODEC_ID_NONE)) {
        return -1;
    }

    s->audio_ntptime_of_zero_ts = ntptime - ts;
    return 0;
}

struct rtcp_sr {
#ifdef __BIG_ENDIAN__
    uint16_t pt : 8;
    uint16_t v : 2;
    uint16_t p : 1;
    uint16_t rc : 5;
#else
    uint16_t rc : 5;
    uint16_t p : 1;
    uint16_t v : 2;
    uint16_t pt : 8;
#endif
    uint16_t length;
    uint32_t ssrc;
    uint32_t ntpts_msw;
    uint32_t ntpts_lsw;
    uint32_t rtp_ts;
    uint32_t packet_count;
    uint32_t octet_count;
};

static int rtcp_try_tx_sr(struct rtp_connection *c, uint64_t ntptime_of_zero_ts, uint64_t ts, uint32_t sample_rate)
{
    uint32_t rtp_ts = 0;
    uint32_t ntpts_msw = 0;
    uint32_t ntpts_lsw = 0;
    struct rtcp_sr sr = {0};
    int size = sizeof(sr);

    if (c->rtcp_last_ts && (ts < (c->rtcp_last_ts + 5000000ULL))) {
        return 0;
    }

    ntpts_msw = (uint32_t)((ntptime_of_zero_ts + ts) / 1000000ULL);
    ntpts_lsw = (uint32_t)(((ntptime_of_zero_ts + ts) % 1000000ULL) * (1ULL << 32) / 1000000ULL);
    rtp_ts = (uint32_t)(ts * sample_rate / 1000000ULL);

    sr.v = 2;
    sr.p = 0;
    sr.rc = 0;
    sr.pt = 200;
    sr.length = htons(size / 4 - 1);
    sr.ssrc = htonl(c->ssrc);
    sr.ntpts_msw = htonl(ntpts_msw);
    sr.ntpts_lsw = htonl(ntpts_lsw);
    sr.rtp_ts = htonl(rtp_ts);
    sr.packet_count = htonl(c->rtcp_packet_count);
    sr.octet_count = htonl(c->rtcp_octet_count);

    if (c->is_over_tcp) {
        int ret = -1;
        SOCKET sockfd = c->tcp_sockfd;
        uint8_t szbuf[4 + sizeof(struct rtcp_sr)];

        sockfd = c->tcp_sockfd;
        szbuf[0] = '$';
        szbuf[1] = c->tcp_interleaved[1];
        *((uint16_t *)&szbuf[2]) = htons(size);
        memcpy(&szbuf[4], &sr, size);

        ret = send(sockfd, (const char *)szbuf, sizeof(szbuf), MSG_NOSIGNAL);
        if (ret == SOCKET_ERROR) {
            if ((sk_errno() != SK_EAGAIN) && (sk_errno() != SK_EINTR)) {
                simple_rtsp_server_warn("rtcp over tcp send frame to %s failed, errstr:[%s]", inet_ntoa(c->peer_addr), sk_strerror(sk_errno()));
                return -1;
            }

            return 0;
        }
    } else {
        int ret = -1;
        struct sockaddr_in inaddr;
        SOCKET sockfd = c->udp_sockfd[1];

        memset(&inaddr, 0, sizeof(inaddr));
        inaddr.sin_family = AF_INET;
        inaddr.sin_addr = c->peer_addr;
        inaddr.sin_port = htons(c->udp_peerport[1]);

        ret = sendto(sockfd, (const char *)&sr, size, 0, (struct sockaddr *)&inaddr, sizeof(inaddr));
        if (ret == SOCKET_ERROR) {
            if ((sk_errno() != SK_EAGAIN) && (sk_errno() != SK_EINTR)) {
                simple_rtsp_server_warn("rtcp over udp send %d bytes to %s failed, errstr:[%s]", size, inet_ntoa(c->peer_addr), sk_strerror(sk_errno()));
                return -1;
            }

            return 0;
        }
    }

    c->rtcp_last_ts = ts;
    return size;
}
