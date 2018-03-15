#!/bin/sh

# For Vagrant provisioning - Check root and change to vagrant user
if [ $(id -u) = 0 ]; then
   USER='vagrant'
fi

# Install GNU ARM toolchain
echo -e "\n\nInstalling PyCharm..."
sudo apt-get -y install lib32ncurses5
cd /home/$USER && wget -nv https://launchpad.net/gcc-arm-embedded/5.0/5-2016-q3-update/+download/gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2
cd /usr/local
sudo tar xjf /home/$USER/gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2
rm /home/$USER/gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2
