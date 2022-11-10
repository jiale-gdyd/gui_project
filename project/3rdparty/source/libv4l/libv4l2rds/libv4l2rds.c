#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>

#include <libv4l/videodev2.h>
#include <libv4l/libv4l2rds.h>

#include "../config.h"

struct rds_private_state {
    struct v4l2_rds         handle;
    uint8_t                 decode_state;
    uint16_t                new_pi;
    uint8_t                 new_ps[8];
    uint8_t                 new_ps_valid[8];
    uint8_t                 new_pty;
    uint8_t                 new_ptyn[2][4];
    bool                    new_ptyn_valid[2];
    uint8_t                 new_rt[64];
    uint8_t                 next_rt_segment;
    uint8_t                 new_di;
    uint8_t                 next_di_segment;
    uint8_t                 new_ecc;
    uint8_t                 new_lc;
    uint32_t                new_mjd;
    uint8_t                 utc_hour;
    uint8_t                 utc_minute;
    uint8_t                 utc_offset;
    uint8_t                 continuity_id;
    uint8_t                 grp_seq_id;
    bool                    optional_tmc[112];
    struct v4l2_rds_group   prev_tmc_group;
    struct v4l2_rds_group   prev_tmc_sys_group;
    struct v4l2_rds_tmc_msg new_tmc_msg;
    struct v4l2_rds_group   rds_group;
    struct v4l2_rds_data    rds_data_raw[4];
};

enum rds_state {
    RDS_EMPTY,
    RDS_A_RECEIVED,
    RDS_B_RECEIVED,
    RDS_C_RECEIVED,
};

static inline uint8_t set_bit(uint8_t input, uint8_t bitmask, bool bitvalue)
{
    return bitvalue ? input | bitmask : input & ~bitmask;
}

static uint32_t rds_decode_a(struct rds_private_state *priv_state, struct v4l2_rds_data *rds_data)
{
    uint32_t updated_fields = 0;
    struct v4l2_rds *handle = &priv_state->handle;
    uint16_t pi = (rds_data->msb << 8) | rds_data->lsb;

    priv_state->rds_group.pi = pi;

    if ((pi != handle->pi) && (pi == priv_state->new_pi)) {
        handle->pi = pi;
        handle->valid_fields |= V4L2_RDS_PI;
        updated_fields |= V4L2_RDS_PI;
    } else if ((pi != handle->pi) && (pi != priv_state->new_pi)) {
        priv_state->new_pi = pi;
    }

    return updated_fields;
}

static uint32_t rds_decode_b(struct rds_private_state *priv_state, struct v4l2_rds_data *rds_data)
{
    uint8_t pty;
    bool traffic_prog;
    uint32_t updated_fields = 0;
    struct v4l2_rds *handle = &priv_state->handle;
    struct v4l2_rds_group *grp = &priv_state->rds_group;

    grp->group_id = rds_data->msb >> 4 ;
    grp->group_version = (rds_data->msb & 0x08) ? 'B' : 'A';

    traffic_prog = rds_data->msb & 0x04;
    if (handle->tp != traffic_prog) {
        handle->tp = traffic_prog;
        updated_fields |= V4L2_RDS_TP;
    }
    handle->valid_fields |= V4L2_RDS_TP;

    grp->data_b_lsb = rds_data->lsb & 0x1f;
    pty = (rds_data->msb << 3) | (rds_data->lsb >> 5);
    pty &= 0x1f;

    if (handle->pty == pty) {
        priv_state->new_pty = pty;
        return updated_fields;
    }

    if (priv_state->new_pty == pty) {
        handle->pty = priv_state->new_pty;
        updated_fields |= V4L2_RDS_PTY;
        handle->valid_fields |= V4L2_RDS_PTY;
    } else {
        priv_state->new_pty = pty;
    }

    return updated_fields;
}

static void rds_decode_c(struct rds_private_state *priv_state, struct v4l2_rds_data *rds_data)
{
    struct v4l2_rds_group *grp = &priv_state->rds_group;

    grp->data_c_msb = rds_data->msb;
    grp->data_c_lsb = rds_data->lsb;
}

static void rds_decode_d(struct rds_private_state *priv_state, struct v4l2_rds_data *rds_data)
{
    struct v4l2_rds_group *grp = &priv_state->rds_group;

    grp->data_d_msb = rds_data->msb;
    grp->data_d_lsb = rds_data->lsb;
}

static uint32_t rds_decode_af(uint8_t af, bool is_vhf)
{
    uint32_t freq = 0;

    if ((af == 0) || (af >= 205)) {
        return 0;
    }

    if (is_vhf) {
        freq = 87500000 + af * 100000;
    } else if (af <= 15) {
        freq = 152000 + af * 9000;
    } else {
        freq = 531000 + af * 9000;
    }

    return freq;
}

static bool rds_compare_group(const struct v4l2_rds_group *a, const struct v4l2_rds_group *b)
{
    if (a->pi != b->pi) {
        return false;
    }

    if (a->group_version != b->group_version) {
        return false;
    }

    if (a->group_id != b->group_id) {
        return false;
    }

    if (a->data_b_lsb != b->data_b_lsb) {
        return false;
    }

    if ((a->data_c_lsb != b->data_c_lsb) || (a->data_c_msb != b->data_c_msb)) {
        return false;
    }

    if ((a->data_d_lsb != b->data_d_lsb) || (a->data_d_msb != b->data_d_msb)) {
        return false;
    }

    return true;
}

static int rds_add_tmc_station(struct rds_private_state *priv_state, uint16_t pi)
{
    struct v4l2_tmc_tuning *tuning = &priv_state->handle.tmc.tuning;
    uint8_t index = tuning->index;
    uint8_t size = tuning->station_cnt;

    for (int i = 0; i < tuning->station_cnt; i++) {
        if (tuning->station[i].pi == pi) {
            return i;
        }
    }

    tuning->station[index].pi = pi;
    tuning->index = (index+1 < MAX_TMC_ALT_STATIONS) ? (index+1) : 0;
    tuning->station_cnt = (size+1 <= MAX_TMC_ALT_STATIONS) ? (size+1) : MAX_TMC_ALT_STATIONS;

    return index;
}

