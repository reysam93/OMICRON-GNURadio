#!/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/investigador/gr-tutorials/tutorial4/gr-cpp_tutorial/python
export GR_CONF_CONTROLPORT_ON=False
export PATH=/home/investigador/gr-tutorials/tutorial4/gr-cpp_tutorial/build/python:$PATH
export LD_LIBRARY_PATH=/home/investigador/gr-tutorials/tutorial4/gr-cpp_tutorial/build/lib:$LD_LIBRARY_PATH
export PYTHONPATH=/home/investigador/gr-tutorials/tutorial4/gr-cpp_tutorial/build/swig:$PYTHONPATH
/usr/bin/python2 /home/investigador/gr-tutorials/tutorial4/gr-cpp_tutorial/python/qa_my_qpsk_demod_cb.py 
