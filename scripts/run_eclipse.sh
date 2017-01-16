#!/bin/sh

# Eclipse Mars hangs with Ubuntu 16.04 unless we disable GTK3.
# https://bugs.launchpad.net/ubuntu/+source/java-common/+bug/1552764
export SWT_GTK3=0

# Check eclipse.ini setting
if grep -e '--launcher.GTK_version 2' /opt/eclipse_gnuarm/eclipse.ini > /dev/null
then
    :
else
    sudo sed /opt/eclipse_gnuarm/eclipse.ini -e 's/--launcher.appendVmargs/--launcher.GTK_version 2\n--launcher.appendVmargs/g' -i
fi

# Execute
/opt/eclipse_gnuarm/eclipse
