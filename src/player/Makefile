PROJECT = H-MS1110

MODULE = PLAYER

STM32_MCU = F103CB

DISPLAY = SSD1322
DISPVAR = SPI

APROC_LIST = TDA7719
FEATURE_LIST =

DEBUG = 1
OPT = -Os


# Lowercase argument
lc = $(shell echo $1 | tr '[:upper:]' '[:lower:]')

ifeq "$(STM32_MCU)" "F103CB"
  STM32_FAMILY = STM32F1
  STM32_GROUP  = $(STM32_FAMILY)03xB
endif

TARGET = $(call lc, $(PROJECT)_$(MODULE))

C_DEFS += -DUSE_FULL_LL_DRIVER -D$(STM32_GROUP) -D_$(STM32_MCU)

C_DEFS += -D_PRINTF_USART1
C_DEFS += -D_MODULE_$(MODULE)
C_DEFS += -D_DISP_RST_ENABLED
C_DEFS += -D_INPUT_ANALOG
C_DEFS += -D_GLCD_OFT_Y=24

COMMON = ../common

C_SOURCES += $(call lc, $(MODULE)).c

C_SOURCES += $(COMMON)/amp.c
C_SOURCES += $(COMMON)/debug.c
C_SOURCES += $(COMMON)/eemul.c
C_SOURCES += $(COMMON)/i2c.c
C_SOURCES += $(COMMON)/input.c
C_SOURCES += $(COMMON)/main.c
C_SOURCES += $(COMMON)/mpc.c
C_SOURCES += $(COMMON)/rc.c
C_SOURCES += $(COMMON)/ringbuf.c
C_SOURCES += $(COMMON)/rtc.c
C_SOURCES += $(COMMON)/settings.c
C_SOURCES += $(COMMON)/setup.c
C_SOURCES += $(COMMON)/spi.c
C_SOURCES += $(COMMON)/swtimers.c
C_SOURCES += $(COMMON)/sync.c
C_SOURCES += $(COMMON)/timers.c
C_SOURCES += $(COMMON)/tunersync.c
C_SOURCES += $(COMMON)/usart.c
C_SOURCES += $(COMMON)/utils.c

C_SOURCES += $(COMMON)/tuner/rds/parser.c

C_SOURCES += $(wildcard $(COMMON)/tr/labels*.c)

# Input source files
C_SOURCES += $(COMMON)/input/analog.c
C_SOURCES += $(COMMON)/input/matrix.c

# Display source files
C_SOURCES += $(wildcard $(COMMON)/display/fonts/font*.c)

ifneq (,$(filter $(DISPLAY), \
  SSD1322   \
))
  DISPTYPE = gg
  DISPSIZE = 256x64
  C_DEFS += -D_DISP_FB
endif

C_SOURCES += $(COMMON)/gui/view/inputview.c
C_SOURCES += $(COMMON)/gui/view/mpcview.c
C_SOURCES += $(COMMON)/gui/view/tunerview.c

C_SOURCES += $(COMMON)/screen/setupview.c
C_SOURCES += $(COMMON)/screen/spectrumview.c
C_SOURCES += $(COMMON)/screen/timeview.c
C_SOURCES += $(COMMON)/screen/tuneview.c

C_SOURCES += $(COMMON)/display/$(DISPTYPE)$(DISPSIZE)/$(call lc,$(DISPLAY)).c
C_SOURCES += $(COMMON)/display/dispdrv.c
C_SOURCES += $(COMMON)/display/glcd.c
C_DEFS += -D_$(DISPLAY)
C_DEFS += -D_DISP_$(DISPVAR)

