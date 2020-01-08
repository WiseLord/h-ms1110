PROJECT = h-ms1110

DISPLAY = SSD1322
DISPVAR = SPI

# Lowercase argument
lc = $(shell echo $1 | tr '[:upper:]' '[:lower:]')

STM32_FAMILY = STM32F1
STM32_GROUP  = $(STM32_FAMILY)03xB
STM32_DEV    = $(STM32_FAMILY)03C8

TARGET = $(call lc, $(PROJECT)_$(DISPLAY)_$(DISPVAR))

C_DEFS = -DUSE_FULL_LL_DRIVER -D$(STM32_GROUP) -D_$(STM32_FAMILY)

C_DEFS += -D_DISP_RST_ENABLED

C_SOURCES = main.c

C_SOURCES += amp.c
C_SOURCES += input.c
C_SOURCES += pins.c
C_SOURCES += settings.c
C_SOURCES += spi.c
C_SOURCES += usart.c
C_SOURCES += utils.c

# Display source files
C_SOURCES += $(wildcard display/fonts/font*.c)

DISPTYPE = gc

ifneq (,$(filter $(DISPLAY), \
  SSD1322   \
))
  DISPTYPE = gg
  DISPSIZE = 256x64
  C_DEFS += -D_DISP_FB
endif

C_SOURCES += display/$(DISPTYPE)$(DISPSIZE)/$(call lc,$(DISPLAY)).c
C_SOURCES += display/dispdrv.c
C_SOURCES += display/glcd.c
C_DEFS += -D_$(DISPLAY)
C_DEFS += -D_DISP_$(DISPVAR)
C_DEFS += -D_DISP_$(DISPSIZE)

C_INCLUDES += \
  -Idisplay

C_SOURCES += \
  drivers/$(STM32_FAMILY)xx_HAL_Driver/Src/$(call lc, $(STM32_FAMILY))xx_ll_gpio.c \
  drivers/$(STM32_FAMILY)xx_HAL_Driver/Src/$(call lc, $(STM32_FAMILY))xx_ll_i2c.c \
  drivers/$(STM32_FAMILY)xx_HAL_Driver/Src/$(call lc, $(STM32_FAMILY))xx_ll_rcc.c \
  drivers/$(STM32_FAMILY)xx_HAL_Driver/Src/$(call lc, $(STM32_FAMILY))xx_ll_utils.c \
  drivers/$(STM32_FAMILY)xx_HAL_Driver/Src/$(call lc, $(STM32_FAMILY))xx_ll_usart.c \
  system/system_$(call lc, $(STM32_FAMILY))xx.c

C_INCLUDES += \
  -Idrivers/$(STM32_FAMILY)xx_HAL_Driver/Inc \
  -Idrivers/CMSIS/Device/ST/$(STM32_FAMILY)xx/Include \
  -Idrivers/CMSIS/Include

AS_DEFS =

ASM_SOURCES = \
  system/startup_$(call lc, $(STM32_GROUP)).s

# Build directory
BUILD_DIR = build

DEBUG = 1

# Compiler
FPU =
FLOAT-ABI =
MCU = -mcpu=cortex-m3 -mthumb $(FPU) $(FLOAT-ABI)
OPT = -Os -fshort-enums -ffunction-sections -fdata-sections
WARN = -Wall -Werror
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) $(WARN)
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) $(WARN)
ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif
# Dependency information
CFLAGS += -MMD -MP -MT $(BUILD_DIR)/$(*F).o -MF $(BUILD_DIR)/$(*D)/$(*F).d

LDSCRIPT = system/$(call lc, $(STM32_DEV))_flash.ld
LIBS = -lc -lm -lnosys
LIBDIR =
LDFLAGS = $(MCU) -specs=nosys.specs -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections
#LDFLAGS = $(WARN) -mmcu=$(MCU) -Wl,--gc-sections -Wl,--relax

# Main definitions
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
AR = $(PREFIX)ar
SZ = $(PREFIX)size

OPENOCD := openocd
OPENOCD_CFG := system/$(call lc, $(STM32_GROUP))_openocd.cfg

OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

ELF = $(BUILD_DIR)/$(TARGET).elf
BIN = flash/$(TARGET).bin

all: $(BIN) size

$(BIN): $(ELF)
	$(CP) -O binary $(ELF) $(BIN)

$(ELF): $(OBJECTS)
	@mkdir -p flash
	$(CC) $(LDFLAGS) -o $(ELF) $(OBJECTS)

size: $(ELF)
	$(SZ) $(ELF)

$(BUILD_DIR)/%.o: %.c Makefile
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $(C_DEFS) -o $@ $<

$(BUILD_DIR)/%.o: %.s Makefile
	@mkdir -p $(dir $@)
	$(AS) -c $(CFLAGS) $(AS_DEFS) -o $@ $<

clean:
	rm -rf $(BUILD_DIR)

clean_disp:
	@rm -f $(patsubst %.c,$(BUILD_DIR)/%.o, $(notdir $(wildcard display/gc*/*.c)))
	@rm -f $(BUILD_DIR)/dispdrv.o $(BUILD_DIR)/pins.o

.PHONY: flash
flash: $(BIN)
	$(OPENOCD) -f $(OPENOCD_CFG) -c "stm_flash $(BIN)" -c shutdown

# Other dependencies
-include $(OBJECTS:.o=.d)
