#!/bin/bash

set -eu

# For Vagrant provisioning - Check root and change to vagrant user
if [ $(id -u) = 0 ]; then
   USER='vagrant'
fi

export CONFIG_SHELL_RC_PATH=/home/vagrant/.zshrc
