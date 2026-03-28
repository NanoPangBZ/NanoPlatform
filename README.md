# NanoPlatform

​	这是一个用于嵌入式软件开发的简易开发平台，用于快速开发简单产品级的嵌入式软件，且相对与arduino更加适合产品级的软件开发。支持类似与arduino包的模块扩展，且对扩展模块更改的自由度会更高。也会在平台中积累相关的通用组件，支持可配置、可扩展。

​	设计目标 : 高开发效率、高稳定性、简易、良好的扩展性和适配性、专业、开发时具备更高的自由度。

# 支持的Arch

- ch32f103(等待开发)
- gd32f4xx(开发中)
- n32g003(等待开发)
- rt1052(等待开发)
- simulator(等待开发)
  - 基于SDL的模拟器
- stm32f1xx(等待开发)
- stm32f4xx(等待开发)
- stm32g0(等待开发)
- stm32h7xx(等待开发)

# 验证类产品开发

- develop
  - NanoPlatform开发板
- nano_general_motor_board
  - 通用电机驱动板

# 其它项目

- nano_net(待办)
  - 一个适合多控制器产品的自定义私有网络通信库
- NanoPlatform-Terminal(待办)
  - 使用nano_net，用于监控、控制、调试基于NanoPlatform开发的设备。
  - 使用C++开发，支持windows、linux、MacOS
- nano_net_gen_node(待办)
  - 支持nano_net的通用硬件节点
  - 支持无线通信，用于桥接nano_net网络