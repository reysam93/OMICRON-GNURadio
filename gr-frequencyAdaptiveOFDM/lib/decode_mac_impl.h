/* -*- c++ -*- */
/* 
 * Copyright 2016 Samuel Rey <samuel.rey.escudero@gmail.com>
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

#ifndef INCLUDED_FREQUENCYADAPTIVEOFDM_DECODE_MAC_IMPL_H
#define INCLUDED_FREQUENCYADAPTIVEOFDM_DECODE_MAC_IMPL_H

#include <frequencyAdaptiveOFDM/decode_mac.h>

#include "utils.h"
#include "viterbi_decoder/viterbi_decoder.h"

namespace gr {
  namespace frequencyAdaptiveOFDM {

    class decode_mac_impl : public decode_mac
    {
     private:
      bool d_debug;
      bool d_log;
      bool d_debug_checksum;

      frame_param d_frame;
      ofdm_param d_ofdm;
      std::vector<double> d_snr;  // dB
      double d_nom_freq;  // nominal frequency, Hz
      double d_freq_offset;  // frequency offset, Hz
      viterbi_decoder d_decoder;

      uint8_t d_rx_symbols[48 * MAX_SYM];
      uint8_t d_rx_bits[MAX_ENCODED_BITS];
      uint8_t d_deinterleaved_bits[MAX_ENCODED_BITS];
      uint8_t out_bytes[MAX_PSDU_SIZE + 2]; // 2 for signal field

      int copied;
      bool d_frame_complete;

     public:
      decode_mac_impl(bool log, bool debug, bool debug_checksum);
      ~decode_mac_impl();


      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

      void regroup_symbols();
      void decode();
      void deinterleave();
      void descramble (uint8_t *decoded_bits);
      void print_output();
    };

  } // namespace frequencyAdaptiveOFDM
} // namespace gr

#endif /* INCLUDED_FREQUENCYADAPTIVEOFDM_DECODE_MAC_IMPL_H */

