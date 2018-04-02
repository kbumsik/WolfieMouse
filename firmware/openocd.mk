# Add help text
define OPENOCD_HELP_TEXT = 

OpenOCD commands:
  you can optionally use JLink with `PROGRAMMER=jlink`. ST-Link is used by default.
  Example: `make flash PROGRAMMER=jlink`
	flash  - Flash using OpenOCD
	reset  - Reset the target MCU using OpenOCD
	gdb    - Start OpenOCD as GDB server

endef

HELP_TEXT += $(OPENOCD_HELP_TEXT)

# Openocd name
OPENOCD ?= openocd

# PROGRAMMER can be imported from arugment variable
PROGRAMMER ?= stlink
ifeq ($(PROGRAMMER),stlink)
  OPENOCD_CFG = board/st_nucleo_f4.cfg
endif
ifeq ($(PROGRAMMER),jlink)
  OPENOCD_CFG = ../openocd_jlink_ob_stlink.cfg
endif

# Targets
.PHONY: flash
flash: $(BUILD_DIR)/$(TARGET).hex
	$(OPENOCD) -f $(OPENOCD_CFG) -c \
	"init; \
	reset halt; \
	sleep 100; \
	flash write_image erase $^; \
	reset run; \
	shutdown"

.PHONY: reset
reset:
	$(OPENOCD) -f $(OPENOCD_CFG) -c \
	"init; \
	reset halt; \
	sleep 100; \
	reset run; \
	shutdown"

.PHONY: gdb
gdb: $(BUILD_DIR)/$(TARGET).hex
	$(OPENOCD) -f $(OPENOCD_CFG) -c \
	"init; \
	reset halt; \
	sleep 100; \
	flash write_image erase $^; \
	reset halt"
