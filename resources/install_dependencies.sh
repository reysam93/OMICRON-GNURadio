#!/bin/bash

if [ -d gr-adaptiveOFDM  ] && [ -d gr-frequencyAdaptiveOFDM ]; then
   echo "Running in root directory"
elif [ -d ../gr-adaptiveOFDM ] && [ -d ../gr-frequencyAdaptiveOFDM ]; then
	echo "Root directory found"
	cd ..
else
   echo "Error: files gr-adaptiveOFDM or gr-frequencyAdaptiveOFDM not found"
   echo "Please, make sure to run the script in the root folder of the project: OMICRON-GNURadio"
   echo "It is also posible run the script on its folder: resources"
   exit 1
fi

root_dir=`pwd`

echo "Installing dependencies:"
echo "   Installing log4cpp"
sudo apt-get install -y liblog4cpp5-dev

mkdir ../tmp/tmpDependenciesDir
cd /tmp/tmpDependenciesDir

echo "   Installing gr-foo"
git clone https://github.com/bastibl/gr-foo.git
cd gr-foo
git checkout master
mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig

echo "   Installing gr-ieee802-11"
git clone git://github.com/bastibl/gr-ieee802-11.git
cd gr-ieee802-11
mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig

echo "   Increasing maximum shared memory"
sudo sysctl -w kernel.shmmax=2147483648

echo "   Running volk_profile"
volk_profile

cd $root_dir
rm -rf /tmp/tmpDependenciesDir

echo "Setting project environment in .bashrc"
sed "/OMICRON_ROOT_DIR/d" ~/.bashrc > /tmp/bashrc.tmp
echo "# OMICRON_ROOT_DIR: variable with the path for the OMICRON project" >> /tmp/bashrc.tmp
echo "OMICRON_ROOT_DIR=$root_dir" >> /tmp/bashrc.tmp
mv /tmp/bashrc.tmp ~/.bashrc
rm /tmp/bashrc.tmp

echo "Installing gr-adaptiveOFDM module"
cd gr-adaptiveOFDM
sudo make build

echo "Installing gr-frequencyAdaptiveOFDM"
cd ../gr-frequencyAdaptiveOFDM
sudo make build

echo "Done!"