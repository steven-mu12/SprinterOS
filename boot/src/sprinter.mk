#  ******************************************************************************
#  @file           : src/sprinter.mk
#  @author         : Steven Mu
#  @summary		   : Makefile to generate objs for each source file
#  ******************************************************************************

SOURCE_DIR = src
BUILD_DIR  = build/obj
INCLUDES   = -Iinc

DEFS      := -DDEBUG -DSTM32 -DSTM32F7 -DSTM32F767ZITx -D__FPU_PRESENT=1 -D__FPU_USED=1
CFLAGS    := $(MCUFLAGS) $(DEFS) -O2 -g3 -ffunction-sections -fdata-sections -Wall -Wextra -Wpedantic \
			 -Wconversion -Wshadow -Wdouble-promotion -Wformat=2

# --- Sources ---
C_SRCS := \
$(SOURCE_DIR)/sprinter/core/syscalls.c \
$(SOURCE_DIR)/sprinter/core/sysmem.c  \
$(SOURCE_DIR)/sprinter/peripherals/gpio.c \
$(SOURCE_DIR)/sprinter/peripherals/iwdg.c \
$(SOURCE_DIR)/sprinter/peripherals/uart.c \
$(SOURCE_DIR)/sprinter/peripherals/rcc.c \
$(SOURCE_DIR)/sprinter/peripherals/timer.c \
$(SOURCE_DIR)/sprinter/peripherals/spi.c \
$(SOURCE_DIR)/main.c

S_SRCS := \
$(SOURCE_DIR)/startup/startup_stm32f767zitx.s

# --- Objects and deps in build/obj ---
OBJS := \
$(patsubst $(SOURCE_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SRCS)) \
$(patsubst $(SOURCE_DIR)/%.s, $(BUILD_DIR)/%.o, $(S_SRCS)) 

DEPS := $(OBJS:.o=.d)

# --- Build rules for startup asm and c sources ---
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	@mkdir -p $(dir $@)
	$(COMPILER) $(MCUFLAGS) $(DEFS) $(INCLUDES) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.s
	@mkdir -p $(dir $@)
	$(COMPILER) $(MCUFLAGS) $(DEFS) -c $< -o $@

# Clean rules for object builds
.PHONY: clean-objects

clean-objects:
	@rm -f $(OBJS) $(DEPS)
