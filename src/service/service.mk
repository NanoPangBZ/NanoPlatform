SERVICE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

#服务源文件
SERVICE_SRCS += $(SERVICE_DIR)led_tick/led_tick_service.c
