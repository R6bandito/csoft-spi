/**
 * @file    Cus_Spi_mid.c
 * @brief   SPI bus management layer
 * @license MIT
 * @copyright Copyright (c) 2026 [R6bandito]
*/


#include "Cus_Spi_mid.h"


/* ****************************************** */
void Cus_SPI_Mid_ResetIdle( uint8_t mode );
void Cus_SPI_Mid_SendByte_MSB_NOLOCK( uint8_t tx, uint8_t mode );
void Cus_SPI_Mid_SendByte_LSB_NOLOCK( uint8_t tx, uint8_t mode );
uint8_t Cus_SPI_Mid_TransferByte_MSB_NOLOCK( uint8_t tx, uint8_t mode );
uint8_t Cus_SPI_Mid_TransferByte_LSB_NOLOCK( uint8_t tx, uint8_t mode );
void Cus_SPI_Mid_TransferBlock_MSB_NOLOCK( uint8_t *tx, uint8_t *rx, uint32_t len, uint8_t mode );
void Cus_SPI_Mid_TransferBlock_LSB_NOLOCK( uint8_t *tx, uint8_t *rx, uint32_t len, uint8_t mode );
uint8_t Cus_SPI_Mid_RecvByte_MSB_NOLOCK( uint8_t mode ); 
uint8_t Cus_SPI_Mid_RecvByte_LSB_NOLOCK( uint8_t mode );
/* ****************************************** */


/* ************* Defines ************* */
#ifdef __GNUC__
  #define CUS_SPI_MID_BIT_TRANSFER_MODE0(data)                                \
                                            ({                                \
                                              uint8_t bit;                    \
                                              Cus_SPI_MOSI_Write(data);       \
                                              Cus_SPI_Delay_us(SPI_DELAY_US); \
                                              Cus_SPI_CLK_Write(1);           \
                                              Cus_SPI_Delay_us(SPI_DELAY_US); \
                                              bit = Cus_SPI_MISO_Read();      \
                                              Cus_SPI_CLK_Write(0);           \
                                              Cus_SPI_Delay_us(SPI_DELAY_US); \
                                              bit;                            \
                                            }) 

  #define CUS_SPI_MID_BIT_TRANSFER_MODE1(data)                                \
                                            ({                                \
                                              uint8_t bit;                    \
                                              Cus_SPI_MOSI_Write(data);       \
                                              Cus_SPI_Delay_us(SPI_DELAY_US); \
                                              Cus_SPI_CLK_Write(1);           \
                                              Cus_SPI_Delay_us(SPI_DELAY_US); \
                                              Cus_SPI_CLK_Write(0);           \
                                              bit = Cus_SPI_MISO_Read();      \
                                              Cus_SPI_Delay_us(SPI_DELAY_US); \
                                              bit;                            \
                                            })                                           

  #define CUS_SPI_MID_BIT_TRANSFER_MODE2(data)                                \
                                            ({                                \
                                              uint8_t bit;                    \
                                              Cus_SPI_MOSI_Write(data);       \
                                              Cus_SPI_Delay_us(SPI_DELAY_US); \
                                              Cus_SPI_CLK_Write(0);           \
                                              Cus_SPI_Delay_us(SPI_DELAY_US); \
                                              bit = Cus_SPI_MISO_Read();      \
                                              Cus_SPI_CLK_Write(1);           \
                                              Cus_SPI_Delay_us(SPI_DELAY_US); \
                                              bit;                            \
                                            })                                   
                                            
  #define CUS_SPI_MID_BIT_TRANSFER_MODE3(data)                                \
                                            ({                                \
                                              uint8_t bit;                    \
                                              Cus_SPI_MOSI_Write(data);       \
                                              Cus_SPI_Delay_us(SPI_DELAY_US); \
                                              Cus_SPI_CLK_Write(0);           \
                                              Cus_SPI_Delay_us(SPI_DELAY_US); \
                                              Cus_SPI_CLK_Write(1);           \
                                              bit = Cus_SPI_MISO_Read();      \
                                              Cus_SPI_Delay_us(SPI_DELAY_US); \
                                              bit;                            \
                                            })  
