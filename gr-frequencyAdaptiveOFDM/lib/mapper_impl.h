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


#ifndef INCLUDED_FREQUENCYADAPTIVEOFDM_MAPPER_IMPL_H
#define INCLUDED_FREQUENCYADAPTIVEOFDM_MAPPER_IMPL_H

#include <frequencyAdaptiveOFDM/mapper.h>
#include "utils.h"

namespace gr {
  namespace frequencyAdaptiveOFDM {

    class mapper_impl : public mapper
    {
    private:
      bool d_debug;
      char* d_symbols;
      int d_symbols_offset;
      int d_symbols_len;
      ofdm_param d_ofdm;
      gr::thread::mutex d_mutex;


    public:
      mapper_impl(Encoding encoding, bool debug);
      ~mapper_impl();

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

      void print_message(const char *msg, size_t len);
      void set_encoding(Encoding encoding);
    };


  } // namespace frequencyAdaptiveOFDM
} // namespace gr

#endif /* INCLUDED_FREQUENCYADAPTIVEOFDM_MAPPER_IMPL_H */