static bool rds_add_tmc_af(struct rds_private_state *priv_state)
{
    uint8_t af_index;
    uint8_t mapped_af_index;
    struct v4l2_tmc_alt_freq *afi;
    struct v4l2_rds_group *grp = &priv_state->rds_group;
    uint16_t pi_on = grp->data_d_msb << 8 | grp->data_d_lsb;
    uint8_t variant = grp->data_b_lsb & 0x0f;
    uint8_t station_index = rds_add_tmc_station(priv_state, pi_on);
    uint32_t freq_a = rds_decode_af(grp->data_c_msb, true);
    uint32_t freq_b = rds_decode_af(grp->data_c_lsb, true);

    afi = &priv_state->handle.tmc.tuning.station[station_index].afi;
    af_index = afi->af_index;
    mapped_af_index = afi->mapped_af_index;

    if (variant == 6) {
        for (int i = 0; i < afi->af_size; i++) {
            freq_a = (freq_a == afi->af[i]) ? 0 : freq_a;
            freq_b = (freq_b == afi->af[i]) ? 0 : freq_b;
        }

        if ((freq_a == 0) && (freq_b == 0)) {
            return false;
        }

        if (freq_a != 0) {
            afi->af[af_index] = freq_a;
            af_index = (af_index+1 < MAX_TMC_AF_CNT) ? af_index+1 : 0;
            afi->af_size++;
        }

        if (freq_b != 0) {
            afi->af[af_index] = freq_b;
            af_index = (af_index+1 < MAX_TMC_AF_CNT) ? af_index+1 : 0;
            afi->af_size++;
        }
 
        afi->af_index = af_index;
        if (afi->af_size >= MAX_TMC_AF_CNT) {
            afi->af_size = MAX_TMC_AF_CNT;
        }

        return true;
    }

    if (variant == 7) {
        for (int i = 0; i < afi->mapped_af_size; i++) {
            if (freq_a == afi->mapped_af_tuning[i]) {
                afi->mapped_af[i] = freq_b;
                return true;
            }
        }

        if ((freq_a != 0) && (freq_b != 0)) {
            mapped_af_index = (mapped_af_index+1 >= MAX_TMC_AF_CNT) ? 0 : mapped_af_index + 1;
            afi->mapped_af[mapped_af_index] = freq_b;
            afi->mapped_af_tuning[mapped_af_index] = freq_a;
            afi->mapped_af_size++;
        }

        afi->mapped_af_index = mapped_af_index;
        if (afi->mapped_af_size >= MAX_TMC_AF_CNT) {
            afi->mapped_af_size = MAX_TMC_AF_CNT;
        }

        return true;
    }

    return false;
}

void rds_tmc_decode_additional(struct rds_private_state *priv_state)
{
    uint8_t len;
    uint8_t label;
    uint16_t data;
    uint8_t array_idx = 0;
    const uint8_t label_len = 4;
    struct v4l2_rds_tmc_msg *msg = &priv_state->handle.tmc.tmc_msg;
    struct v4l2_tmc_additional *fields = &msg->additional.fields[0];
    uint8_t *field_idx = &msg->additional.size;
    static const uint8_t additional_lut[16] = {
        3, 3, 5, 5, 5, 8, 8, 8, 8, 11, 16, 16, 16, 16, 0, 0
    };

    *field_idx = 0;
    memset(fields, 0, sizeof(*fields));

    while (array_idx < (msg->length * 28)) {
        label = 0;
        for (int i = 0; i < label_len; i++) {
            if (priv_state->optional_tmc[array_idx++]) {
                label |= 1 << (label_len - 1 - i);
            }
        }

        data = 0;
        len = additional_lut[label];
        for (int i = 0; i < len; i++) {
            if (priv_state->optional_tmc[array_idx++]) {
                data |= 1 << (len - 1 - i);
            }
        }

        if (label == 15) {
            continue;
        }

        if ((label == 0) && (data == 0)) {
            continue;
        }

        fields[*field_idx].label = label;
        fields[*field_idx].data = data;
        *field_idx += 1;
    }
}

static uint32_t rds_decode_tmc_system(struct rds_private_state *priv_state)
{
    uint8_t variant_code;
    struct v4l2_rds_group *group = &priv_state->rds_group;
    struct v4l2_rds_tmc *tmc = &priv_state->handle.tmc;

    if (!rds_compare_group(&priv_state->prev_tmc_sys_group, &priv_state->rds_group)) {
        priv_state->prev_tmc_sys_group = priv_state->rds_group;
        return 0;
    }

    variant_code = priv_state->rds_group.data_c_msb >> 6;
    switch (variant_code) {
        case 0x00:
            tmc->ltn = (((group->data_c_msb & 0x0f) << 2)) | (group->data_c_lsb >> 6);
            tmc->afi = group->data_c_lsb & 0x20;
            tmc->enhanced_mode = group->data_c_lsb & 0x10;
            tmc->mgs = group->data_c_lsb & 0x0f;
            break;

        case 0x01:
            tmc->gap = (group->data_c_msb & 0x30) >> 4;
            tmc->sid = (((group->data_c_msb & 0x0f) << 2)) | (group->data_c_lsb >> 6);
            if (!tmc->enhanced_mode) {
                break;
            }

            tmc->t_a = (group->data_c_lsb & 0x30) >> 4;
            tmc->t_w = (group->data_c_lsb & 0x0c) >> 2;
            tmc->t_d = group->data_c_lsb & 0x03;
            break;
    }

    return V4L2_RDS_TMC_SYS;
}

static uint32_t rds_decode_tmc_single_group(struct rds_private_state *priv_state)
{
    struct v4l2_rds_tmc_msg msg;
    struct v4l2_rds_group *grp = &priv_state->rds_group;

    msg.dp = grp->data_b_lsb & 0x07;
    msg.follow_diversion = grp->data_c_msb & 0x80;
    msg.neg_direction = grp->data_c_msb & 0x40;
    msg.extent = (grp->data_c_msb & 0x38) >> 3;
    msg.event = ((grp->data_c_msb & 0x07) << 8) | grp->data_c_lsb;
    msg.location = (grp->data_d_msb << 8) | grp->data_c_lsb;
    msg.sid = 0;

    priv_state->handle.tmc.tmc_msg = msg;
    priv_state->handle.valid_fields |= V4L2_RDS_TMC_SG;
    priv_state->handle.valid_fields &= ~V4L2_RDS_TMC_MG;

    return V4L2_RDS_TMC_SG;
}

