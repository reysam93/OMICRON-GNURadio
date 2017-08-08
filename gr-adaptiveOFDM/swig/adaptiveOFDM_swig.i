/* -*- c++ -*- */

#define ADAPTIVEOFDM_API
#define DIGITAL_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "adaptiveOFDM_swig_doc.i"

%{
#include "adaptiveOFDM/mapper.h"
#include "adaptiveOFDM/parse_mac.h"
#include "adaptiveOFDM/mac.h"
#include "adaptiveOFDM/mac_and_parse.h"
#include "adaptiveOFDM/constellations.h"
#include "adaptiveOFDM/frame_equalizer.h"
%}

//%include "gnuradio/digital/packet_header_default.h"
%ignore gr::digital::constellation_bpsk;
%ignore gr::digital::constellation_qpsk;
%ignore gr::digital::constellation_16qam;
%include "gnuradio/digital/constellation.h"

%include "adaptiveOFDM/mapper.h"
%include "adaptiveOFDM/parse_mac.h"
%include "adaptiveOFDM/mac.h"
%include "adaptiveOFDM/mac_and_parse.h"
%include "adaptiveOFDM/constellations.h"
%include "adaptiveOFDM/frame_equalizer.h"

GR_SWIG_BLOCK_MAGIC2(adaptiveOFDM, mapper);
GR_SWIG_BLOCK_MAGIC2(adaptiveOFDM, parse_mac);
GR_SWIG_BLOCK_MAGIC2(adaptiveOFDM, mac);
GR_SWIG_BLOCK_MAGIC2(adaptiveOFDM, mac_and_parse);
GR_SWIG_BLOCK_MAGIC2(adaptiveOFDM, frame_equalizer);

%template(constellation_bpsk_sptr) boost::shared_ptr<gr::adaptiveOFDM::constellation_bpsk>;
%pythoncode %{
constellation_bpsk_sptr.__repr__ = lambda self: "<constellation BPSK>"
constellation_bpsk = constellation_bpsk.make;
%}

%template(constellation_qpsk_sptr) boost::shared_ptr<gr::adaptiveOFDM::constellation_qpsk>;
%pythoncode %{
constellation_qpsk_sptr.__repr__ = lambda self: "<constellation QPSK>"
constellation_qpsk = constellation_qpsk.make;
%}

%template(constellation_16qam_sptr) boost::shared_ptr<gr::adaptiveOFDM::constellation_16qam>;
%pythoncode %{
constellation_16qam_sptr.__repr__ = lambda self: "<constellation 16QAM>"
constellation_16qam = constellation_16qam.make;
%}

%template(constellation_64qam_sptr) boost::shared_ptr<gr::adaptiveOFDM::constellation_64qam>;
%pythoncode %{
constellation_64qam_sptr.__repr__ = lambda self: "<constellation 64QAM>"
constellation_64qam = constellation_64qam.make;
%}