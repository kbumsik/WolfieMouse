# For the original from the Linux kernel,
# See: https://elixir.bootlin.com/linux/v4.4.157/source/drivers/usb/serial

# Generic USB serial
obj-m += usbserial.o
usbserial-y := \
		usb-serial.o \
		generic.o \
		bus.o

# USB serial devices
obj-m += cp210x.o \
		 ftdi_sio.o \
		 usb-serial-simple.o
