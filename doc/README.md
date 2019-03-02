# Documentation

## Table of Contents

* Tutorial
  * [What is micromouse competition?](#What-is-Micromouse)
  * [Get-Started](#Getting-Started)
  * [Vagrant-HOWTO](#Vagrant-HOWTO)
  * [Github-HOWTO](#Github-HOWTO)
  * [KiCad-HOWTO](#KiCad-HOWTO)

* Code Documentation
  * [Class Design](Class-design.md)
  * [Hardware Design](Hardware-design.md)

* Reports
  * [Feb 2017 report](Report_Feb_2017.md)


## What is Micromouse

* [2015 Micromouse IEEE Region 1 competition slide](http://ewh.ieee.org/reg/1/sac/assets/Documents/2015-R1-Micromouse-Competition.pdf)
* [Micromouse Rule](http://ewh.ieee.org/reg/1/sac/assets/Documents/2016/2016-R1-Micromouse-Guideline.pdf)

### Video

* [Simeple example video](https://www.youtube.com/watch?v=_L9rkLAskWU)
* [Crazay World record video](https://youtu.be/blpnbIzq6G0?t=30s)


## Getting Started

### Quick setup using a virtual machine (recommended)

1. Install tools
    * [git](https://git-scm.com/downloads)
    * [VirtualBox](https://www.virtualbox.org/wiki/Downloads)
    * [VirtualBox Extension Pack](https://www.virtualbox.org/wiki/Downloads). [(Video)](https://youtu.be/P5exlXPF1SM)
    * [Vagrant](https://www.vagrantup.com/)
    * [MobaXterm](http://mobaxterm.mobatek.net/) for windows, [XQuartz](https://www.xquartz.org/) for macOS
    * Add to `PATH` with following folders: [(Video)](https://youtu.be/biMVFUH0grA).
        * git: `{your-git-installation-path}/Git/bin` and `{your-git-installation-path}/Git/usr/bin`
        * VirtualBox: `{your-VirtualBox-installation-path}/Oracle/VirtualBox`
2. [Fork this repo and clone it](HOWTO/Github_HOWTO.md#Set-up-your-repository)
3. Run `$ vagrant up` to install VM. [(Video)](https://youtu.be/IwcibyR7uL4)
    * This is a virtual machine installation command. Requires around 8GB.
    * This will take at least 20 minutes. Have a cup of coffee :)
4. After successful install, connect to the machine via SSH using MobaXterm. [(Video)](https://youtu.be/jf9KIUKjGEk)
5. `$ cd ~/wolfiemouse`. This is our project root directory.

### Regular setup without virtual machine (when virtual machine not working properly)

To be added.


## Vagrant-HOWTO

Vagrant is a tool for Virtual Machine. Vagrant abstract virtual machine interfaces
and will automate system configuration of a VM, so we will have a unified 
working environment.

### Prerequisites

See `Install tools` section in [Get Started Document](../Get-started.md). Don't
forget to add `Git` directories in `PATH`.

### How to boot a vagrant VM

The following command will boot up the VM in background:

> $ vagrant up

If you run the above code for the first time, it will install the operating
system and the software we use in the project. This initial process will take
around 20 minuetes so have a cup of coffee.

### How to connect a terminal(SSH) to the VM

We can connect the virtual machine through SSH(remote terminal). The defualt
configuration to connect is:

* Address: 127.0.0.1
* Port: 2222
* User: vagrant

### How to close the VM

Simple, just commend:

> $ vagrant halt

### How to delete the VM

This is also simple. Run:

> $ vagrant destroy

You can re-install the VM using `$ vagrant up` any time.

### Troubleshooting

1. It says we need to install `SSH`: Don't forget add `Git` directories
(`{your-git-installation-path}/Git/bin` and `{your-git-installation-path}/Git/usr/bin`)
in `PATH` environment variable. Git has SSH tools for you.
2. Timeout error : Firstly, check if your CPU supports virtualization (VT-x).
If it doesn't you cannot use the VM and Vagrant. Consider installing Ubuntu in
a separated partition. If your CPU supports VT-x but you still have that problem,
the BIOS might disabled the CPU virtualization feature. Go to BIOS and enable it.


## Github-HOWTO

Reference: https://help.github.com/articles/syncing-a-fork/

### Set up your repository

* Fork our github repo https://github.com/kbumsik/WolfieMouse
* Clone your forked repo. e.g:

> $ git clone --recursive https://github.com/{your-id}/WolfieMouse

* Add our repo as a new remote `upstream`

> $ git remote add upstream https://github.com/kbumsik/WolfieMouse

### Make new change input your repository

Check what you changed

> $ git status

Add your changes into staged mode

> $ git add {files-you-changed} or  $ git add --all

Double check change

> $ git status

Commit change

> $ git commit -m "some message. e.g. what you changed"

Push it to your repository

> $ git push origin master

Then you can make a pull request later


### Update your repo with `upstream`

* Update your local repo with `upstream`

First get information of `upstream`

> $ git fetch upstream

next,

> $ git pull upstream master

or

> $ git rebase upstream/master

Then

> $ git submodule update --recursive

* push it to `origin` repo, your forked repository

> $ git push origin master

### Useful git commands

* `git log --decorate --all --graph`
* `git fetch upstream master`
* `git stash` and `git stash pop`
* `git diff <commit>`

## KiCad-HOWTO

KiCad is one of the most famous EDA (Electronic Design Automation) software.
This is mainly developed and supported my CERN. It provides a full set of
PCB design tools, including shcematic, PCB footprint, and 3D model generation.
The best thing about KiCad is absolutly free.

However, it comes at a price. I mean it is not easy to get used to it.
Especially, there are a lot of _very_ tricky thing you would hardly figure out 
(seriously.) So I highly recommend going there some tutorials. By the way, do
not try to read manuals. It is not worth time :P

### Recommended Video Tutorials

* [(Reletivly) quick tutorial from scrach to making an actual device](https://contextualelectronics.com/learning/getting-to-blinky-4-0/)
* [Detail Youtube series](https://www.youtube.com/user/contextualelectronic/playlists)

### Other Tutorials

* [List of tutorials from KiCad official website](http://kicad-pcb.org/help/tutorials/)
