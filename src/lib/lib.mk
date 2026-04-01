LIB_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

#哈希算法
LIB_SRCS += $(LIB_DIR)hash/md5.c
LIB_SRCS += $(LIB_DIR)hash/nano_hash.c

#数学算法
LIB_SRCS += $(LIB_DIR)math/low_pass_filter.c
LIB_SRCS += $(LIB_DIR)math/high_pass_filter.c