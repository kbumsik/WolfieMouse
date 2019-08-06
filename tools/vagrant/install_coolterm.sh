#!/bin/bash

set -eu

sudo apt-get install -y wget unzip
sudo apt-get install -y libgtk-3-0 libunwind-dev

# Install CooltermLinux under /opt
wget -nv http://freeware.the-meiers.org/CoolTermLinux.zip
echo 'e7ed1424c1bd8e1fec77a7f864867fd74f9f00cc85cad1684623233c3f268622  CoolTermLinux.zip' \
    | sha256sum --check -
sudo mkdir -p /opt 2>&1
sudo unzip -d /opt CoolTermLinux.zip
rm CoolTermLinux.zip

# Create coolterm command
sudo cat >> /usr/local/bin/coolterm <<'EOF'
#!/bin/sh
exec /opt/CoolTermLinux/Coolterm
EOF
sudo chmod +x /usr/local/bin/coolterm
