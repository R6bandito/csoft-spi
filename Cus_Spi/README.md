# Cus_SPI - 轻量级软件 SPI 库
​	纯C实现的轻量级软件SPI通信协议库，支持 **LSB/MSB位序切换**，**四种SPI通信模式**，**多设备总线管理**。

整体实现分为三层架构，不依赖任何特定MCU，只需重写移植层的几个弱函数即可轻松移植到任何平台。

------

## ↗特性

- 纯C实现，无外设标准库依赖（仅需 `<stdint.h>` / `<stdbool.h>`）。
- 支持 **SPI 模式 0/1/2/3**。
- 支持 **MSB First与LSB First位序**切换。
- 支持**全双工收发与半双工通信**（只发不收/只收不发）。
- **循环链表总线结构**（支持 o(1)挂载设备与o(1)解挂设备）。
- **关键时序临界区保护**（用户可根据自身运行环境覆盖临界区方法）。
- 底层硬件无关，移植简便。
- 易于拓展，分层架构边界清晰，拓展逻辑清晰，方便自定义行为。

------

## 👥适用群体

- **相关方向学生**：通过重写移植层基本方法能够快速启动通信，对外API接口清晰易读，适用于学生项目与SPI相关外设建立通信。
- **小项目 / 原型验证**：无需从头编写 SPI 时序，快速实现传感器、Flash、显示屏等设备的通信。
- **需要多SPI设备管理逻辑**：采用哨兵双向链表结构管理SPI总线，SPI外设挂载于总线上，便于管理。使用临界区管理天然防止多设备同时操作总线导致异常。
- **资源受限开发者**：当 MCU 硬件 SPI 通道不足，或需要额外软件模拟 SPI 时，本库可直接复用。

------

## 📁文件结构

```
Cus_SPI/
├── Cus_Spi_port.h        // 硬件移植层：宏映射 + 弱函数声明
├── Cus_Spi_port.c        // 弱函数默认实现（用户主要需要修改的文件）
├── Cus_Spi_mid.h         // 时序原语层：无锁 SPI 传输函数
├── Cus_Spi_mid.c         // 时序原语实现（四种模式，MSB/LSB，多个传输方法（字节发送接收/字节块发送接收）等）
├── Cus_Spi.h             // 总线管理层：设备链表 + 片选回调
└── Cus_Spi.c             // 总线管理实现（注册设备，带临界区的传输 API）
```

------

## 🛠 移植指南

1. 将所有源文件与.h文件复制到你的工程中。

2. 重写移植层（`Cus_Spi_port.c`）中的__weak方法：

   ```c
   /* 请务必根据自身运行环境重写以下方法. 以确保通信层能够正确操作底层硬件完成时序动作. */ 
   __weak uint8_t Cus_GPIO_Read_MISO( void );		
   __weak void Cus_GPIO_Write_MOSI( uint8_t level );
   __weak void Cus_GPIO_Write_CS( uint16_t devId, uint8_t level );
   __weak void Cus_GPIO_Write_CLK( uint8_t level );
   
   /* 请务必根据自身运行环境重写以下方法. 以确保通信层时序正确. */
   /* Ps: 我的工具库仓库中(Cus_Utils_Lib)已经提供了基于 DWT/Systick 的延时逻辑. 可直接使用. */
   __weak void Cus_delay_us( uint32_t us );
   __weak void Cus_delay_ms( uint32_t ms );
   
   /* 请务必根据自身运行环境重写以下方法. 以便通信层能够正确进行临界保护. */
   __weak void Cus_Critical_Enter( void );
   __weak void Cus_Critical_Exit( void );
   ```

3. 配置延时。目前版本(1.0)你可以通过修改 `Cus_Spi_mid.h`中的`SPI_DELAY_US`（半周期微秒数，默认 5µs → 速率约 100kHz）来配置你的通信速率（后续考虑提供更直观的配置方法）。

4. 头文件包含。在你的代码中 `#include "Cus_Spi.h"`。之后调用相关API即可。

------

## ⚙️宏配置选项

|     宏名称     |                           描述                            | 默认值 |
| :------------: | :-------------------------------------------------------: | :----: |
|  SPI_DELAY_US  | SPI 时钟半周期微秒数（值越小速度越快，受 CPU 能力限制）。 |   5    |
| Cus_SPI_DBG_EN |          调试输出开关（当前版本(1.0)仅作预留）。          |   0    |

