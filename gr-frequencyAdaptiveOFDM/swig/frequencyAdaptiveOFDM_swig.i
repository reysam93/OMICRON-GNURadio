/* -*- c++ -*- */

#define FREQUENCYADAPTIVEOFDM_API
#define DIGITAL_API


%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "frequencyAdaptiveOFDM_swig_doc.i"

%{
#include "frequencyAdaptiveOFDM/mapper.h"
#include "frequencyAdaptiveOFDM/signal_field.h"
#include "frequencyAdaptiveOFDM/chunks_to_symbols.h"
%}

%include "gnuradio/digital/packet_header_default.h"

%include "frequencyAdaptiveOFDM/mapper.h"
GR_SWIG_BLOCK_MAGIC2(frequencyAdaptiveOFDM, mapper);
%include "frequencyAdaptiveOFDM/signal_field.h"

%template(signal_field_sptr) boost::shared_ptr<gr::frequencyAdaptiveOFDM::signal_field>;
%pythoncode %{
signal_field_sptr.__repr__ = lambda self: "<signal_field>"
signal_field = signal_field.make;
%}

%include "frequencyAdaptiveOFDM/chunks_to_symbols.h"
GR_SWIG_BLOCK_MAGIC2(frequencyAdaptiveOFDM, chunks_to_symbols);
