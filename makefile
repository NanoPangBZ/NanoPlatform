ROOT_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
PROJECT_SRC_DIR := $(ROOT_DIR)src/

TARGET ?= develop
TARGET_DIR := $(PROJECT_SRC_DIR)target/$(TARGET)
TARGET_MK := $(TARGET_DIR)/target.mk
AVAILABLE_TARGETS := $(sort $(patsubst $(PROJECT_SRC_DIR)target/%/target.mk,%,$(wildcard $(PROJECT_SRC_DIR)target/*/target.mk)))

ifeq ($(wildcard $(TARGET_MK)),)
$(error Invalid target '$(TARGET)'. Available targets: $(AVAILABLE_TARGETS))
endif

include $(TARGET_MK)
include $(PROJECT_SRC_DIR)framework/frmwk.mk

BUILD_DIR ?= $(ROOT_DIR)build/$(TARGET)
OBJ_DIR := $(BUILD_DIR)/obj
APP := $(BUILD_DIR)/$(TARGET).elf
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

SRCS := $(MAIN_SRCS) $(FRMWK_SRCS) $(TARGET_SRCS)
OBJS_C := $(patsubst $(PROJECT_SRC_DIR)%.c,$(OBJ_DIR)/%.o,$(filter %.c,$(SRCS)))
OBJS_S_CAP := $(patsubst $(PROJECT_SRC_DIR)%.S,$(OBJ_DIR)/%.o,$(filter %.S,$(SRCS)))
OBJS_S_LOW := $(patsubst $(PROJECT_SRC_DIR)%.s,$(OBJ_DIR)/%.o,$(filter %.s,$(SRCS)))
OBJS := $(OBJS_C) $(OBJS_S_CAP) $(OBJS_S_LOW)
TOTAL_OBJS := $(words $(OBJS))

CFLAGS ?= -O0 -g3 -Wall -Wextra -std=c11
CFLAGS += -I$(PROJECT_SRC_DIR) $(FRMWK_INC_DIRS) $(TARGET_INC_DIRS)
CFLAGS += $(TARGET_CFLAGS)

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
		printf '%b\n' "$(COLOR_ERROR)[ERROR]$(COLOR_RESET) 编译失败: $(call REL_PATH,$<)"; \
		rm -f "$$tmp_log"; \
		exit 1; \
	fi
endef

.PHONY: all clean print-target prepare

all: $(APP)

prepare:
	@$(call MKDIR_P,$(BUILD_DIR))
	@: > "$(BUILD_LOG)"
	@date +%s > "$(BUILD_TIME_FILE)"

print-target:
	@echo TARGET=$(TARGET)
	@echo TARGET_MK=$(TARGET_MK)


$(APP): $(OBJS) | prepare
	@$(call MKDIR_P,$(dir $@))
	@tmp_log=$$(mktemp "$(BUILD_DIR)/.link.XXXXXX"); \
	printf '%b\n' "$(COLOR_INFO)[INFO]$(COLOR_RESET) 链接目标: $(APP)"; \
	if $(CC) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $@ >"$$tmp_log" 2>&1; then \
		cat "$$tmp_log" >> "$(BUILD_LOG)"; \
		$(PRINT_TOOL_OUTPUT); \
		warnings=$$(grep -Eic 'warning:' "$(BUILD_LOG)" || true); \
		errors=$$(grep -Eic 'error:|fatal error:|dangerous relocation|undefined reference' "$(BUILD_LOG)" || true); \
		start_time=$$(cat "$(BUILD_TIME_FILE)" 2>/dev/null || date +%s); \
		end_time=$$(date +%s); \
		elapsed=$$((end_time - start_time)); \
		minutes=$$((elapsed / 60)); \
		seconds=$$((elapsed % 60)); \
		printf '%b\n' "$(COLOR_SUCCESS)[SUCCESS]$(COLOR_RESET) 构建完成"; \
		printf '%b\n' "$(COLOR_INFO)[INFO]$(COLOR_RESET) 警告: $$warnings, 错误: $$errors, 耗时: $$(printf '%02d:%02d' $$minutes $$seconds)"; \
		printf '%b\n' "$(COLOR_INFO)[INFO]$(COLOR_RESET) 目标路径: $(APP)"; \
		rm -f "$$tmp_log"; \
	else \
		cat "$$tmp_log" >> "$(BUILD_LOG)"; \
		$(PRINT_TOOL_OUTPUT); \
		printf '%b\n' "$(COLOR_ERROR)[ERROR]$(COLOR_RESET) 链接失败: $(APP)"; \
		rm -f "$$tmp_log"; \
		exit 1; \
	fi

$(OBJ_DIR)/%.o: $(PROJECT_SRC_DIR)%.c | prepare
	$(call BUILD_OBJECT_RULE,$(CC) $(CFLAGS) -c $< -o $@)

$(OBJ_DIR)/%.o: $(PROJECT_SRC_DIR)%.S | prepare
	$(call BUILD_OBJECT_RULE,$(CC) $(CFLAGS) -c $< -o $@)

$(OBJ_DIR)/%.o: $(PROJECT_SRC_DIR)%.s | prepare
	$(call BUILD_OBJECT_RULE,$(CC) $(CFLAGS) -c $< -o $@)

clean:
	@$(call RMDIR_R,$(BUILD_DIR))
