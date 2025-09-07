#  ******************************************************************************
#  @file           : sprinter.mk
#  @author         : Steven Mu
#  @summary		   : Makefile to generate objs for each source file
#  ******************************************************************************

SOURCE_DIR = src
BUILD_DIR  = build/obj
INCLUDES   = -Iinc -Isrc

DEFS      := -DDEBUG -DSTM32 -DSTM32F7 -DSTM32F767ZITx
CFLAGS    := $(MCUFLAGS) $(DEFS) -O2 -g3 -ffunction-sections -fdata-sections -Wall -Wextra -Wpedantic \
			 -Wconversion -Wshadow -Wdouble-promotion -Wformat=2

# --- Sources ---
C_SRCS := \
$(SOURCE_DIR)/core/tcb_buf.c \
$(SOURCE_DIR)/main.c

# --- Objects and deps in build/obj ---
OBJS := \
$(patsubst $(SOURCE_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SRCS))

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
