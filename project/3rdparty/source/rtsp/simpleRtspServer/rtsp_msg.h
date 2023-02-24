#ifndef RTSP_SIMPLE_RTSP_SERVER_RTP_RTSP_MSG_H
#define RTSP_SIMPLE_RTSP_SERVER_RTP_RTSP_MSG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum __rtsp_msg_type_e {
    RTSP_MSG_TYPE_REQUEST = 0,
    RTSP_MSG_TYPE_RESPONSE,
    RTSP_MSG_TYPE_INTERLEAVED,
    RTSP_MSG_TYPE_BUTT,
} rtsp_msg_type_e;

typedef enum __rtsp_msg_method_e {
    RTSP_MSG_METHOD_OPTIONS = 0,
    RTSP_MSG_METHOD_DESCRIBE,
    RTSP_MSG_METHOD_SETUP,
    RTSP_MSG_METHOD_PLAY,
    RTSP_MSG_METHOD_RECORD,
    RTSP_MSG_METHOD_PAUSE,
    RTSP_MSG_METHOD_TEARDOWN,
    RTSP_MSG_METHOD_ANNOUNCE,
    RTSP_MSG_METHOD_SET_PARAMETER,
    RTSP_MSG_METHOD_GET_PARAMETER,
    RTSP_MSG_METHOD_REDIRECT,
    RTSP_MSG_METHOD_BUTT,
} rtsp_msg_method_e;

typedef enum __rtsp_msg_uri_scheme_e {
    RTSP_MSG_URI_SCHEME_RTSP = 0,
    RTSP_MSG_URI_SCHEME_RTSPU,
    RTSP_MSG_URI_SCHEME_BUTT,
} rtsp_msg_uri_scheme_e;

typedef struct __rtsp_msg_uri_s {
    rtsp_msg_uri_scheme_e scheme;
    uint16_t              port;
    char                  ipaddr[32];
    char                  abspath[64];
} rtsp_msg_uri_s;

typedef enum __rtsp_msg_version_e {
    RTSP_MSG_VERSION_1_0 = 0,
    RTSP_MSG_VERSION_BUTT,
} rtsp_msg_version_e;

typedef struct __rtsp_msg_request_line_s {
    rtsp_msg_method_e  method;
    rtsp_msg_uri_s     uri;
    rtsp_msg_version_e version;
} rtsp_msg_request_line_s;

typedef struct __rtsp_msg_response_line_s {
    rtsp_msg_version_e version;
    uint32_t           status_code;
} rtsp_msg_response_line_s;

typedef struct __rtsp_msg_interleaved_line_s {
    uint8_t  channel;
    uint16_t length;
    uint8_t  reserved;
} rtsp_msg_interleaved_line_s;

typedef struct __rtsp_msg_cseq_s {
    uint32_t cseq;
} rtsp_msg_cseq_s;

typedef struct __rtsp_msg_date_s {
    char http_date[32];
} rtsp_msg_date_s;

typedef struct __rtsp_msg_session_s {
    uint32_t session;
} rtsp_msg_session_s;

typedef enum __rtsp_msg_transport_type_e {
    RTSP_MSG_TRANSPORT_TYPE_RTP_AVP = 0,
    RTSP_MSG_TRANSPORT_TYPE_RTP_AVP_TCP,
    RTSP_MSG_TRANSPORT_TYPE_BUTT,
} rtsp_msg_transport_type_e;

typedef struct __rtsp_msg_transport_s {
    rtsp_msg_transport_type_e type;
    uint32_t                  flags;
#define RTSP_MSG_TRANSPORT_FLAG_SSRC        (1 << 0)
#define RTSP_MSG_TRANSPORT_FLAG_UNICAST     (1 << 1)
#define RTSP_MSG_TRANSPORT_FLAG_MULTICAST   (1 << 2)
#define RTSP_MSG_TRANSPORT_FLAG_CLIENT_PORT (1 << 3)
#define RTSP_MSG_TRANSPORT_FLAG_SERVER_PORT (1 << 4)
#define RTSP_MSG_TRANSPORT_FLAG_INTERLEAVED (1 << 5)
    uint32_t                  ssrc;
    uint16_t                  client_port;
    uint16_t                  server_port;
    uint8_t                   interleaved;
} rtsp_msg_transport_s;

