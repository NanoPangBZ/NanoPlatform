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
include $(SRC_DIR)/arch_plt/gd32f4xx/arch_plt.mk

# 加入arch平台的弱符号实现
include $(SRC_DIR)/arch_plt/weak/arch_plt.mk

# 收集arch平台的头文件搜索路径、源文件、编译器选项
TARGET_INC_DIRS += $(ARCH_PLT_INC_DIRS)
TARGET_SRCS += $(ARCH_PLT_SRC)
TARGET_CFLAGS += $(ARCH_PLT_CFLAGS)

# 加入目标私有的头文件搜索路径、源文件
TARGET_INC_DIRS += -I$(TARGET_DIR)cfg
TARGET_SRCS += $(TARGET_DIR)src/target_main.c
TARGET_CFLAGS += -DGD32F427

# 链接器选项
LDFLAGS += $(ARCH_PLT_LDFLAGS)
LDFLAGS += -T$(TARGET_DIR)target.ld
