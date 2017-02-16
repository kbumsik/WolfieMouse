# -*- mode: ruby -*-
# vi: set ft=ruby :

VB_NAME ||= "WolfieVM"
FOLDER_NAME ||= "WolfieMouse"

require 'fileutils'

kb_lib_vagrantfile = File.expand_path('source/kb_lib/Vagrantfile')
KB_LIB_BASE ||= "source/kb_lib/"

if File.exists?(kb_lib_vagrantfile)
    load kb_lib_vagrantfile
else
    abort "No Vagrantfile found in the source/kb_lib directory. 
          Did you do git pull --recurse-submodules? ABORTING!"
end

Vagrant.configure(2) do |config|
  config.vm.provider "virtualbox" do |vb|
    vb.name = VB_NAME
  end
  config.vm.synced_folder ".", "/home/vagrant/" + FOLDER_NAME
end