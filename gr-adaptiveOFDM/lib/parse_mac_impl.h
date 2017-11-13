/* -*- c++ -*- */
/*
 * Copyright 2017 Samuel Rey <samuel.rey.escudero@gmail.com>
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

#ifndef INCLUDED_ADAPTIVEOFDM_PARSE_MAC_IMPL_H
#define INCLUDED_ADAPTIVEOFDM_PARSE_MAC_IMPL_H

#include <adaptiveOFDM/mac_and_parse.h>
#include <adaptiveOFDM/parse_mac.h>


namespace gr {
  namespace adaptiveOFDM {

    class parse_mac_impl : public parse_mac
    {
     public:
      parse_mac_impl(mac_and_parse*  m_and_p,
                      std::vector<uint8_t> src_mac,
                      bool debug,
                      char* rx_packets_f);
      ~parse_mac_impl();

    private:
      mac_and_parse*  d_mac_and_parse;

      double d_snr;
      uint8_t d_src_mac[6];
      int d_last_seq_no;

      // For meassuring QoS
      char* rx_packets_fn;
      long n_rx_packets;
      bool d_debug;

      // Intantaneous PER
      float d_lost_packets;
      float d_100_rx_packets;

      void print_mac_address(uint8_t *addr, bool new_line = false);
      bool equal_mac(uint8_t *addr1, uint8_t *addr2);
      void print_ascii(char* buf, int length);
      void send_frame_data(int enc);
      void send_data(char* buf, int length);
      void parse_management(char *buf, int length);
      void process_ack();
      void parse_data(char *buf, int length);
      void parse_control(char *buf, int length);
      void parse_body(char* frame, mac_header *h, int data_len);
      void decide_encoding();
      void phy_in (pmt::pmt_t msg);
    };

  } // namespace adaptiveOFDM
} // namespace gr

#endif /* INCLUDED_aDAPTIVEOFDM_PARSE_MAC_IMPL_H */
