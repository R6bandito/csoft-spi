/**
 * @file    Cus_Spi_port.c
 * @brief   SPI bus management layer
 * @license MIT
 * @copyright Copyright (c) 2026 [R6bandito]
*/


#include "Cus_Spi_port.h"


__weak uint8_t Cus_GPIO_Read_MISO( void )
{
  return 0;
}


__weak void Cus_GPIO_Write_MOSI( uint8_t level )
{
  (void)level;
}


__weak void Cus_GPIO_Write_CS( uint16_t devId, uint8_t level )
{
  (void)level;
  (void)devId;
}


__weak void Cus_GPIO_Write_CLK( uint8_t level )
{
  (void)level;
}


__weak void Cus_Critical_Enter( void )
{
  (void)0;
}


__weak void Cus_Critical_Exit( void )
{
  (void)0;
}
