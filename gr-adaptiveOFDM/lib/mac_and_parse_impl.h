/* -*- c++ -*- */
/* 
 * Copyright 2016 Samuel Rey Escudero <samuel.rey.escudero@gmail.com>
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

#ifndef INCLUDED_ADAPTIVEOFDM_MAC_AND_PARSE_IMPL_H
#define INCLUDED_ADAPTIVEOFDM_MAC_AND_PARSE_IMPL_H

#include <adaptiveOFDM/mac_and_parse.h>

namespace gr {
  namespace adaptiveOFDM {

    class mac_and_parse_impl : public mac_and_parse
    {
     public:
      mac_and_parse_impl(std::vector<uint8_t> src_mac, std::vector<uint8_t> dst_mac, std::vector<uint8_t> bss_mac, bool log, bool debug);
      ~mac_and_parse_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace adaptiveOFDM
} // namespace gr

#endif /* INCLUDED_ADAPTIVEOFDM_MAC_AND_PARSE_IMPL_H */