static uint32_t rds_decode_tmc_multi_group(struct rds_private_state *priv_state)
{
    uint64_t buffer;
    uint8_t grp_seq_id;
    bool message_completed = false;
    struct v4l2_rds_group *grp = &priv_state->rds_group;
    struct v4l2_rds_tmc_msg *msg = &priv_state->new_tmc_msg;

    grp_seq_id = (grp->data_c_msb & 0x30) >> 4;

    if (grp->data_c_msb & 0x80) {
        memset(msg, 0, sizeof(*msg));
        memset(priv_state->optional_tmc, 0, 112 * sizeof(bool)); 

        priv_state->continuity_id = grp->data_b_lsb & 0x07;
        msg->follow_diversion = grp->data_c_msb & 0x80;
        msg->neg_direction = grp->data_c_msb & 0x40;
        msg->extent = (grp->data_c_msb & 0x38) >> 3;
        msg->event = ((grp->data_c_msb & 0x07) << 8) | grp->data_c_lsb;
        msg->location = (grp->data_d_msb << 8) | grp->data_c_lsb;
    } else if ((grp->data_c_msb & 0x40) && ((grp->data_b_lsb & 0x07) == priv_state->continuity_id)) {
        priv_state->grp_seq_id = grp_seq_id;
        msg->length = 1;
        buffer = grp->data_c_msb << 24 | grp->data_c_lsb << 16 | grp->data_d_msb << 8 | grp->data_d_lsb;

        for (int i = 27; i >= 0; i--) {
            if (buffer & (1 << i)) {
                priv_state->optional_tmc[27 - i] = true;
            }
        }

        if (grp_seq_id == 0) {
            message_completed = true;
        }
    } else if (((grp->data_b_lsb & 0x07) == priv_state->continuity_id) && (grp_seq_id == priv_state->grp_seq_id-1)) {
        priv_state->grp_seq_id = grp_seq_id;
        msg->length += 1;
        buffer = grp->data_c_msb << 24 | grp->data_c_lsb << 16| grp->data_d_msb << 8 | grp->data_d_lsb;

        for (int i = 27; i >= 0; i--) {
            if (buffer & (1 << i)) {
                priv_state->optional_tmc[msg->length * 28 + 27 - i] = true;
            }
        }

        if (grp_seq_id == 0) {
            message_completed = true;
        }
    }

    if (message_completed) {
        priv_state->handle.tmc.tmc_msg = *msg;
        rds_tmc_decode_additional(priv_state);
        priv_state->handle.valid_fields |= V4L2_RDS_TMC_MG;
        priv_state->handle.valid_fields &= ~V4L2_RDS_TMC_SG;
    }

    return V4L2_RDS_TMC_MG;
}

static uint32_t rds_decode_tmc_tuning(struct rds_private_state *priv_state)
{
    uint8_t index;
    struct v4l2_rds_group *group = &priv_state->rds_group;
    struct v4l2_rds_tmc *tmc = &priv_state->handle.tmc;
    uint8_t variant_code = group->data_b_lsb & 0x0f;
    uint16_t pi_on = (group->data_d_msb << 8) | group->data_d_lsb;

    if ((variant_code >= 4) && (variant_code <= 5)) {
        int offset = 4 * (variant_code - 4);
        tmc->spn[0 + offset] = group->data_c_msb;
        tmc->spn[1 + offset] = group->data_c_lsb;
        tmc->spn[2 + offset] = group->data_d_msb;
        tmc->spn[3 + offset] = group->data_d_lsb;
    } else if ((variant_code == 6) || (variant_code == 7)) {
        rds_add_tmc_af(priv_state);
    } else if (variant_code == 8) {
        uint16_t pi_on_2 = (group->data_c_msb << 8) | group->data_c_lsb;

        rds_add_tmc_station(priv_state, pi_on);
        if (pi_on_2 != 0) {
            rds_add_tmc_station(priv_state, pi_on_2);
        }
    } else if (variant_code == 9) {
        index = rds_add_tmc_station(priv_state, pi_on);

        tmc->tuning.station[index].sid = group->data_c_lsb & 0x3F;
        tmc->tuning.station[index].msg = (group->data_c_msb & 0x03) << 2;
        tmc->tuning.station[index].msg |= (group->data_c_lsb >> 6) & 0x03;
        tmc->tuning.station[index].ltn = group->data_c_msb >> 2;
    }

    return V4L2_RDS_TMC_TUNING;
}

static bool rds_add_oda(struct rds_private_state *priv_state, struct v4l2_rds_oda oda)
{
    struct v4l2_rds *handle = &priv_state->handle;

    for (int i = 0; i < handle->rds_oda.size; i++) {
        if (handle->rds_oda.oda[i].group_id == oda.group_id) {
            handle->rds_oda.oda[i].aid = oda.aid;
            return false;
        }
    }

    if (handle->rds_oda.size >= MAX_ODA_CNT) {
        return false;
    }

    handle->rds_oda.oda[handle->rds_oda.size++] = oda;
    return true;
}

static bool rds_add_af_to_list(struct v4l2_rds_af_set *af_set, uint8_t af, bool is_vhf)
{
    uint32_t freq = rds_decode_af(af, is_vhf);
    if (freq == 0) {
        return false;
    }

    if ((af_set->size >= MAX_AF_CNT) || (af_set->size >= af_set->announced_af)) {
        return false;
    }

    for (int i = 0; i < af_set->size; i++) {
        if (af_set->af[i] == freq) {
            return false;
        }
    }

    af_set->af[af_set->size++] = freq;
    return true;
}

