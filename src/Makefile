PROJECT = H-MS1110

MODULE = ${H_MS1110_MODULE}
ifeq "$(MODULE)" ""
  MODULE = PLAYER
endif

STM32_MCU = F103CB

DISPLAY = SSD1322
DISPVAR = SPI

APROC_LIST = TDA7719
TUNER_LIST = LC7213X
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

ifeq "$(MODULE)" "PLAYER"
  FEATURE_LIST += USE_MPC
  FEATURE_LIST += USE_RC

  C_DEFS += -D_INPUT_ANALOG
  C_DEFS += -D_GLCD_OFT_Y=24
endif

ifeq "$(MODULE)" "TUNER"
  FEATURE_LIST += USE_RDS_DEMOD
endif

C_DEFS += -DUSE_FULL_LL_DRIVER -D$(STM32_GROUP) -D_$(STM32_MCU)

C_DEFS += -D_PRINTF_USART1
C_DEFS += -D_MODULE_$(MODULE)
C_DEFS += -D_DISP_RST_ENABLED
C_DEFS += -DI2C2_BUF_SIZE=96
C_DEFS += $(addprefix -D_, $(FEATURE_LIST))

C_SOURCES += $(call lc, amp_$(MODULE)).c

C_SOURCES += amp.c
C_SOURCES += eemul.c
C_SOURCES += fft.c
C_SOURCES += i2c.c
C_SOURCES += input.c
C_SOURCES += main.c
C_SOURCES += mpc.c
C_SOURCES += rc.c
C_SOURCES += ringbuf.c
C_SOURCES += rtc.c
C_SOURCES += settings.c
C_SOURCES += setup.c
C_SOURCES += spectrum.c
C_SOURCES += spi.c
C_SOURCES += swtimers.c
C_SOURCES += sync.c
C_SOURCES += timers.c
C_SOURCES += tunersync.c
C_SOURCES += usart.c
C_SOURCES += utils.c

C_SOURCES += $(wildcard tr/labels*.c)

# Input source files
C_SOURCES += input/analog.c
C_SOURCES += input/matrix.c

# Display source files
C_SOURCES += $(wildcard display/fonts/font*.c)

ifneq (,$(filter $(DISPLAY), \
  SSD1322   \
))
  DISPTYPE = gg
  DISPSIZE = 256x64
  C_DEFS += -D_DISP_FB
endif

C_SOURCES += gui/view/datetimeview.c
C_SOURCES += gui/view/inputview.c
C_SOURCES += gui/view/mpcview.c
C_SOURCES += gui/view/tunerview.c
C_SOURCES += gui/view/tuneview.c
C_SOURCES += gui/view/starsview.c

C_SOURCES += screen/setupview.c
C_SOURCES += screen/spectrumview.c

C_SOURCES += display/dispdrv/$(call lc,$(DISPLAY)).c
C_SOURCES += display/dispdrv.c
C_SOURCES += display/glcd.c
C_DEFS += -D_$(DISPLAY)
C_DEFS += -D_DISP_$(DISPVAR)

C_SOURCES += gui/canvas.c
C_SOURCES += gui/palette.c
C_SOURCES += $(wildcard gui/fonts/font*.c)
C_SOURCES += $(wildcard gui/widget/*.c)

C_SOURCES += $(wildcard gui/icons.c)
C_SOURCES += $(wildcard gui/icons/icon*.c)

C_SOURCES += tuner/rds/parser.c
C_SOURCES += tuner/rds/decoder.c
C_SOURCES += tuner/rds/demod.c

# Tuner source files
C_SOURCES += $(addprefix tuner/, $(addsuffix .c, $(call lc, $(TUNER_LIST))))
C_SOURCES += tuner/stations.c
C_SOURCES += tuner/tuner.c
C_DEFS += $(addprefix -D_, $(TUNER_LIST))

# Audio source files
C_SOURCES += $(addprefix audio/, $(addsuffix .c, $(call lc, $(APROC_LIST))))
C_SOURCES += audio/audio.c
C_DEFS += $(addprefix -D_, $(APROC_LIST))

C_INCLUDES += \
  -Idisplay/fonts \
  -I.

C_SOURCES += \
  drivers/$(STM32_FAMILY)xx_HAL_Driver/Src/$(call lc, $(STM32_FAMILY))xx_ll_gpio.c \
  drivers/$(STM32_FAMILY)xx_HAL_Driver/Src/$(call lc, $(STM32_FAMILY))xx_ll_i2c.c \
  drivers/$(STM32_FAMILY)xx_HAL_Driver/Src/$(call lc, $(STM32_FAMILY))xx_ll_rcc.c \
  drivers/$(STM32_FAMILY)xx_HAL_Driver/Src/$(call lc, $(STM32_FAMILY))xx_ll_rtc.c \
  drivers/$(STM32_FAMILY)xx_HAL_Driver/Src/$(call lc, $(STM32_FAMILY))xx_ll_usart.c \
  drivers/$(STM32_FAMILY)xx_HAL_Driver/Src/$(call lc, $(STM32_FAMILY))xx_ll_utils.c \
  system/system_$(call lc, $(STM32_FAMILY))xx.c

C_INCLUDES += \
  -Idrivers/$(STM32_FAMILY)xx_HAL_Driver/Inc \
  -Idrivers/CMSIS/Device/ST/$(STM32_FAMILY)xx/Include \
  -Idrivers/CMSIS/Include \
  -Isystem

AS_DEFS +=

ASM_SOURCES += \
  system/startup_$(call lc, $(STM32_GROUP)).s

# Build directory
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj_$(TARGET)

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

LDSCRIPT = system/$(call lc, STM32$(STM32_MCU))_flash.ld
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
ifeq "$(MODULE)" "PLAYER"
  OPENOCD_CFG := system/$(call lc, $(STM32_GROUP))_openocd_stlink.cfg
else
  OPENOCD_CFG := system/$(call lc, $(STM32_GROUP))_openocd_jlink.cfg
endif

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
