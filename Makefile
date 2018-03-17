.all:

.PHONY: all welcome

all: welcome
	@echo ""
	@exit 1

welcome:
	@echo "Welcome to Wolfiemouse!"
	@echo ""
	@echo "You should run 'make' in application's directory instead."
	@echo "    e.g. /firmware/blinky_nucleo64"
	@echo ""
	@echo "For more information, please look at /firmware/README.md."
