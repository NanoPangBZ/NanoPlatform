LIB_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

#哈希算法
LIB_SRCS += $(LIB_DIR)hash/md5.c
LIB_SRCS += $(LIB_DIR)hash/nano_hash.c

#数学算法
#1、滤波器
LIB_SRCS += $(LIB_DIR)math/low_pass_filter.c
LIB_SRCS += $(LIB_DIR)math/high_pass_filter.c
#2、曲线表
LIB_SRCS += $(LIB_DIR)math/float_curve_table.c
LIB_SRCS += $(LIB_DIR)math/int8_curve_table.c
#3、快速三角
LIB_SRCS += $(LIB_DIR)math/fast_sin.c
LIB_SRCS += $(LIB_DIR)math/int16_curve_table.c
