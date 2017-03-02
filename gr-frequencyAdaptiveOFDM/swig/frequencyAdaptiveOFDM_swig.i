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
#include "frequencyAdaptiveOFDM/mac_and_parse.h"
#include "frequencyAdaptiveOFDM/rb_const_demux.h"
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

%template(constellation_qpsk_sptr) boost::shared_ptr<gr::frequencyAdaptiveOFDM::constellation_qpsk>;
%pythoncode %{
constellation_qpsk_sptr.__repr__ = lambda self: "<constellation QPSK>"
constellation_qpsk = constellation_qpsk.make;
%}

%template(constellation_16qam_sptr) boost::shared_ptr<gr::frequencyAdaptiveOFDM::constellation_16qam>;
%pythoncode %{
constellation_16qam_sptr.__repr__ = lambda self: "<constellation 16QAM>"
constellation_16qam = constellation_16qam.make;
%}

%template(constellation_64qam_sptr) boost::shared_ptr<gr::frequencyAdaptiveOFDM::constellation_64qam>;
%pythoncode %{
constellation_64qam_sptr.__repr__ = lambda self: "<constellation 64QAM>"
constellation_64qam = constellation_64qam.make;
%}


%include "frequencyAdaptiveOFDM/mac_and_parse.h"
GR_SWIG_BLOCK_MAGIC2(frequencyAdaptiveOFDM, mac_and_parse);
%include "frequencyAdaptiveOFDM/rb_const_demux.h"
GR_SWIG_BLOCK_MAGIC2(frequencyAdaptiveOFDM, rb_const_demux);
