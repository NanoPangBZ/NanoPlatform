ARCH_WEAK_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# 由 arch/arch.mk 统一并入 ARCH_PLT_SRC
ARCH_WEAK_SRCS += $(ARCH_WEAK_DIR)libc_weak.c
ARCH_WEAK_SRCS += $(ARCH_WEAK_DIR)arch_delay_weak.c
ARCH_WEAK_SRCS += $(ARCH_WEAK_DIR)arch_init_weak.c
ARCH_WEAK_SRCS += $(ARCH_WEAK_DIR)arch_uart_weak.c
ARCH_WEAK_SRCS += $(ARCH_WEAK_DIR)arch_gpio_weak.c
ARCH_WEAK_SRCS += $(ARCH_WEAK_DIR)arch_adc_weak.c
ARCH_WEAK_SRCS += $(ARCH_WEAK_DIR)arch_exti_weak.c
ARCH_WEAK_SRCS += $(ARCH_WEAK_DIR)arch_spi_weak.c
ARCH_WEAK_SRCS += $(ARCH_WEAK_DIR)arch_i2c_weak.c
ARCH_WEAK_SRCS += $(ARCH_WEAK_DIR)arch_lp_weak.c
ARCH_WEAK_SRCS += $(ARCH_WEAK_DIR)arch_cpu_weak.c
ARCH_WEAK_SRCS += $(ARCH_WEAK_DIR)arch_flash_weak.c
