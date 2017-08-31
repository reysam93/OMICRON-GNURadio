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

#ifndef INCLUDED_FREQUENCYADAPTIVEOFDM_MAC_AND_PARSE_IMPL_H
#define INCLUDED_FREQUENCYADAPTIVEOFDM_MAC_AND_PARSE_IMPL_H

#include <frequencyAdaptiveOFDM/mac_and_parse.h>
#include <queue>

namespace gr {
  namespace frequencyAdaptiveOFDM {

    class mac_and_parse_impl : public mac_and_parse
    {
     public:
      mac_and_parse_impl(std::vector<uint8_t> src_mac,
                         std::vector<uint8_t> dst_mac,
                         std::vector<uint8_t> bss_mac,
                         bool log,
                         bool debug,
                         char* tx_packets_f,
                         char* rx_packets_f);
      ~mac_and_parse_impl();
      

    private:
      // For both
      ofdm_param d_ofdm;
      std::queue<timer_t> d_timerid_queue;
      std::vector<double> d_snr;
      
      void send_message(int psdu_length, ofdm_param ofdm);
      void print_mac_address(uint8_t *addr, bool new_line = false);
      bool equal_mac(uint8_t *addr1, uint8_t *addr2);
      void print_ascii(char* buf, int length);
      bool check_mac(std::vector<uint8_t> mac);
      void set_encoding(std::vector<int> encoding, int punct);


      // For MAC
      timer_t d_timerid;
      uint16_t d_seq_nr;
      uint8_t d_src_mac[6];
      uint8_t d_dst_mac[6];
      uint8_t d_bss_mac[6];
      uint8_t d_psdu[1528];

      static void ack_timeout(sigval_t sigval);
      void set_timeout();
      void generate_mac_data_frame(const char *msdu, int msdu_size, int *psdu_size);
      
      void app_in (pmt::pmt_t msg);
      
      // For Parse 
      bool d_log;
      bool d_debug;
      int d_last_seq_no;

      void generate_mac_ack_frame(uint8_t ra[], int *psdu_size);
      void send_frame_data();
      void send_data(char* buf, int length);
      void parse_management(char *buf, int length);
      void process_ack();
      void parse_data(char *buf, int length);
      void parse_control(char *buf, int length);
      void parse_body(char* frame, mac_header *h, int data_len);
      void decide_encoding();
      void phy_in (pmt::pmt_t msg);

      // For meassuring QoS
      char* tx_packets_fn;
      char* rx_packets_fn;
      long n_tx_packets;
      long n_rx_packets;
    };

  } // namespace frequencyAdaptiveOFDM
} // namespace gr

#endif /* INCLUDED_FREQUENCYADAPTIVEOFDM_MAC_AND_PARSE_IMPL_H */

