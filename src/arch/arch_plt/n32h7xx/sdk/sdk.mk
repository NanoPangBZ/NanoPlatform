SDK_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# 头文件包含路径（布局对齐 Nations firmware：CMSIS/core、CMSIS/device、std_periph/inc）
SDK_INC_DIRS += -I$(SDK_DIR)
SDK_INC_DIRS += -I$(SDK_DIR)CMSIS/core
SDK_INC_DIRS += -I$(SDK_DIR)CMSIS/device
SDK_INC_DIRS += -I$(SDK_DIR)n32h7xx_std_periph_driver/inc

# 与官方 template.mk 中 CM7 分支一致；仅源码编译标准外设，不链接 Nations 提供的 algo 等 .lib（USELIB=0）
SDK_CFLAGS += -DCORE_CM7
SDK_CFLAGS += -DN32H78x
SDK_CFLAGS += -DUSE_STDPERIPH_DRIVER
SDK_CFLAGS += -D__ICACHE_PRESENT=1
SDK_CFLAGS += -D__DCACHE_PRESENT=1

# CMSIS-Core
SDK_SRCS += $(SDK_DIR)CMSIS/device/system_n32h7xx.c

# 启动文件（GCC，CM7；对应 template.mk 中 startup_n32h78x_cm7_gcc.s）
SDK_SRCS += $(SDK_DIR)CMSIS/device/startup/startup_n32h78x_cm7_gcc.s

# N32H7xx 标准外设库（wildcard 源码，不使用 SDK 预编译库）
SDK_SRCS += $(wildcard $(SDK_DIR)n32h7xx_std_periph_driver/src/*.c)
