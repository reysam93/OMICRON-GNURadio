/* -*- c++ -*- */
/* 
 * Copyright 2017 <+YOU OR YOUR COMPANY+>.
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

#ifndef INCLUDED_FREQUENCYADAPTIVEOFDM_MAC_AND_PARSE_IMPL_H
#define INCLUDED_FREQUENCYADAPTIVEOFDM_MAC_AND_PARSE_IMPL_H

#include <frequencyAdaptiveOFDM/mac_and_parse.h>
#include "utils.h"

namespace gr {
  namespace frequencyAdaptiveOFDM {

    class mac_and_parse_impl : public mac_and_parse
    {
     public:
      mac_and_parse_impl(std::vector<uint8_t> src_mac, std::vector<uint8_t> dst_mac, std::vector<uint8_t> bss_mac, bool log, bool debug);
      ~mac_and_parse_impl();
      void app_in (pmt::pmt_t msg);
      void send_message(int psdu_length);
      void generate_mac_data_frame(const char *msdu, int msdu_size, int *psdu_size);
      void generate_mac_ack_frame(uint8_t ra[], int *psdu_size);
      void phy_in (pmt::pmt_t msg);
      void parse_management(char *buf, int length);
      void parse_data(char *buf, int length);
      void parse_control(char *buf, int length);
      void parse_body(char* frame, mac_header *h, int data_len);
      void print_mac_address(uint8_t *addr, bool new_line = false);
      bool equal_mac(uint8_t *addr1, uint8_t *addr2);
      void print_ascii(char* buf, int length);
      void send_data(char* buf, int length);
      bool check_mac(std::vector<uint8_t> mac);
      void decide_modulation();
      void set_encoding(int encoding);

    private:
      // For MAC
      uint16_t d_seq_nr;
      uint8_t d_src_mac[6];
      uint8_t d_dst_mac[6];
      uint8_t d_bss_mac[6];
      uint8_t d_psdu[1528];
      bool ack_received;

      // For Parse 
      double d_snr;
      bool d_log;
      bool d_debug;
      int d_last_seq_no;
    };

  } // namespace frequencyAdaptiveOFDM
} // namespace gr

#endif /* INCLUDED_FREQUENCYADAPTIVEOFDM_MAC_AND_PARSE_IMPL_H */

