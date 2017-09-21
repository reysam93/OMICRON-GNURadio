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
#include "frequencyAdaptiveOFDM/constellations.h"
#include "frequencyAdaptiveOFDM/frame_equalizer.h"
#include "frequencyAdaptiveOFDM/decode_mac.h"
#include "frequencyAdaptiveOFDM/mac.h"
#include "frequencyAdaptiveOFDM/parse_mac.h"
#include "frequencyAdaptiveOFDM/rb_const_demux.h"
#include "frequencyAdaptiveOFDM/mac_and_parse.h"
%}

%include "gnuradio/digital/packet_header_default.h"
%ignore gr::digital::constellation_bpsk;
%ignore gr::digital::constellation_qpsk;
%ignore gr::digital::constellation_16qam;
%include "gnuradio/digital/constellation.h"
%include "frequencyAdaptiveOFDM/frame_equalizer.h"


%include "frequencyAdaptiveOFDM/mapper.h"
%include "frequencyAdaptiveOFDM/signal_field.h"
%include "frequencyAdaptiveOFDM/chunks_to_symbols.h"
%include "frequencyAdaptiveOFDM/constellations.h"
%include "frequencyAdaptiveOFDM/decode_mac.h"

GR_SWIG_BLOCK_MAGIC2(frequencyAdaptiveOFDM, mapper);
GR_SWIG_BLOCK_MAGIC2(frequencyAdaptiveOFDM, chunks_to_symbols);
GR_SWIG_BLOCK_MAGIC2(frequencyAdaptiveOFDM, frame_equalizer);
GR_SWIG_BLOCK_MAGIC2(frequencyAdaptiveOFDM, decode_mac);

%template(signal_field_sptr) boost::shared_ptr<gr::frequencyAdaptiveOFDM::signal_field>;
%pythoncode %{
signal_field_sptr.__repr__ = lambda self: "<signal_field>"
signal_field = signal_field.make;
%}

%template(constellation_bpsk_sptr) boost::shared_ptr<gr::frequencyAdaptiveOFDM::constellation_bpsk>;
%pythoncode %{
constellation_bpsk_sptr.__repr__ = lambda self: "<constellation BPSK>"
constellation_bpsk = constellation_bpsk.make;
%}



%include "frequencyAdaptiveOFDM/mac.h"
GR_SWIG_BLOCK_MAGIC2(frequencyAdaptiveOFDM, mac);
%include "frequencyAdaptiveOFDM/parse_mac.h"
GR_SWIG_BLOCK_MAGIC2(frequencyAdaptiveOFDM, parse_mac);
%include "frequencyAdaptiveOFDM/rb_const_demux.h"
GR_SWIG_BLOCK_MAGIC2(frequencyAdaptiveOFDM, rb_const_demux);
%include "frequencyAdaptiveOFDM/mac_and_parse.h"
GR_SWIG_BLOCK_MAGIC2(frequencyAdaptiveOFDM, mac_and_parse);
