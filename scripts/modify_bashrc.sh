#!/bin/sh

# Set user first!
USER='vagrant'

# append .bashrc
echo -e "\n\nModifying .bashrc..."
sudo apt-get -y install fortune cowsay
# Quate EOF so that bash takes input literally
cat >> /home/$USER/.bashrc <<'EOF'      
# Change console folder colors shard with windows
LS_COLORS="ow=01;36;40" && export LS_COLORS

# Cowsay
echo -e "WolfieMouse says:\n\n"$(fortune) | cowsay
EOF

#echo -e '\n\n# Change console folder colors shard with windows\nLS_COLORS="ow=01;36;40" && export LS_COLORS\n\n# Cowsay\necho -e "WolfieMouse says:\n\n"$(fortune) | cowsay\n' >> /home/$USER/.bashrc
sed /home/$USER/.bashrc -e 's:\#force_color_prompt=yes:force_color_prompt=yes:g' -i
