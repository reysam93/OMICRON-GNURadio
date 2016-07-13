#!/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/investigador/gr-tutorials/tutorial4/gr-cpp_tutorial/lib
export GR_CONF_CONTROLPORT_ON=False
export PATH=/home/investigador/gr-tutorials/tutorial4/gr-cpp_tutorial/build/lib:$PATH
export LD_LIBRARY_PATH=/home/investigador/gr-tutorials/tutorial4/gr-cpp_tutorial/build/lib:$LD_LIBRARY_PATH
export PYTHONPATH=$PYTHONPATH
test-cpp_tutorial 
