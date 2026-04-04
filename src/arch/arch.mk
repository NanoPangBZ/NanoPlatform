ARCH_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# 弱符号实现
include $(ARCH_DIR)/weak/arch_weak.mk
# 对应平台的arch实现
include $(ARCH_DIR)/arch_plt/arch_plt.mk

# 平台 arch_plt.mk 须写入 ARCH_PLT_SRC（及 *_INC_DIRS / *_CFLAGS / *_LDFLAGS），勿使用 ARCH_PLT_SRCS
ARCH_SRC += $(ARCH_WEAK_SRCS) $(ARCH_PLT_SRC)
ARCH_INC_DIRS += $(ARCH_PLT_INC_DIRS)
ARCH_CFLAGS += $(ARCH_PLT_CFLAGS)
ARCH_LDFLAGS += $(ARCH_PLT_LDFLAGS)

