#!/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/samuel/GNURadio/gr-adaptiveOFDM/python
export PATH=/home/samuel/GNURadio/gr-adaptiveOFDM/build/python:$PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH
export PYTHONPATH=/home/samuel/GNURadio/gr-adaptiveOFDM/build/swig:$PYTHONPATH
/usr/bin/python2 /home/samuel/GNURadio/gr-adaptiveOFDM/python/qa_adaptive_ofdm_tx_bc.py 
