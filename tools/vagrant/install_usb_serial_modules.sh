#!/bin/bash

set -eu

# Ubuntu Vagrant box doesn't have USB serial kernel modules by default.
# I manually compiled the corresponding kernel modules to enable them.
# Compiled target Linux version is 4.4.0-154-generic.

sudo apt-get update && sudo apt-get install -y build-essential linux-headers-$(uname -r)
sudo apt-get update && sudo apt-get install -y rsync

WOLFIE_DIR=$(pwd)/wolfiemouse
PRV_DIR=$(pwd)

# Build kernel modules
cd $WOLFIE_DIR/tools/vagrant/ubuntu-xenial-kernel-source
make

# Copy compiled .ko modules
sudo rsync -avm --include="*/" --include="*.ko" --exclude="*" ./ /lib/modules/$(uname -r)/kernel/

# Update module dependencies
sudo depmod

cd $PRV_DIR

