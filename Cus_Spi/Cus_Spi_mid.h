/**
 * @file    Cus_Spi_mid.h
 * @brief   SPI bus management layer
 * @license MIT
 * @copyright Copyright (c) 2026 [R6bandito]
*/


#ifndef __CUS_SPI_MID_H__
#define __CUS_SPI_MID_H__


#include "Cus_Spi_port.h"


/* ******** Defines ******** */
  #define SPI_DELAY_US      (5)
/* ******** Defines ******** */


/* ************************************ */ 
void Cus_SPI_Mid_ResetIdle( uint8_t mode );                                    // 总线电平复位.
void Cus_SPI_Mid_SendByte_MSB_NOLOCK( uint8_t tx, uint8_t mode );              // 只发送一个字节.(MSB字序/只发不收)
void Cus_SPI_Mid_SendByte_LSB_NOLOCK( uint8_t tx, uint8_t mode );              // 只发送一个字节.(LSB字序/只发不收)
uint8_t Cus_SPI_Mid_RecvByte_MSB_NOLOCK( uint8_t mode );                       // 只接收一个字节.(MSB字序/只发不收)
uint8_t Cus_SPI_Mid_RecvByte_LSB_NOLOCK( uint8_t mode );                       // 只接收一个字节.(LSB字序/只发不收)
uint8_t Cus_SPI_Mid_TransferByte_MSB_NOLOCK( uint8_t tx, uint8_t mode );       // 传输一个字节.(MSB字序/发与接收)
uint8_t Cus_SPI_Mid_TransferByte_LSB_NOLOCK( uint8_t tx, uint8_t mode );       // 传输一个字节.(LSB字序/发与接收)
void Cus_SPI_Mid_TransferBlock_MSB_NOLOCK( uint8_t *tx, uint8_t *rx, uint32_t len, uint8_t mode );      // 多字节传输.(MSB字序)
void Cus_SPI_Mid_TransferBlock_LSB_NOLOCK( uint8_t *tx, uint8_t *rx, uint32_t len, uint8_t mode );      // 多字节传输.(LSB自序)
/* ************************************ */


/* ************************************ */



/* ************************************ */

#endif /* __CUS_SPI_MID_H__ */ 
