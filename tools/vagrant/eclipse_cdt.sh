#!/bin/bash

set -eu

# For Vagrant provisioning - Check root and change to vagrant user
if [ $(id -u) = 0 ]; then
   USER='vagrant'
fi

# Eclipse CDT tools
install-eclipse_cdt () {
	echo -e "\n\nInstalling Eclipse CDT standalone debuggger..."
	echo -e "\nInstalling OpenJDK8 first..."
    sudo apt-get install -y openjdk-8-jre

	echo -e "\nDownloading Eclipse CDT standalone debuggger..."
	wget -nv 'http://mirror.math.princeton.edu/pub/eclipse//tools/cdt/releases/9.4/cdt-9.4.2/rcp/cdt-stand-alone-debugger-9.4.2-20180212-2019-linux.gtk.x86_64.tar.gz' \
        -O cdt-stand-alone-debugger-linux.gtk.x86_64.tar.gz

	echo -e "\nExtracting the Package..."
	tar -xvf cdt-stand-alone-debugger-linux.gtk.x86_64.tar.gz
	sudo rsync -a --delete-after cdt-stand-alone-debugger/ /opt/cdt-stand-alone-debugger/
	rm cdt-stand-alone-debugger-linux.gtk.x86_64.tar.gz
	rm -rf cdt-stand-alone-debugger/

	echo -e "\nAdding Eclipse cdt to PATH"
	cat >> /home/$USER/.bashrc <<-EOF
#Add Eclipse CDT debugger to PATH
export PATH=\$PATH:/opt/cdt-stand-alone-debugger
EOF
	echo -e "\nDone installing Eclipse CDT debugger."
}

install-eclipse_cdt
