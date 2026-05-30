/**
 * @file    Cus_Spi_port.h
 * @brief   SPI bus management layer
 * @license MIT
 * @copyright Copyright (c) 2026 [R6bandito]
*/


#ifndef __CUS_SPI_PORT_H__
#define __CUS_SPI_PORT_H__


#include <stdint.h>


/* ****** GPIO Mapping Conf ****** */
  /* 通信底层 GPIO 操作映射. */
  #define Cus_SPI_MISO_Read()                    Cus_GPIO_Read_MISO()
  #define Cus_SPI_MOSI_Write( level )            Cus_GPIO_Write_MOSI(level)
  #define Cus_SPI_CS_Write( devId, level )       Cus_GPIO_Write_CS(devId,level)
  #define Cus_SPI_CLK_Write( level )             Cus_GPIO_Write_CLK(level)
/* ****** GPIO Mapping Conf ****** */


/* ****** Delay Mapping Conf ****** */
  #define Cus_SPI_Delay_us(us)                   Cus_delay_us(us)
  #define Cus_SPI_Delay_ms(ms)                   Cus_delay_ms(ms)
/* ****** Delay Mapping Conf ****** */


/* ****** Critical Mapping Conf ****** */
  /* 临界段操作重映射. */
  #define Cus_SPI_ENTER_CRITICAL()               Cus_Critical_Enter()
  #define Cus_SPI_EXIT_CRITICAL()                Cus_Critical_Exit()
/* ****** Critical Mapping Conf ****** */


/* ****** Options Conf ****** */
  #define Cus_SPI_DBG_EN      (0)   // 是否开启调试输出.

/* ****** Options Conf ****** */


/* *************************************************** */
/* 请务必根据自身运行环境重写以下方法. 以便通信层能够正确操作底层硬件. */
__weak uint8_t Cus_GPIO_Read_MISO( void );
__weak void Cus_GPIO_Write_MOSI( uint8_t level );
__weak void Cus_GPIO_Write_CS( uint16_t devId, uint8_t level );
__weak void Cus_GPIO_Write_CLK( uint8_t level );
/* *************************************************** */


/* *************************************************** */
/* 请务必根据自身运行环境重写以下方法. 以保证通信层时序正确. */
__weak void Cus_delay_us( uint32_t us );
__weak void Cus_delay_ms( uint32_t ms );
/* *************************************************** */


/* *************************************************** */
/* 请务必根据自身运行环境重写以下方法. 以便通信层能够正确进行临界保护. */
__weak void Cus_Critical_Enter( void );
__weak void Cus_Critical_Exit( void );
/* *************************************************** */


#endif /* __CUS_SPI_PORT_H__ */
