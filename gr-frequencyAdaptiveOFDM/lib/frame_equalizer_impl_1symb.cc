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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "frame_equalizer_impl.h"
#include "equalizer/base.h"
#include "equalizer/comb.h"
#include "equalizer/lms.h"
#include "equalizer/ls.h"
#include "equalizer/sta.h"
#include "utils.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace frequencyAdaptiveOFDM {

    frame_equalizer::sptr
    frame_equalizer::make(Equalizer algo, double freq, double bw, bool log, bool debug, bool debug_parity, char* delay_file) {
      return gnuradio::get_initial_sptr
        (new frame_equalizer_impl(algo, freq, bw, log, debug, debug_parity, delay_file));
    }


    frame_equalizer_impl::frame_equalizer_impl(Equalizer algo, double freq, double bw, bool log,
                                                bool debug, bool debug_parity, char* delay_file) :
      gr::block("frame_equalizer",
          gr::io_signature::make(1, 1, 64 * sizeof(gr_complex)),
          gr::io_signature::make(1, 1, 48)),
      d_current_symbol(0), d_log(log), d_debug(debug), d_debug_parity(debug_parity),
      d_equalizer(NULL), d_freq(freq), d_bw(bw), d_frame_bytes(0), d_frame_symbols(0),
      d_freq_offset_from_synclong(0.0), d_frame_enc(4, BPSK), d_frame_punct(P_1_2) {

      message_port_register_out(pmt::mp("symbols"));

      d_bpsk = constellation_bpsk::make();
      d_qpsk = constellation_qpsk::make();
      d_16qam = constellation_16qam::make();
      d_64qam = constellation_64qam::make();

      d_frame_mod[0] = d_bpsk;
      d_frame_mod[1] = d_bpsk;
      d_frame_mod[2] = d_bpsk;
      d_frame_mod[3] = d_bpsk;

      set_tag_propagation_policy(block::TPP_DONT);
      set_algorithm(algo);

      gettimeofday(&last_time, NULL);
      if(delay_file != ""){
        delay_fstream.open(delay_file, std::ofstream::out);
      }
    }

    frame_equalizer_impl::~frame_equalizer_impl() {
      if (delay_fstream.is_open()) {
        delay_fstream.close();
      }
    }


    void
    frame_equalizer_impl::set_algorithm(Equalizer algo) {
      gr::thread::scoped_lock lock(d_mutex);
      delete d_equalizer;
      dout << "FRAME EQ: Algorithm set to: ";
      switch(algo) {

      case COMB:
        dout << "Comb" << std::endl;
        d_equalizer = new equalizer::comb();
        break;
      case LS:
        dout << "LS" << std::endl;
        d_equalizer = new equalizer::ls();
        break;
      case LMS:
        dout << "LMS" << std::endl;
        d_equalizer = new equalizer::lms();
        break;
      case STA:
        dout << "STA" << std::endl;
        d_equalizer = new equalizer::sta();
        break;
      default:
        throw std::runtime_error("Algorithm not implemented");
      }
    }

    void
    frame_equalizer_impl::set_bandwidth(double bw) {
      gr::thread::scoped_lock lock(d_mutex);
      d_bw = bw;
    }

    void
    frame_equalizer_impl::set_frequency(double freq) {
      gr::thread::scoped_lock lock(d_mutex);
      d_freq = freq;
    }

    void
    frame_equalizer_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required) {
      ninput_items_required[0] = noutput_items;
    }

    int
    frame_equalizer_impl::general_work (int noutput_items,
        gr_vector_int &ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items) {

      gr::thread::scoped_lock lock(d_mutex);

      const gr_complex *in = (const gr_complex *) input_items[0];
      uint8_t *out = (uint8_t *) output_items[0];

      int i = 0;
      int o = 0;
      gr_complex symbols[48];
      gr_complex current_symbol[64];

      while((i < ninput_items[0]) && (o < noutput_items)) {
        // delay in ms
        float delay;
        timeval time_now;
        bool new_frame = false;

        get_tags_in_window(tags, 0, i, i + 1, pmt::string_to_symbol("wifi_start"));

        // new frame
        if(tags.size()) {
          d_current_symbol = 0;
          d_frame_symbols = 0;
          d_frame_mod[0] = d_bpsk;
          d_frame_mod[1] = d_bpsk;
          d_frame_mod[2] = d_bpsk;
          d_frame_mod[3] = d_bpsk;

          d_freq_offset_from_synclong = pmt::to_double(tags.front().value) * d_bw / (2 * M_PI);
          d_epsilon0 = pmt::to_double(tags.front().value) * d_bw / (2 * M_PI * d_freq);
          d_er = 0;
          new_frame = true;

          dout << "FRAME EQ: new frame. Epsilon: " << d_epsilon0 << std::endl;
        }

        // not interesting -> skip
        if(d_current_symbol > (d_frame_symbols + 2)) {
          i++;
          continue;
        }

        gettimeofday(&time_now, NULL);
        delay = (time_now.tv_sec - last_time.tv_sec)*1000.0 + (time_now.tv_usec - last_time.tv_usec)/1000.0;
        last_time = time_now;
        if(delay_fstream.is_open()){
          delay_fstream << d_current_symbol << ", " << delay << ", " << new_frame << "\n";
        }
        if(d_log){
          std::cout << "OFDM symbol " << d_current_symbol << " delay: " << delay << " ms. New frame: " << new_frame << "\n";
        }

        std::memcpy(current_symbol, in + i*64, 64*sizeof(gr_complex));

        // compensate sampling offset
        for(int j = 0; j < 64; j++) {
          current_symbol[j] *= exp(gr_complex(0, 2*M_PI*d_current_symbol*80*(d_epsilon0 + d_er)*(j-32)/64));
        }

        gr_complex p = equalizer::base::POLARITY[(d_current_symbol - 2) % 127];
        gr_complex sum =
          (current_symbol[11] *  p) +
          (current_symbol[25] *  p) +
          (current_symbol[39] *  p) +
          (current_symbol[53] * -p);

        double beta;
        if(d_current_symbol < 2) {
          beta = arg(
              current_symbol[11] -
              current_symbol[25] +
              current_symbol[39] +
              current_symbol[53]);

        } else {
          beta = arg(
              (current_symbol[11] *  p) +
              (current_symbol[39] *  p) +
              (current_symbol[25] *  p) +
              (current_symbol[53] * -p));
        }

        double er = arg(
            (conj(d_prev_pilots[0]) * current_symbol[11] *  p) +
            (conj(d_prev_pilots[1]) * current_symbol[25] *  p) +
            (conj(d_prev_pilots[2]) * current_symbol[39] *  p) +
            (conj(d_prev_pilots[3]) * current_symbol[53] * -p));

        er *= d_bw / (2 * M_PI * d_freq * 80);

        d_prev_pilots[0] = current_symbol[11] *  p;
        d_prev_pilots[1] = current_symbol[25] *  p;
        d_prev_pilots[2] = current_symbol[39] *  p;
        d_prev_pilots[3] = current_symbol[53] * -p;

        // compensate residual frequency offset
        for(int j = 0; j < 64; j++) {
          current_symbol[j] *= exp(gr_complex(0, -beta));
        }

        // update estimate of residual frequency offset
        if(d_current_symbol >= 2) {
          double alpha = 0.1;
          d_er = (1-alpha) * d_er + alpha * er;
        }

        // do equalization
        d_equalizer->equalize(current_symbol, d_current_symbol,
            symbols, out + 48, d_frame_mod);

        // signal field
        if(d_current_symbol == 2) {
          if(decode_signal_field(out + o * 48)) {
            if (d_debug){
              std::cout << "FRAME EQ: frame coding:\n";
              ofdm_param ofdm(d_frame_enc, d_frame_punct);
              ofdm.print();
            }

            pmt::pmt_t dict = pmt::make_dict();
            dict = pmt::dict_add(dict, pmt::mp("frame_bytes"), pmt::from_uint64(d_frame_bytes));
            dict = pmt::dict_add(dict, pmt::mp("encoding"), pmt::init_s32vector(4, d_frame_enc));
            dict = pmt::dict_add(dict, pmt::mp("puncturing"), pmt::from_long(d_frame_punct));
            //dict = pmt::dict_add(dict, pmt::mp("snr"), pmt::init_f64vector(4, d_equalizer->resource_blocks_snr()));
            dict = pmt::dict_add(dict, pmt::mp("snr"), pmt::init_f64vector(4, d_equalizer->min_rb_snr()));
            dict = pmt::dict_add(dict, pmt::mp("freq"), pmt::from_double(d_freq));
            dict = pmt::dict_add(dict, pmt::mp("freq_offset"), pmt::from_double(d_freq_offset_from_synclong));
            add_item_tag(0, nitems_written(0) + o,
                pmt::string_to_symbol("wifi_start"),
                dict,
                pmt::string_to_symbol(alias()));
          }
        }
        if(d_current_symbol > 2) {
          o++;
          pmt::pmt_t pdu = pmt::make_dict();
          message_port_pub(pmt::mp("symbols"), pmt::cons(pmt::make_dict(), pmt::init_c32vector(48, symbols)));
        }
        i++;
        d_current_symbol++;
      }
      consume(0, i);
      return o;
    }

    bool
    frame_equalizer_impl::decode_signal_field(uint8_t *rx_bits) {
      std::vector<int> resource_block_e (4, BPSK);
      static ofdm_param ofdm(resource_block_e, P_1_2);
      static frame_param frame(ofdm, 0);
      //frame.to_header_param();

      interleave((char*)rx_bits, (char*)d_deinterleaved, frame, ofdm, true);
      uint8_t *decoded_bits = d_decoder.decode(&ofdm, &frame, d_deinterleaved);
      return parse_signal(decoded_bits);
    }

    bool
    frame_equalizer_impl::parse_signal(uint8_t *decoded_bits) {
      for (int i = 0; i < 4; i++){
        d_frame_enc[i] = 0;
      }
      d_frame_punct = P_1_2;

      d_frame_bytes = 0;
      bool parity = false;
      for(int i = 0; i < 21; i++) {
        parity ^= decoded_bits[i];

        if((i < 2) && decoded_bits[i]) {
          d_frame_enc[0] = d_frame_enc[0] | (1 << i);
        }else if(i < 4 && decoded_bits[i]){
          d_frame_enc[1] = d_frame_enc[1] | (1 << (i-2));
        }else if(i < 6 && decoded_bits[i]){
          d_frame_enc[2] = d_frame_enc[2] | (1 << (i-4));
        }else if(i < 8 && decoded_bits[i]){
          d_frame_enc[3] = d_frame_enc[3] | (1 << (i-6));
        }else if (i == 8) {
          d_frame_punct = decoded_bits[i];
        }

        if(decoded_bits[i] && (i > 8) && (i < 21)) {
          d_frame_bytes = d_frame_bytes | (1 << (i-9));
        }
      }

      if(parity != decoded_bits[21]) {
        if (d_debug || d_debug_parity){
          std::cout << "WARNING: FRAME EQUALIZER: wrong parity.\n";
        }
        //return false;
      }

      bool all_mod_64QAM = true;
      for(int i = 0; i < 4; i++){
        if(d_frame_enc[i] == BPSK){
          all_mod_64QAM = false;
          d_frame_mod[i] = d_bpsk;
        }else if(d_frame_enc[i] == QPSK){
          all_mod_64QAM = false;
          d_frame_mod[i] = d_qpsk;
        }else if(d_frame_enc[i] == QAM16){
          all_mod_64QAM = false;
          d_frame_mod[i] = d_16qam;
        }else if(d_frame_enc[i] == QAM64){
          d_frame_mod[i] = d_64qam;
        }else{
          std::cerr << "ERROR: FRAME EQUALIZER: wrong modulation found.\n";
          return false;
        }
      }


      if (all_mod_64QAM && d_frame_punct == P_1_2) {
        d_frame_punct = P_2_3;
      }


      ofdm_param ofdm_received(d_frame_enc, d_frame_punct);
      frame_param frame_received(ofdm_received, d_frame_bytes);
      d_frame_symbols = frame_received.n_sym;
      return true;
    }

    int
    frame_equalizer_impl::interleaver_pattern[48] = {
       0, 3, 6, 9,12,15,18,21,
      24,27,30,33,36,39,42,45,
       1, 4, 7,10,13,16,19,22,
      25,28,31,34,37,40,43,46,
       2, 5, 8,11,14,17,20,23,
      26,29,32,35,38,41,44,47
    };
  } /* namespace frequencyAdaptiveOFDM */
} /* namespace gr */
