#ifndef __SPI_H
#define __SPI_H

void SPI1_Init(void);

void W25Q64_WaitBusy();
void W25Q64_Sector_Erase(uint32_t Adress);
void W25Q64_Page_Program(uint32_t Adress, uint8_t *Array, uint16_t Count);
void W25Q64_Read_Data(uint32_t Adress, uint8_t *Array, uint16_t Count);

#endif
