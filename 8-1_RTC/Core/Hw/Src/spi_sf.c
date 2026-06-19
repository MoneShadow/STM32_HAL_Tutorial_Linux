#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_rcc.h"
#include <stdint.h>
#include "spi_sf.h"

#define SPI1_SF_NSS(x)  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, (GPIO_PinState)(x))
#define SPI1_SF_SCK(x)  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, (GPIO_PinState)(x))
#define SPI1_SF_Read()  HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6)
#define SPI1_SF_Send(x) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, (GPIO_PinState)(x))

GPIO_InitTypeDef hgpioa;

void SPI1_SF_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* SCK MOSI NSS */
    hgpioa.Mode = GPIO_MODE_OUTPUT_PP;
    hgpioa.Pin = GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_4;
    hgpioa.Pull = GPIO_NOPULL;
    hgpioa.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &hgpioa);

    /* MISO */
    hgpioa.Mode = GPIO_MODE_INPUT;
    hgpioa.Pin = GPIO_PIN_6;
    hgpioa.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &hgpioa);

    /* 模式0 第一个边沿采集数据 第二个边沿移出数据 */
    SPI1_SF_SCK(0);
    SPI1_SF_NSS(1);
}

void SPI_SF_Transmit(uint8_t *TxData) {
    /* 抛玉引砖 */

    /* 模式0 要提前一次移出数据 */
    if (((*TxData) & 0x80) == 0) {    // 主机移出数据 MO
        SPI1_SF_Send(0);
    }
    else {
        SPI1_SF_Send(1);
    }
    /* 等待从机移出数据 SO */

    SPI1_SF_SCK(1); // 等待采样数据
    /* 主从机采样 MI SI */

    for (uint8_t i = 1; i < 8; i++) {
        SPI1_SF_SCK(0); // 等待移出数据
        if (((*TxData) & (0x80 >> i)) == 0) {    // 主机移出数据
            SPI1_SF_Send(0);
        }
        else {
            SPI1_SF_Send(1);
        }
        /* 等待从机移出数据 SO */

        SPI1_SF_SCK(1); // 等待采样数据
        /* 主从机采样 MI SI */
    }

    SPI1_SF_SCK(0);
}

void SPI_SF_Receive(uint8_t *RxData) {
    /* 抛砖引玉 */
    *RxData = 0;

    /* 模式0 要提前一次移出数据 */   
    SPI1_SF_Send(1);    // 主机移出数据 MO
    /* 等待从机移出数据 SO */

    SPI1_SF_SCK(1); // 等待采样数据
    if (SPI1_SF_Read() == GPIO_PIN_SET) {
        (*RxData) |= 0x80;    // 主机采样 MI
    }
    /* 从机采样 SI */

    for (uint8_t i = 1; i < 8; i++) {
        SPI1_SF_SCK(0);     // 等待移出数据

        SPI1_SF_Send(1);    // 主机移出数据
        /* 等待从机移出数据 SO */

        SPI1_SF_SCK(1); // 等待采样数据

        if (SPI1_SF_Read() == GPIO_PIN_SET) {
            (*RxData) |= (0x80 >> i);    // 主机采样 MI
        }
        /* 从机采样 SI */
    }

    SPI1_SF_SCK(0);
}

void SPI_SF_TransmitReceive(uint8_t *TxData, uint8_t *RxData) {
    /* 抛砖引玉 */
    *RxData = 0;

    /* 模式0 要提前一次移出数据 */
    if (((*TxData) & 0x80) == 0) {    // 主机移出数据 MO
        SPI1_SF_Send(0);
    }
    else {
        SPI1_SF_Send(1);
    }
    /* 等待从机移出数据 SO */

    SPI1_SF_SCK(1); // 等待采样数据
    if (SPI1_SF_Read() == GPIO_PIN_SET) {
        (*RxData) |= 0x80;    // 主机采样 MI
    }
    /* 从机采样 SI */

    for (uint8_t i = 1; i < 8; i++) {
        SPI1_SF_SCK(0);     // 等待移出数据

        if (((*TxData) & (0x80 >> i)) == 0) {    // 主机移出数据
            SPI1_SF_Send(0);
        }
        else {
            SPI1_SF_Send(1);
        }
        /* 等待从机移出数据 SO */

        SPI1_SF_SCK(1); // 等待采样数据

        if (SPI1_SF_Read() == GPIO_PIN_SET) {
            (*RxData) |= (0x80 >> i);    // 主机采样 MI
        }
        /* 从机采样 SI */
    }

    SPI1_SF_SCK(0);
}