------

## 🚀快速开始

- **首先你需要实现硬件移植抽象层**（协议栈运作根基！）。
  根据你的MCU与具体运行环境，仿照下例实现相关__weak函数：

  ```c
  /* 示例环境：STM32F103ZET6  基于HAL库 */
  
  /* ************ myspi_port.h  ************ */
  #define SPI_CLK_PIN    GPIO_PIN_5
  #define SPI_MOSI_PIN   GPIO_PIN_7
  #define SPI_MISO_PIN   GPIO_PIN_6
  #define SPI_CS_PIN_A   GPIO_PIN_4		// 假设有 A B 两个SPI设备.
  #define SPI_CS_PIN_B   GPIO_PIN_7
  #define SPI_PORT       GPIOA
  
  void HAL_SPI_Port_Init(void); 
  /* ************ myspi_port.h  ************ */
  
  /* ************ myspi_port.c  ************ */
  #include "myspi_port.h"
  #include "stm32f1xx_hal.h"
  #include "Cus_Spi_port.h"
  
  static GPIO_InitTypeDef gpio_init;
  
  void HAL_SPI_Port_Init(void) 
  {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init.Pin = SPI_CLK_PIN | SPI_MOSI_PIN | SPI_CS_PIN;
    HAL_GPIO_Init(SPI_PORT, &gpio_init);
    
    gpio_init.Mode = GPIO_MODE_INPUT;
    gpio_init.Pin = SPI_MISO_PIN;
    HAL_GPIO_Init(SPI_PORT, &gpio_init);
    
    // 初始空闲电平
    HAL_GPIO_WritePin(SPI_PORT, SPI_CLK_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SPI_PORT, SPI_CS_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SPI_PORT, SPI_MOSI_PIN, GPIO_PIN_SET);
  }
  
  uint8_t Cus_GPIO_Read_MISO(void) 
  {
    return HAL_GPIO_ReadPin(SPI_PORT, SPI_MISO_PIN);
  }
  
  void Cus_GPIO_Write_MOSI(uint8_t level) 
  {
    HAL_GPIO_WritePin(SPI_PORT, SPI_MOSI_PIN, level ? GPIO_PIN_SET : GPIO_PIN_RESET);
  }
  
  void Cus_GPIO_Write_CS(uint16_t devId, uint8_t level) 
  {
      /* A B 两个设备需要由用户指定对应的devId. 该ID在注册进设备后将交由协议栈控制. 此处简略为0和1 */ 
      GPIO_PinState pinState = level ? GPIO_PIN_SET : GPIO_PIN_RESET;
      switch (devId) 
      {
          case 0:	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, pinState);	break;
          case 1: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, pinState); break;
          // 可继续增加更多设备
          default:	break;
      }
  }
  
  void Cus_GPIO_Write_CLK(uint8_t level) 
  {
    HAL_GPIO_WritePin(SPI_PORT, SPI_CLK_PIN, level ? GPIO_PIN_SET : GPIO_PIN_RESET);
  }
  
  void Cus_Critical_Enter(void) { __disable_irq(); }
  void Cus_Critical_Exit(void)  { __enable_irq();  }
  /* ************ myspi_port.c  ************ */
  
  // 参考实现..
  ```

- **初始化总线并注册设备**

  ```c
  #include "Cus_Spi.h"
  
  /* 全局SPI总线与设备. */
  spiBus_t myBus;
  deviceNode_t DevA;
  deviceNode_t DevB;
  
  int main( void ) 
  {
      /* 初始化总线（片选回调会自动使用 Cus_SPI_CS_Write）. */
      Cus_SPI_BusInit(&myBus);
      
      /* 初始化设备节点（A设备ID 0，模式0，MSB优先/B设备ID 1，模式0，MSB优先）. */
      Cus_SPI_DeviceInit(&DevA, 0, CUS_SPI_MODE_0, true);
      Cus_SPI_DeviceInit(&DevB, 1, CUS_SPI_MODE_0, true);
      
      /* 将设备注册到总线. */
      Cus_SPI_RegisterDevice(&myBus, &DevA);
      Cus_SPI_RegisterDevice(&myBus, &DevB);
      
      /* 此时. SPI总线上就已经挂载了 Device A 和 Device B. 并且模式已设置完毕. 可以启动通信. */
  }
  
  ```

