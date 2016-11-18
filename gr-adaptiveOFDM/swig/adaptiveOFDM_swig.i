/* -*- c++ -*- */

#define ADAPTIVEOFDM_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "adaptiveOFDM_swig_doc.i"

%{
#include "adaptiveOFDM/parse_mac.h"
#include "adaptiveOFDM/mac.h"
#include "adaptiveOFDM/mac_and_parse.h"
%}


%include "adaptiveOFDM/parse_mac.h"
GR_SWIG_BLOCK_MAGIC2(adaptiveOFDM, parse_mac);
%include "adaptiveOFDM/mac.h"
GR_SWIG_BLOCK_MAGIC2(adaptiveOFDM, mac);%include "adaptiveOFDM/mac_and_parse.h"
GR_SWIG_BLOCK_MAGIC2(adaptiveOFDM, mac_and_parse);