static bool rds_add_af(struct rds_private_state *priv_state)
{
    bool updated_af = false;
    struct v4l2_rds *handle = &priv_state->handle;
    uint8_t c_msb = priv_state->rds_group.data_c_msb;
    uint8_t c_lsb = priv_state->rds_group.data_c_lsb;
    struct v4l2_rds_af_set *af_set = &handle->rds_af;

    if (c_msb == 250) {
        if (rds_add_af_to_list(af_set, c_lsb, false)) {
            updated_af = true;
        }

        c_lsb = 0;
    }

    if ((c_msb >= 224) && (c_msb <= 249)) {
        if (af_set->announced_af != c_msb - 224) {
            updated_af = true;
            af_set->size = 0;
        }

        af_set->announced_af = c_msb - 224;
    }

    if (c_msb < 205) {
        if (rds_add_af_to_list(af_set, c_msb, true)) {
            updated_af = true;
        }
    }

    if (c_lsb < 205) {
        if (rds_add_af_to_list(af_set, c_lsb, true)) {
            updated_af = true;
        }
    }

    if ((af_set->size >= af_set->announced_af) && (af_set->announced_af != 0)) {
        handle->valid_fields |= V4L2_RDS_AF;
    }

    return updated_af;
}

static bool rds_add_ps(struct rds_private_state *priv_state, uint8_t pos, uint8_t ps_char)
{
    if (ps_char == priv_state->new_ps[pos]) {
        priv_state->new_ps_valid[pos] = 1;
    } else {
        priv_state->new_ps[pos] = ps_char;
        memset(priv_state->new_ps_valid, 0, 8);
    }

    for (int i = 0; i < 8; i++) {
        if (priv_state->new_ps_valid[i] != 1) {
            return false;
        }
    }

    return true;
}

static uint8_t rds_add_eon_entry(struct rds_private_state *priv_state, uint16_t pi)
{
    struct v4l2_rds *handle = &priv_state->handle;
    uint8_t index = handle->rds_eon.index;
    uint8_t size = handle->rds_eon.size;

    for (int i = 0; i < handle->rds_eon.size; i++) {
        if (handle->rds_eon.eon[i].pi == pi) {
            return i;
        }
    }

    handle->rds_eon.eon[index].pi = pi;
    handle->rds_eon.eon[index].valid_fields |= V4L2_RDS_PI;
    handle->rds_eon.index = (index+1 < MAX_EON_CNT) ? (index+1) : 0;
    handle->rds_eon.size = (size+1 <= MAX_EON_CNT) ? (size+1) : MAX_EON_CNT;

    return index;
}

static bool rds_check_eon_entry(struct rds_private_state *priv_state, uint16_t pi)
{
    struct v4l2_rds *handle = &priv_state->handle;

    for (int i = 0; i <= handle->rds_eon.size; i++) {
        if (handle->rds_eon.eon[i].pi == pi) {
            return true;
        }
    }

    return false;
}

static uint32_t rds_decode_group0(struct rds_private_state *priv_state)
{
    bool tmp;
    bool new_ps = false;
    uint32_t updated_fields = 0;
    struct v4l2_rds *handle = &priv_state->handle;
    struct v4l2_rds_group *grp = &priv_state->rds_group;

    tmp = grp->data_b_lsb & 0x10;
    if (handle->ta != tmp) {
        handle->ta = tmp;
        updated_fields |= V4L2_RDS_TA;
    }
    handle->valid_fields |= V4L2_RDS_TA;

    tmp = grp->data_b_lsb & 0x08;
    if (handle->ms != tmp) {
        handle->ms = tmp;
        updated_fields |= V4L2_RDS_MS;
    }
    handle->valid_fields |= V4L2_RDS_MS;

    uint8_t segment = grp->data_b_lsb & 0x03;

    rds_add_ps(priv_state, segment * 2, grp->data_d_msb);
    new_ps = rds_add_ps(priv_state, segment * 2 + 1, grp->data_d_lsb);
    if (new_ps) {
        if (memcmp(priv_state->new_ps, handle->ps, 8) != 0) {
            memcpy(handle->ps, priv_state->new_ps, 8);
            updated_fields |= V4L2_RDS_PS;
        }

        handle->valid_fields |= V4L2_RDS_PS;
    }

    bool bit2 = grp->data_b_lsb & 0x04;
    if (segment == 0 || segment == priv_state->next_di_segment) {
        switch (segment) {
            case 0:
                priv_state->new_di = set_bit(priv_state->new_di, V4L2_RDS_FLAG_DYNAMIC_PTY, bit2);
                priv_state->next_di_segment = 1;
                break;

            case 1:
                priv_state->new_di = set_bit(priv_state->new_di, V4L2_RDS_FLAG_COMPRESSED, bit2);
                priv_state->next_di_segment = 2;
                break;

            case 2:
                priv_state->new_di = set_bit(priv_state->new_di, V4L2_RDS_FLAG_ARTIFICIAL_HEAD, bit2);
                priv_state->next_di_segment = 3;
                break;

            case 3:
                priv_state->new_di = set_bit(priv_state->new_di, V4L2_RDS_FLAG_STEREO, bit2);
                if (handle->di != priv_state->new_di) {
                    handle->di = priv_state->new_di;
                    updated_fields |= V4L2_RDS_DI;
                }
                priv_state->next_di_segment = 0;
                handle->valid_fields |= V4L2_RDS_DI;
                break;
        }
    } else {
        priv_state->next_di_segment = 0;
        priv_state->new_di = 0;
    }

    if (grp->group_version == 'A') {
        if (rds_add_af(priv_state)) {
            updated_fields |= V4L2_RDS_AF;
        }
    }

    return updated_fields;
}

static uint32_t rds_decode_group1(struct rds_private_state *priv_state)
{
    uint8_t variant_code = 0;
    uint32_t updated_fields = 0;
    struct v4l2_rds *handle = &priv_state->handle;
    struct v4l2_rds_group *grp = &priv_state->rds_group;

    if (grp->group_version != 'A') {
        return 0;
    }

    variant_code = (grp->data_c_msb >> 4) & 0x07;
    if (variant_code == 0) {
        if (grp->data_c_lsb == priv_state->new_ecc) {
            handle->valid_fields |= V4L2_RDS_ECC;
            if (handle->ecc != grp->data_c_lsb) {
                updated_fields |= V4L2_RDS_ECC;
            }
            handle->ecc = grp->data_c_lsb;
        } else {
            priv_state->new_ecc = grp->data_c_lsb;
        }
    } else if (variant_code == 0x03) {
        if (grp->data_c_lsb == priv_state->new_lc) {
            handle->valid_fields |= V4L2_RDS_LC;
            updated_fields |= V4L2_RDS_LC;
            handle->lc = grp->data_c_lsb;
        } else {
            priv_state->new_lc = grp->data_c_lsb;
        }
    }

    return updated_fields;
}

