# OpenOCD target configuration
STLINK_BOARD_CFG := board/st_nucleo_f4.cfg
JLINK_BOARD_CFG := ../openocd_jlink_ob_stlink.cfg

OPENOCD ?= openocd

define OPENOCD_HELP_TEXT = 
	flash  - Flash using OpenOCD
	jlink  - Flash using JLink
	reset  - Reset the target MCU using OpenOCD
	gdb    - Start OpenOCD as GDB server

endef

HELP_TEXT += $(OPENOCD_HELP_TEXT)

.PHONY: flash
flash: $(BUILD_DIR)/$(TARGET).hex
	$(OPENOCD) -f $(STLINK_BOARD_CFG) -c \
	"init; \
	reset halt; \
	sleep 100; \
	flash write_image erase $^; \
	reset run; \
	shutdown"

.PHONY: jlink
jlink: $(BUILD_DIR)/$(TARGET).hex
	$(OPENOCD) -f $(JLINK_BOARD_CFG) -c \
	"init; \
	reset halt; \
	sleep 100; \
	flash write_image erase $^; \
	reset run; \
	shutdown"

.PHONY: reset
reset:
	$(OPENOCD) -c \
	"init; \
	reset halt; \
	sleep 100; \
	reset run; \
	shutdown"

.PHONY: gdb
gdb: $(BUILD_DIR)/$(TARGET).hex
	$(OPENOCD) -c \
	"init; \
	reset halt; \
	sleep 100; \
	flash write_image erase $^; \
	reset halt"
