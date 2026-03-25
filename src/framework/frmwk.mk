FRMWK_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

#头文件包含路径
FRMWK_INC_DIRS += -I$(FRMWK_DIR)

#框架源文件
FRMWK_SRCS += $(FRMWK_DIR)impl/nano_framework_core.c
FRMWK_SRCS += $(FRMWK_DIR)impl/nano_function_gruop.c
