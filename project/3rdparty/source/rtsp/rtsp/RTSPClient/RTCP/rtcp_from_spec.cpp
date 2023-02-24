#include <rtsp/internal/rtcp_from_spec.h>

namespace rtsp {
double rtcp_interval(int members, int senders, double rtcp_bw, int we_sent, double avg_rtcp_size, int initial)
{
    double const RTCP_MIN_TIME = 5.;
    double const RTCP_SENDER_BW_FRACTION = 0.25;
    double const RTCP_RCVR_BW_FRACTION = (1 - RTCP_SENDER_BW_FRACTION);

    double const COMPENSATION = 2.71828 - 1.5;

    int n;
    double t;
    double rtcp_min_time = RTCP_MIN_TIME;

    if (initial) {
        rtcp_min_time /= 2;
    }

    n = members;
    if ((senders > 0) && (senders < (members * RTCP_SENDER_BW_FRACTION))) {
        if (we_sent) {
            rtcp_bw *= RTCP_SENDER_BW_FRACTION;
            n = senders;
        } else {
            rtcp_bw *= RTCP_RCVR_BW_FRACTION;
            n -= senders;
        }
    }

    t = avg_rtcp_size * n / rtcp_bw;
    if (t < rtcp_min_time) {
        t = rtcp_min_time;
    }

    t = t * (drand48() + 0.5);
    t = t / COMPENSATION;

    return t;
}

void OnExpire(event e, int members, int senders, double rtcp_bw, int we_sent, double *avg_rtcp_size, int *initial, time_tp tc, time_tp *tp, int *pmembers)
{
    double t;
    double tn;

    if (TypeOfEvent(e) == EVENT_BYE) {
        t = rtcp_interval(members, senders, rtcp_bw, we_sent, *avg_rtcp_size, *initial);
        tn = *tp + t;
        if (tn <= tc) {
            SendBYEPacket(e);
            exit(1);
        } else {
            Schedule(tn, e);
        }
    } else if (TypeOfEvent(e) == EVENT_REPORT) {
        t = rtcp_interval(members, senders, rtcp_bw, we_sent, *avg_rtcp_size, *initial);
        tn = *tp + t;

        if (tn <= tc) {
            SendRTCPReport(e);
            *avg_rtcp_size = (1./16.)*SentPacketSize(e) + (15./16.) * (*avg_rtcp_size);
            *tp = tc;

            t = rtcp_interval(members, senders, rtcp_bw, we_sent, *avg_rtcp_size, *initial);
            Schedule(t+tc,e);
            *initial = 0;
        } else {
            Schedule(tn, e);
        }

        *pmembers = members;
    }
}

void OnReceive(packet p, event e, int *members, int *pmembers, int *senders, double *avg_rtcp_size, double *tp, double tc, double tn)
{
    if (PacketType(p) == PACKET_RTCP_REPORT) {
        if (NewMember(p) && (TypeOfEvent(e) == EVENT_REPORT)) {
            AddMember(p);
            *members += 1;
        }

        *avg_rtcp_size = (1./16.)*ReceivedPacketSize(p) + (15./16.)*(*avg_rtcp_size);
    } else if (PacketType(p) == PACKET_RTP) {
        if (NewMember(p) && (TypeOfEvent(e) == EVENT_REPORT)) {
            AddMember(p);
            *members += 1;
        }

        if (NewSender(p) && (TypeOfEvent(e) == EVENT_REPORT)) {
            AddSender(p);
            *senders += 1;
        }
    } else if (PacketType(p) == PACKET_BYE) {
        *avg_rtcp_size = (1./16.)*ReceivedPacketSize(p) + (15./16.)*(*avg_rtcp_size);

        if (TypeOfEvent(e) == EVENT_REPORT) {
            if (NewSender(p) == FALSE) {
                RemoveSender(p);
                *senders -= 1;
            }

            if (NewMember(p) == FALSE) {
                RemoveMember(p);
                *members -= 1;
            }

            if (*members < *pmembers) {
                tn = tc + (((double) *members)/(*pmembers))*(tn - tc);
                *tp = tc - (((double) *members)/(*pmembers))*(tc - *tp);

                Reschedule(tn, e);
                *pmembers = *members;
            }
        } else if (TypeOfEvent(e) == EVENT_BYE) {
            *members += 1;
        }
    }
}
}
