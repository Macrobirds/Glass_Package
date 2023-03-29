#ifndef __RINGING_BUF_H
#define __RINGING_BUF_H

struct ringbuf{
    int read_pos;
    int write_pos;
    int * ring_buf;
    int len;
};

int ringbuf_init(struct ringbuf *,int);
int is_ringbuf_empty(struct ringbuf *);
int is_ringbuf_full(struct ringbuf *);
void put_ringbuf(struct ringbuf *,int);
int get_ringbuf(struct ringbuf *);
int ringbuf_release(struct ringbuf *);


#endif