#else 
  static inline uint8_t CUS_SPI_MID_BIT_TRANSFER_MODE0( uint8_t data )
  {
    uint8_t bit;                    
    Cus_SPI_MOSI_Write(data);       
    Cus_SPI_Delay_us(SPI_DELAY_US); 
    Cus_SPI_CLK_Write(1);           
    Cus_SPI_Delay_us(SPI_DELAY_US); 
    bit = Cus_SPI_MISO_Read();      
    Cus_SPI_CLK_Write(0);           
    Cus_SPI_Delay_us(SPI_DELAY_US); 
    return bit;                            
  }

  static inline uint8_t CUS_SPI_MID_BIT_TRANSFER_MODE1( uint8_t data )
  {
    uint8_t bit;                    
    Cus_SPI_MOSI_Write(data);       
    Cus_SPI_Delay_us(SPI_DELAY_US); 
    Cus_SPI_CLK_Write(1);           
    Cus_SPI_Delay_us(SPI_DELAY_US); 
    Cus_SPI_CLK_Write(0);           
    bit = Cus_SPI_MISO_Read();      
    Cus_SPI_Delay_us(SPI_DELAY_US); 
    return bit;                            
  }

  static inline uint8_t CUS_SPI_MID_BIT_TRANSFER_MODE2( uint8_t data )
  {
    uint8_t bit;                    
    Cus_SPI_MOSI_Write(data);       
    Cus_SPI_Delay_us(SPI_DELAY_US); 
    Cus_SPI_CLK_Write(0);           
    Cus_SPI_Delay_us(SPI_DELAY_US); 
    bit = Cus_SPI_MISO_Read();      
    Cus_SPI_CLK_Write(1);           
    Cus_SPI_Delay_us(SPI_DELAY_US); 
    return bit;                            
  }

  static inline uint8_t CUS_SPI_MID_BIT_TRANSFER_MODE3( uint8_t data )
  {
    uint8_t bit;                    
    Cus_SPI_MOSI_Write(data);       
    Cus_SPI_Delay_us(SPI_DELAY_US); 
    Cus_SPI_CLK_Write(0);           
    Cus_SPI_Delay_us(SPI_DELAY_US); 
    Cus_SPI_CLK_Write(1);           
    bit = Cus_SPI_MISO_Read();      
    Cus_SPI_Delay_us(SPI_DELAY_US); 
    return bit;                            
  }
#endif /* __GNUC */
/* ************* Defines ************* */



void Cus_SPI_Mid_ResetIdle( uint8_t mode )
{
  switch (mode)
  {
    case 0:
    case 1: Cus_SPI_CLK_Write(0);   break;    // 模式 0/1. 空闲总线电平拉低.

    case 2:
    case 3: Cus_SPI_CLK_Write(1);     break;    // 模式 2/3. 空闲总线电平拉高.

    default:  break;
  }

  /* 延时一段时间. 确保总线电平稳定. */
  Cus_SPI_Delay_us(SPI_DELAY_US);
}


void Cus_SPI_Mid_SendByte_MSB_NOLOCK( uint8_t tx, uint8_t mode )
{
  /* 总线状态复原. 准备发起一次通讯 */
  Cus_SPI_Mid_ResetIdle(mode);

  uint8_t send_bit = 0;

  for( int8_t i = 7; i >= 0; i-- )
  {
    send_bit = (tx >> i) & 0x01;
    switch (mode)
    {
      case 0:   CUS_SPI_MID_BIT_TRANSFER_MODE0(send_bit);  break;
      case 1:   CUS_SPI_MID_BIT_TRANSFER_MODE1(send_bit);  break;
      case 2:   CUS_SPI_MID_BIT_TRANSFER_MODE2(send_bit);  break;
      case 3:   CUS_SPI_MID_BIT_TRANSFER_MODE3(send_bit);  break;

      default:  break;
    }
  }
}


