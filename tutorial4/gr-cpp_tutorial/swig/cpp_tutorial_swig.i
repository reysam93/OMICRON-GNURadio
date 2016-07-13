/* -*- c++ -*- */

#define CPP_TUTORIAL_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "cpp_tutorial_swig_doc.i"

%{
#include "cpp_tutorial/my_qpsk_demod_cb.h"
%}


%include "cpp_tutorial/my_qpsk_demod_cb.h"
GR_SWIG_BLOCK_MAGIC2(cpp_tutorial, my_qpsk_demod_cb);
