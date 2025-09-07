#  ******************************************************************************
#  @file           : Makefile (kernel)
#  @author         : Steven Mu
#  @summary		   : Kernel makefile
#  ******************************************************************************

# Build includes
-include src/sprinter.mk

# General Project Parameters
TARGET := sprinterOS
BUILD_DIR := build
BUILD_OBJ_DIR := $(BUILD_DIR)/obj
SOURCE_DIR := src

ELF_TARGET := $(BUILD_DIR)/$(TARGET).elf
BIN_TARGET := $(BUILD_DIR)/$(TARGET).bin
LIST_TARGET := $(BUILD_DIR)/$(TARGET).list
MAP_TARGET := $(BUILD_DIR)/$(TARGET).map

LDSCRIPT := ../boot/$(SOURCE_DIR)/startup/STM32F767ZITX_FLASH.ld

# Compiler Toolchain
COMPILER  := arm-none-eabi-gcc
OBJCOPY   := arm-none-eabi-objcopy
OBJDUMP	  := arm-none-eabi-objdump
SIZE      := arm-none-eabi-size

# Compiler Flags
MCUFLAGS  := -mcpu=cortex-m7 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard
LDFLAGS   := $(MCUFLAGS) -T $(LDSCRIPT) \
             --specs=nano.specs --specs=nosys.specs \
             -Wl,--gc-sections -Wl,-Map=$(MAP_TARGET) -Wl,--print-memory-usage \
             -Wl,--start-group -lc -lm -lnosys -Wl,--end-group

# "make sprinter"
# - Creates a flashable binary sprinterOS executive
sprinter: $(BIN_TARGET)

# OBJCOPY -O binary means we want binary output, input < (elf target) and output bin file
$(BIN_TARGET): $(ELF_TARGET)
	$(OBJCOPY) -O binary $< $@

# OBJDUMP -h sprints headers, -S is "source converted to assembly dump" hybrid
$(LIST_TARGET): $(ELF_TARGET)
	$(OBJDUMP) -h -S $< > $@

# Uses the elf file for size file
size: $(ELF_TARGET)
	$(SIZE) $<

# Main boot image
$(ELF_TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(COMPILER) $(MCUFLAGS) $(OBJS) $(LDFLAGS) -o $@

# "make clean"
# Removes build artifacts
clean:
	@rm -vf $(BUILD_DIR)/*.elf \
	        $(BUILD_DIR)/*.bin \
	        $(BUILD_DIR)/*.list \
	        $(BUILD_DIR)/*.map \
	        $(BUILD_DIR)/*.hex
	@rm -vf $(BUILD_OBJ_DIR)/*.o \
	        $(BUILD_OBJ_DIR)/*.d
	@rm -vrf $(BUILD_OBJ_DIR)