void Cus_SPI_Mid_SendByte_LSB_NOLOCK( uint8_t tx, uint8_t mode )
{
  /* 复原总线状态. 准备开始通讯. */
  Cus_SPI_Mid_ResetIdle(mode);

  uint8_t send_bit = 0;

  for( uint8_t i = 0; i < 8; i++ )
  {
    send_bit = (tx >> i) & 0x01;

    switch (mode)
    {
      case 0:   CUS_SPI_MID_BIT_TRANSFER_MODE0(send_bit);  break;
      case 1:   CUS_SPI_MID_BIT_TRANSFER_MODE1(send_bit);  break;
      case 2:   CUS_SPI_MID_BIT_TRANSFER_MODE2(send_bit);  break;
      case 3:   CUS_SPI_MID_BIT_TRANSFER_MODE3(send_bit);  break;
      
      default:  break;
    }
  }
}


uint8_t Cus_SPI_Mid_RecvByte_MSB_NOLOCK( uint8_t mode )
{
  /* 复原总线状态. 准备开始一次接收通讯. */
  Cus_SPI_Mid_ResetIdle(mode);

  uint8_t recv_byte = 0;
  uint8_t bit = 0;

  for( uint8_t i = 0; i < 8; i++ )
  {
    switch (mode)
    {
      case 0:  bit = CUS_SPI_MID_BIT_TRANSFER_MODE0(1);  break;
      case 1:  bit = CUS_SPI_MID_BIT_TRANSFER_MODE1(1);  break;
      case 2:  bit = CUS_SPI_MID_BIT_TRANSFER_MODE2(1);  break;
      case 3:  bit = CUS_SPI_MID_BIT_TRANSFER_MODE3(1);  break;
      
      default:  break;
    }

    recv_byte |= (bit << (7 - i));
  }

  return recv_byte;
}


uint8_t Cus_SPI_Mid_RecvByte_LSB_NOLOCK( uint8_t mode )
{
  /* 复原总线状态. 准备开始一次接收通讯. */
  Cus_SPI_Mid_ResetIdle(mode);

  uint8_t recv_byte = 0;
  uint8_t bit = 0;

  for( uint8_t i = 0; i < 8; i++ )
  {
    switch (mode)
    {
      case 0:  bit = CUS_SPI_MID_BIT_TRANSFER_MODE0(1); break;
      case 1:  bit = CUS_SPI_MID_BIT_TRANSFER_MODE1(1); break;
      case 2:  bit = CUS_SPI_MID_BIT_TRANSFER_MODE2(1); break;
      case 3:  bit = CUS_SPI_MID_BIT_TRANSFER_MODE3(1); break;
      
      default:  break;
    }

    recv_byte |= (bit << i);
  }

  return recv_byte;
}


uint8_t Cus_SPI_Mid_TransferByte_MSB_NOLOCK( uint8_t tx, uint8_t mode )
{
  /* 总线状态复位. */
  Cus_SPI_Mid_ResetIdle(mode);

  uint8_t recv_byte = 0;
  uint8_t send_bit = 0;
  uint8_t recv_bit = 0;

  for( uint8_t i = 0; i < 8; i++ )
  {
    send_bit = (tx >> (7 - i)) & 0x01;

    switch (mode)
    {
      case 0:  recv_bit = CUS_SPI_MID_BIT_TRANSFER_MODE0(send_bit); break;
      case 1:  recv_bit = CUS_SPI_MID_BIT_TRANSFER_MODE1(send_bit); break;
      case 2:  recv_bit = CUS_SPI_MID_BIT_TRANSFER_MODE2(send_bit); break;
      case 3:  recv_bit = CUS_SPI_MID_BIT_TRANSFER_MODE3(send_bit); break;
      
      default:  break;
    }

    recv_byte |= (recv_bit << (7 - i));
  }

  return recv_byte;
}


