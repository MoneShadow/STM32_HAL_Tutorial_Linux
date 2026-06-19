#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_rcc.h"
#include <stdint.h>
#include "spi.h"

#define SPI1_NSS(x) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, (GPIO_PinState)(x));

SPI_HandleTypeDef hspi1;

void SPI1_Init(void) {
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    HAL_SPI_Init(&hspi1);
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == SPI1) {
        __HAL_RCC_SPI1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitTypeDef hgpioa;

        /* NSS  */
        hgpioa.Mode = GPIO_MODE_OUTPUT_PP;
        hgpioa.Pin = GPIO_PIN_4;
        hgpioa.Pull = GPIO_NOPULL;
        hgpioa.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &hgpioa);

        /* SCK MOSI */
        hgpioa.Mode = GPIO_MODE_AF_PP;
        hgpioa.Pin = GPIO_PIN_5 | GPIO_PIN_7;
        hgpioa.Pull = GPIO_NOPULL;
        hgpioa.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &hgpioa);

        /* MISO */
        hgpioa.Mode = GPIO_MODE_INPUT;
        hgpioa.Pin = GPIO_PIN_6;
        hgpioa.Pull = GPIO_PULLUP;
        HAL_GPIO_Init(GPIOA, &hgpioa);
    }
}

/*----------W25Q64----------*/

/* 写使能 */
void W25Q64_Write_Enable() {
    uint8_t cmd = 0x06;
	SPI1_NSS(0);
	HAL_SPI_Transmit(&hspi1, &cmd, 1, 1000);
	SPI1_NSS(1);
}

/* 写失能 */
void W25Q64_Write_Disable() {
    uint8_t cmd = 0x04;
	SPI1_NSS(0);
	HAL_SPI_Transmit(&hspi1, &cmd, 1, 1000);
	SPI1_NSS(1);
}

/* 读取状态寄存器等待不忙 */
void W25Q64_WaitBusy() {
	uint16_t Tout = 10000;
    uint8_t cmd = 0x05;
	uint8_t Status = 0;
	SPI1_NSS(0);
	do {
		HAL_SPI_TransmitReceive(&hspi1, &cmd, &Status, 1, 1000);
		Tout--;
		if (Tout == 0)
			break;
	} while (Status & 0x01);
	SPI1_NSS(1);
}

/* 页擦除 */
void W25Q64_Sector_Erase(uint32_t Adress) {
	W25Q64_Write_Enable();
	
    uint8_t cmd[4];
    cmd[0] = 0x20;
    cmd[1] = Adress >> 16;
    cmd[2] = Adress >> 8;
    cmd[3] = Adress;
	SPI1_NSS(0);
	HAL_SPI_Transmit(&hspi1, &cmd[0], 1, 1000);
	HAL_SPI_Transmit(&hspi1, &cmd[1], 1, 1000);
	HAL_SPI_Transmit(&hspi1, &cmd[2], 1, 1000);
	HAL_SPI_Transmit(&hspi1, &cmd[3], 1, 1000);
	SPI1_NSS(1);

	W25Q64_WaitBusy();
}

/* 页编程 */
void W25Q64_Page_Program(uint32_t Adress, uint8_t *Array, uint16_t Count) {
	W25Q64_Write_Enable();

    uint8_t cmd[4];
    cmd[0] = 0x02;
    cmd[1] = Adress >> 16;
    cmd[2] = Adress >> 8;
    cmd[3] = Adress;
	SPI1_NSS(0);
	HAL_SPI_Transmit(&hspi1, &cmd[0], 1, 1000);
	HAL_SPI_Transmit(&hspi1, &cmd[1], 1, 1000);
	HAL_SPI_Transmit(&hspi1, &cmd[2], 1, 1000);
	HAL_SPI_Transmit(&hspi1, &cmd[3], 1, 1000);
    HAL_SPI_Transmit(&hspi1, Array, Count, 1000);
	SPI1_NSS(1);
	
	W25Q64_WaitBusy();
}

/* 读取 */
void W25Q64_Read_Data(uint32_t Adress, uint8_t *Array, uint16_t Count) {
    uint8_t cmd[4];
    cmd[0] = 0x03;
    cmd[1] = Adress >> 16;
    cmd[2] = Adress >> 8;
    cmd[3] = Adress;
	SPI1_NSS(0);
	HAL_SPI_Transmit(&hspi1, &cmd[0], 1, 1000);
	HAL_SPI_Transmit(&hspi1, &cmd[1], 1, 1000);
	HAL_SPI_Transmit(&hspi1, &cmd[2], 1, 1000);
	HAL_SPI_Transmit(&hspi1, &cmd[3], 1, 1000);
    HAL_SPI_Receive(&hspi1, Array, Count, 1000);
	SPI1_NSS(1);
}

