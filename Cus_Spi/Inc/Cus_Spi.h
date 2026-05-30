/**
 * @file    Cus_Spi.h
 * @brief   SPI bus management layer
 * @license MIT
 * @copyright Copyright (c) 2026 [R6bandito]
*/


#ifndef __CUS_SPI_H__
#define __CUS_SPI_H__

#include <stdint.h>
#include <stdbool.h>


/* *********** Defines *********** */
#ifndef NULL
  #define NULL              (void *)0
#endif 
/* *********** Defines *********** */

/* ***************** Enum ***************** */
typedef enum
{
  CUS_SPI_MODE_0 = 0,
  CUS_SPI_MODE_1,
  CUS_SPI_MODE_2,
  CUS_SPI_MODE_3

} spiMode_t;
/* ***************** Enum ***************** */

typedef struct spiBus spiBus_t;   // 前置声明
typedef struct device deviceNode_t;

/* ***************** Dev_Node ***************** */
typedef struct device
{
  struct device *next;
  struct device *prev;
  spiMode_t devMode;          // 该设备进行通信的SPI模式.
  uint16_t devId;             // 该设备ID.
  bool is_MSBFirst;           // ture=MSB, false=LSB.
  const spiBus_t *spiBus;     // 所属总线.

} deviceNode_t;
/* ***************** Dev_Node ***************** */


/* ***************** SPI_SENTINEL ***************** */
typedef struct 
{
  struct device *next;
  struct device *prev;

} spiSentinel_t;
/* ***************** SPI_SENTINEL ***************** */


/* ***************** SPI_BUS ***************** */
/* SPI 总线结构定义 */
typedef struct spiBus
{
  uint16_t devCount;                                      // 当前SPI总线挂载设备数.
  bool isBusValid;                                        // SPI总线保护锁. (当前SPI总线是否可用).
  struct device *pxIndex;                                  // 当前设备索引.

  void (*select)( uint16_t devId, uint8_t pin_level );     // 片选控制回调.
  spiSentinel_t borderNode;                               // 哨兵节点.
} spiBus_t;
/* ***************** SPI_BUS ***************** */


/* ***************** API_Declare ***************** */
void Cus_SPI_BusInit( spiBus_t *bus );
void Cus_SPI_RegisterDevice( spiBus_t *bus, deviceNode_t *dev );
void Cus_SPI_DeviceInit( deviceNode_t *dev, uint16_t devId, spiMode_t mode, bool msbFirst );
void Cus_SPI_UnregisterDevice( spiBus_t *bus, deviceNode_t *dev );
/* ***************** API_Declare ***************** */

/* ***************** API_Declare ***************** */
void Cus_SPI_SendByte( const spiBus_t *spiBus, const deviceNode_t *dev, uint8_t tx_Byte );
uint8_t Cus_SPI_RecvByte( const spiBus_t *spiBus, const deviceNode_t *dev );
uint8_t Cus_SPI_TransferByte( const spiBus_t *spiBus, const deviceNode_t *dev, uint8_t tx_Byte );
void Cus_SPI_TransferBlock( const spiBus_t *spiBus, const deviceNode_t *dev, uint8_t *tx, uint8_t *rx, uint32_t len );
/* ***************** API_Declare ***************** */


#endif /* __CUS_SPI_H__ */
