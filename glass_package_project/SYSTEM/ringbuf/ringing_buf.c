#include "ringing_buf.h"
#include <linux/slab.h>

// struct ringbuf{
//     int read_pos;
//     int write_pos;
//     int ring_buf *;
//     int size;
// };

#define NEXT_POS(x,len) ((x+1)%len)


int ringbuf_init(struct ringbuf * buf,int size)
{
    buf->read_pos=0;
    buf->write_pos=0;
    buf->len=size;
    buf->ring_buf=kzalloc(sizeof(int)*size,GFP_KERNEL);
    if(!buf->ring_buf)
    {
        return -1;
    }
    return 0;
}


int is_ringbuf_empty(struct ringbuf * buf)
{
   if(buf->write_pos==buf->read_pos)
   {
    return 1;
   }else
   {
    return 0;
   }
   
   
}
int is_ringbuf_full(struct ringbuf * buf)
{
    if(buf->read_pos==NEXT_POS(buf->write_pos,buf->len))
    {
        return 1;
    }else
    {
        return 0;
    }
    
}
void put_ringbuf(struct ringbuf * buf,int value)
{
    if(!is_ringbuf_full(buf))
    {
        buf->ring_buf[buf->write_pos]=value;
        buf->write_pos=NEXT_POS(buf->write_pos,buf->len);
    }
}
int get_ringbuf(struct ringbuf * buf)
{
    int value=-1;
    if(!is_ringbuf_empty(buf))
    {
        value=buf->ring_buf[buf->read_pos];
        buf->read_pos=NEXT_POS(buf->read_pos,buf->len);
        return  value;
    }

    return value;
}

int ringbuf_release(struct ringbuf *buf)
{
    kfree(buf->ring_buf);
    return 0;
}