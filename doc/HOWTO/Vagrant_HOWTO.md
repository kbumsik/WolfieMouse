# Vagrant-HOWTO

Vagrant is a tool for Virtual Machine. Vagrant abstract virtual machine interfaces
and will automate system configuration of a VM, so we will have a unified 
working environment.

## Prerequisites

See `Install tools` section in [Get Started Document](../Get-started.md). Don't
forget to add `Git` directories in `PATH`.

## How to boot a vagrant VM

The following command will boot up the VM in background:

> $ vagrant up

If you run the above code for the first time, it will install the operating
system and the software we use in the project. This initial process will take
around 20 minuetes so have a cup of coffee.

## How to connect a terminal(SSH) to the VM

We can connect the virtual machine through SSH(remote terminal). The defualt
configuration to connect is:

* Address: 127.0.0.1
* Port: 2222
* User: vagrant

## How to close the VM

Simple, just commend:

> $ vagrant halt

## How to delete the VM

This is also simple. Run:

> $ vagrant destroy

You can re-install the VM using `$ vagrant up` any time.

## Troubleshooting

1. It says we need to install `SSH`: Don't forget add `Git` directories
(`{your-git-installation-path}/Git/bin` and `{your-git-installation-path}/Git/usr/bin`)
in `PATH` environment variable. Git has SSH tools for you.
2. Timeout error : Firstly, check if your CPU supports virtualization (VT-x).
If it doesn't you cannot use the VM and Vagrant. Consider installing Ubuntu in
a separated partition. If your CPU supports VT-x but you still have that problem,
the BIOS might disabled the CPU virtualization feature. Go to BIOS and enable it.
