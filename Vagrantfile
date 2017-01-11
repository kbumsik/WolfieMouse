# -*- mode: ruby -*-
# vi: set ft=ruby :

WOLFIEBASE ||= "./"

# All Vagrant configuration is done below. The "2" in Vagrant.configure
# configures the configuration version (we support older styles for
# backwards compatibility). Please don't change it unless you know what
# you're doing.
Vagrant.configure("2") do |config|

  # Enable X11 forwarding
  config.ssh.forward_x11 = true
  
  # The most common configuration options are documented and commented below.
  # For a complete reference, please see the online documentation at
  # https://docs.vagrantup.com.

  # Every Vagrant development environment requires a box. You can search for
  # boxes at https://atlas.hashicorp.com/search.

  config.vm.box = "ubuntu/xenial64"

  # Disable automatic box update checking. If you disable this, then
  # boxes will only be checked for updates when the user runs
  # `vagrant box outdated`. This is not recommended.
  # config.vm.box_check_update = false

  # Create a forwarded port mapping which allows access to a specific port
  # within the machine from a port on the host machine. In the example below,
  # accessing "localhost:8080" will access port 80 on the guest machine.
  # config.vm.network "forwarded_port", guest: 80, host: 8080

  # Create a private network, which allows host-only access to the machine
  # using a specific IP.
  # config.vm.network "private_network", ip: "192.168.33.10"

  # Create a public network, which generally matched to bridged network.
  # Bridged networks make the machine appear as another physical device on
  # your network.
  # config.vm.network "public_network"

  # Share an additional folder to the guest VM. The first argument is
  # the path on the host to the actual folder. The second argument is
  # the path on the guest to mount the folder. And the optional third
  # argument is a set of non-required options.

  config.vm.synced_folder WOLFIEBASE, "/home/ubuntu/wolfieMouse"

  # Provider-specific configuration so you can fine-tune various
  # backing providers for Vagrant. These expose provider-specific options.
  # Example for VirtualBox:
  #
  # config.vm.provider "virtualbox" do |vb|
  #   # Display the VirtualBox GUI when booting the machine
  #   vb.gui = true
  #
  #   # Customize the amount of memory on the VM:
  #   vb.memory = "1024"
  # end
  #
  # View the documentation for the provider you are using for more
  # information on available options.

  config.vm.provider "virtualbox" do |vb|
    # Set virtualbox name
    vb.name = "Ubuntu-MCU-Development-VM"

    # Enable USB device
    vb.customize ["modifyvm", :id, "--usb", "on"]
    vb.customize ["modifyvm", :id, "--usbxhci", "on"]

    # Add USB device filter.
    # Reference: http://code-chronicle.blogspot.com/2014/08/connect-usb-device-through-vagrant.html

    # vb.customize ["usbfilter", "add", "0", 
    #   "--target", :id, 
    #   "--name", "This is the identifier",
    #   "--manufacturer", "SuYin",
    #   "--product", "Laptop_Integrated_Webcam_HD"]
  end

  # Define a Vagrant Push strategy for pushing to Atlas. Other push strategies
  # such as FTP and Heroku are also available. See the documentation at
  # https://docs.vagrantup.com/v2/push/atlas.html for more information.
  # config.push.define "atlas" do |push|
  #   push.app = "YOUR_ATLAS_USERNAME/YOUR_APPLICATION_NAME"
  # end

  # Enable provisioning with a shell script. Additional provisioners such as
  # Puppet, Chef, Ansible, Salt, and Docker are also available. Please see the
  # documentation for more information about their specific syntax and use.
  
  config.vm.provision "shell", inline: <<-SHELL
    apt-get update &&  apt-get -y install wget make

    # Install TrueSTUDIO
    echo "Downloading Atollic TrueSTUDIO (IDE)..."
    cd /opt && wget -nv https://dl.dropboxusercontent.com/u/78819851/Atollic_TrueSTUDIO_for_ARM_linux_x86_64_v7.0.1_20161219-1526-alpha1.tar.gz -O 'Atollic_TrueSTUDIO.tar.gz'
    echo "Installing IDE..."
    sudo tar -C /opt -xf Atollic_TrueSTUDIO.tar.gz
    cd /opt/Atollic_TrueSTUDIO_for_ARM_7.0.1 && yes 1 | sudo ./install.sh
    sudo rm /opt/Atollic_TrueSTUDIO.tar.gz

    # Install GNU ARM toolchain
    sudo apt-get -y install lib32ncurses5
    cd /home/ubuntu && wget -nv https://launchpad.net/gcc-arm-embedded/5.0/5-2016-q3-update/+download/gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2
    cd /usr/local
    sudo tar xjf /home/ubuntu/gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2
    rm /home/ubuntu/gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2
    
    # Install OpenOCD
    # Reference: http://gnuarmeclipse.github.io/openocd/install/
    cd /home/ubuntu && wget -nv https://github.com/gnuarmeclipse/openocd/releases/download/gae-0.10.0-20161028/gnuarmeclipse-openocd-debian64-0.10.0-201610281609-dev.tgz
    sudo mkdir -p /opt/gnuarmeclipse
    cd /opt/gnuarmeclipse
    sudo tar xf /home/ubuntu/gnuarmeclipse-openocd-debian64-0.10.0-201610281609-dev.tgz
    rm /home/ubuntu/gnuarmeclipse-openocd-debian64-0.10.0-201610281609-dev.tgz

    # Add udev rule for OpenOCD
    sudo cp /opt/gnuarmeclipse/openocd/0.10.0-201610281609-dev/contrib/99-openocd.rules /etc/udev/rules.d/
    sudo udevadm control --reload-rules
    sudo usermod -aG plugdev ubuntu

    # Install QEMU
    # Reference: http://gnuarmeclipse.github.io/qemu/install/
    cd /home/ubuntu && wget -nv https://github.com/gnuarmeclipse/qemu/releases/download/gae-2.8.0-20161227/gnuarmeclipse-qemu-debian64-2.8.0-201612271623-dev.tgz
    cd /opt/gnuarmeclipse
    sudo tar xf /home/ubuntu/gnuarmeclipse-qemu-debian64-2.8.0-201612271623-dev.tgz
    rm /home/ubuntu/gnuarmeclipse-qemu-debian64-2.8.0-201612271623-dev.tgz

    # Install J-Link
    # Reference: http://gnuarmeclipse.github.io/debug/jlink/install/
    # Used personal Dropbox link as JEGGER homepage requires addtional sign-in
    cd /home/ubuntu && wget -nv https://dl.dropboxusercontent.com/u/78819851/JLink_Linux_V612e_x86_64.deb
    sudo dpkg -i /home/ubuntu/JLink_Linux_V612e_x86_64.deb
    rm /home/ubuntu/JLink_Linux_V612e_x86_64.deb
  SHELL
end
