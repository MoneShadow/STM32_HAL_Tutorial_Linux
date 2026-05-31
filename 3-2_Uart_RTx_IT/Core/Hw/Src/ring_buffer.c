#include "stm32f1xx_hal.h"
#include "ring_buffer.h"

/* 初始化缓冲区 */
void Rbuffer_Init(ring_buffer *rb, uint8_t *mem, uint32_t len) {
    rb->buffer = mem;
    rb->head = 0;
    rb->tail = 0;
    rb->size = len;
}

/* 写一个字节 */
uint8_t R_WriteByte(ring_buffer *rb, uint8_t Byte) {
    /* 先检查缓冲是否已满 */
    uint32_t next = NEXT_IDX(rb->head, rb);
    if (next == rb->tail) {
        return 1; // 缓冲区满
    }
    /* 有空闲的话就写入字节 */
    rb->buffer[rb->head] = Byte;
    rb->head = NEXT_IDX(rb->head, rb);
    return 0; // 写入成功
}

/* 读取一个字节 */
uint8_t R_ReadByte(ring_buffer *rb) {
    /* 先检查是否有数据 */
    if (rb->head == rb->tail) {
        return 1; // 缓冲区中无数据
    }
    /* 有数据的话就读取一个字节 */
    uint8_t Byte = rb->buffer[rb->tail];
    rb->tail = NEXT_IDX(rb->tail, rb);
    return Byte; // 读取成功
}

/* 检查当前数据量 */
uint32_t R_Avilable(ring_buffer *rb) {
    if (rb->head == rb->tail) {
        return 0;
    }
    return (((rb->head - rb->tail) + rb->size) % rb->size);
}

/* 检查空闲空间 */
uint32_t R_Free(ring_buffer *rb) {
    return rb->size - R_Avilable(rb) - 1;
}