static uint32_t rds_decode_group2(struct rds_private_state *priv_state)
{
    uint32_t updated_fields = 0;
    struct v4l2_rds *handle = &priv_state->handle;
    struct v4l2_rds_group *grp = &priv_state->rds_group;

    uint8_t segment = grp->data_b_lsb & 0x0f;
    bool rt_ab_flag_n = grp->data_b_lsb & 0x10;

    if (rt_ab_flag_n != handle->rt_ab_flag) {
        handle->rt_ab_flag = rt_ab_flag_n;
        memset(handle->rt, 0, 64);
        handle->valid_fields &= ~V4L2_RDS_RT;
        updated_fields |= V4L2_RDS_RT;
        priv_state->next_rt_segment = 0;
    }

    if (grp->group_version == 'A') {
        if ((segment == 0) || (segment == priv_state->next_rt_segment)) {
            priv_state->new_rt[segment * 4 + 0] = grp->data_c_msb;
            priv_state->new_rt[segment * 4 + 1] = grp->data_c_lsb;
            priv_state->new_rt[segment * 4 + 2] = grp->data_d_msb;
            priv_state->new_rt[segment * 4 + 3] = grp->data_d_lsb;
            priv_state->next_rt_segment = segment + 1;

            if (segment == 0x0f) {
                handle->rt_length = 64;
                handle->valid_fields |= V4L2_RDS_RT;
                if (memcmp(handle->rt, priv_state->new_rt, 64)) {
                    memcpy(handle->rt, priv_state->new_rt, 64);
                    updated_fields |= V4L2_RDS_RT;
                }

                priv_state->next_rt_segment = 0;
            }
        }
    } else {
        if ((segment == 0) || (segment == priv_state->next_rt_segment)) {
            priv_state->new_rt[segment * 2 + 0] = grp->data_d_msb;
            priv_state->new_rt[segment * 2 + 1] = grp->data_d_lsb;

            priv_state->next_rt_segment = segment + 1;
            if (segment == 0x0f) {
                handle->rt_length = 32;
                handle->valid_fields |= V4L2_RDS_RT;
                updated_fields |= V4L2_RDS_RT;
                if (memcmp(handle->rt, priv_state->new_rt, 32)) {
                    memcpy(handle->rt, priv_state->new_rt, 32);
                    updated_fields |= V4L2_RDS_RT;
                }

                priv_state->next_rt_segment = 0;
            }
        }
    }

    for (int i = 0; i < 64; i++) {
        if (priv_state->new_rt[i] == 0x0d) {
            priv_state->new_rt[i] = '\0';
            handle->rt_length = i;
            handle->valid_fields |= V4L2_RDS_RT;
            if (memcmp(handle->rt, priv_state->new_rt, handle->rt_length)) {
                memcpy(handle->rt, priv_state->new_rt, handle->rt_length);
                updated_fields |= V4L2_RDS_RT;
            }

            priv_state->next_rt_segment = 0;
        }
    }

    return updated_fields;
}

static uint32_t rds_decode_group3(struct rds_private_state *priv_state)
{
    struct v4l2_rds_oda new_oda;
    uint32_t updated_fields = 0;
    struct v4l2_rds *handle = &priv_state->handle;
    struct v4l2_rds_group *grp = &priv_state->rds_group;

    if (grp->group_version != 'A') {
        return 0;
    }

    new_oda.group_version = (grp->data_b_lsb & 0x01) ? 'B' : 'A';
    new_oda.group_id = (grp->data_b_lsb & 0x1e) >> 1;
    new_oda.aid = (grp->data_d_msb << 8) | grp->data_d_lsb;

    if (rds_add_oda(priv_state, new_oda)) {
        handle->decode_information |= V4L2_RDS_ODA;
        updated_fields |= V4L2_RDS_ODA;
    }

    if ((new_oda.aid == 0xcd46) || (new_oda.aid == 0xcd47)) {
        rds_decode_tmc_system(priv_state);
    }

    return updated_fields;
}

static time_t rds_decode_mjd(const struct rds_private_state *priv_state)
{
    struct tm new_time;
    int y, m, d, k = 0;
    uint32_t offset = priv_state->utc_offset & 0x1f;
    uint32_t local_mjd = priv_state->new_mjd;
    uint8_t local_hour = priv_state->utc_hour;
    uint8_t local_minute = priv_state->utc_minute;

    if (priv_state->utc_offset & 0x20) {
        local_hour -= offset / 2;
        local_minute -= (offset % 2) * 30;
    } else {
        local_hour += offset / 2;
        local_minute += (offset % 2) * 30;
    }

    y = (int)((local_mjd - 15078.2) / 365.25);
    m = (int)((local_mjd - 14956.1 - (int)(y * 365.25)) / 30.6001);
    d = (int)(local_mjd - 14956 - (int)(y * 365.25) - (int)(m * 30.6001));
    if ((m == 14) || (m == 15)) {
        k = 1;
    }

    y = y + k;
    m = m - 1 - k * 12;

    new_time.tm_sec = 0;
    new_time.tm_min = local_minute;
    new_time.tm_hour = local_hour;
    new_time.tm_mday = d;
    new_time.tm_mon = m - 1;
    new_time.tm_year = y;

    if (priv_state->utc_offset & 0x20) {
        new_time.tm_gmtoff = -offset * 1800;
    } else {
        new_time.tm_gmtoff = offset * 1800;
    }

    return mktime(&new_time);
}