C_SOURCES += $(COMMON)/gui/canvas.c
C_SOURCES += $(COMMON)/gui/palette.c
C_SOURCES += $(wildcard $(COMMON)/gui/fonts/font*.c)
C_SOURCES += $(wildcard $(COMMON)/gui/widget/*.c)

C_SOURCES += $(wildcard $(COMMON)/gui/icons.c)
C_SOURCES += $(wildcard $(COMMON)/gui/icons/icon*.c)

# Audio source files
C_SOURCES += $(addprefix $(COMMON)/audio/, $(addsuffix .c, $(call lc, $(APROC_LIST))))
C_SOURCES += $(COMMON)/audio/audio.c
C_DEFS += $(addprefix -D_, $(APROC_LIST))

C_INCLUDES += \
  -I$(COMMON)/display/fonts \
  -I$(COMMON) \
  -I.

C_SOURCES += \
  $(COMMON)/drivers/$(STM32_FAMILY)xx_HAL_Driver/Src/$(call lc, $(STM32_FAMILY))xx_ll_gpio.c \
  $(COMMON)/drivers/$(STM32_FAMILY)xx_HAL_Driver/Src/$(call lc, $(STM32_FAMILY))xx_ll_i2c.c \
  $(COMMON)/drivers/$(STM32_FAMILY)xx_HAL_Driver/Src/$(call lc, $(STM32_FAMILY))xx_ll_rcc.c \
  $(COMMON)/drivers/$(STM32_FAMILY)xx_HAL_Driver/Src/$(call lc, $(STM32_FAMILY))xx_ll_rtc.c \
  $(COMMON)/drivers/$(STM32_FAMILY)xx_HAL_Driver/Src/$(call lc, $(STM32_FAMILY))xx_ll_usart.c \
  $(COMMON)/drivers/$(STM32_FAMILY)xx_HAL_Driver/Src/$(call lc, $(STM32_FAMILY))xx_ll_utils.c \
  $(COMMON)/system/system_$(call lc, $(STM32_FAMILY))xx.c

C_INCLUDES += \
  -I$(COMMON)/drivers/$(STM32_FAMILY)xx_HAL_Driver/Inc \
  -I$(COMMON)/drivers/CMSIS/Device/ST/$(STM32_FAMILY)xx/Include \
  -I$(COMMON)/drivers/CMSIS/Include \
  -I$(COMMON)/system

AS_DEFS +=

ASM_SOURCES += \
  $(COMMON)/system/startup_$(call lc, $(STM32_GROUP)).s

# Build directory
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

ifeq "$(STM32_FAMILY)" "STM32F1"
  CPU = -mcpu=cortex-m3
  FPU =
  FLOAT-ABI =
endif

ifeq "$(STM32_FAMILY)" "STM32F3"
  CPU = -mcpu=cortex-m4
  FPU = -mfpu=fpv4-sp-d16
  FLOAT-ABI = -mfloat-abi=hard
endif

# Compiler
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)
OPT_FLAGS = $(OPT) -fshort-enums -ffunction-sections -fdata-sections -ffreestanding
#OPT_FLAGS += -flto
WARN += -Wall
WARN += -Werror

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT_FLAGS) $(WARN)
ASFLAGS = $(MCU) $(AS_DEFS) $(WARN)

ifeq ($(DEBUG), 1)
  CFLAGS += -g -gdwarf-2
endif

# Dependency information
CFLAGS += -MMD -MP -MT $(OBJ_DIR)/$(*F).o -MF $(OBJ_DIR)/$(*D)/$(*F).d

LDSCRIPT = ../common/system/$(call lc, STM32$(STM32_MCU))_flash.ld
LIBS = -lc -lm -lnosys
LIBDIR =
LDFLAGS = $(MCU) -specs=nosys.specs -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections
#LDFLAGS = $(WARN) -mmcu=$(MCU) -Wl,--gc-sections -Wl,--relax

# Main definitions
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
OD = $(PREFIX)objdump
AR = $(PREFIX)ar
SZ = $(PREFIX)size

OPENOCD := openocd
OPENOCD_CFG := $(COMMON)/system/$(call lc, $(STM32_GROUP))_openocd_stlink.cfg

# .s files should be first in linking process to handle -flto properly
OBJECTS += $(addprefix $(OBJ_DIR)/,$(ASM_SOURCES:.s=.o))
OBJECTS += $(addprefix $(OBJ_DIR)/,$(C_SOURCES:.c=.o))

ELF = $(BUILD_DIR)/$(TARGET).elf
BIN = flash/$(TARGET).bin

all: $(BIN) size

$(BIN): $(ELF)
	$(CP) -O binary $(ELF) $(BIN)
	$(OD) -h -S $(ELF) > $(BUILD_DIR)/$(TARGET).lss

$(ELF): $(OBJECTS)
	@mkdir -p flash
	$(CC) $(LDFLAGS) -o $(ELF) $(OBJECTS)

size: $(ELF)
	$(SZ) $(ELF)

$(OBJ_DIR)/%.o: %.c Makefile
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $(C_DEFS) -o $@ $<

$(OBJ_DIR)/%.o: %.s Makefile
	@mkdir -p $(dir $@)
	$(AS) -c $(ASFLAGS) $(AS_DEFS) -o $@ $<

clean:
	@rm -rf $(BUILD_DIR)

.PHONY: flash
flash: $(BIN)
	$(OPENOCD) -f $(OPENOCD_CFG) -c "stm_flash $(BIN)" -c shutdown

.PHONY: erase
erase:
	$(OPENOCD) -f $(OPENOCD_CFG) -c "stm_erase" -c shutdown

# Other dependencies
-include $(OBJECTS:.o=.d)
