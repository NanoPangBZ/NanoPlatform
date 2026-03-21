TARGET_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
SRC_DIR := $(abspath $(TARGET_DIR)/../..)

# Use ARM GCC cross toolchain
CROSS_COMPILE ?= arm-none-eabi-
CC := $(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++
AR := $(CROSS_COMPILE)ar
OBJCOPY := $(CROSS_COMPILE)objcopy
SIZE := $(CROSS_COMPILE)size

# Add current target directory to header search path
TARGET_INC_DIRS += -I$(TARGET_DIR)

# Include GD32F4 platform SDK build definitions
include $(SRC_DIR)/plt_sdk/gd32f4xx/plt_sdk.mk
