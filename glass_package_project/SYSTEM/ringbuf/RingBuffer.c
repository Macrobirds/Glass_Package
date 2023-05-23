/**
  ******************************************************************************
  * @file    RingBuffer.c
  * @author  XinLi
  * @version v1.2
  * @date    15-January-2018
  * @brief   Ring buffer module source file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>Copyright &copy; 2018 XinLi</center></h2>
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <https://www.gnu.org/licenses/>.
  *
  ******************************************************************************
  */

/* Header includes -----------------------------------------------------------*/
#include "Globalconfig.h"
#include <string.h>

/* Macro definitions ---------------------------------------------------------*/
#define min(a, b)  (((a) < (b)) ? (a) : (b))

/* Type definitions ----------------------------------------------------------*/
/* Variable declarations -----------------------------------------------------*/
/* Variable definitions ------------------------------------------------------*/
/* Function declarations -----------------------------------------------------*/
static bool is_power_of_2(uint8_t x);
static uint8_t roundup_pow_of_two(uint8_t x);

/* Function definitions ------------------------------------------------------*/

/**
  * @brief  Allocates a new FIFO and its internal buffer.
  * @param  [in] size: The size of the internal buffer to be allocated.
  * @note   The size will be rounded-up to a power of 2.
  * @return RingBuffer pointer.
  */
RingBuffer *RingBuffer_Malloc(uint8_t size)
{
  RingBuffer *fifo = RING_BUFFER_MALLOC(sizeof(RingBuffer));

  if(fifo != NULL)
  {
    if(is_power_of_2(size) != true)
    {
      size = roundup_pow_of_two(size);
    }

    fifo->buffer = RING_BUFFER_MALLOC(size);

    if(fifo->buffer == NULL)
    {
			printf("fifo buf malloc false\r\n");
      RING_BUFFER_FREE(fifo);
      return NULL;
    }

    fifo->size = size;
    fifo->in = fifo->out = 0;
		printf("fifo malloc success ,size is %d fifo in:%d fifo out: %d\r\n",fifo->size,fifo->in,fifo->out);
  }else
	{
		printf("fifo malloc false\r\n");
	}

  return fifo;
}

/**
  * @brief  Frees the FIFO.
  * @param  [in] fifo: The fifo to be freed.
  * @return None.
  */
void RingBuffer_Free(RingBuffer *fifo)
{
  RING_BUFFER_FREE(fifo->buffer);
  RING_BUFFER_FREE(fifo);
}

/**
  * @brief  Puts some data into the FIFO.
  * @param  [in] fifo: The fifo to be used.
  * @param  [in] in:   The data to be added.
  * @param  [in] len:  The length of the data to be added.
  * @return The number of bytes copied.
  * @note   This function copies at most @len bytes from the @in into
  *         the FIFO depending on the free space, and returns the number
  *         of bytes copied.
  */
uint8_t RingBuffer_In(RingBuffer *fifo, const void *in, uint8_t len)
{
	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)  
  len = min(len, RingBuffer_Avail(fifo));
  /* First put the data starting from fifo->in to buffer end. */
  uint8_t l = min(len, fifo->size - fifo->in);
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
	
	 memcpy(fifo->buffer + fifo->in , in, l);
	if(len>l)
	{
			/* Then put the rest (if any) at the beginning of the buffer. */
		memcpy(fifo->buffer, (uint8_t *)in + l, len - l);
	}

  fifo->in += len;
	fifo->in %= fifo->size;


  return len;
}

/**
  * @brief  Gets some data from the FIFO.
  * @param  [in] fifo: The fifo to be used.
  * @param  [in] out:  Where the data must be copied.
  * @param  [in] len:  The size of the destination buffer.
  * @return The number of copied bytes.
  * @note   This function copies at most @len bytes from the FIFO into
  *         the @out and returns the number of copied bytes.
  */
uint8_t RingBuffer_Out(RingBuffer *fifo, void *out, uint8_t len)
{

	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)  	
  len = min(len, RingBuffer_Len(fifo));
  /* First get the data from fifo->out until the end of the buffer. */
  uint8_t l = min(len, fifo->size - fifo->out);
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
	
  memcpy(out, fifo->buffer + fifo->out, l);
  /* Then get the rest (if any) from the beginning of the buffer. */
	if(len>l)
	{
		memcpy((uint8_t *)out + l, fifo->buffer, len - l);
	}
  

  fifo->out += len;
	fifo->out%=fifo->size;

  return len;
}

/**
  * @brief  Determine whether some value is a power of two.
  * @param  [in] x: The number to be confirmed.
  * @retval true:   Yes.
  * @retval false:  No.
  * @note   Where zero is not considered a power of two.
  */
static bool is_power_of_2(uint8_t x)
{
  return (x != 0) && ((x & (x - 1)) == 0);
}

/**
  * @brief  Round the given value up to nearest power of two.
  * @param  [in] x: The number to be converted.
  * @return The power of two.
  */
static uint8_t roundup_pow_of_two(uint8_t x)
{
  uint8_t b = 0;

  for(int i = 0; i < 32; i++)
  {
    b = 1UL << i;

    if(x <= b)
    {
      break;
    }
  }

  return b;
}
