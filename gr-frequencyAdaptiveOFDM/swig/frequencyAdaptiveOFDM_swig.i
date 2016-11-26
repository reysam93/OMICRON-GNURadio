/* -*- c++ -*- */

#define FREQUENCYADAPTIVEOFDM_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "frequencyAdaptiveOFDM_swig_doc.i"

%{
#include "frequencyAdaptiveOFDM/mapper.h"
#include "frequencyAdaptiveOFDM/signal_field.h"
%}


%include "frequencyAdaptiveOFDM/mapper.h"
GR_SWIG_BLOCK_MAGIC2(frequencyAdaptiveOFDM, mapper);
%include "frequencyAdaptiveOFDM/signal_field.h"
