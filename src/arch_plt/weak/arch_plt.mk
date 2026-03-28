ARCH_PLT_WEAK_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

ARCH_PLT_SRC += $(ARCH_PLT_WEAK_DIR)libc_weak.c
ARCH_PLT_SRC += $(ARCH_PLT_WEAK_DIR)arch_delay_weak.c
ARCH_PLT_SRC += $(ARCH_PLT_WEAK_DIR)arch_init_weak.c
ARCH_PLT_SRC += $(ARCH_PLT_WEAK_DIR)arch_uart_weak.c
ARCH_PLT_SRC += $(ARCH_PLT_WEAK_DIR)arch_gpio_weak.c
ARCH_PLT_SRC += $(ARCH_PLT_WEAK_DIR)arch_spi_weak.c
ARCH_PLT_SRC += $(ARCH_PLT_WEAK_DIR)arch_cpu_weak.c
ARCH_PLT_SRC += $(ARCH_PLT_WEAK_DIR)arch_flash_weak.c
