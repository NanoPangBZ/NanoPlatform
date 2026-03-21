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

BUILD_DIR ?= $(ROOT_DIR)build/$(TARGET)
OBJ_DIR := $(BUILD_DIR)/obj
APP := $(BUILD_DIR)/nano_platform.elf

CORE_SRCS := \
	$(PROJECT_SRC_DIR)main.c \
	$(PROJECT_SRC_DIR)framework/impl/nano_framework_core.c \
	$(PROJECT_SRC_DIR)arch/gd32f4/arch_cpu.c

SDK_SRCS += $(PLT_SDK_SRCS)
SRCS := $(CORE_SRCS) $(SDK_SRCS)
OBJS := $(patsubst $(PROJECT_SRC_DIR)%.c,$(OBJ_DIR)/%.o,$(SRCS))

CFLAGS ?= -O0 -g3 -Wall -Wextra -std=c11
CFLAGS += -I$(PROJECT_SRC_DIR)
CFLAGS += $(TARGET_INC_DIRS)

LDFLAGS ?=
LDLIBS ?=

ifeq ($(OS),Windows_NT)
MKDIR_P = if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))"
RMDIR_R = if exist "$(subst /,\,$(1))" rmdir /S /Q "$(subst /,\,$(1))"
else
MKDIR_P = mkdir -p "$(1)"
RMDIR_R = rm -rf "$(1)"
endif

.PHONY: all clean print-target

all: $(APP)

print-target:
	@echo TARGET=$(TARGET)
	@echo TARGET_MK=$(TARGET_MK)

$(APP): $(OBJS)
	@$(call MKDIR_P,$(dir $@))
	$(CC) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(PROJECT_SRC_DIR)%.c
	@$(call MKDIR_P,$(dir $@))
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@$(call RMDIR_R,$(BUILD_DIR))
