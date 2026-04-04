ROOT_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
PROJECT_SRC_DIR := $(ROOT_DIR)src/

#是否启用单元测试
UNIT_TEST ?= 0

#选择是否debug构建
DEBUG ?= 0

TARGET ?= develop
TARGET_DIR := $(PROJECT_SRC_DIR)target/$(TARGET)
TARGET_MK := $(TARGET_DIR)/target.mk
AVAILABLE_TARGETS := $(sort $(patsubst $(PROJECT_SRC_DIR)target/%/target.mk,%,$(wildcard $(PROJECT_SRC_DIR)target/*/target.mk)))

ifeq ($(wildcard $(TARGET_MK)),)
$(error Invalid target '$(TARGET)'. Available targets: $(AVAILABLE_TARGETS))
endif

include $(TARGET_MK)
include $(PROJECT_SRC_DIR)framework/frmwk.mk
include $(PROJECT_SRC_DIR)lib/lib.mk

BUILD_DIR ?= $(ROOT_DIR)build/$(TARGET)
OBJ_DIR := $(BUILD_DIR)/obj
APP := $(BUILD_DIR)/$(TARGET).elf
APP_BIN := $(BUILD_DIR)/$(TARGET).bin
APP_HEX := $(BUILD_DIR)/$(TARGET).hex
APP_MAP := $(BUILD_DIR)/$(TARGET).map
BUILD_LOG := $(BUILD_DIR)/.build.log
BUILD_TIME_FILE := $(BUILD_DIR)/.build.start

ESC := \033
COLOR_INFO := $(ESC)[1;34m
COLOR_WARN := $(ESC)[1;33m
COLOR_ERROR := $(ESC)[1;31m
COLOR_SUCCESS := $(ESC)[1;32m
COLOR_RESET := $(ESC)[0m

MAIN_SRCS := \
	$(PROJECT_SRC_DIR)main.c

SRCS := $(TARGET_SRCS) $(FRMWK_SRCS) $(LIB_SRCS) $(MAIN_SRCS)
OBJS_C := $(patsubst $(PROJECT_SRC_DIR)%.c,$(OBJ_DIR)/%.o,$(filter %.c,$(SRCS)))
OBJS_S_CAP := $(patsubst $(PROJECT_SRC_DIR)%.S,$(OBJ_DIR)/%.o,$(filter %.S,$(SRCS)))
OBJS_S_LOW := $(patsubst $(PROJECT_SRC_DIR)%.s,$(OBJ_DIR)/%.o,$(filter %.s,$(SRCS)))
OBJS := $(OBJS_C) $(OBJS_S_CAP) $(OBJS_S_LOW)
TOTAL_OBJS := $(words $(OBJS))

ifeq ($(UNIT_TEST),1)
include $(PROJECT_SRC_DIR)test/test.mk
OBJS += $(patsubst $(PROJECT_SRC_DIR)%.c,$(OBJ_DIR)/%.o,$(filter %.c,$(TEST_SRCS)))
endif

DEPS := $(OBJS:%.o=%.d)

ifeq ($(DEBUG),1)
CFLAGS ?= -O0 -g3 -Wall -Wextra -std=c11
else
CFLAGS ?= -Os -g3 -Wall -Wextra -std=c11
endif

CFLAGS += -I$(PROJECT_SRC_DIR) $(FRMWK_INC_DIRS) $(TARGET_INC_DIRS)
CFLAGS += $(TARGET_CFLAGS)
# 生成 .d 依赖（用户头文件），头文件变更时自动重编对应 .o
CFLAGS += -MMD -MP

LDFLAGS ?=
LDLIBS ?=

ifeq ($(OS),Windows_NT)
MKDIR_P = if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))"
RMDIR_R = if exist "$(subst /,\,$(1))" rmdir /S /Q "$(subst /,\,$(1))"
else
MKDIR_P = mkdir -p "$(1)"
RMDIR_R = rm -rf "$(1)"
endif

REL_PATH = $(patsubst $(ROOT_DIR)%,%,$(1))

ifeq ($(OS),Windows_NT)
define BUILD_OBJECT_RULE
	@$(call MKDIR_P,$(dir $@))
	@echo [INFO] [$(call REL_PATH,$<)]
	@$(1) >> "$(BUILD_LOG)" 2>&1 || (echo [ERROR] Compile failed: $(call REL_PATH,$<) & exit /b 1)
endef
else
define PRINT_TOOL_OUTPUT
if [ -s "$$tmp_log" ]; then \
	awk 'BEGIN { warn = "$(COLOR_WARN)"; err = "$(COLOR_ERROR)"; reset = "$(COLOR_RESET)" } /warning:/ { print warn $$0 reset; next } /error:|fatal error:|dangerous relocation|undefined reference/ { print err $$0 reset; next } { print $$0 }' "$$tmp_log"; \
fi
endef

define BUILD_OBJECT_RULE
	@$(call MKDIR_P,$(dir $@))
	@current=$$(awk 'BEGIN { n = split("$(strip $(OBJS))", objs, " "); for (i = 1; i <= n; ++i) if (objs[i] == "$@") { print i; exit } }'); \
	tmp_log=$$(mktemp "$(BUILD_DIR)/.obj.XXXXXX"); \
	printf '%b\n' "$(COLOR_INFO)[INFO]$(COLOR_RESET) [$$current/$(TOTAL_OBJS)] $(call REL_PATH,$<)"; \
	if $(1) >"$$tmp_log" 2>&1; then \
		cat "$$tmp_log" >> "$(BUILD_LOG)"; \
		$(PRINT_TOOL_OUTPUT); \
		rm -f "$$tmp_log"; \
	else \
		cat "$$tmp_log" >> "$(BUILD_LOG)"; \
		$(PRINT_TOOL_OUTPUT); \
		printf '%b\n' "$(COLOR_ERROR)[ERROR]$(COLOR_RESET) Compile failed: $(call REL_PATH,$<)"; \
		rm -f "$$tmp_log"; \
		exit 1; \
	fi
endef
endif

.PHONY: all clean print-target prepare always_link

all: $(APP)

# 空 phony：作为 $(APP) 的普通前提，使每次 make 都执行链接（.o 仍按依赖增量编译）
always_link:

ifeq ($(OS),Windows_NT)
prepare:
	@$(call MKDIR_P,$(BUILD_DIR))
	@type NUL > "$(BUILD_LOG)"
	@powershell -NoProfile -Command "[DateTimeOffset]::UtcNow.ToUnixTimeSeconds()" > "$(BUILD_TIME_FILE)"

$(APP): $(OBJS) always_link | prepare
	@$(call MKDIR_P,$(dir $@))
	@echo [INFO] Linking target: $(APP)
	@$(CC) $(OBJS) $(LDFLAGS) -Wl,-Map,$(APP_MAP) $(LDLIBS) -o $@ >> "$(BUILD_LOG)" 2>&1 || (echo [ERROR] Link failed: $(APP) & exit /b 1)
	@$(OBJCOPY) -O binary $@ $(APP_BIN) >> "$(BUILD_LOG)" 2>&1 || (echo [ERROR] Generate binary failed: $(APP_BIN) & exit /b 1)
	@$(OBJCOPY) -O ihex $@ $(APP_HEX) >> "$(BUILD_LOG)" 2>&1 || (echo [ERROR] Generate hex failed: $(APP_HEX) & exit /b 1)
	@echo [SUCCESS] Build completed
	@echo [INFO] Target path: $(APP)
	@echo [INFO] BIN path: $(APP_BIN)
	@echo [INFO] HEX path: $(APP_HEX)
	@echo [INFO] MAP path: $(APP_MAP)
else
prepare:
	@$(call MKDIR_P,$(BUILD_DIR))
	@: > "$(BUILD_LOG)"
	@date +%s > "$(BUILD_TIME_FILE)"

$(APP): $(OBJS) always_link | prepare
	@$(call MKDIR_P,$(dir $@))
	@tmp_log=$$(mktemp "$(BUILD_DIR)/.link.XXXXXX"); \
	printf '%b\n' "$(COLOR_INFO)[INFO]$(COLOR_RESET) Linking target: $(APP)"; \
	if $(CC) $(OBJS) $(LDFLAGS) -Wl,-Map,$(APP_MAP) $(LDLIBS) -o $@ >"$$tmp_log" 2>&1; then \
		if ! $(OBJCOPY) -O binary $@ $(APP_BIN) >>"$$tmp_log" 2>&1; then \
			cat "$$tmp_log" >> "$(BUILD_LOG)"; \
			$(PRINT_TOOL_OUTPUT); \
			printf '%b\n' "$(COLOR_ERROR)[ERROR]$(COLOR_RESET) Generate binary failed: $(APP_BIN)"; \
			rm -f "$$tmp_log"; \
			exit 1; \
		fi; \
		if ! $(OBJCOPY) -O ihex $@ $(APP_HEX) >>"$$tmp_log" 2>&1; then \
			cat "$$tmp_log" >> "$(BUILD_LOG)"; \
			$(PRINT_TOOL_OUTPUT); \
			printf '%b\n' "$(COLOR_ERROR)[ERROR]$(COLOR_RESET) Generate hex failed: $(APP_HEX)"; \
			rm -f "$$tmp_log"; \
			exit 1; \
		fi; \
		cat "$$tmp_log" >> "$(BUILD_LOG)"; \
		$(PRINT_TOOL_OUTPUT); \
		warnings=$$(grep -Eic 'warning:' "$(BUILD_LOG)" || true); \
		errors=$$(grep -Eic 'error:|fatal error:|dangerous relocation|undefined reference' "$(BUILD_LOG)" || true); \
		start_time=$$(cat "$(BUILD_TIME_FILE)" 2>/dev/null || date +%s); \
		end_time=$$(date +%s); \
		elapsed=$$((end_time - start_time)); \
		minutes=$$((elapsed / 60)); \
		seconds=$$((elapsed % 60)); \
		printf '%b\n' "$(COLOR_SUCCESS)[SUCCESS]$(COLOR_RESET) Build completed"; \
		printf '%b\n' "$(COLOR_INFO)[INFO]$(COLOR_RESET) Warnings: $$warnings, Errors: $$errors, Duration: $$(printf '%02d:%02d' $$minutes $$seconds)"; \
		printf '%b\n' "$(COLOR_INFO)[INFO]$(COLOR_RESET) Target path: $(APP)"; \
		printf '%b\n' "$(COLOR_INFO)[INFO]$(COLOR_RESET) BIN path: $(APP_BIN)"; \
		printf '%b\n' "$(COLOR_INFO)[INFO]$(COLOR_RESET) HEX path: $(APP_HEX)"; \
		printf '%b\n' "$(COLOR_INFO)[INFO]$(COLOR_RESET) MAP path: $(APP_MAP)"; \
		rm -f "$$tmp_log"; \
	else \
		cat "$$tmp_log" >> "$(BUILD_LOG)"; \
		$(PRINT_TOOL_OUTPUT); \
		printf '%b\n' "$(COLOR_ERROR)[ERROR]$(COLOR_RESET) Link failed: $(APP)"; \
		rm -f "$$tmp_log"; \
		exit 1; \
	fi
endif

print-target:
	@echo TARGET=$(TARGET)
	@echo TARGET_MK=$(TARGET_MK)

# 仅对 n32h787 厂商 SDK 路径下的目标文件附加 -w；其余源码仍保留完整告警（-Wall -Wextra 等）
$(OBJ_DIR)/arch/arch_plt/n32h787/sdk/%.o: CFLAGS += -w

$(OBJ_DIR)/%.o: $(PROJECT_SRC_DIR)%.c | prepare
	$(call BUILD_OBJECT_RULE,$(CC) $(CFLAGS) -c $< -o $@)

$(OBJ_DIR)/%.o: $(PROJECT_SRC_DIR)%.S | prepare
	$(call BUILD_OBJECT_RULE,$(CC) $(CFLAGS) -c $< -o $@)

$(OBJ_DIR)/%.o: $(PROJECT_SRC_DIR)%.s | prepare
	$(call BUILD_OBJECT_RULE,$(CC) $(CFLAGS) -c $< -o $@)

-include $(DEPS)

clean:
	@$(call RMDIR_R,$(BUILD_DIR))
