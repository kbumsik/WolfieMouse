#!/bin/sh

ln -s ~/wolfieMouse ~/Desktop/
sed ~/.config/xfce4/xfconf/xfce-perchannel-xml/xfce4-keyboard-shortcuts.xml -e 's:"switch_window_key":"empty":g' -i
xset s blank
