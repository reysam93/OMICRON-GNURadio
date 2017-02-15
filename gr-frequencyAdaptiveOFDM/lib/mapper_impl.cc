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

#include <gnuradio/io_signature.h>
#include "mapper_impl.h"


namespace gr {
  namespace frequencyAdaptiveOFDM {

    mapper::sptr
    mapper::make(std::vector<int> pilots_enc, bool debug)
    {
      return gnuradio::get_initial_sptr
        (new mapper_impl(pilots_enc, debug));
    }

    mapper_impl::mapper_impl(std::vector<int> pilots_enc, bool debug)
      : gr::block("mapper",
          gr::io_signature::make(0, 0, 0),
          gr::io_signature::make(1, 1, sizeof(char))),
          d_symbols_offset(0),
          d_symbols(NULL),
          d_debug(debug),
          d_ofdm(pilots_enc)
    {
      message_port_register_in(pmt::mp("in"));
      set_encoding(pilots_enc);
    }

    mapper_impl::~mapper_impl()
    {
      free(d_symbols);
    }

    void 
    mapper_impl::print_message(const char *msg, size_t len) {


      dout << std::hex << "MAPPER input symbols" << std::endl
        << "===========================" << std::endl;

      for(int i = 0; i < len; i++) {
        dout << std::hex << (int)msg[i] << " ";
      }

      dout << std::dec << std::endl;
    }

    int 
    mapper_impl::general_work(int noutput, gr_vector_int& ninput_items,
          gr_vector_const_void_star& input_items,
          gr_vector_void_star& output_items ) {

      unsigned char *out = (unsigned char*)output_items[0];

      while(!d_symbols_offset) {
        pmt::pmt_t msg(delete_head_nowait(pmt::intern("in")));

        if(!msg.get()) {
          return 0;
        }

        if(pmt::is_pair(msg)) {
          dout << "MAPPER: received new message" << std::endl;
          gr::thread::scoped_lock lock(d_mutex);

          int psdu_length = pmt::blob_length(pmt::cdr(msg));
          const char *psdu = static_cast<const char*>(pmt::blob_data(pmt::cdr(msg)));

          // ############ INSERT MAC STUFF
          frame_param frame(d_ofdm, psdu_length);

          if(frame.n_sym > MAX_SYM) {
            std::cerr << "ERROR: MAPPER: packet too large, maximum number of symbols is " << MAX_SYM << std::endl;
            return 0;
          }

          //alloc memory for modulation steps
          char *data_bits        = (char*)calloc(frame.n_data_bits, sizeof(char));
          char *scrambled_data   = (char*)calloc(frame.n_data_bits, sizeof(char));
          char *encoded_data     = (char*)calloc(frame.n_data_bits * 2, sizeof(char));
          char *punctured_data   = (char*)calloc(frame.n_encoded_bits, sizeof(char));
          char *interleaved_data = (char*)calloc(frame.n_encoded_bits, sizeof(char));
          char *symbols          = (char*)calloc((frame.n_encoded_bits / d_ofdm.n_bpsc), sizeof(char));

          //generate the WIFI data field, adding service field and pad bits
          generate_bits(psdu, data_bits, frame);

          // scrambling
          static uint8_t scrambler = 1;
          scramble(data_bits, scrambled_data, frame, scrambler++);
          if(scrambler > 127) {
            scrambler = 1;
          }

          // reset tail bits
          reset_tail_bits(scrambled_data, frame);
          // encoding
          convolutional_encoding(scrambled_data, encoded_data, frame);
          // puncturing
          //puncturing(encoded_data, punctured_data, frame, d_ofdm);
          
          // interleaving
          interleave(encoded_data, interleaved_data, frame, d_ofdm);

          // one byte per symbol
          split_symbols(interleaved_data, symbols, frame, d_ofdm);

          d_symbols_len = frame.n_sym * 48;

          d_symbols = (char*)calloc(d_symbols_len, 1);
          std::memcpy(d_symbols, symbols, d_symbols_len);


          // add tags
          pmt::pmt_t key = pmt::string_to_symbol("packet_len");
          pmt::pmt_t value = pmt::from_long(d_symbols_len);
          pmt::pmt_t srcid = pmt::string_to_symbol(alias());
          add_item_tag(0, nitems_written(0), key, value, srcid);

          pmt::pmt_t psdu_bytes = pmt::from_long(psdu_length);
          add_item_tag(0, nitems_written(0), pmt::mp("psdu_len"),
              psdu_bytes, srcid);

          pmt::pmt_t encoding = pmt::init_s32vector(4, d_ofdm.resource_blocks_e);
          add_item_tag(0, nitems_written(0), pmt::mp("encoding"),
              encoding, srcid);


          free(data_bits);
          free(scrambled_data);
          free(encoded_data);
          free(punctured_data);
          free(interleaved_data);
          free(symbols);

          break;
        }
      }

      int i = std::min(noutput, d_symbols_len - d_symbols_offset);
      std::memcpy(out, d_symbols + d_symbols_offset, i);
      d_symbols_offset += i;

      if(d_symbols_offset == d_symbols_len) {
        d_symbols_offset = 0;
        free(d_symbols);
        d_symbols = 0;
      }

      return i;
    }

    void 
    mapper_impl::set_encoding(std::vector<int> pilots_enc) {
      if (d_debug){
        dout << "MAPPER ENCODDING: ";
        d_ofdm.print();
      }

      gr::thread::scoped_lock lock(d_mutex);
      d_ofdm = ofdm_param(pilots_enc);
    }

  } /* namespace frequencyAdaptiveOFDM */
} /* namespace gr */

