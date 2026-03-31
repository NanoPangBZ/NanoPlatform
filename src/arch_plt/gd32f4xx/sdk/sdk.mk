SDK_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

#头文件包含路径
SDK_INC_DIRS += -I$(SDK_DIR)
SDK_INC_DIRS += -I$(SDK_DIR)CMSIS
SDK_INC_DIRS += -I$(SDK_DIR)CMSIS/GD/GD32F4xx/Include
SDK_INC_DIRS += -I$(SDK_DIR)GD32F4xx_standard_peripheral/Include

#CMSIS-Core
SDK_SRCS += $(SDK_DIR)CMSIS/GD/GD32F4xx/Source/system_gd32f4xx.c

#GD32F4xx标准外设库
SDK_SRCS += $(wildcard $(SDK_DIR)GD32F4xx_standard_peripheral/Source/*.c)

# SDK_SRCS += $(SDK_DIR)GD32F4xx_standard_peripheral/Source/gd32f4xx_usart.c
# SDK_SRCS += $(SDK_DIR)GD32F4xx_standard_peripheral/Source/gd32f4xx_rcu.c
# SDK_SRCS += $(SDK_DIR)GD32F4xx_standard_peripheral/Source/gd32f4xx_gpio.c
# SDK_SRCS += $(SDK_DIR)GD32F4xx_standard_peripheral/Source/gd32f4xx_adc.c
# SDK_SRCS += $(SDK_DIR)GD32F4xx_standard_peripheral/Source/gd32f4xx_can.c
# SDK_SRCS += $(SDK_DIR)GD32F4xx_standard_peripheral/Source/gd32f4xx_crc.c


