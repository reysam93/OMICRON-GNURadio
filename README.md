# OMICRON - GNU Radio
This repository contains the modules and examples inside the OMICRON project, realized by the TSC of the URJC.
The aim of this experiments is to try the flexibility of SDR and GNU radio, realizing different types of applications and develop time and frequency adaptive transceivers.

## Installation
### Dependencies
#### GNU Radio 3.7
The version of GNU Radio required is the 3.7.10. Some components, especially from the OFDM folder may not work correctly with previous versions.
Instructions for installing it can be found [here](http://gnuradio.org/redmine/projects/gnuradio/wiki/InstallingGRFromSource).

#### gr-ieee802-11
The examples inside the WiFi folder are based on the gr-ieee802-11 repository for being compatible with the 802.11 standard. For this reason, it is necessary to install this modules and its dependencies. The instructions for doing this can be found [here](https://github.com/bastibl/gr-ieee802-11#installation).

### Installation of OMICRON-GNURadio
This steps will install the two modules that contains this repository. The explanation of this modules can be found later.
```
git clone https://github.com/reysam93/OMICRON-GNURadio.git
# installing gr-GNU_tutorials
cd gr-GNU_tutorials
mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig

# installing gr-adaptiveOFDM
cd gr-adaptiveOFDM
mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig
```

## gr-GNU_tutorials
This module allows to execute the file inside Tutorials folder. This contains examples that have been realized following the GNU Radio [guided tutorials](http://gnuradio.org/redmine/projects/gnuradio/wiki/Guided_Tutorials). This tutorial offers a first contact with GNU Radio and it is highly recommended for new users.

## gr-adaptiveOFDM
This module contains the blocks for implementing time adaptive modulation following the ieee 802.11 standard. Examples of this functionality can be found inside the WiFi folder. 
It is also necessary for executing the examples inside the OFDM folder.
