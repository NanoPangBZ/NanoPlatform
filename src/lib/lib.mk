LIB_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

#库源文件
LIB_SRCS += $(LIB_DIR)hash/md5.c
LIB_SRCS += $(LIB_DIR)hash/nano_hash.c