static uint32_t rds_decode_group4(struct rds_private_state *priv_state)
{
    uint32_t mjd;
    uint32_t updated_fields = 0;
    struct v4l2_rds *handle = &priv_state->handle;
    struct v4l2_rds_group *grp = &priv_state->rds_group;

    if (grp->group_version != 'A') {
        return 0;
    }

    mjd = ((grp->data_b_lsb & 0x03) << 15) | (grp->data_c_msb << 7) | (grp->data_c_lsb >> 1);
    if (priv_state->new_mjd != mjd) {
        priv_state->new_mjd = mjd;
        return 0;
    }

    priv_state->utc_hour = ((grp->data_c_lsb & 0x01) << 4) | (grp->data_d_msb >> 4);
    priv_state->utc_minute = ((grp->data_d_msb & 0x0f) << 2) | (grp->data_d_lsb >> 6);

    priv_state->utc_offset = grp->data_d_lsb & 0x3f;

    handle->time = rds_decode_mjd(priv_state);
    updated_fields |= V4L2_RDS_TIME;
    handle->valid_fields |= V4L2_RDS_TIME;

    return updated_fields;
}

static uint32_t rds_decode_group8(struct rds_private_state *priv_state)
{
    uint8_t tuning_variant;
    struct v4l2_rds_group *grp = &priv_state->rds_group;

    if (grp->group_version != 'A') {
        return 0;
    }

    if (!rds_compare_group(&priv_state->prev_tmc_group, &priv_state->rds_group)) {
        priv_state->prev_tmc_group = priv_state->rds_group;
        return 0;
    }

    priv_state->prev_tmc_group.group_version = 0;

    if ((grp->data_b_lsb & V4L2_TMC_SINGLE_GROUP) && !(grp->data_b_lsb & V4L2_TMC_TUNING_INFO)) {
        return rds_decode_tmc_single_group(priv_state);
    }

    if (!(grp->data_b_lsb & V4L2_TMC_SINGLE_GROUP) && !(grp->data_b_lsb & V4L2_TMC_TUNING_INFO)) {
        return rds_decode_tmc_multi_group(priv_state);
    }

    tuning_variant = grp->data_b_lsb & 0x0f;
    if ((grp->data_b_lsb & V4L2_TMC_TUNING_INFO) && (tuning_variant >= 4) && (tuning_variant <= 9)) {
        priv_state->handle.valid_fields |= V4L2_RDS_TMC_TUNING;
        return rds_decode_tmc_tuning(priv_state);
    }

    return 0;
}

static uint32_t rds_decode_group10(struct rds_private_state *priv_state)
{
    uint8_t ptyn_tmp[4];
    uint32_t updated_fields = 0;
    struct v4l2_rds *handle = &priv_state->handle;
    struct v4l2_rds_group *grp = &priv_state->rds_group;
    uint8_t segment_code = grp->data_b_lsb & 0x01;
    bool ptyn_ab_flag_n = grp->data_b_lsb & 0x10;

    if (grp->group_version != 'A') {
        return 0;
    }

    if (ptyn_ab_flag_n != handle->ptyn_ab_flag) {
        handle->ptyn_ab_flag = ptyn_ab_flag_n;
        memset(handle->ptyn, 0, 8 * sizeof(char));
        memset(priv_state->new_ptyn, 0, 8 * sizeof(char));
        memset(priv_state->new_ptyn_valid, 0, 2 * sizeof(bool));
        handle->valid_fields &= ~V4L2_RDS_PTYN;
        updated_fields |= V4L2_RDS_PTYN;
    }

    ptyn_tmp[0] = grp->data_c_msb;
    ptyn_tmp[1] = grp->data_c_lsb;
    ptyn_tmp[2] = grp->data_d_msb;
    ptyn_tmp[3] = grp->data_d_lsb;

    if (memcmp(ptyn_tmp, priv_state->new_ptyn[segment_code], 4) == 0) {
        priv_state->new_ptyn_valid[segment_code] = true;
    } else {
        for (int i = 0; i < 4; i++) {
            priv_state->new_ptyn[segment_code][i] = ptyn_tmp[i];
        }

        priv_state->new_ptyn_valid[segment_code] = false;
    }

    if (priv_state->new_ptyn_valid[0] && priv_state->new_ptyn_valid[1]) {
        for (int i = 0; i < 4; i++) {
            handle->ptyn[i] = priv_state->new_ptyn[0][i];
            handle->ptyn[4 + i] = priv_state->new_ptyn[1][i];
        }

        handle->valid_fields |= V4L2_RDS_PTYN;
        updated_fields |= V4L2_RDS_PTYN;
    }

    return updated_fields;
}

