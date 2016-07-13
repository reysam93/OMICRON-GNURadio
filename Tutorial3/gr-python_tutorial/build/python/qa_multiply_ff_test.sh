#!/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/investigador/gr-tutorials/tutorial3/gr-python_tutorial/python
export GR_CONF_CONTROLPORT_ON=False
export PATH=/home/investigador/gr-tutorials/tutorial3/gr-python_tutorial/build/python:$PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH
export PYTHONPATH=/home/investigador/gr-tutorials/tutorial3/gr-python_tutorial/build/swig:$PYTHONPATH
/usr/bin/python2 /home/investigador/gr-tutorials/tutorial3/gr-python_tutorial/python/qa_multiply_ff.py 
