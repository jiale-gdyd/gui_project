#ifndef RTSP_SIMPLE_RTSP_SERVER_STREAM_QUEUE_H
#define RTSP_SIMPLE_RTSP_SERVER_STREAM_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

struct stream_queue {
    int  pktsiz;
    int  nbpkts;
    int  head;
    int  tail;
    int  *pktlen;
    char *buf;
};

void streamq_free(struct stream_queue *q);
struct stream_queue *streamq_alloc(int pktsiz, int nbpkts);


int streamq_inused(struct stream_queue *q, int index);
int streamq_query(struct stream_queue *q, int index, char **ppacket, int **ppktlen);

int streamq_head(struct stream_queue *q);
int streamq_tail(struct stream_queue *q);
int streamq_next(struct stream_queue *q, int index);

int streamq_pop(struct stream_queue *q);
int streamq_push(struct stream_queue *q);

#ifdef __cplusplus
}
#endif

#endif
