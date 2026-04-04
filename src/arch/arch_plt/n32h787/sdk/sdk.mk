SDK_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# 与官方 template.mk 中 DCORE 对齐：DCORE=CM4 选 CM4，否则默认 CM7
ifeq ($(DCORE),CM4)
N32H787_CORE := CM4
else ifeq ($(DCORE),CM7)
N32H787_CORE := CM7
else
N32H787_CORE ?= CM7
endif

# 头文件（对应 template 中 CMSIS core/device 与 std_periph_driver/inc）
SDK_INC_DIRS += -I$(SDK_DIR)CMSIS/core
SDK_INC_DIRS += -I$(SDK_DIR)CMSIS/device
SDK_INC_DIRS += -I$(SDK_DIR)n32h7xx_std_periph_driver/inc

# CMSIS 系统初始化
SDK_SRCS += $(SDK_DIR)CMSIS/device/system_n32h7xx.c

# 标准外设驱动（template 中 firmware/n32h7xx_std_periph_driver/src）
SDK_SRCS += $(wildcard $(SDK_DIR)n32h7xx_std_periph_driver/src/*.c)
#排除xspi_v1
SDK_SRCS := $(filter-out $(SDK_DIR)n32h7xx_std_periph_driver/src/n32h7xx_xspi.c, $(SDK_SRCS))

# 与 template.mk 中 DEFS 一致
SDK_CFLAGS += -DN32H78x
SDK_CFLAGS += -DUSE_STDPERIPH_DRIVER
ifeq ($(N32H787_CORE),CM4)
SDK_CFLAGS += -DCORE_CM4
else
SDK_CFLAGS += -DCORE_CM7
endif
