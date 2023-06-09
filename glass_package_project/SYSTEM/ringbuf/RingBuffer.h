/**
 ******************************************************************************
 * @file    RingBuffer.h
 * @author  XinLi
 * @version v1.2
 * @date    15-January-2018
 * @brief   Header file for RingBuffer.c module.
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

#ifndef __RINGBUFFER_H
#define __RINGBUFFER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Header includes -----------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "malloc.h"

/* Macro definitions ---------------------------------------------------------*/
#define RING_BUFFER_MALLOC(size) mymalloc(SRAMIN, size)
#define RING_BUFFER_FREE(block) myfree(SRAMIN, block)

  /* Type definitions ----------------------------------------------------------*/
  typedef struct
  {
    uint8_t *buffer;
    uint8_t size;
    uint8_t in;
    uint8_t out;
  } RingBuffer;

  /* Variable declarations -----------------------------------------------------*/
  /* Variable definitions ------------------------------------------------------*/
  /* Function declarations -----------------------------------------------------*/
  RingBuffer *RingBuffer_Malloc(uint8_t size);
  void RingBuffer_Free(RingBuffer *fifo);

  uint8_t RingBuffer_In(RingBuffer *fifo, const void *in, uint8_t len);
  uint8_t RingBuffer_Out(RingBuffer *fifo, void *out, uint8_t len);

  /* Function definitions ------------------------------------------------------*/

  /**
   * @brief  Removes the entire FIFO contents.
   * @param  [in] fifo: The fifo to be emptied.
   * @return None.
   */
  static inline void RingBuffer_Reset(RingBuffer *fifo)
  {
    fifo->in = fifo->out = 0;
  }

  /**
   * @brief  Returns the size of the FIFO in bytes.
   * @param  [in] fifo: The fifo to be used.
   * @return The size of the FIFO.
   */
  static inline uint8_t RingBuffer_Size(RingBuffer *fifo)
  {
    return fifo->size;
  }

  /**
   * @brief  Returns the number of used bytes in the FIFO.
   * @param  [in] fifo: The fifo to be used.
   * @return The number of used bytes.
   */
  static inline uint8_t RingBuffer_Len(RingBuffer *fifo)
  {
    if (fifo->in >= fifo->out)
      return fifo->in - fifo->out;
    else
      return fifo->size - (fifo->out - fifo->in);
  }

  /**
   * @brief  Returns the number of bytes available in the FIFO.
   * @param  [in] fifo: The fifo to be used.
   * @return The number of bytes available.
   */
  static inline uint8_t RingBuffer_Avail(RingBuffer *fifo)
  {
    return RingBuffer_Size(fifo) - RingBuffer_Len(fifo);
  }

  /**
   * @brief  Is the FIFO empty?
   * @param  [in] fifo: The fifo to be used.
   * @retval true:      Yes.
   * @retval false:     No.
   */
  static inline bool RingBuffer_IsEmpty(RingBuffer *fifo)
  {
    return RingBuffer_Len(fifo) == 0;
  }

  /**
   * @brief  Is the FIFO full?
   * @param  [in] fifo: The fifo to be used.
   * @retval true:      Yes.
   * @retval false:     No.
   */
  static inline bool RingBuffer_IsFull(RingBuffer *fifo)
  {
    return RingBuffer_Avail(fifo) == 0;
  }

#ifdef __cplusplus
}
#endif

#endif /* __RINGBUFFER_H */