/*----------W25Q64----------*/

/* 写使能 */
void W25Q64_Write_Enable_SF() {
    uint8_t cmd = 0x06;
	SPI1_SF_NSS(0);
	SPI_SF_Transmit(&cmd);
	SPI1_SF_NSS(1);
}

/* 写失能 */
void W25Q64_Write_Disable_SF() {
    uint8_t cmd = 0x04;
	SPI1_SF_NSS(0);
	SPI_SF_Transmit(&cmd);
	SPI1_SF_NSS(1);
}

/* 读取状态寄存器等待不忙 */
void W25Q64_WaitBusy_SF() {
	uint16_t Tout = 10000;
    uint8_t cmd = 0x05;
	uint8_t Status = 0;
	SPI1_SF_NSS(0);
    SPI_SF_Transmit(&cmd);
	do {
        SPI_SF_TransmitReceive(&cmd, &Status);
		Tout--;
		if (Tout == 0)
			break;
	} while (Status & 0x01);
	SPI1_SF_NSS(1);
}

/* 页擦除 */
void W25Q64_Sector_Erase_SF(uint32_t Adress) {
	W25Q64_Write_Enable_SF();
	
    uint8_t cmd[4];
    cmd[0] = 0x20;
    cmd[1] = Adress >> 16;
    cmd[2] = Adress >> 8;
    cmd[3] = Adress;
	SPI1_SF_NSS(0);
	SPI_SF_Transmit(&cmd[0]);
	SPI_SF_Transmit(&cmd[1]);
	SPI_SF_Transmit(&cmd[2]);
	SPI_SF_Transmit(&cmd[3]);
	SPI1_SF_NSS(1);

	W25Q64_WaitBusy_SF();
}

/* 页编程 */
void W25Q64_Page_Program_SF(uint32_t Adress, uint8_t *Array, uint16_t Count) {
	W25Q64_Write_Enable_SF();

    uint8_t cmd[4];
    cmd[0] = 0x02;
    cmd[1] = Adress >> 16;
    cmd[2] = Adress >> 8;
    cmd[3] = Adress;
	SPI1_SF_NSS(0);
	SPI_SF_Transmit(&cmd[0]);
	SPI_SF_Transmit(&cmd[1]);
	SPI_SF_Transmit(&cmd[2]);
	SPI_SF_Transmit(&cmd[3]);
    for (uint16_t i = 0, j = Count; j > 0; i++, j--) {
        SPI_SF_Transmit(&Array[i]);
    }
    
	SPI1_SF_NSS(1);
	
	W25Q64_WaitBusy_SF();
}

/* 读取 */
void W25Q64_Read_Data_SF(uint32_t Adress, uint8_t *Array, uint16_t Count) {
    uint8_t cmd[4];
    cmd[0] = 0x03;
    cmd[1] = Adress >> 16;
    cmd[2] = Adress >> 8;
    cmd[3] = Adress;
	SPI1_SF_NSS(0);
	SPI_SF_Transmit(&cmd[0]);
	SPI_SF_Transmit(&cmd[1]);
	SPI_SF_Transmit(&cmd[2]);
	SPI_SF_Transmit(&cmd[3]);
    for (uint16_t i = 0, j = Count; j > 0; i++, j--) {
        SPI_SF_Receive(&Array[i]);
    }
	SPI1_SF_NSS(1);
}