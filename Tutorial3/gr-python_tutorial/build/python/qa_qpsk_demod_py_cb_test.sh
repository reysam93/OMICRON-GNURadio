#!/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/samuelvm/GNURadio-Tutorials/Tutorial3/gr-python_tutorial/python
export GR_CONF_CONTROLPORT_ON=False
export PATH=/home/samuelvm/GNURadio-Tutorials/Tutorial3/gr-python_tutorial/build/python:$PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH
export PYTHONPATH=/home/samuelvm/GNURadio-Tutorials/Tutorial3/gr-python_tutorial/build/swig:$PYTHONPATH
/usr/bin/python2 /home/samuelvm/GNURadio-Tutorials/Tutorial3/gr-python_tutorial/python/qa_qpsk_demod_py_cb.py 
