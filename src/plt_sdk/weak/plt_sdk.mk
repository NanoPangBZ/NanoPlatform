PLT_WEAK_SDK_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

PLT_SDK_SRCS += $(PLT_WEAK_SDK_DIR)arch_delay_weak.c
PLT_SDK_SRCS += $(PLT_WEAK_SDK_DIR)arch_init_weak.c
PLT_SDK_SRCS += $(PLT_WEAK_SDK_DIR)arch_uart_weak.c
PLT_SDK_SRCS += $(PLT_WEAK_SDK_DIR)arch_gpio_weak.c
PLT_SDK_SRCS += $(PLT_WEAK_SDK_DIR)arch_spi_weak.c
PLT_SDK_SRCS += $(PLT_WEAK_SDK_DIR)arch_cpu_weak.c
