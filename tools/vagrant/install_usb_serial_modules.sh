#!/bin/bash

# Ubuntu Vagrant box doesn't have USB serial kernel modules by default.
# I manually compiled the corresponding kernel modules to enable them.
# Compiled target Linux version is 4.4.0-154-generic.

WOLFIE_PATH=$(pwd)/wolfiemouse

# FTDI USB UART modules
sudo mkdir /lib/modules/$(uname -r)/kernel/drivers/usb/serial
sudo cp $WOLFIE_PATH/tools/vagrant/usbserial.ko /lib/modules/$(uname -r)/kernel/drivers/usb/serial/
sudo cp $WOLFIE_PATH/tools/vagrant/ftdi_sio.ko /lib/modules/$(uname -r)/kernel/drivers/usb/serial/
sudo cp $WOLFIE_PATH/tools/vagrant/cp210x.ko /lib/modules/$(uname -r)/kernel/drivers/usb/serial/

# CDC ACM module (ST-Link VCP)
sudo mkdir /lib/modules/$(uname -r)/kernel/drivers/usb/class
sudo cp $WOLFIE_PATH/tools/vagrant/cdc-acm.ko /lib/modules/$(uname -r)/kernel/drivers/usb/class/

# Update module dependencies
sudo depmod
