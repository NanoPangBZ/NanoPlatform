TARGET_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
SRC_DIR := $(abspath $(TARGET_DIR)/../..)

# Use ARM GCC cross toolchain
CROSS_COMPILE ?= arm-none-eabi-
CC := $(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++
AR := $(CROSS_COMPILE)ar
OBJCOPY := $(CROSS_COMPILE)objcopy
SIZE := $(CROSS_COMPILE)size

# 加入对应平台SDK的Makefile
include $(SRC_DIR)/plt_sdk/gd32f4xx/plt_sdk.mk

# Add current target directory to header search path
TARGET_INC_DIRS += -I$(TARGET_DIR)

# Add platform SDK include paths and sources
TARGET_INC_DIRS += $(PLT_SDK_INC_DIRS)
TARGET_SRCS += $(PLT_SDK_SRCS)

# 全局宏定义
TARGET_CFLAGS += -DGD32F427

# Use the target-local linker script; nosys.specs provides _exit stub for bare-metal
LDFLAGS += -T$(TARGET_DIR)target.ld --specs=nosys.specs
