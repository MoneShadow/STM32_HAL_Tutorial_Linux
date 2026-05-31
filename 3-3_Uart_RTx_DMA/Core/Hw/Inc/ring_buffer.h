#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

#include <stdint.h>
/* 下一个索引 */
#define NEXT_IDX(idx, rb)   ((idx + 1) % (rb->size))

/* 定义一个结构体存储 读索引 写索引 缓冲区长度 缓存区 */
typedef struct {
    __IO uint32_t head;
    __IO uint32_t tail;
    uint32_t size;
    uint8_t *buffer;
} ring_buffer;

void Rbuffer_Init(ring_buffer *rb, uint8_t *mem, uint32_t len);
uint8_t R_WriteByte(ring_buffer *rb, uint8_t Byte);
uint8_t R_ReadByte(ring_buffer *rb, uint8_t *Byte);
uint32_t R_Avilable(ring_buffer *rb);
uint32_t R_Free(ring_buffer *rb);

#endif
