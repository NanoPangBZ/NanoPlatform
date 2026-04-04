#*********************************************************************************************************
#    Copyright (c) 2025, Nations Technologies Inc.
# 
#     All rights reserved.
#
#     This software is the exclusive property of Nations Technologies Inc. (Hereinafter 
# referred to as NATIONS). This software, and the product of NATIONS described herein 
# (Hereinafter referred to as the Product) are owned by NATIONS under the laws and treaties
# of the People's Republic of China and other applicable jurisdictions worldwide.
#
#     NATIONS does not grant any license under its patents, copyrights, trademarks, or other 
# intellectual property rights. Names and brands of third party may be mentioned or referred 
# thereto (if any) for identification purposes only.
#
#     NATIONS reserves the right to make changes, corrections, enhancements, modifications, and 
# improvements to this software at any time without notice. Please contact NATIONS and obtain 
# the latest version of this software before placing orders.
#
#     Although NATIONS has attempted to provide accurate and reliable information, NATIONS assumes 
# no responsibility for the accuracy and reliability of this software.
# 
#     It is the responsibility of the user of this software to properly design, program, and test 
# the functionality and safety of any application made of this information and any resulting product. 
# In no event shall NATIONS be liable for any direct, indirect, incidental, special,exemplary, or 
# consequential damages arising in any way out of the use of this software or the Product.
#
#     NATIONS Products are neither intended nor warranted for usage in systems or equipment, any
# malfunction or failure of which may cause loss of human life, bodily injury or severe property 
# damage. Such applications are deemed, "Insecure Usage".
#
#     All Insecure Usage shall be made at user's risk. User shall indemnify NATIONS and hold NATIONS 
# harmless from and against all claims, costs, damages, and other liabilities, arising from or related 
# to any customer's Insecure Usage.
#
#     Any express or implied warranty with regard to this software or the Product, including,but not 
# limited to, the warranties of merchantability, fitness for a particular purpose and non-infringement
# are disclaimed to the fullest extent permitted by law.
#
#     Unless otherwise explicitly permitted by NATIONS, anyone may not duplicate, modify, transcribe
# or otherwise distribute this software for any purposes, in whole or in part.
#
#     NATIONS products and technologies shall not be used for or incorporated into any products or systems
# whose manufacture, use, or sale is prohibited under any applicable domestic or foreign laws or regulations. 
# User shall comply with any applicable export control laws and regulations promulgated and administered by 
# the governments of any countries asserting jurisdiction over the parties or transactions.
# ************************************************************************************************************/

######################################
# target
######################################
ifeq ($(DCORE), CM4)
TARGET = output_CM4
else
TARGET = output_CM7
endif

######################################
# building variables
######################################
# debug build
ifeq ($(release), y)
DEBUG = 0
else
DEBUG = 1
endif
# optimization
OPT = -O0

#######################################
# Build path
#######################################
# BUILD_DIR = build

ifeq ($(DCORE), CM4)
BUILD_DIR = build_CM4
else
BUILD_DIR = build_CM7
endif

######################################
# chip platform info
######################################
TARGET_PLATFORM := n32h7xx
ifeq ($(DCORE), CM4)
TARGET_STARTUP  := n32h78x_cm4
DEFS += -DCORE_CM4
else
TARGET_STARTUP  := n32h78x_cm7
DEFS += -DCORE_CM7
endif
DEFS += -DN32H78x
DEFS += -DUSE_STDPERIPH_DRIVER

######################################
# Algo libs
######################################
USELIB = 0

######################################
#TOOLS CHAIN
######################################
CROSS_COMPILE = arm-none-eabi-

