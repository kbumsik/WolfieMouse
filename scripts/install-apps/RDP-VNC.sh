#!/bin/sh

# For Vagrant provisioning - Check root and change to vagrant user
if [ $(id -u) = 0 ]; then
   USER='vagrant'
fi

# RDP and desktop. Reference: https://peteris.rocks/blog/remote-desktop-and-vnc-on-ubuntu-server/

# Xfce desktop
# Install RDP server and xfce4
echo -e "\n\nInstalling RDP and Xfce..."
sudo apt-get install -y xfce4 tightvncserver xrdp
# Xstartup confiuration for Xfce
mkdir -p /home/$USER/.vnc/
cat > /home/$USER/.vnc/xstartup <<EOF
#!/bin/sh
startxfce4 &
EOF
chmod +x /home/$USER/.vnc/xstartup

# Configure VNC
echo wolfie | vncpasswd -f > /home/$USER/.vnc/passwd
chmod 600 /home/$USER/.vnc/passwd
sudo chown -R $USER:$USER /home/$USER/.vnc

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
touch /home/$USER/.Xauthority
mkdir -p /home/$USER/.config
sudo chown $USER:$USER /home/$USER/.Xauthority /home/$USER/.config

# Make a symbolic link for Desktop folder
#ln -s /home/$USER/wolfieMouse /home/$USER/Desktop/

# Fix ignoring tab key in Xfce
#sed /home/$USER/.config/xfce4/xfconf/xfce-perchannel-xml/xfce4-keyboard-shortcuts.xml -e 's:"switch_window_key":"empty":g' -i
# Change screen saver - default screen saver consumes a lot of resource
#xset s blank
