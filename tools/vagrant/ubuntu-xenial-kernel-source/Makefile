# To see what's going on with this Makefile, look through other Makefiles
# in the subdirectories.
# Reference: https://www.kernel.org/doc/Documentation/kbuild/modules.txt
obj-m += drivers/

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
 
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

