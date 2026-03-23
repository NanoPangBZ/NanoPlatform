PLT_SDK_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

#头文件包含路径
PLT_SDK_INC_DIRS += -I$(PLT_SDK_DIR)
PLT_SDK_INC_DIRS += -I$(PLT_SDK_DIR)CMSIS
PLT_SDK_INC_DIRS += -I$(PLT_SDK_DIR)CMSIS/GD/GD32F4xx/Include
PLT_SDK_INC_DIRS += -I$(PLT_SDK_DIR)GD32F4xx_standard_peripheral/Include

#CMSIS-Core
PLT_SDK_SRCS += $(PLT_SDK_DIR)CMSIS/GD/GD32F4xx/Source/system_gd32f4xx.c

#GD32F4xx libc
PLT_SDK_SRCS += $(PLT_SDK_DIR)gd32f4xx_libc_file.c

#编译器附加选项
PLT_SDK_CFLAGS += -mcpu=cortex-m4 -mthumb

#链接器附加选项
PLT_SDK_LDFLAGS += -mcpu=cortex-m4 -mthumb
PLT_SDK_LDFLAGS += --specs=nosys.specs

#启动文件
PLT_SDK_SRCS += $(PLT_SDK_DIR)startup_gd32f407_427.s