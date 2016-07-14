/* -*- c++ -*- */

#define GNU_TUTORIALS_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "GNU_tutorials_swig_doc.i"

%{
#include "GNU_tutorials/my_qpsk_demod_cb.h"
%}


%include "GNU_tutorials/my_qpsk_demod_cb.h"
GR_SWIG_BLOCK_MAGIC2(GNU_tutorials, my_qpsk_demod_cb);