static uint32_t rds_decode_group14(struct rds_private_state *priv_state)
{
    uint16_t pi_on;
    uint16_t lsf_on;
    uint8_t pty_on;
    uint8_t eon_index;
    bool tp_on, ta_on;
    uint8_t variant_code;
    uint32_t updated_fields = 0;
    struct v4l2_rds_eon *eon_entry;
    bool new_a = false, new_b = false;
    struct v4l2_rds *handle = &priv_state->handle;
    struct v4l2_rds_group *grp = &priv_state->rds_group;

    if (grp->group_version != 'A') {
        return 0;
    }

    variant_code = grp->data_b_lsb & 0x0f;
    pi_on = (grp->data_d_msb << 8) | grp->data_d_lsb;
    tp_on = grp->data_b_lsb & 0x10;

    if (rds_check_eon_entry(priv_state, pi_on)) {
        eon_index = rds_add_eon_entry(priv_state, pi_on);
        eon_entry = &handle->rds_eon.eon[eon_index];
        eon_entry->tp = tp_on;
        eon_entry->valid_fields |= V4L2_RDS_TP;
        updated_fields |= V4L2_RDS_EON;
    }

    if (((variant_code >=5) && (variant_code <= 11)) || (variant_code >= 14)) {
        return updated_fields;
    }

    eon_index = rds_add_eon_entry(priv_state, pi_on);
    eon_entry = &handle->rds_eon.eon[eon_index];

    if (variant_code < 4) {
        eon_entry->ps[variant_code * 2] = grp->data_c_msb;
        eon_entry->ps[variant_code * 2 + 1] = grp->data_c_lsb;
        eon_entry->valid_fields |= V4L2_RDS_PS;
        updated_fields |= V4L2_RDS_EON;
    } else if (variant_code == 4) {
        uint8_t c_msb = grp->data_c_msb;
        uint8_t c_lsb = grp->data_c_lsb;

        if ((c_msb >= 224) && (c_msb <= 249)) {
            eon_entry->af.announced_af = c_msb - 224;
        }

        if (c_msb < 205) {
            new_a = rds_add_af_to_list(&eon_entry->af, grp->data_c_msb, true);
        }

        if (c_lsb < 205) {
            new_b = rds_add_af_to_list(&eon_entry->af, grp->data_c_lsb, true);
        }

        if (new_a || new_b) {
            eon_entry->valid_fields |= V4L2_RDS_AF;
            updated_fields |= V4L2_RDS_EON;
        }
    } else if (variant_code == 12) {
        lsf_on = (grp->data_c_msb << 8) | grp->data_c_lsb;
        new_a = (eon_entry->lsf == lsf_on);
        if (new_a) {
            eon_entry->lsf = lsf_on;
            eon_entry->valid_fields |= V4L2_RDS_LSF;
            updated_fields |= V4L2_RDS_EON;
        }
    } else if (variant_code == 13) {
        pty_on = grp->data_c_msb >> 3;
        ta_on = grp->data_c_lsb & 0x01;
        new_a = (eon_entry->pty == pty_on);
        if (new_a) {
            eon_entry->pty = pty_on;
            eon_entry->valid_fields |= V4L2_RDS_PTY;
        }

        new_b = (eon_entry->ta == ta_on);
        eon_entry->ta = ta_on;
        eon_entry->valid_fields |= V4L2_RDS_TA;
        if (new_a || new_b) {
            updated_fields |= V4L2_RDS_EON;
        }
    }

    if (handle->rds_eon.size > 0) {
        handle->valid_fields |= V4L2_RDS_EON;
    }

    return updated_fields;
}

typedef uint32_t (*decode_group_func)(struct rds_private_state *);

static const decode_group_func decode_group[16] = {
    [0]  = rds_decode_group0,
    [1]  = rds_decode_group1,
    [2]  = rds_decode_group2,
    [3]  = rds_decode_group3,
    [4]  = rds_decode_group4,
    [8]  = rds_decode_group8,
    [10] = rds_decode_group10,
    [14] = rds_decode_group14
};

static uint32_t rds_decode_group(struct rds_private_state *priv_state)
{
    struct v4l2_rds *handle = &priv_state->handle;
    uint8_t group_id = priv_state->rds_group.group_id;

    handle->rds_statistics.group_type_cnt[group_id]++;
    if (decode_group[group_id]) {
        return (*decode_group[group_id])(priv_state);
    }

    return 0;
}

struct v4l2_rds *v4l2_rds_create(bool is_rbds)
{
    struct rds_private_state *internal_handle = calloc(1, sizeof(struct rds_private_state));
    internal_handle->handle.is_rbds = is_rbds;

    return (struct v4l2_rds *)internal_handle;
}

void v4l2_rds_destroy(struct v4l2_rds *handle)
{
    if (handle) {
        free(handle);
    }
}

void v4l2_rds_reset(struct v4l2_rds *handle, bool reset_statistics)
{
    struct rds_private_state *priv_state = (struct rds_private_state *)handle;
    bool is_rbds = handle->is_rbds;
    struct v4l2_rds_statistics rds_statistics = handle->rds_statistics;

    memset(priv_state, 0, sizeof(*priv_state));
    handle->is_rbds = is_rbds;
    if (!reset_statistics) {
        handle->rds_statistics = rds_statistics;
    }
}

uint32_t v4l2_rds_add(struct v4l2_rds *handle, struct v4l2_rds_data *rds_data)
{
    uint32_t updated_fields = 0;
    struct rds_private_state *priv_state = (struct rds_private_state *) handle;
    struct v4l2_rds_data *rds_data_raw = priv_state->rds_data_raw;
    struct v4l2_rds_statistics *rds_stats = &handle->rds_statistics;
    uint8_t *decode_state = &(priv_state->decode_state);
    int block_id = rds_data->block & V4L2_RDS_BLOCK_MSK;

    rds_stats->block_cnt++;
    if ((rds_data->block & V4L2_RDS_BLOCK_ERROR) || (block_id == V4L2_RDS_BLOCK_INVALID)) {
        block_id = -1;
        rds_stats->block_error_cnt++;
    } else if (rds_data->block & V4L2_RDS_BLOCK_CORRECTED) {
        rds_stats->block_corrected_cnt++;
    }

    switch (*decode_state) {
        case RDS_EMPTY:
            if (block_id == 0) {
                *decode_state = RDS_A_RECEIVED;
                memset(rds_data_raw, 0, sizeof(*rds_data_raw));
                rds_data_raw[0] = *rds_data;
            } else {
                rds_stats->group_error_cnt++;
            }
            break;

        case RDS_A_RECEIVED:
            if (block_id == 1) {
                *decode_state = RDS_B_RECEIVED;
                rds_data_raw[1] = *rds_data;
            } else {
                rds_stats->group_error_cnt++;
                *decode_state = RDS_EMPTY;
            }
            break;

        case RDS_B_RECEIVED:
            if ((block_id == 2) || (block_id == 4)) {
                *decode_state = RDS_C_RECEIVED;
                rds_data_raw[2] = *rds_data;
            } else {
                rds_stats->group_error_cnt++;
                *decode_state = RDS_EMPTY;
            }
            break;

        case RDS_C_RECEIVED:
            if (block_id == 3) {
                *decode_state = RDS_EMPTY;
                rds_data_raw[3] = *rds_data;
                rds_stats->group_cnt++;

                memset(&priv_state->rds_group, 0, sizeof(priv_state->rds_group));
                updated_fields |= rds_decode_a(priv_state, &rds_data_raw[0]);
                updated_fields |= rds_decode_b(priv_state, &rds_data_raw[1]);
                rds_decode_c(priv_state, &rds_data_raw[2]);
                rds_decode_d(priv_state, &rds_data_raw[3]);
                updated_fields |= rds_decode_group(priv_state);
                return updated_fields;
            }

            rds_stats->group_error_cnt++;
            *decode_state = RDS_EMPTY;
            break;

        default:
            rds_stats->group_error_cnt++;
            *decode_state = RDS_EMPTY;
    }

    return 0;
}