uint8_t Cus_SPI_Mid_TransferByte_LSB_NOLOCK( uint8_t tx, uint8_t mode )
{
  Cus_SPI_Mid_ResetIdle(mode);

  uint8_t recv_byte = 0;
  uint8_t send_bit = 0;
  uint8_t recv_bit = 0;  

  for( uint8_t i = 0; i < 8; i++ )
  {
    send_bit = (tx >> i) & 0x01;

    switch (mode)
    {
      case 0: recv_bit = CUS_SPI_MID_BIT_TRANSFER_MODE0(send_bit);  break;
      case 1: recv_bit = CUS_SPI_MID_BIT_TRANSFER_MODE1(send_bit);  break;
      case 2: recv_bit = CUS_SPI_MID_BIT_TRANSFER_MODE2(send_bit);  break;
      case 3: recv_bit = CUS_SPI_MID_BIT_TRANSFER_MODE3(send_bit);  break;
      
      default:   break;
    }

    recv_byte |= (recv_bit << i);
  }

  return recv_byte;
}


void Cus_SPI_Mid_TransferBlock_MSB_NOLOCK( uint8_t *tx, uint8_t *rx, uint32_t len, uint8_t mode )
{
  if ( len == 0 )   return;

  uint8_t tx_byte = 0;
  uint8_t tx_bit = 0;
  uint8_t rx_byte = 0;
  uint8_t rx_bit = 0;

  Cus_SPI_Mid_ResetIdle(mode);

  for( uint32_t i = 0; i < len; i++ )
  {
    tx_byte = (tx ? tx[i] : 0xFF);

    /* 此处多字节发送为了性能考虑采用内联宏形式而非调用 Cus_SPI_Mid_TransferByte_MSB_NOLOCK(). */
    rx_byte = 0;
    for( uint8_t j = 0; j < 8; j++ )
    {
      tx_bit = (tx_byte >> (7 - j)) & 0x01;

      switch (mode)
      {
        case 0:  rx_bit = CUS_SPI_MID_BIT_TRANSFER_MODE0(tx_bit); break;
        case 1:  rx_bit = CUS_SPI_MID_BIT_TRANSFER_MODE1(tx_bit); break;
        case 2:  rx_bit = CUS_SPI_MID_BIT_TRANSFER_MODE2(tx_bit); break;
        case 3:  rx_bit = CUS_SPI_MID_BIT_TRANSFER_MODE3(tx_bit); break;
        
        default:    break;
      }

      rx_byte |= (rx_bit << (7 - j));
    }

    /* 如果传入 rx 有效，则返回MISO读取值. 否则静默丢弃. */
    if ( rx )   rx[i] = rx_byte;
  }
}


void Cus_SPI_Mid_TransferBlock_LSB_NOLOCK( uint8_t *tx, uint8_t *rx, uint32_t len, uint8_t mode )
{
  if ( len == 0 )   return;

  uint8_t tx_byte = 0;
  uint8_t tx_bit = 0;
  uint8_t rx_byte = 0;
  uint8_t rx_bit = 0;

  Cus_SPI_Mid_ResetIdle(mode);

  for( uint32_t i = 0; i < len; i++ )
  {
    tx_byte = (tx ? tx[i] : 0xFF);

    rx_byte = 0;    // 清0. 防止上一次接收字节的 1 为干扰后续数据.
    for( uint8_t j = 0; j < 8; j++ )
    {
      tx_bit = (tx_byte >> j) & 0x01;

      switch (mode)
      {
        case 0: rx_bit = CUS_SPI_MID_BIT_TRANSFER_MODE0(tx_bit); break;
        case 1: rx_bit = CUS_SPI_MID_BIT_TRANSFER_MODE1(tx_bit); break;
        case 2: rx_bit = CUS_SPI_MID_BIT_TRANSFER_MODE2(tx_bit); break;
        case 3: rx_bit = CUS_SPI_MID_BIT_TRANSFER_MODE3(tx_bit); break;
        
        default:  break;
      }

      rx_byte |= (rx_bit << j);
    }

    if ( rx )   rx[i] = rx_byte;
  }
}

