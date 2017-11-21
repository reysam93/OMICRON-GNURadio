/* -*- c++ -*- */
/*
 * Copyright 2016   Samuel Rey <samuel.rey.escudero@gmail.com>
 *                  Bastian Bloessl <bloessl@ccs-labs.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_FREQUENCYADAPTIVEOFDM_FRAME_EQUALIZER_IMPL_H
#define INCLUDED_FREQUENCYADAPTIVEOFDM_FRAME_EQUALIZER_IMPL_H

#include <frequencyAdaptiveOFDM/frame_equalizer.h>
#include <frequencyAdaptiveOFDM/constellations.h>
#include "equalizer/base.h"
#include "viterbi_decoder/viterbi_decoder.h"
#include <sys/time.h>
#include <fstream>

namespace gr {
  namespace frequencyAdaptiveOFDM {

    class frame_equalizer_impl : public frame_equalizer
    {
     public:
      frame_equalizer_impl(Equalizer algo, double freq, double bw, bool log, bool debug, bool debug_parity, char* delay_file);
      ~frame_equalizer_impl();

      void set_algorithm(Equalizer algo);
      void set_bandwidth(double bw);
      void set_frequency(double freq);

      void forecast (int noutput_items, gr_vector_int &ninput_items_required);
      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

    private:
      bool parse_signal(uint8_t *signal);
      bool decode_signal_field(uint8_t *rx_bits);

      equalizer::base *d_equalizer;
      gr::thread::mutex d_mutex;
      std::vector<gr::tag_t> tags;
      int  d_current_symbol;
      viterbi_decoder d_decoder;

      // freq offset
      double d_freq;  // Hz
      double d_freq_offset_from_synclong;  // Hz, estimation from "sync_long" block
      double d_bw;  // Hz
      double d_er;
      double d_epsilon0;
      gr_complex d_prev_pilots[4];

      int  d_frame_bytes;
      int  d_frame_symbols;
      std::vector<int>  d_frame_enc;
      int d_frame_punct;

      uint8_t d_deinterleaved[48];
      gr_complex symbols[48];

      boost::shared_ptr<gr::digital::constellation> d_frame_mod[4];
      constellation_bpsk::sptr d_bpsk;
      constellation_qpsk::sptr d_qpsk;
      constellation_16qam::sptr d_16qam;
      constellation_64qam::sptr d_64qam;

      timeval last_time;
      std::ofstream delay_fstream;

      // Debug
      bool d_debug;
      bool d_log;
      bool d_debug_parity;

      static int interleaver_pattern[48];
    };

  } // namespace frequencyAdaptiveOFDM
} // namespace gr

#endif /* INCLUDED_FREQUENCYADAPTIVEOFDM_FRAME_EQUALIZER_IMPL_H */
