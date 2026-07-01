PROJECT = pressure_tjc_x5
BUILD_DIR = build

CC = arm-none-eabi-gcc
AS = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

MCU = cortex-m3
CFLAGS = -mcpu=$(MCU) -mthumb -O0 -g3 -Wall -ffunction-sections -fdata-sections
CFLAGS += -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER
CFLAGS += -IUSER -ICMSIS/core -ICMSIS/device -IFWlib/inc

LDSCRIPT = CMSIS/STM32F103RC_FLASH.ld
LDFLAGS = -mcpu=$(MCU) -mthumb -specs=nano.specs -Wl,--gc-sections -T$(LDSCRIPT)

C_SOURCES = \
$(wildcard USER/*.c) \
$(wildcard FWlib/src/*.c)

ASM_SOURCES = CMSIS/startup/startup_stm32f10x_hd.s

OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))

vpath %.c $(sort $(dir $(C_SOURCES)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

all: $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).hex $(BUILD_DIR)/$(PROJECT).bin

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	$(AS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(PROJECT).elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SIZE) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf
	$(OBJCOPY) -O ihex $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf
	$(OBJCOPY) -O binary $< $@

clean:
	del /Q $(BUILD_DIR)\* 2>nul || exit 0

.PHONY: all clean
