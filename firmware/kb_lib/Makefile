ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

.PHONY: truestudio eclipse

eclipse:
	$(ROOT_DIR)/scripts/eclipse.sh

truestudio:
	$(ROOT_DIR)/scripts/truestudio.sh

test:
	$(ROOT_DIR)/scripts/cow.sh

