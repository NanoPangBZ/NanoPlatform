NKV_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# nkv 模块的源文件
NKV_SRCS += $(NKV_DIR)nkv.c