typedef enum __rtsp_msg_time_type_e {
    RTSP_MSG_TIME_TYPE_SMPTE = 0,
    RTSP_MSG_TIME_TYPE_NPT,
    RTSP_MSG_TIME_TYPE_UTC,
    RTSP_MSG_TIME_TYPE_BUTT,
} rtsp_msg_time_type_e;

typedef struct __rtsp_msg_time_smpte_s {
    uint32_t seconds;
    uint32_t subframes;
} rtsp_msg_time_smpte_s;

typedef struct __rtsp_msg_time_npt_s {
    uint32_t secords;
    uint32_t usecords;
} rtsp_msg_time_npt_s;

typedef struct __rtsp_msg_time_utc_s {
    uint32_t secords;
    uint32_t usecords;
} rtsp_msg_time_utc_s;

typedef struct __rtsp_msg_range_s {
    rtsp_msg_time_type_e type;
    union __start_u {
        rtsp_msg_time_smpte_s smpte;
        rtsp_msg_time_npt_s   npt;
        rtsp_msg_time_utc_s   utc;
    } start;
    union __end_u {
        rtsp_msg_time_smpte_s smpte;
        rtsp_msg_time_npt_s   npt;
        rtsp_msg_time_utc_s   utc;
    } end;
} rtsp_msg_range_s;

typedef enum __rtsp_msg_content_type_e {
    RTSP_MSG_CONTENT_TYPE_SDP = 0,
    RTSP_MSG_CONTENT_TYPE_RTSL,
    RTSP_MSG_CONTENT_TYPE_MHEG,
    RTSP_MSG_CONTENT_TYPE_BUTT,
} rtsp_msg_content_type_e;

typedef struct __rtsp_msg_accept_s {
    uint32_t accept;
#define RTSP_MSG_ACCEPT_SDP     (1 << RTSP_MSG_CONTENT_TYPE_SDP)
#define RTSP_MSG_ACCEPT_RTSL    (1 << RTSP_MSG_CONTENT_TYPE_RTSL)
#define RTSP_MSG_ACCEPT_MHEG    (1 << RTSP_MSG_CONTENT_TYPE_MHEG)
} rtsp_msg_accept_s;

typedef struct __rtsp_msg_authorization_s {
    char authorization[128];
} rtsp_msg_authorization_s;

typedef struct __rtsp_msg_user_agent_s {
    char user_agent[64];
} rtsp_msg_user_agent_s;

typedef struct __rtsp_msg_public_s {
    uint32_t public_;
#define RTSP_MSG_PUBLIC_OPTIONS         (1 << RTSP_MSG_METHOD_OPTIONS)
#define RTSP_MSG_PUBLIC_DESCRIBE        (1 << RTSP_MSG_METHOD_DESCRIBE)
#define RTSP_MSG_PUBLIC_SETUP           (1 << RTSP_MSG_METHOD_SETUP)
#define RTSP_MSG_PUBLIC_PLAY            (1 << RTSP_MSG_METHOD_PLAY)
#define RTSP_MSG_PUBLIC_RECORD          (1 << RTSP_MSG_METHOD_RECORD)
#define RTSP_MSG_PUBLIC_PAUSE           (1 << RTSP_MSG_METHOD_PAUSE)
#define RTSP_MSG_PUBLIC_TEARDOWN        (1 << RTSP_MSG_METHOD_TEARDOWN)
#define RTSP_MSG_PUBLIC_ANNOUNCE        (1 << RTSP_MSG_METHOD_ANNOUNCE)
#define RTSP_MSG_PUBLIC_SET_PARAMETER   (1 << RTSP_MSG_METHOD_SET_PARAMETER)
#define RTSP_MSG_PUBLIC_GET_PARAMETER   (1 << RTSP_MSG_METHOD_GET_PARAMETER)
#define RTSP_MSG_PUBLIC_REDIRECT        (1 << RTSP_MSG_METHOD_REDIRECT)
} rtsp_msg_public_s;

typedef struct __rtsp_msg_rtp_subinfo_s {
    rtsp_msg_uri_s url;
    uint32_t       isseq;
    union __param_u {
        uint32_t   rtptime;
        uint32_t   seq;
    } param;
} rtsp_msg_rtp_subinfo_s;

typedef struct __rtsp_msg_rtp_info_s {
    uint32_t               ninfos;
    rtsp_msg_rtp_subinfo_s **info_array;
} rtsp_msg_rtp_info_s;

