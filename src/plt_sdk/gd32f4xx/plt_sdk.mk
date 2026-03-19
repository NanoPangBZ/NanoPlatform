PLT_SDK_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

PLT_SDK_SRCS := $(wildcard $(PLT_SDK_DIR)src/*.c)
