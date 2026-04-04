ARCH_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

ARCH_IMPL_INC_DIRS += -I$(ARCH_DIR)

ARCH_IMPL_SRCS += $(ARCH_DIR)private/clock.c
ARCH_IMPL_SRCS += $(ARCH_DIR)private/delay.c

ARCH_IMPL_SRCS += $(ARCH_DIR)arch_init.c

# N32H787_CORE / DCORE 在 sdk/sdk.mk 中解析；此处仅消费 N32H787_CORE
ifeq ($(N32H787_CORE),CM4)
ARCH_IMPL_CFLAGS += -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
ARCH_IMPL_LDFLAGS += -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
ARCH_IMPL_SRCS += $(SDK_DIR)CMSIS/device/startup/startup_n32h78x_cm4_gcc.s
else
# 与 template.mk 中 CM7 + FPU 用法一致（fpv5-d16 hard）
ARCH_IMPL_CFLAGS += -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard
ARCH_IMPL_LDFLAGS += -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard
ARCH_IMPL_SRCS += $(SDK_DIR)CMSIS/device/startup/startup_n32h78x_cm7_gcc.s
ARCH_IMPL_CFLAGS += -D__ICACHE_PRESENT=1 -D__DCACHE_PRESENT=1
endif

ARCH_IMPL_LDFLAGS += --specs=nosys.specs
