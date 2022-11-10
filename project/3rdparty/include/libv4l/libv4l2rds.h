#ifndef LIBV4L_LIBV4L2RDS
#define LIBV4L_LIBV4L2RDS

#include <stdint.h>
#include <stdbool.h>
#include <libv4l/videodev2.h>

#ifdef __cplusplus
extern "C" {
#endif

#if HAVE_VISIBILITY
#define LIBV4L_PUBLIC                   __attribute__ ((visibility("default")))
#else
#define LIBV4L_PUBLIC
#endif

#define V4L2_RDS_VERSION                (2)

#define MAX_ODA_CNT                     18
#define MAX_AF_CNT                      25
#define MAX_TMC_ADDITIONAL              28
#define MAX_TMC_ALT_STATIONS            32
#define MAX_TMC_AF_CNT                  4
#define MAX_EON_CNT                     20

#define V4L2_RDS_PI                     0x01        /* Program Identification */
#define V4L2_RDS_PTY                    0x02        /* Program Type */
#define V4L2_RDS_TP                     0x04        /* Traffic Program */
#define V4L2_RDS_PS                     0x08        /* Program Service Name */
#define V4L2_RDS_TA                     0x10        /* Traffic Announcement */
#define V4L2_RDS_DI                     0x20        /* Decoder Information */
#define V4L2_RDS_MS                     0x40        /* Music / Speech flag */
#define V4L2_RDS_PTYN                   0x80        /* Program Type Name */
#define V4L2_RDS_RT                     0x100       /* Radio-Text */
#define V4L2_RDS_TIME                   0x200       /* Date and Time information */
#define V4L2_RDS_TMC                    0x400       /* TMC availability */
#define V4L2_RDS_AF                     0x800       /* AF (alternative freq) available */
#define V4L2_RDS_ECC                    0x1000      /* Extended County Code */
#define V4L2_RDS_LC                     0x2000      /* Language Code */
#define V4L2_RDS_TMC_SG                 0x4000      /* RDS-TMC single group */
#define V4L2_RDS_TMC_MG                 0x8000      /* RDS-TMC multi group */
#define V4L2_RDS_TMC_SYS                0x10000     /* RDS-TMC system information */
#define V4L2_RDS_EON                    0x20000     /* Enhanced Other Network Info */
#define V4L2_RDS_LSF                    0x40000     /* Linkage information */
#define V4L2_RDS_TMC_TUNING             0x80000     /* RDS-TMC tuning information */

#define V4L2_RDS_GROUP_NEW              0x01        /* New group received */
#define V4L2_RDS_ODA                    0x02        /* Open Data Group announced */

#define V4L2_RDS_FLAG_STEREO            0x01
#define V4L2_RDS_FLAG_ARTIFICIAL_HEAD   0x02
#define V4L2_RDS_FLAG_COMPRESSED        0x04
#define V4L2_RDS_FLAG_DYNAMIC_PTY       0x08

#define V4L2_TMC_TUNING_INFO            0x10        /* Bit 4 indicates Tuning Info / User msg */
#define V4L2_TMC_SINGLE_GROUP           0x08        /* Bit 3 indicates Single / Multi-group msg */

struct v4l2_rds_group {
    uint16_t pi;                                    /* Program Identification */
    char     group_version;                         /* group version ('A' / 'B') */
    uint8_t  group_id;                              /* group number (0..16) */

    uint8_t  data_b_lsb;
    uint8_t  data_c_msb;
    uint8_t  data_c_lsb;
    uint8_t  data_d_msb;
    uint8_t  data_d_lsb;
};

struct v4l2_rds_statistics {
    uint32_t block_cnt;                             /* total amount of received blocks */
    uint32_t group_cnt;                             /* total amount of successfully decoded groups */
    uint32_t block_error_cnt;                       /* blocks that were marked as erroneous and had to be dropped */
    uint32_t group_error_cnt;                       /* group decoding processes that had to be aborted because of erroneous blocks or wrong order of blocks */
    uint32_t block_corrected_cnt;                   /* blocks that contained 1-bit errors which were corrected */
    uint32_t group_type_cnt[16];                    /* number of occurrence for each defined RDS group */
};

struct v4l2_rds_oda {
    uint8_t  group_id;                              /* RDS group used to broadcast this ODA */
    char     group_version;                         /* group version (A / B) for this ODA */
    uint16_t aid;                                   /* Application Identification for this ODA, AIDs are centrally administered by the RDS Registration Office (rds.org.uk) */
};

struct v4l2_rds_oda_set {
    uint8_t             size;                       /* number of ODAs defined by this channel */
    struct v4l2_rds_oda oda[MAX_ODA_CNT];
};

struct v4l2_rds_af_set {
    uint8_t  size;                                  /* size of the set (might be smaller than the announced size) */
    uint8_t  announced_af;                          /* number of announced AF */
    uint32_t af[MAX_AF_CNT];                        /* AFs defined in Hz */
};

struct v4l2_rds_eon {
    uint32_t               valid_fields;
    uint16_t               pi;
    uint8_t                ps[9];
    uint8_t                pty;
    bool                   ta;
    bool                   tp;
    uint16_t               lsf;                     /* Linkage Set Number */
    struct v4l2_rds_af_set af;
};

struct v4l2_rds_eon_set {
    uint8_t             size;                       /* size of the table */
    uint8_t             index;                      /* current position in the table */
    struct v4l2_rds_eon eon[MAX_EON_CNT];           /* Information about other radio channels */
};

struct v4l2_tmc_alt_freq {
    uint8_t  af_size;                               /* number of known AFs */
    uint8_t  af_index;
    uint8_t  mapped_af_size;                        /* number of mapped AFs */
    uint8_t  mapped_af_index;
    uint32_t af[MAX_TMC_AF_CNT];                    /* AFs defined in Hz */
    uint32_t mapped_af[MAX_TMC_AF_CNT];             /* mapped AFs defined in Hz */
    uint32_t mapped_af_tuning[MAX_TMC_AF_CNT];      /* mapped AFs defined in Hz */
};

struct v4l2_tmc_station {
    uint16_t                 pi;
    uint8_t                  ltn;                   /* database-ID of ON */
    uint8_t                  msg;                   /* msg parameters of ON */
    uint8_t                  sid;                   /* service-ID of ON */
    struct v4l2_tmc_alt_freq afi;
};

struct v4l2_tmc_tuning {
    uint8_t                 station_cnt;            /* number of announced alternative stations */
    uint8_t                 index;
    struct v4l2_tmc_station station[MAX_TMC_ALT_STATIONS];
};

struct v4l2_tmc_additional {
    uint8_t label;
    uint16_t data;
};

struct v4l2_tmc_additional_set {
    uint8_t                    size;
    struct v4l2_tmc_additional fields[MAX_TMC_ADDITIONAL];
};

struct v4l2_rds_tmc_msg {
    uint8_t                        length;          /* length of multi-group message (0..4) */
    uint8_t                        sid;             /* service identifier at time of reception */
    uint8_t                        extent;
    uint8_t                        dp;              /* duration and persistence */
    uint16_t                       event;           /* TMC event code */
    uint16_t                       location;        /* TMC event location */
    bool                           follow_diversion;/* indicates if the driver is adviced to follow the diversion */
    bool                           neg_direction;   /* indicates negative / positive direction */
    struct v4l2_tmc_additional_set additional;
};

struct v4l2_rds_tmc {
    uint8_t                 ltn;                    /* location_table_number */
    bool                    afi;                    /* alternative frequency indicator */
    bool                    enhanced_mode;
    uint8_t                 mgs;                    /* message geographical scope */
    uint8_t                 sid;                    /* service identifier (unique ID on national level) */
    uint8_t                 gap;                    /* Gap parameters */
    uint8_t                 t_a;                    /* activity time (only if mode = enhanced) */
    uint8_t                 t_w;                    /* window time (only if mode = enhanced */
    uint8_t                 t_d;                    /* delay time (only if mode = enhanced */
    uint8_t                 spn[9];                 /* service provider name */
    struct v4l2_rds_tmc_msg tmc_msg;
    struct v4l2_tmc_tuning  tuning;
};

struct v4l2_rds {
    uint32_t                   decode_information;  /* state of decoding process */
    uint32_t                   valid_fields;        /* currently valid info fields of this structure */

    bool                       is_rbds;             /* use RBDS standard version of LUTs */
    uint16_t                   pi;                  /* Program Identification */
    uint8_t                    ps[9];               /* Program Service Name, UTF-8 encoding, '\0' terminated */
    uint8_t                    pty;                 /* Program Type */
    uint8_t                    ptyn[9];             /* Program Type Name, UTF-8 encoding, '\0' terminated */
    bool                       ptyn_ab_flag;        /* PTYN A/B flag (toggled), to signal change of PTYN */
    uint8_t                    rt_length;           /* length of RT string */
    uint8_t                    rt[65];              /* Radio-Text string, UTF-8 encoding, '\0' terminated */
    bool                       rt_ab_flag;          /* RT A/B flag (toggled), to signal transmission of new RT */
    bool                       ta;                  /* Traffic Announcement */
    bool                       tp;                  /* Traffic Program */
    bool                       ms;                  /* Music / Speech flag */
    uint8_t                    di;                  /* Decoder Information */
    uint8_t                    ecc;                 /* Extended Country Code */
    uint8_t                    lc;                  /* Language Code */
    time_t                     time;                /* local time and date of transmission */

    struct v4l2_rds_statistics rds_statistics;
    struct v4l2_rds_oda_set    rds_oda;             /* Open Data Services */
    struct v4l2_rds_af_set     rds_af;              /* Alternative Frequencies */
    struct v4l2_rds_eon_set    rds_eon;             /* EON information */
    struct v4l2_rds_tmc        tmc;                 /* TMC information */
};

LIBV4L_PUBLIC struct v4l2_rds *v4l2_rds_create(bool is_rbds);
LIBV4L_PUBLIC void v4l2_rds_destroy(struct v4l2_rds *handle);

LIBV4L_PUBLIC void v4l2_rds_reset(struct v4l2_rds *handle, bool reset_statistics);
LIBV4L_PUBLIC uint32_t v4l2_rds_add(struct v4l2_rds *handle, struct v4l2_rds_data *rds_data);

LIBV4L_PUBLIC const char *v4l2_rds_get_pty_str(const struct v4l2_rds *handle);
LIBV4L_PUBLIC const char *v4l2_rds_get_language_str(const struct v4l2_rds *handle);
LIBV4L_PUBLIC const char *v4l2_rds_get_country_str(const struct v4l2_rds *handle);
LIBV4L_PUBLIC const char *v4l2_rds_get_coverage_str(const struct v4l2_rds *handle);

LIBV4L_PUBLIC const struct v4l2_rds_group *v4l2_rds_get_group(const struct v4l2_rds *handle);

#ifdef __cplusplus
}
#endif

#endif
