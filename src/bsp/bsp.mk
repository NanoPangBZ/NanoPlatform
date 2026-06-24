BSP_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

#bsp源文件
BSP_SRCS += $(BSP_DIR)rgb_light_bar/bsp_rgb_light_bar.c
