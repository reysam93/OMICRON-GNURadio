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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "mac_and_parse_impl.h"

namespace gr {
  namespace adaptiveOFDM {

    mac_and_parse::sptr
    mac_and_parse::make(std::vector<uint8_t> src_mac, std::vector<uint8_t> dst_mac, std::vector<uint8_t> bss_mac,
                          bool debug, char* tx_packets_f, char* rx_packets_f)
    {
      return gnuradio::get_initial_sptr
        (new mac_and_parse_impl(src_mac, dst_mac, bss_mac, debug, tx_packets_f, rx_packets_f));
    }

    bool 
    mac_and_parse::check_mac(std::vector<uint8_t> mac) {
      return mac.size() == 6;
    }

    mac_and_parse_impl::mac_and_parse_impl(std::vector<uint8_t> src_mac, std::vector<uint8_t> dst_mac, std::vector<uint8_t> bss_mac,
                          bool debug, char* tx_packets_f, char* rx_packets_f)
      : gr::hier_block2("mac_and_parse",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
        d_debug(debug),
        d_ofdm(BPSK_1_2)
    {
      message_port_register_hier_in(pmt::mp("app in"));
      message_port_register_hier_in(pmt::mp("phy in"));
      message_port_register_hier_out(pmt::mp("phy out"));
      message_port_register_hier_out(pmt::mp("fer"));
      message_port_register_hier_out(pmt::mp("frame data"));
      message_port_register_hier_out(pmt::mp("app out"));
      
      pthread_mutex_init(&d_mutex, NULL);
      createBlocks(src_mac, dst_mac, bss_mac, debug, tx_packets_f, rx_packets_f);
      connectBlocks();
    }

    mac_and_parse_impl::~mac_and_parse_impl()
    {
      pthread_mutex_destroy(&d_mutex);
    }

    void
    mac_and_parse_impl::createBlocks(std::vector<uint8_t> src_mac, std::vector<uint8_t> dst_mac, std::vector<uint8_t> bss_mac,
                          bool debug, char* tx_packets_f, char* rx_packets_f) {
      d_mac = mac::make(this, src_mac, dst_mac, bss_mac, debug, tx_packets_f); 
      d_parse_mac = parse_mac::make(this, src_mac, debug, rx_packets_f);
    }

    void
    mac_and_parse_impl::connectBlocks() {
      msg_connect(self(), "app in", d_mac, "app in");
      msg_connect(self(), "phy in", d_parse_mac, "phy in");
      msg_connect(d_mac, "phy out", self(), "phy out");
      msg_connect(d_parse_mac, "app out", self(), "app out");
      msg_connect(d_parse_mac, "frame data", self(), "frame data");
      msg_connect(d_parse_mac, "fer", self(), "fer");        
    }

    Encoding
    mac_and_parse_impl::getEncoding() {
      Encoding tmp;
      pthread_mutex_lock(&d_mutex);
      tmp = d_ofdm.encoding;
      pthread_mutex_unlock(&d_mutex);
      return tmp;
    }

    void
    mac_and_parse_impl::setEncoding(Encoding enc) {
      ofdm_param ofdm(enc);
      pthread_mutex_lock(&d_mutex);
      d_ofdm = ofdm;
      pthread_mutex_unlock(&d_mutex);
    
      if (d_debug) {
        std::cout << "MAC_&_PARSE: set encoding\n";
        ofdm.print_encoding();
      }
    }

    void
    mac_and_parse_impl::sendAck(uint8_t ra[], int *psdu_size) {
      //needs to wait 10 usecs before sending ack.
      usleep(SIFS);
      d_mac->sendAck(ra, psdu_size);
    }

    bool
    mac_and_parse_impl::getAckReceived() {
      bool tmp;
      pthread_mutex_lock(&d_mutex);
      tmp = d_ack_received;
      pthread_mutex_unlock(&d_mutex);
      return tmp;
    }
    
    void
    mac_and_parse_impl::setAckReceived(bool received) {
      pthread_mutex_lock(&d_mutex);
      d_ack_received = received;
      pthread_mutex_unlock(&d_mutex);
    }

    void
    mac_and_parse_impl::decrease_encoding() {
      Encoding enc = getEncoding();
      if (enc > BPSK_1_2) {
          enc = Encoding(int(enc) - 1);
      } else {
        return;
      }
      setEncoding(enc);
    }
  } /* namespace adaptiveOFDM */
} /* namespace gr */
