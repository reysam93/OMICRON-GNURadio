/* -*- c++ -*- */
/*
 * Copyright 2017 Samuel Rey <samuel.rey.escudero@gmail.com>
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
#include <frequencyAdaptiveOFDM/mac.h>
#include <frequencyAdaptiveOFDM/parse_mac.h>

namespace gr {
  namespace frequencyAdaptiveOFDM {

    class mac_and_parse_impl : public mac_and_parse
    {
     public:
      mac_and_parse_impl(std::vector<uint8_t> src_mac,
                         std::vector<uint8_t> dst_mac,
                         std::vector<uint8_t> bss_mac,
                         bool debug,
                         bool debug_ack,
                         bool debug_delay,
                         char* tx_packets_f,
                         char* rx_packets_f,
                         char* tx_enc_f);
      ~mac_and_parse_impl();

      std::vector<int> getEncoding();
      int getPuncturing();
      void setEncoding(std::vector<int> pilots_enc, int puncturing);
      bool getAckReceived();
      void setAckReceived(bool received);

      void sendAck(uint8_t ra[], int *psdu_size);
      void decrease_encoding();

    private:
      boost::shared_ptr<mac> d_mac;
      boost::shared_ptr<parse_mac> d_parse_mac;
      pthread_mutex_t d_mutex;

      ofdm_param d_ofdm;
      bool d_ack_received;

      void createBlocks(std::vector<uint8_t> src_mac, std::vector<uint8_t> dst_mac, std::vector<uint8_t> bss_mac,
                          bool debug, char* tx_packets_f, char* rx_packets_f, char* tx_enc_f);
      void connectBlocks();
    };

  } // namespace frequencyAdaptiveOFDM
} // namespace gr

#endif /* INCLUDED_FREQUENCYADAPTIVEOFDM_MAC_AND_PARSE_IMPL_H */