const char *v4l2_rds_get_pty_str(const struct v4l2_rds *handle)
{
    const uint8_t pty = handle->pty;

    if (pty >= 32) {
        return NULL;
    }

    static const char *rds_lut[32] = {
        "None", "News", "Affairs", "Info", "Sport", "Education", "Drama",
        "Culture", "Science", "Varied Speech", "Pop Music",
        "Rock Music", "Easy Listening", "Light Classics M",
        "Serious Classics", "Other Music", "Weather", "Finance",
        "Children", "Social Affairs", "Religion", "Phone In",
        "Travel & Touring", "Leisure & Hobby", "Jazz Music",
        "Country Music", "National Music", "Oldies Music", "Folk Music",
        "Documentary", "Alarm Test", "Alarm!"
    };

    static const char *rbds_lut[32] = {
        "None", "News", "Information", "Sports", "Talk", "Rock",
        "Classic Rock", "Adult Hits", "Soft Rock", "Top 40", "Country",
        "Oldies", "Soft", "Nostalgia", "Jazz", "Classical",
        "R&B", "Soft R&B", "Foreign Language", "Religious Music",
        "Religious Talk", "Personality", "Public", "College",
        "Spanish Talk", "Spanish Music", "Hip-Hop", "Unassigned",
        "Unassigned", "Weather", "Emergency Test", "Emergency"
    };

    return handle->is_rbds ? rbds_lut[pty] : rds_lut[pty];
}

const char *v4l2_rds_get_country_str(const struct v4l2_rds *handle)
{
    uint8_t ecc_h = handle->ecc >> 4;
    uint8_t ecc_l = handle->ecc & 0x0f;
    uint8_t country_code = handle->pi >> 12;

    static const char *e_lut[5][16] = {
        {
            NULL, "DE", "DZ", "AD", "IL", "IT", "BE", "RU", "PS", "AL",
            "AT", "HU", "MT", "DE", NULL, "EG"
        }, {
            NULL, "GR", "CY", "SM", "CH", "JO", "FI", "LU", "BG", "DK",
            "GI", "IQ", "GB", "LY", "RO", "FR"
        }, {
            NULL, "MA", "CZ", "PL", "VA", "SK", "SY", "TN", NULL, "LI",
            "IS", "MC", "LT", "RS", "ES", "NO"
        }, {
            NULL, "ME", "IE", "TR", "MK", NULL, NULL, NULL, "NL", "LV",
            "LB", "AZ", "HR", "KZ", "SE", "BY"
        }, {
            NULL, "MD", "EE", "KG", NULL, NULL, "UA", "-", "PT", "SI",
            "AM", NULL, "GE", NULL, NULL, "BA"
        }
    };

    if ((ecc_h == 0x0e) && (ecc_l <= 0x04)) {
        return e_lut[ecc_l][country_code];
    }

    return "Unknown";
}

static const char *rds_language_lut(const uint8_t lc)
{
    const char *language;
    const uint8_t max_lc = 127;

    static const char *language_lut[128] = {
        "Unknown", "Albanian", "Breton", "Catalan",
        "Croatian", "Welsh", "Czech", "Danish",
        "German", "English", "Spanish", "Esperanto",
        "Estonian", "Basque", "Faroese", "French",
        "Frisian", "Irish", "Gaelic", "Galician",
        "Icelandic", "Italian", "Lappish", "Latin",
        "Latvian", "Luxembourgian", "Lithuanian", "Hungarian",
        "Maltese", "Dutch", "Norwegian", "Occitan",
        "Polish", "Portuguese", "Romanian", "Ramansh",
        "Serbian", "Slovak", "Slovene", "Finnish",
        "Swedish", "Turkish", "Flemish", "Walloon",
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, "Zulu", "Vietnamese", "Uzbek",
        "Urdu", "Ukrainian", "Thai", "Telugu",
        "Tatar", "Tamil", "Tadzhik", "Swahili",
        "Sranan Tongo", "Somali", "Sinhalese", "Shona",
        "Serbo-Croat", "Ruthenian", "Russian", "Quechua",
        "Pushtu", "Punjabi", "Persian", "Papamiento",
        "Oriya", "Nepali", "Ndebele", "Marathi",
        "Moldavian", "Malaysian", "Malagasay", "Macedonian",
        "Laotian", "Korean", "Khmer", "Kazahkh",
        "Kannada", "Japanese", "Indonesian", "Hindi",
        "Hebrew", "Hausa", "Gurani", "Gujurati",
        "Greek", "Georgian", "Fulani", "Dani",
        "Churash", "Chinese", "Burmese", "Bulgarian",
        "Bengali", "Belorussian", "Bambora", "Azerbaijani",
        "Assamese", "Armenian", "Arabic", "Amharic"
    };

    language = (lc > max_lc) ? "Unknown" : language_lut[lc];
    if (!language) {
        return "Unknown";
    }

    return language;
}

const char *v4l2_rds_get_language_str(const struct v4l2_rds *handle)
{
    return rds_language_lut(handle->lc);
}

const char *v4l2_rds_get_coverage_str(const struct v4l2_rds *handle)
{
    uint8_t coverage = (handle->pi >> 8) & 0x0f;
    uint8_t nibble = (handle->pi >> 12) & 0x0f;
    static const char *coverage_lut[16] = {
        "Local", "International", "National", "Supra-Regional",
        "Regional 1", "Regional 2", "Regional 3", "Regional 4",
        "Regional 5", "Regional 6", "Regional 7", "Regional 8",
        "Regional 9", "Regional 10", "Regional 11", "Regional 12"
    };

    if (!handle->is_rbds || ((nibble == 0xb) || (nibble == 0xd) || (nibble == 0xe))) {
        return coverage_lut[coverage];
    }

    return "Not Available";
}

const struct v4l2_rds_group *v4l2_rds_get_group(const struct v4l2_rds *handle)
{
    struct rds_private_state *priv_state = (struct rds_private_state *)handle;
    return &priv_state->rds_group;
}
