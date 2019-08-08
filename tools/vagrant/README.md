# Vagrant tools

This directory contains shell scripts and kernel module sources to build our
Vagrant VM image.

## How to update USB serial kernel modules for new Ubuntu version

Look at [`ubuntu-xenial-kernel-source`](ubuntu-xenial-kernel-source) for more
detail about the USB serial kernel modules. This section is to instruct how to
update the source for newer version of Ubuntu (or any other Linux distros.)

1. Get the source code. Replace `ubuntu-xenial` with other version of Ubuntu
(e.g. ubuntu-disco, ubuntu-trusty.)

```
$ git clone git://kernel.ubuntu.com/ubuntu/ubuntu-xenial.git
```

As a result, you will get a huge source tree under `ubuntu-xenial`.

2. We do not need all the source code but kernel modules related to USB serial
devices. So copy the related files to `ubuntu-xenial-kernel-source`.

```
$ rsync -avm \
	--include="*/" \
	--include="*cdc-acm*" \
	--include="*cp210x*" \
	--include="*ftdi_sio*" \
	--include="*pl2303*" \
	--include="*usb-serial.c" \
	--include="*usb-serial-simple.c" \
	--include="*/usb/serial/bus.c" \
	--include="*/usb/serial/generic.c" \
	--exclude="*" \
	--exclude="Documentation" \
	ubuntu-xenial/ ubuntu-xenial-kernel-source/
```

3. Fix `install_usb_serial_modules.sh` and `Makefile`s in `ubuntu-xenial-kernel-source`
accordingly, if you need to do so. [The official linux documentation](https://www.kernel.org/doc/Documentation/kbuild/modules.txt)
will help when you are modifying them. 