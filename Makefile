# Makefile — stc32g144k246-blink（SDCC MCS-251，中文注释）
#
# 工具链固定指向工作区内的 sdcc-c251，不依赖 PATH。
# 代码定位：STC32G 复位向量在 FF:0000，用户程序从 0xFF0000 开始放。
TOOLCHAIN := /Users/theo-richard/workspace/toolchains/sdcc-c251/bin/
CC        := $(TOOLCHAIN)sdcc
PACKIHX   := $(TOOLCHAIN)packihx
OBJCOPY   := $(TOOLCHAIN)sdobjcopy

TARGET    := stc32g144k246-blink
BUILD     := build

# --model-small：16 位数据/代码指针；--code-loc 0xFF0000：复位入口
# --iram-size 0x4000：告诉链接器片内 edata 有 16KB
MCU       := -mmcs251 --model-small --code-loc 0xFF0000 --iram-size 0x4000
INCLUDES  := -ICore/Inc -IConfig -IDrivers/STC32G/Inc
CFLAGS    := $(MCU) $(INCLUDES) --std-c23
LDFLAGS   := $(MCU)

C_SOURCES := Core/Src/main.c \
             Core/Src/delay.c
OBJECTS   := $(addprefix $(BUILD)/,$(notdir $(C_SOURCES:.c=.rel)))

all: $(BUILD)/$(TARGET).ihx $(BUILD)/$(TARGET).hex $(BUILD)/$(TARGET).bin

$(BUILD):
	mkdir -p $@

$(BUILD)/%.rel: Core/Src/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/$(TARGET).ihx: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(BUILD)/$(TARGET).hex: $(BUILD)/$(TARGET).ihx
	$(PACKIHX) $< > $@

$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).ihx
	$(OBJCOPY) -I ihex -O binary $< $@

size: $(BUILD)/$(TARGET).ihx
	@echo "=== 代码/数据区（来自 $(BUILD)/$(TARGET).mem）==="
	@grep -E "ROM/EPROM/FLASH|EXTERNAL RAM|PAGED EXT|initial stack" $(BUILD)/$(TARGET).mem || true

dis: $(OBJECTS)
	@echo "反汇编/汇编列表：$(BUILD)/*.lst（sdcc -c 生成）"
	@ls -l $(BUILD)/*.lst

symbols: $(BUILD)/$(TARGET).map
	@grep -E "^C:|^ +Value" $(BUILD)/$(TARGET).map | head -80

verify: all
	/usr/bin/python3 -I tools/verify.py $(BUILD)/$(TARGET).ihx $(BUILD)/$(TARGET).map

clean:
	rm -rf $(BUILD)

.PHONY: all size dis symbols verify clean
