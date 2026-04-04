ARCH_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

#头文件包含路径
ARCH_IMPL_INC_DIRS += -I$(ARCH_DIR)

#GD32F4xx arch抽象层实现
ARCH_IMPL_SRCS += $(ARCH_DIR)arch_init.c
ARCH_IMPL_SRCS += $(ARCH_DIR)arch_uart.c
ARCH_IMPL_SRCS += $(ARCH_DIR)arch_delay.c
ARCH_IMPL_SRCS += $(ARCH_DIR)arch_gpio.c

#GD32F4xx libc
ARCH_IMPL_SRCS += $(ARCH_DIR)gd32f4xx_libc_file.c

#编译器附加选项
ARCH_IMPL_CFLAGS += -mcpu=cortex-m4 -mthumb

#链接器附加选项
ARCH_IMPL_LDFLAGS += -mcpu=cortex-m4 -mthumb
ARCH_IMPL_LDFLAGS += --specs=nosys.specs

#启动文件
ARCH_IMPL_SRCS += $(ARCH_DIR)startup_gd32f407_427.s