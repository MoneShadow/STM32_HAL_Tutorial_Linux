#ifndef __SPI_SF_H
#define  __SPI_SF_H

void SPI1_SF_Init(void);

void W25Q64_WaitBusy_SF();
void W25Q64_Sector_Erase_SF(uint32_t Adress);
void W25Q64_Page_Program_SF(uint32_t Adress, uint8_t *Array, uint16_t Count);
void W25Q64_Read_Data_SF(uint32_t Adress, uint8_t *Array, uint16_t Count);

#endif