typedef struct __rtsp_msg_server_s {
    char server[64];
} rtsp_msg_server_s;

typedef struct __rtsp_msg_content_length_s {
    uint32_t length;
} rtsp_msg_content_length_s;

typedef struct __rtsp_msg_content_type_s {
    rtsp_msg_content_type_e type;
} rtsp_msg_content_type_s;

typedef struct __rtsp_msg_hdr_s {
    union __start_line_u {
        rtsp_msg_request_line_s     reqline;
        rtsp_msg_response_line_s    resline;
        rtsp_msg_interleaved_line_s interline;
    } startline;

    rtsp_msg_cseq_s           *cseq;
    rtsp_msg_date_s           *date;
    rtsp_msg_session_s        *session;
    rtsp_msg_transport_s      *transport;
    rtsp_msg_range_s          *range;

    rtsp_msg_accept_s         *accept;
    rtsp_msg_authorization_s  *authorization;
    rtsp_msg_user_agent_s     *user_agent;

    rtsp_msg_public_s         *public_;
    rtsp_msg_rtp_info_s       *rtp_info;
    rtsp_msg_server_s         *server;

    rtsp_msg_content_length_s *content_length;
    rtsp_msg_content_type_s   *content_type;
} rtsp_msg_hdr_s;

typedef struct __rtsp_msg_body_s {
    void *body;
} rtsp_msg_body_s;

typedef struct __rtsp_msg_s {
    rtsp_msg_type_e type;
    rtsp_msg_hdr_s  hdrs;
    rtsp_msg_body_s body;
} rtsp_msg_t;

void rtsp_mem_free(void *ptr);
void *rtsp_mem_alloc(int size);

char *rtsp_str_dup(const char *str);
void *rtsp_mem_dup(const void *ptr, int size);

int rtsp_msg_init(rtsp_msg_t *msg);
void rtsp_msg_free(rtsp_msg_t *msg);

int rtsp_msg_build_to_array(const rtsp_msg_t *msg, void *data, int size);
int rtsp_msg_parse_from_array(rtsp_msg_t *msg, const void *data, int size);

int rtsp_msg_set_response(rtsp_msg_t *msg, int status_code);
int rtsp_msg_set_request(rtsp_msg_t *msg, rtsp_msg_method_e mt, const char *ipaddr, const char *abspath);

int rtsp_msg_set_cseq(rtsp_msg_t *msg, uint32_t cseq);
int rtsp_msg_get_cseq(const rtsp_msg_t *msg, uint32_t *cseq);

int rtsp_msg_set_session(rtsp_msg_t *msg, uint32_t session);
int rtsp_msg_get_session(const rtsp_msg_t *msg, uint32_t *session);

int rtsp_msg_set_date(rtsp_msg_t *msg, const char *date);
int rtsp_msg_get_date(const rtsp_msg_t *msg, char *date, int len);

int rtsp_msg_set_transport_tcp(rtsp_msg_t *msg, uint32_t ssrc, int interleaved);
int rtsp_msg_set_transport_udp(rtsp_msg_t *msg, uint32_t ssrc, int client_port, int server_port);

int rtsp_msg_set_accept(rtsp_msg_t *msg, uint32_t accept);
int rtsp_msg_get_accept(const rtsp_msg_t *msg, uint32_t *accept);

int rtsp_msg_set_user_agent(rtsp_msg_t *msg, const char *user_agent);
int rtsp_msg_get_user_agent(const rtsp_msg_t *msg, char *user_agent, int len);

int rtsp_msg_set_public(rtsp_msg_t *msg, uint32_t public_);
int rtsp_msg_get_public(const rtsp_msg_t *msg, uint32_t *public_);

int rtsp_msg_set_server(rtsp_msg_t *msg, const char *server);
int rtsp_msg_get_server(const rtsp_msg_t *msg, char *server, int len);

int rtsp_msg_set_content_type(rtsp_msg_t *msg, int type);
int rtsp_msg_get_content_type(const rtsp_msg_t *msg, int *type);

int rtsp_msg_set_content_length(rtsp_msg_t *msg, int length);
int rtsp_msg_get_content_length(const rtsp_msg_t *msg, int *length);

uint32_t rtsp_msg_gen_session_id(void);

#ifdef __cplusplus
}
#endif

#endif
