#ifndef __KEY_H
#define __KEY_H

void Key_Init(void);
uint8_t Key_ReadStatus(uint8_t KeyNum);
uint8_t Key_Scan(void);

#endif