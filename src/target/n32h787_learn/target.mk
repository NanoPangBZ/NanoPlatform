TARGET_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
SRC_DIR := $(abspath $(TARGET_DIR)/../..)

# 工具链选择
CROSS_COMPILE ?= arm-none-eabi-
CC := $(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++
AR := $(CROSS_COMPILE)ar
OBJCOPY := $(CROSS_COMPILE)objcopy
SIZE := $(CROSS_COMPILE)size

# 选择arch平台
# ARCH_PLT := ch32f103
# ARCH_PLT := gd32f4xx
# ARCH_PLT := n32g003
ARCH_PLT := n32h787
# ARCH_PLT := rt1052
# ARCH_PLT := simulator
# ARCH_PLT := stm32f1xx
# ARCH_PLT := stm32f4xx
# ARCH_PLT := stm32g0
# ARCH_PLT := stm32h7xx
include $(SRC_DIR)/arch/arch.mk

# 收集arch平台的头文件搜索路径、源文件、编译器选项
TARGET_INC_DIRS += $(ARCH_INC_DIRS)
TARGET_SRCS += $(ARCH_SRC)
TARGET_CFLAGS += $(ARCH_CFLAGS)

# 加入目标私有的头文件搜索路径、源文件
TARGET_INC_DIRS += -I$(TARGET_DIR)cfg
TARGET_SRCS += $(TARGET_DIR)src/target_main.c
TARGET_CFLAGS += -DN32H78x
TARGET_CFLAGS += -DN32H787

# 链接器选项
LDFLAGS += $(ARCH_LDFLAGS)
LDFLAGS += -T$(TARGET_DIR)target.ld
