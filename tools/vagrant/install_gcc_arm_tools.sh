#!/bin/bash

set -eu

# For Vagrant provisioning - Check root and change to vagrant user
if [ $(id -u) = 0 ]; then
   USER='vagrant'
fi

################################################################################
# Common installation commends
################################################################################

# Common packages for installing
install-installer-essential () {
	echo -e "\n\nInstalling neccessary tools for installing"

	sudo apt-get update
	sudo apt-get -y install wget make curl

	echo -e "\nDone Installing the neccessary tools."
}

# GNU ARM Embedded Toolchain
# https://developer.arm.com/open-source/gnu-toolchain/gnu-rm
install-gcc-arm-embedded () {
	echo -e "\n\nInstalling GNU ARM Embedded Toolchain..."
	echo -e "\nInstalling Dependancies..."
	
	sudo apt-get -y install lib32ncurses5

	echo -e "\nDownlaoding the Package..."
	wget -nv 'https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/7-2017q4/gcc-arm-none-eabi-7-2017-q4-major-linux.tar.bz2' \
		-O gcc-arm-none-eabi.tar.bz2

	echo -e "\nExtracting the Package..."
	tar xjf gcc-arm-none-eabi.tar.bz2
	sudo rsync -a --delete-after gcc-arm-none-eabi-7-2017-q4-major/ /opt/gcc-arm-none-eabi/
	rm gcc-arm-none-eabi.tar.bz2
	rm -rf gcc-arm-none-eabi-7-2017-q4-major
	
	echo -e "\nAdding PATH to the toolchain..."
	cat >> /home/$USER/.bashrc <<-EOF
	#Add GNU ARM Embedded Toolchain to PATH
	export PATH=\$PATH:/opt/gcc-arm-none-eabi/bin
	EOF

	echo -e "\nDone installing GNU ARM Embedded Toolchain."
}

# OpenOCD
# http://openocd.org/
# https://sourceforge.net/projects/openocd/
install-openocd () {
	echo -e "\n\nInstalling OpenOCD..."

	# OpenOCD Ubuntu repo is too old. So we need to compile instead.
	sudo apt-get install -y git
	sudo apt-get install -y build-essential pkg-config autoconf automake \
		libtool libusb-dev libusb-1.0-0-dev libhidapi-dev

	echo -e "\nCloning OpenOCD repository..."
	git clone http://repo.or.cz/openocd.git openocd
	cd openocd
	git checkout v0.10.0
	echo -e "\nCompiling OpenOCD..."
	./bootstrap
	./configure
	make
	echo -e "\nInstalling OpenOCD..."
	sudo make install
	cd ..
	sudo rm -r openocd
	sudo cp /usr/local/share/openocd/contrib/60-openocd.rules /etc/udev/rules.d/
	
	echo -e "\nDone installing OpenOCD."
}

# J-Link Software and Documentation Pack
# https://www.segger.com/downloads/jlink
# Additional reference: http://gnuarmeclipse.github.io/debug/jlink/install/
# Used personal Dropbox link as JEGGER homepage requires addtional sign-in
install-jlink () {
	echo -e "\n\nInstalling J-Link Software and Documentation Pack..."
	echo -e "\nDownloading J-Link Software and Documentation Pack..."
	
	wget --post-data "accept_license_agreement=accepted&submit=Download+software&non_emb_ctr=confirmed" \
			-nv "https://www.segger.com/downloads/jlink/JLink_Linux_x86_64.deb" \
			-O "JLink_Linux_x86_64.deb"
		
	echo -e "\nInstalling the Package..."
	yes Y | sudo dpkg -i "JLink_Linux_x86_64.deb"
	rm "JLink_Linux_x86_64.deb"

	echo -e "\nDone installing J-Link Software and Documentation Pack."
}


################################################################################
# Main code
################################################################################

# Install tools such as wget and curl
install-installer-essential

# Install GNU ARM toolchain
install-gcc-arm-embedded

# Install OpenOCD
install-openocd

# Install J-Link
install-jlink
