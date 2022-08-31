#include "spi.h"
#include "main.h"

extern SPI_HandleTypeDef hspi1;

uint8_t spi_exchange(uint8_t v) {
	uint8_t recv;

	HAL_SPI_TransmitReceive(&hspi1, &v, &recv, 1, 1000);

  return recv;
}

/* pull CS/SS low - RD15*/
void spi_flash_cs_en(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
}

void spi_flash_cs_dis(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}
