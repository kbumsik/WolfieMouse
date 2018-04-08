define HELP_TEXT =

Build commands:
  Example: `make all`
	all    - Buid the application
	clean  - Cleanup the build files
	help   - Prompt the help information

endef
#######################################
# build the application
#######################################
# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

# build C files
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

# build CPP files
$(BUILD_DIR)/%.o: %.cc | $(BUILD_DIR) 
	$(CXX) -c $(CXXFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.cc=.lst)) $< -o $@

# build ASM files
$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(MAZE_STRING_HPP) $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@echo ""
	@echo "===== Build Completed ====="
	$(SZ) $@
	@echo ""

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@

$(MAZE_STRING_HPP): $(MAZE_QUESTION)
	python ../mazeTextToCCString.py $^ > $@

#######################################
# Others
#######################################
.PHONY: clean
clean:
	-rm -fR $(BUILD_DIR) $(MAZE_STRING_HPP)

.PHONY: help
help:
	$(info $(HELP_TEXT))
