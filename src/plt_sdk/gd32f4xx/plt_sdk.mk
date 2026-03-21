PLT_SDK_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

#头文件包含路径
PLT_SDK_INC_DIRS += -I$(PLT_SDK_DIR)
PLT_SDK_INC_DIRS += -I$(PLT_SDK_DIR)CMSIS
PLT_SDK_INC_DIRS += -I$(PLT_SDK_DIR)CMSIS/GD/GD32F4xx/Include
PLT_SDK_INC_DIRS += -I$(PLT_SDK_DIR)GD32F4xx_standard_peripheral/Include

#CMSIS-Core
PLT_SDK_SRCS += $(PLT_SDK_DIR)CMSIS/GD/GD32F4xx/Source/system_gd32f4xx.c

#启动文件
PLT_SDK_SRCS += $(PLT_SDK_DIR)startup_gd32f407_427.s