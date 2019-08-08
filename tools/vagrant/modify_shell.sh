#!/bin/sh

set -eu

# For Vagrant provisioning - Check root and change to vagrant user
if [ $(id -u) = 0 ]; then
   USER='vagrant'
fi

# append .bashrc
echo -e "\n\nConfiguring zsh..."
sudo apt-get update
sudo apt-get -y install fortune cowsay
# Set bashrc
cat >> /home/$USER/.zshrc <<'EOF'
# Change console folder colors shard with windows
LS_COLORS="ow=01;36;40" && export LS_COLORS

# Cowsay
fortune | cowsay
EOF