- **进行SPI通信**

  ```c
  uint8_t tx_data = 0x55;
  uint8_t rx_data;
  
  /* 仅发送（忽略接收）. */
  Cus_SPI_SendByte(&myBus, &DevA, tx_data);
  
  /* 仅接收（发送0xFF）*/
  rx_data = Cus_SPI_RecvByte(&myBus, &DevB);
  
  /* 全双工传输一个字节 */
  rx_data = Cus_SPI_TransferByte(&myBus, &DevA, tx_data);
  
  // 多字节块传输（发送数组，同时接收）
  uint8_t tx_buf[4] = {0x01, 0x02, 0x03, 0x04};
  uint8_t rx_buf[4];
  Cus_SPI_TransferBlock(&myBus, &DevB, tx_buf, rx_buf, 4);
  ```

------

## 📖API Reference

- **总线与设备管理**

| 函数                                           |                             参数                             |               说明               |
| :--------------------------------------------- | :----------------------------------------------------------: | :------------------------------: |
| Cus_SPI_BusInit(bus)                           |                    bus(需要初始化的总线)                     | 初始化总线（哨兵节点、清零计数） |
| Cus_SPI_RegisterDevice(bus, dev)               |                 bus(操作总线)；dev(注册设备)                 |   将设备注册到总线（插入链表）   |
| Cus_SPI_DeviceInit(dev, devId, mode, msbFirst) | dev(所要初始化的设备)；devId(设备ID)；mode(SPI模式);msbFirst(布尔型变量 true=MSB,false=LSB) |     初始化设备节点（不挂载）     |
| Cus_SPI_UnregisterDevice(bus, dev)             |               bus(操作总线)；dev(要解挂的设备)               |          从总线注销设备          |

- **数据传输**

| 函数                                         | 参数                                                         | 说明                                  |
| -------------------------------------------- | ------------------------------------------------------------ | ------------------------------------- |
| Cus_SPI_SendByte(bus, dev, tx)               | bus(操作总线)；dev(发送设备)；tx(要发送的字节)               | 仅发送一个字节(不接收)                |
| Cus_SPI_RecvByte(bus, dev)                   | bus(操作总线)；dev(接收设备)                                 | 仅接收一个字节（发送 0xFF）           |
| Cus_SPI_TransferByte(bus, dev, tx)           | bus(操作总线)；dev(发送接收设备)；tx(要发送字节)             | 全双工传输一个字节                    |
| Cus_SPI_TransferBlock(bus, dev, tx, rx, len) | bus(操作总线)；dev(发送接收设备)；tx(发送缓冲区指针)；rx(接收缓冲区指针)；len(传输长度) | 多字节块传输（允许 tx 或 rx 为 NULL） |

------

## 📄附注

1. 针对 `void Cus_SPI_TransferBlock(const spiBus_t *spiBus, const deviceNode_t *dev, uint8_t *tx, uint8_t *rx, uint32_t len)`

   其参数 tx 与 rx 可以分别为 NULL. 当 tx 为 NULL 时表示只接收不发送(表现为发送0xFF)；当 rx 为NULL时表示只发送不接收(表现为丢弃接收值)。**tx 与 rx 不能同时为NULL**，否则将提前返回。

2. 对于此类发送API：

   ```c
   Cus_SPI_SendByte(DevA.spiBus, &DevA, 0x55);
   rx2 = Cus_SPI_TransferByte(DevA.spiBus, &DevA, tx);
   Cus_SPI_TransferBlock(DevA.spiBus, &DevA, tx_block, NULL, 4); // 只发送
   ```

   在传 `const spiBus_t *spiBus` 这个入口参数时，建议使用设备内部维持的 `.spiBus` 成员传入。这种方法直观便捷且不易出错。但是在注册设备时`Cus_SPI_RegisterDevice()`必须直接传入对应的bus总线变量，不允许间接传入，因为设备刚Init之后 .spiBus 成员为 NULL。

------

## 📄许可证

​	MIT License。详见项目跟目录的LICENSE文件。

------

**本库长期维护**

**当前版本 : Version 1.0**  

