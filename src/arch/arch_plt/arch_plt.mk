# 根据 target 中设置的 ARCH_PLT 引入对应芯片/模拟器目录下的 arch_plt.mk
_ARCH_PLT_PICK_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

ifndef ARCH_PLT
$(error ARCH_PLT is not set. Define it in target.mk before including arch/arch.mk, e.g. ARCH_PLT := gd32f4xx)
endif

ARCH_PLT_TOP_MK := $(_ARCH_PLT_PICK_DIR)$(ARCH_PLT)/arch_plt.mk

# 检查对应平台的 arch_plt.mk 是否存在
ifeq ($(wildcard $(ARCH_PLT_TOP_MK)),)
_ARCH_PLT_AVAILABLE := $(sort $(notdir $(patsubst %/,%,$(dir $(wildcard $(_ARCH_PLT_PICK_DIR)*/arch_plt.mk)))))
$(error Unknown ARCH_PLT='$(ARCH_PLT)': missing $(ARCH_PLT_TOP_MK). Available: $(_ARCH_PLT_AVAILABLE))
endif

# 引入对应平台的 arch_plt.mk
include $(ARCH_PLT_TOP_MK)

