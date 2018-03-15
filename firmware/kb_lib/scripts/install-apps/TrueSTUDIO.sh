#!/bin/sh

sudo apt-get update && sudo apt-get -y install wget make

# Install TrueSTUDIO
echo -e "\n\nDownloading Atollic TrueSTUDIO (IDE)..."
cd /opt && sudo wget -nv https://dl.dropboxusercontent.com/u/78819851/Atollic_TrueSTUDIO_Linux/Atollic_TrueSTUDIO_for_ARM_linux_x86_64_v7.1.0_20170202-1003-beta.tar.gz -O 'Atollic_TrueSTUDIO.tar.gz'
echo "Installing IDE..."
sudo tar -C /opt -xf Atollic_TrueSTUDIO.tar.gz
cd /opt/Atollic_TrueSTUDIO_for_ARM_7.1.0 && yes 1 | sudo ./install.sh
sudo rm /opt/Atollic_TrueSTUDIO.tar.gz
