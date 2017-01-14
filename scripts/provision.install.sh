#!/bin/sh

apt-get update &&  apt-get -y install wget make # openjdk-8-jre

# Install TrueSTUDIO
echo -e "\n\nDownloading Atollic TrueSTUDIO (IDE)..."
cd /opt && wget -nv https://dl.dropboxusercontent.com/u/78819851/Atollic_TrueSTUDIO_Linux/Atollic_TrueSTUDIO_for_ARM_linux_x86_64_v7.0.1_20161219-1526-alpha1.tar.gz -O 'Atollic_TrueSTUDIO.tar.gz'
echo "Installing IDE..."
sudo tar -C /opt -xf Atollic_TrueSTUDIO.tar.gz
cd /opt/Atollic_TrueSTUDIO_for_ARM_7.0.1 && yes 1 | sudo ./install.sh
sudo rm /opt/Atollic_TrueSTUDIO.tar.gz

# Install GNU ARM toolchain
echo -e "\n\nInstalling GNU ARM toolchain..."
sudo apt-get -y install lib32ncurses5
cd /home/vagrant && wget -nv https://launchpad.net/gcc-arm-embedded/5.0/5-2016-q3-update/+download/gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2
cd /usr/local
sudo tar xjf /home/vagrant/gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2
rm /home/vagrant/gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2

# Install OpenOCD
# Reference: http://gnuarmeclipse.github.io/openocd/install/
echo -e "\n\nInstalling OpenOCD..."
cd /home/vagrant && wget -nv https://github.com/gnuarmeclipse/openocd/releases/download/gae-0.10.0-20161028/gnuarmeclipse-openocd-debian64-0.10.0-201610281609-dev.tgz
sudo mkdir -p /opt/gnuarmeclipse
cd /opt/gnuarmeclipse
sudo tar xf /home/vagrant/gnuarmeclipse-openocd-debian64-0.10.0-201610281609-dev.tgz
rm /home/vagrant/gnuarmeclipse-openocd-debian64-0.10.0-201610281609-dev.tgz

# Add udev rule for OpenOCD
echo -e "\n\nAdding a udev rule for OpenOCD..."
sudo cp /opt/gnuarmeclipse/openocd/0.10.0-201610281609-dev/contrib/99-openocd.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules
sudo usermod -aG plugdev vagrant

# Install QEMU
# Reference: http://gnuarmeclipse.github.io/qemu/install/
echo -e "\n\nInstalling QEMU..."
cd /home/vagrant && wget -nv https://github.com/gnuarmeclipse/qemu/releases/download/gae-2.8.0-20161227/gnuarmeclipse-qemu-debian64-2.8.0-201612271623-dev.tgz
cd /opt/gnuarmeclipse
sudo tar xf /home/vagrant/gnuarmeclipse-qemu-debian64-2.8.0-201612271623-dev.tgz
rm /home/vagrant/gnuarmeclipse-qemu-debian64-2.8.0-201612271623-dev.tgz

# Install J-Link
# Reference: http://gnuarmeclipse.github.io/debug/jlink/install/
# Used personal Dropbox link as JEGGER homepage requires addtional sign-in
echo -e "\n\nInstalling J-Link..."
cd /home/vagrant && wget -nv https://dl.dropboxusercontent.com/u/78819851/gnu_arm_eclipse/JLink/JLink_Linux_V612e_x86_64.deb
sudo dpkg -i /home/vagrant/JLink_Linux_V612e_x86_64.deb
rm /home/vagrant/JLink_Linux_V612e_x86_64.deb

# Install Oracle JDK for Eclipse
# http://askubuntu.com/questions/190582/installing-java-automatically-with-silent-option
echo -e "\n\nInstalling Oracle JRE..."
apt-get install -y software-properties-common
add-apt-repository ppa:webupd8team/java && apt-get update
echo "oracle-java8-installer shared/accepted-oracle-license-v1-1 select true" | debconf-set-selections
echo "oracle-java8-installer shared/accepted-oracle-license-v1-1 seen true" | debconf-set-selections
apt-get install -y oracle-java8-installer

# Install GNU ARM Eclipse
echo -e "\n\nInstalling GNU ARM Eclipse..."
cd /home/vagrant && wget -nv https://dl.dropboxusercontent.com/u/78819851/gnu_arm_eclipse/Eclipse_20160113/eclipse-cpp-mars-2-linux-gtk-x86_64.tar.gz
cd /opt && sudo tar -xf /home/vagrant/eclipse-cpp-mars-2-linux-gtk-x86_64.tar.gz
sudo mv ./eclipse ./eclipse_gnuarm
# Restore Eclipse settings
cd /home/vagrant && wget -nv https://dl.dropboxusercontent.com/u/78819851/gnu_arm_eclipse/Eclipse_20160113/eclipse_settings.tar.gz
mkdir -p /home/vagrant/.eclipse
cd /home/vagrant/.eclipse && sudo tar -xf /home/vagrant/eclipse_settings.tar.gz
rm /home/vagrant/eclipse_settings.tar.gz /home/vagrant/eclipse-cpp-mars-2-linux-gtk-x86_64.tar.gz
sudo chown -R vagrant:vagrant /home/vagrant/.eclipse

# append .bashrc
echo -e "\n\nModifying .bashrc..."
sudo apt-get -y install fortune cowsay
echo -e '\n\n# Change console folder colors shard with windows\nLS_COLORS="ow=01;36;40" && export LS_COLORS\n\n# Cowsay\necho -e "WolfieMouse says:\n\n"$(fortune) | cowsay\n' >> /home/vagrant/.bashrc
sed /home/vagrant/.bashrc -e 's:\#force_color_prompt=yes:force_color_prompt=yes:g' -i

# Install RDP server and xfce4
echo -e "\n\nInstalling RDP and Xfce..."
sudo apt-get install -y xfce4 tightvncserver xrdp
#sudo apt-get upgrade -y x11-xserver-utils # it prompts strange configuration that prevents executing

# Configure VNC
# Reference: https://peteris.rocks/blog/remote-desktop-and-vnc-on-ubuntu-server/
mkdir -p /home/vagrant/.vnc
echo wolfie | vncpasswd -f > /home/vagrant/.vnc/passwd
chmod 600 /home/vagrant/.vnc/passwd

# Xstartup confiuration
cat > /home/vagrant/.vnc/xstartup <<EOF
#!/bin/sh
startxfce4 &
EOF
chmod +x /home/vagrant/.vnc/xstartup
sudo chown -R vagrant:vagrant /home/vagrant/.vnc

# xrdp configuration
sudo bash -c 'cat > /etc/xrdp/xrdp.ini <<EOF
[globals]
bitmap_cache=yes
bitmap_compression=yes
port=3389
crypt_level=low
channel_code=1
[vnc1]
name=vncserver
lib=libvnc.so
ip=localhost
port=5901
username=
password=wolfie
EOF' 

# Silence warning
touch /home/vagrant/.Xauthority
mkdir -p /home/vagrant/.config
sudo chown vagrant:vagrant /home/vagrant/.Xauthority /home/vagrant/.config
