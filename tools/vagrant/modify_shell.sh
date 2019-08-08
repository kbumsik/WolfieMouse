#!/bin/bash
source /home/vagrant/wolfiemouse/tools/vagrant/config.sh

# append .bashrc
echo -e "\n\nConfiguring zsh..."
sudo apt-get update
sudo apt-get -y install fortune cowsay
# Set bashrc
cat >> $CONFIG_SHELL_RC_PATH <<'EOF'
# Change console folder colors shard with windows
LS_COLORS="ow=01;36;40" && export LS_COLORS

# Cowsay
fortune | cowsay
EOF
