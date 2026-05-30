/**
 * @file    Cus_Spi.c
 * @brief   SPI bus management layer
 * @license MIT
 * @copyright Copyright (c) 2026 [R6bandito]
*/


#include "Cus_Spi.h"
#include "Cus_Spi_mid.h"
#include "Cus_Spi_port.h"


/* ******************************************* */
void Cus_SPI_BusInit( spiBus_t *bus );
void Cus_SPI_RegisterDevice( spiBus_t *bus, deviceNode_t *dev );
void Cus_SPI_DeviceInit( deviceNode_t *dev, uint16_t devId, spiMode_t mode, bool msbFirst );
static inline void __cus_spi_cs_write( uint16_t devId, uint8_t pin_level );
/* ******************************************* */

/* ******************************************* */
void Cus_SPI_SendByte( const spiBus_t *spiBus, const deviceNode_t *dev, uint8_t tx_Byte );
uint8_t Cus_SPI_RecvByte( const spiBus_t *spiBus, const deviceNode_t *dev );
uint8_t Cus_SPI_TransferByte( const spiBus_t *spiBus, const deviceNode_t *dev, uint8_t tx_Byte );
void Cus_SPI_TransferBlock( const spiBus_t *spiBus, const deviceNode_t *dev, uint8_t *tx, uint8_t *rx, uint32_t len );
/* ******************************************* */


/* ==================================================================================================== */
static inline void __cus_spi_cs_write( uint16_t devId, uint8_t pin_level )
{
  Cus_SPI_CS_Write(devId, pin_level);
}


void Cus_SPI_BusInit( spiBus_t *bus )
{
  if ( !bus )   return;

  bus->devCount = 0;
  bus->isBusValid = true;
  bus->select = __cus_spi_cs_write;

  /* 哨兵自环. */
  bus->borderNode.next = &bus->borderNode;
  bus->borderNode.prev = &bus->borderNode;

  /* 当前索引指向哨兵. */
  bus->pxIndex = (deviceNode_t *)bus->borderNode.next;
}


void Cus_SPI_RegisterDevice( spiBus_t *bus, deviceNode_t *dev )
{
  if ( !bus || !dev )   return;

  /* 获取尾节点. */
  deviceNode_t *tailNode = bus->borderNode.prev;

  /* 将设备插入到哨兵之前. */
  dev->next = &bus->borderNode;
  dev->prev = tailNode;
  tailNode->next = dev;
  bus->borderNode.prev = dev;  

  bus->devCount++;

  /* 当前索引指向最新插入的设备. */
  bus->pxIndex = dev;

  /* 挂入总线. */
  dev->spiBus = bus;
}


void Cus_SPI_DeviceInit( deviceNode_t *dev, uint16_t devId, spiMode_t mode, bool msbFirst )
{
  if ( !dev )   return;

  dev->devId = devId;
  dev->devMode = mode;
  dev->is_MSBFirst = msbFirst;
  dev->next = NULL;
  dev->prev = NULL;
}


void Cus_SPI_UnregisterDevice( spiBus_t *bus, deviceNode_t *dev )
{
  if ( !dev || !bus )   return;

  /* 所要摘除设备与挂载总线不符. */
  if ( dev->spiBus != bus )   return;   

  /* 防止用户传入一个未曾被注册过的空设备. */
  if ( !dev->next || !dev->prev )   return;

  /* O(1) 复杂度摘除 */
  dev->prev->next = dev->next;
  dev->next->prev = dev->prev;

  if ( bus->pxIndex == dev )  bus->pxIndex = dev->next;

  dev->spiBus = NULL;
  bus->devCount--;
}
/* ==================================================================================================== */


/* ==================================================================================================== */
void Cus_SPI_SendByte( const spiBus_t *spiBus, const deviceNode_t *dev, uint8_t tx_Byte )
{
  if ( !spiBus || !dev || !spiBus->isBusValid )   return;

  /* 进入临界. */
  Cus_SPI_ENTER_CRITICAL();

  /* 拉低 CSS. 选中对应设备准备开始传输. */
  spiBus->select(dev->devId, 0);

  if ( dev->is_MSBFirst )
  {
    Cus_SPI_Mid_SendByte_MSB_NOLOCK(tx_Byte, dev->devMode);
  }
  else 
  {
    Cus_SPI_Mid_SendByte_LSB_NOLOCK(tx_Byte, dev->devMode);
  }

  /* 通信结束. 释放设备. */
  spiBus->select(dev->devId, 1);

  Cus_SPI_EXIT_CRITICAL();
}


uint8_t Cus_SPI_RecvByte( const spiBus_t *spiBus, const deviceNode_t *dev )
{
  if ( !spiBus || !dev || !spiBus->isBusValid )   return 0;

  /* 进入临界. */
  Cus_SPI_ENTER_CRITICAL();  

  spiBus->select(dev->devId, 0);

  uint8_t recvB = 0;

  if( dev->is_MSBFirst )
  {
    recvB = Cus_SPI_Mid_RecvByte_MSB_NOLOCK(dev->devMode);
  }
  else 
  {
    recvB = Cus_SPI_Mid_RecvByte_LSB_NOLOCK(dev->devMode);
  }

  spiBus->select(dev->devId, 1);

  Cus_SPI_EXIT_CRITICAL();

  return recvB;
}


uint8_t Cus_SPI_TransferByte( const spiBus_t *spiBus, const deviceNode_t *dev, uint8_t tx_Byte )
{
  if ( !spiBus || !dev || !spiBus->isBusValid )   return 0;

  Cus_SPI_ENTER_CRITICAL(); 

  spiBus->select(dev->devId, 0);

  uint8_t recvB;
  if ( dev->is_MSBFirst )
  {
    recvB = Cus_SPI_Mid_TransferByte_MSB_NOLOCK(tx_Byte, dev->devMode);
  }
  else 
  {
    recvB = Cus_SPI_Mid_TransferByte_LSB_NOLOCK(tx_Byte, dev->devMode);
  }

  spiBus->select(dev->devId, 1);

  Cus_SPI_EXIT_CRITICAL();

  return recvB;
}


void Cus_SPI_TransferBlock( const spiBus_t *spiBus, const deviceNode_t *dev, uint8_t *tx, uint8_t *rx, uint32_t len )
{
  if ( !spiBus || !dev || !spiBus->isBusValid )   return;

  /* tx为NULL则只接收. rx为NULL则只发送. tx与rx不可同时为NULL. */
  if ( !tx && !rx )   return;

  Cus_SPI_ENTER_CRITICAL(); 

  spiBus->select(dev->devId, 0);

  if ( dev->is_MSBFirst )
  {
    Cus_SPI_Mid_TransferBlock_MSB_NOLOCK(tx, rx, len, dev->devMode);
  }
  else 
  {
    Cus_SPI_Mid_TransferBlock_LSB_NOLOCK(tx, rx, len, dev->devMode);
  }

  spiBus->select(dev->devId, 1);

  Cus_SPI_EXIT_CRITICAL();
}
/* ==================================================================================================== */




