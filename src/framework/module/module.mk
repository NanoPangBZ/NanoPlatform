MODULE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

include $(MODULE_DIR)bsp/bsp.mk
include $(MODULE_DIR)service/service.mk

#头文件包含路径
MODULE_INC_DIRS += -I$(MODULE_DIR)

#模块源文件
MODULE_SRCS += $(BSP_SRC) $(SERVICE_SRCS)
