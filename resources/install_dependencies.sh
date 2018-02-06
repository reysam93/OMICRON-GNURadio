#!/bin/bash
currentDir=`pwd`

echo "Installing dependencies"
echo "   Installing log4cpp"
sudo apt-get install -y liblog4cpp5-dev

cd /tmp
mkdir tmpDependenciesDir
cd tmpDependenciesDir

echo "   Installing gr-foo"
git clone https://github.com/bastibl/gr-foo.git
cd gr-foo
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

cd ..
rm -rf tmpDependenciesDir
cd $currentDir

echo "Done!"