FRMWK_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

#头文件包含路径
FRMWK_INC_DIRS += -I$(FRMWK_DIR)

#框架源文件
FRMWK_SRCS += $(FRMWK_DIR)core/nano_heap.c
FRMWK_SRCS += $(FRMWK_DIR)core/nano_list.c
FRMWK_SRCS += $(FRMWK_DIR)core/nano_ringbuf.c
FRMWK_SRCS += $(FRMWK_DIR)core/nano_polling_pool.c
FRMWK_SRCS += $(FRMWK_DIR)core/nano_framework_core.c
FRMWK_SRCS += $(FRMWK_DIR)core/nano_function_gruop.c
FRMWK_SRCS += $(FRMWK_DIR)core/nano_net_host.c
FRMWK_SRCS += $(FRMWK_DIR)core/nano_msg.c
FRMWK_SRCS += $(FRMWK_DIR)core/nano_core_sched.c

# 加入nano_net模块
include $(FRMWK_DIR)core/nano_net/nano_net.mk
FRMWK_SRCS += $(NANO_NET_SRCS)

# 加入nkv模块
include $(FRMWK_DIR)core/nkv/nkv.mk
FRMWK_SRCS += $(NKV_SRCS)
