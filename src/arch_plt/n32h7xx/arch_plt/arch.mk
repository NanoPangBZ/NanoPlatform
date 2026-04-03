ARCH_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# N32H78x CM7：与官方 template.mk 中 MCU = -mcpu=cortex-m7 -mthumb $(FPU) 一致
ARCH_CFLAGS += -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard

ARCH_LDFLAGS += -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard
ARCH_LDFLAGS += --specs=nosys.specs
