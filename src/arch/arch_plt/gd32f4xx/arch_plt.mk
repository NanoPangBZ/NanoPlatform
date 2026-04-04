ARCH_PLT_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# sdk
include $(ARCH_PLT_DIR)/sdk/sdk.mk
# arch实现
include $(ARCH_PLT_DIR)/arch_impl/arch.mk

ARCH_PLT_SRC += $(SDK_SRCS) $(ARCH_IMPL_SRCS)
ARCH_PLT_INC_DIRS += $(SDK_INC_DIRS) $(ARCH_IMPL_INC_DIRS) 
ARCH_PLT_CFLAGS += $(SDK_CFLAGS) $(ARCH_IMPL_CFLAGS)
ARCH_PLT_LDFLAGS += $(SDK_LDFLAGS) $(ARCH_IMPL_LDFLAGS)
