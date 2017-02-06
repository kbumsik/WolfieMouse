# Get Started

## Quick setup using a virtual machine (recommended)

1. Install tools
    * [git](https://git-scm.com/downloads)
    * [VirtualBox](https://www.virtualbox.org/wiki/Downloads) and [VirtualBox Extension Pack(https://www.virtualbox.org/wiki/Downloads)
    * [Vagrant](https://www.vagrantup.com/)
    * [MobaXterm](http://mobaxterm.mobatek.net/) for windows, [XQuartz](https://www.xquartz.org/) for macOS
    * Add to `PATH` with following folders:
        * git: `{your-git-installation-path}/Git/bin` and `{your-git-installation-path}/Git/usr/bin`
        * VirtualBox: `{your-VirtualBox-installation-path}/Oracle/VirtualBox`
2. [Fork this repo and clone it](HOWTO/Github_HOWTO.md#Set-up-your-repository)
3. Run `$ vagrant up`
    * This is a virtual machine installation command. Requires around 8GB.
    * This will take at least 20 minutes. Have a cup of coffee :)
4. After successful install, [connect to the machine using SSH](HOWTO/SSH_HOWTO.md)
5. `$ cd ~/WolfieMouse`
6. `$ make eclipse`

## Regular setup without virtual machine (when virtual machine not working properly)

To be added.

## Futher Documentation

See [table of contents of README](README.md#Table-of-Contents).