######################################
# C sources
######################################
ifeq ($(DCORE), CM4)
C_DIRS += ../CM4/src
else
C_DIRS += ../CM7/src
endif
C_DIRS += ../../../bsp/src
C_DIRS += ../../../../../firmware/CMSIS/device
C_DIRS += ../../../../../firmware/$(TARGET_PLATFORM)_std_periph_driver/src
SRC_OBJS_DIRS += $(foreach DIR, $(C_DIRS), $(wildcard $(DIR)/*.c))
C_SOURCES = $(SRC_OBJS_DIRS)

######################################
# ASM sources
######################################
ASM_SOURCES = ../../../../../firmware/CMSIS/device/startup/startup_$(TARGET_STARTUP)_gcc.s

######################################
# C includes
######################################
ifeq ($(DCORE), CM4)
C_INCS += ../CM4/inc
else
C_INCS += ../CM7/inc
endif
C_INCS += ../../../bsp/inc
C_INCS += ../../../../../firmware/CMSIS/core
C_INCS += ../../../../../firmware/CMSIS/device
C_INCS += ../../../../../firmware/$(TARGET_PLATFORM)_std_periph_driver/inc
INCS_OBJS_DIR = $(foreach DIR2, $(C_INCS), $(wildcard $(DIR2)/*.h))
INCS_OBJS_PATH = $(sort $(dir $(INCS_OBJS_DIR)))
C_INCLUDES = $(addprefix -I,$(INCS_OBJS_PATH))

######################################
# Lib files
######################################
ifeq ($(USELIB), 1)
SRC_LIB_DIR += ../../../../../firmware/$(TARGET_PLATFORM)_algo_lib/src
INC_LIB_DIR += ../../../../../firmware/$(TARGET_PLATFORM)_algo_lib/inc
LIB_SOURCES = $(foreach DIR3, $(SRC_LIB_DIR), $(wildcard $(DIR3)/*.lib))
LIB_SOURCES_L = $(addprefix -L,$(LIB_SOURCES))
C_LIBS = $(LIB_SOURCES_L)
LIB_INCS = $(foreach DIR4, $(INC_LIB_DIR), $(wildcard $(DIR4)/*.h))
LIB_INCS_PATH = $(sort $(dir $(LIB_INCS)))
LIB_INCS_PATH_I = $(addprefix -I,$(LIB_INCS_PATH))
C_INCLUDES += $(LIB_INCS_PATH_I)
endif

######################################
# Compile & Link flags
######################################
# cpu
ifeq ($(DCORE), CM4)
CPU = -mcpu=cortex-m4
else
CPU = -mcpu=cortex-m7
endif
# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

#CFLAGS
CFLAGS += $(MCU) -Wall
CFLAGS += $(OPT)
CFLAGS += -ffunction-sections -fdata-sections
#DEBUG
ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif
# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"
#LFLAGS
LFLAGS += $(MCU)
LFLAGS += -Wl,--gc-sections 
LFLAGS += --specs=nosys.specs
LFLAGS += -Xlinker -Map=$(BUILD_DIR)/$(TARGET).map
# link script
ifeq ($(DCORE), CM4)
LDSCRIPT = ../../../../../firmware/CMSIS/device/$(TARGET_STARTUP)_flash.ld
else
LDSCRIPT = ../../../../../firmware/CMSIS/device/$(TARGET_STARTUP)_flash.ld
endif

######################################
# Objects
######################################
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CROSS_COMPILE)gcc $(CFLAGS) $(DEFS) $(C_INCLUDES) $(C_LIBS) -c -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@
    
$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(CROSS_COMPILE)gcc $(CFLAGS) -c $< -o $@
    
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CROSS_COMPILE)gcc $(OBJECTS) $(LFLAGS) -T$(LDSCRIPT) -o $@
	$(CROSS_COMPILE)size $@
	
$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(CROSS_COMPILE)objcopy -O binary -S $< $@

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(CROSS_COMPILE)objcopy -O ihex -S $< $@
    
$(BUILD_DIR):
	@-mkdir $@

SHELL=cmd.exe
#######################################
# clean up
#######################################
clean:
	del $(BUILD_DIR)
#	@-rm -rf $(BUILD_DIR)

#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

#######################################
# download .hex/.bin by jlink
#######################################
#Your JLink installation directory
PATH_WINPC = 'C:/Program Files (x86)/SEGGER/JLink_V640/'
#PATH_LINUX = /opt/SEGGER/JLink_V640b/JLinkExe
JK_DPATH = $(PATH_WINPC)
#Jlink script store directory
JKS_DIR = ../../../../../jlink
#Chip type
ifeq ($(DCORE), CM4)
CHIP_TYPE = N32H785xIx7:CM4
else
CHIP_TYPE = N32H785xIx7:CM7
endif

download:
ifeq ($(DCORE), CM4)
	@$(JK_DPATH)JLink.exe -device $(CHIP_TYPE) -if SWD -speed 4000 -autoconnect 1 -CommanderScript $(JKS_DIR)/flash_CM4.jlink
else
	@$(JK_DPATH)JLink.exe -device $(CHIP_TYPE) -if SWD -speed 4000 -autoconnect 1 -CommanderScript $(JKS_DIR)/flash_CM7.jlink
endif
	@echo "Download Completed!"

debug:
	@$(JK_DPATH)JLinkGDBServer.exe -select USB -device $(CHIP_TYPE) -if SWD -speed auto -noir -LocalhostOnly

# *** EOF ***
