ARCH_PLT_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# SDK（CMSIS + 标准外设库，与 Nations template.mk 源路径语义一致）
include $(ARCH_PLT_DIR)/sdk/sdk.mk
# 启动文件、CPU/FPU 选项
include $(ARCH_PLT_DIR)/arch_impl/arch.mk

ARCH_PLT_SRC += $(SDK_SRCS) $(ARCH_IMPL_SRCS)
ARCH_PLT_INC_DIRS += $(SDK_INC_DIRS) $(ARCH_IMPL_INC_DIRS)
ARCH_PLT_CFLAGS += $(SDK_CFLAGS) $(ARCH_IMPL_CFLAGS)
ARCH_PLT_LDFLAGS += $(SDK_LDFLAGS) $(ARCH_IMPL_LDFLAGS)
