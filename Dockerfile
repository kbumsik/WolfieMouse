FROM ubuntu:16.04

# Install Atollic TrueSTUDIO
RUN apt-get update && apt-get -y install wget
RUN cd /opt && wget https://dl.dropboxusercontent.com/u/78819851/Atollic_TrueSTUDIO_for_ARM_linux_x86_64_v7.0.1_20161219-1526-alpha1.tar.gz
RUN sudo tar -C /opt -xvf https://dl.dropboxusercontent.com/u/78819851/Atollic_TrueSTUDIO_for_ARM_linux_x86_64_v7.0.1_20161219-1526-alpha1.tar.gz
RUN cd /opt/Atollic_TrueSTUDIO_for_ARM_7.0.1 && yes 1 | sudo ./install.sh

# Set up graphical interface

