#ifndef RTSP_INTERNAL_RTCP_FROM_SPEC_H
#define RTSP_INTERNAL_RTCP_FROM_SPEC_H

#include <stdlib.h>

namespace rtsp {
typedef void* event;

#define EVENT_UNKNOWN           0
#define EVENT_REPORT            1
#define EVENT_BYE               2

typedef double time_tp;

typedef void* packet;

#define PACKET_UNKNOWN_TYPE     0
#define PACKET_RTP              1
#define PACKET_RTCP_REPORT      2
#define PACKET_BYE              3

#define drand48                 drand30

#define exit(n)                 do {} while (0)

#ifndef FALSE
#define FALSE                   0
#endif

#ifndef TRUE
#define TRUE                    1
#endif

void OnExpire(event, int, int, double, int, double *, int *, time_tp, time_tp *, int *);
void OnReceive(packet, event, int *, int *, int *, double *, double *, double, double);

void Schedule(double, event);
void Reschedule(double, event);
void SendRTCPReport(event);
void SendBYEPacket(event);
int TypeOfEvent(event);
int SentPacketSize(event);
int PacketType(packet);
int ReceivedPacketSize(packet);
int NewMember(packet);
int NewSender(packet);
void AddMember(packet);
void AddSender(packet);
void RemoveMember(packet);
void RemoveSender(packet);
double drand30(void);
}

#endif
