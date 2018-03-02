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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "parse_mac_impl.h"
#include <gnuradio/block_detail.h>

#if defined(__APPLE__)
#include <architecture/byte_order.h>
#define htole16(x) OSSwapHostToLittleInt16(x)
#else
#include <endian.h>
#endif

#include <boost/crc.hpp>
#include <iostream>
#include <fstream>
#include <stdexcept>


namespace gr {
  namespace frequencyAdaptiveOFDM {

    parse_mac::sptr
    parse_mac::make(void*  m_and_p, std::vector<uint8_t> src_mac, bool debug, char* rx_packets_f) {
      return gnuradio::get_initial_sptr(new parse_mac_impl((mac_and_parse*)m_and_p, src_mac, debug, rx_packets_f));
    }


    parse_mac_impl::parse_mac_impl(mac_and_parse*  m_and_p, std::vector<uint8_t> src_mac, bool debug, char* rx_packets_f) :
        block("parse_mac",
          gr::io_signature::make(0, 0, 0),
          gr::io_signature::make(0, 0, 0)),
        d_last_seq_no(-1),
        d_debug(debug) {

      message_port_register_in(pmt::mp("phy in"));
      message_port_register_out(pmt::mp("per"));
      message_port_register_out(pmt::mp("frame data"));
      message_port_register_out(pmt::mp("app out"));
      set_msg_handler(pmt::mp("phy in"), boost::bind(&parse_mac_impl::phy_in, this, _1));

      d_mac_and_parse = m_and_p;
      n_rx_packets = 0;
      rx_packets_fn = rx_packets_f;

      for(int i = 0; i < 6; i++) {
        d_src_mac[i] = src_mac[i];
      }
      if(!d_mac_and_parse->check_mac(src_mac)) throw std::invalid_argument("wrong mac address size");
    }

    parse_mac_impl::~parse_mac_impl() {}


    void
    parse_mac_impl::phy_in (pmt::pmt_t msg) {
      // this must be a pair
      if (!pmt::is_blob(pmt::cdr(msg))) {
        throw std::runtime_error("PMT must be blob");
      }
      if(pmt::is_eof_object(msg)) {
        detail().get()->set_done(true);
        return;
      } else if(pmt::is_symbol(msg)) {
        return;
      }

      pmt::pmt_t dict = pmt::car(msg);
      d_snr = pmt::f64vector_elements(pmt::dict_ref(dict, pmt::mp("snr"), pmt::init_f64vector(0,0)));
      std::vector<int> enc = pmt::s32vector_elements(pmt::dict_ref(dict,
                              pmt::mp("encoding"), pmt::init_s32vector(0, 0)));
      int punct = pmt::to_long(pmt::dict_ref(dict, pmt::mp("puncturing"), pmt::from_long(-1)));
      msg = pmt::cdr(msg);

      int data_len = pmt::blob_length(msg);
      mac_header *h = (mac_header*)pmt::blob_data(msg);

      if (!equal_mac(d_src_mac, h->addr1)){
        dout << std::endl << std::endl << "Message not for me. Ignoring it." << std::endl;
        return;
      }
      //mylog(boost::format("length: %1%") % data_len );

      dout << std::endl << "new mac frame  (length " << data_len << ")" << std::endl;
      dout << "=========================================" << std::endl;

      #define HEX(a) std::hex << std::setfill('0') << std::setw(2) << int(a) << std::dec
      dout << "duration: " << HEX(h->duration >> 8) << " " << HEX(h->duration  & 0xff) << std::endl;
      dout << "frame control: " << HEX(h->frame_control >> 8) << " " << HEX(h->frame_control & 0xff);

      switch((h->frame_control >> 2) & 3) {
        case 0:
          dout << " (MANAGEMENT)" << std::endl;
          parse_management((char*)h, data_len);
          break;
        case 1:
          dout << " (CONTROL)" << std::endl;
          parse_control((char*)h, data_len);
          break;

        case 2:
          dout << " (DATA)" << std::endl;
          parse_data((char*)h, data_len);
          parse_body((char*)pmt::blob_data(msg), h, data_len);
          int psdu_length;
          d_mac_and_parse->sendAck(h->addr2, &psdu_length);

          // Measuring delay
          /*timeval time_now;
          gettimeofday(&time_now, NULL);
          std::cerr << "\t\tPARSE: DATA message: " << time_now.tv_sec << " seg " << time_now.tv_usec << " us\n";*/

          if(rx_packets_fn != ""){
            n_rx_packets++;
            std::fstream rx_packets_fs(rx_packets_fn, std::ofstream::out);
            rx_packets_fs << n_rx_packets << std::endl;
            rx_packets_fs.close();
          }
          // Only send the received information if its from a data package
          send_frame_data(enc, punct);
          break;
        default:
          dout << " (unknown)" << std::endl;
          break;
      }
      decide_encoding();
    }

    void
    parse_mac_impl::send_frame_data(std::vector<int> enc, int punct) {
      pmt::pmt_t dict = pmt::make_dict();
      dict = pmt::dict_add(dict, pmt::mp("snr"), pmt::init_f64vector(N_RB, d_snr));
      dict = pmt::dict_add(dict, pmt::mp("encoding"), pmt::init_s32vector(N_RB, enc));
      dict = pmt::dict_add(dict, pmt::mp("puncturing"), pmt::from_long(punct));
      message_port_pub(pmt::mp("frame data"), dict);
    }

    void
    parse_mac_impl::parse_management(char *buf, int length) {
      mac_header* h = (mac_header*)buf;

      if(length < 24) {
        dout << "too short for a management frame" << std::endl;
        return;
      }

      dout << "Subtype: ";
      switch(((h->frame_control) >> 4) & 0xf) {
        case 0:
          dout << "Association Request";
          break;
        case 1:
          dout << "Association Response";
          break;
        case 2:
          dout << "Reassociation Request";
          break;
        case 3:
          dout << "Reassociation Response";
          break;
        case 4:
          dout << "Probe Request";
          break;
        case 5:
          dout << "Probe Response";
          break;
        case 6:
          dout << "Timing Advertisement";
          break;
        case 7:
          dout << "Reserved";
          break;
        case 8:
          dout << "Beacon" << std::endl;
          if(length < 38) {
            return;
          }
          {
          uint8_t* len = (uint8_t*) (buf + 24 + 13);
          if(length < 38 + *len) {
            return;
          }
          std::string s(buf + 24 + 14, *len);
          dout << "SSID: " << s;
          }
          break;
        case 9:
          dout << "ATIM";
          break;
        case 10:
          dout << "Disassociation";
          break;
        case 11:
          dout << "Authentication";
          break;
        case 12:
          dout << "Deauthentication";
          break;
        case 13:
          dout << "Action";
          break;
        case 14:
          dout << "Action No ACK";
          break;
        case 15:
          dout << "Reserved";
          break;
        default:
          break;
      }
      dout << std::endl;

      dout << "seq nr: " << int(h->seq_nr >> 4) << std::endl;
      dout << "My mac: ";
      print_mac_address(d_src_mac, true);
      dout << "mac 1: ";
      print_mac_address(h->addr1, true);
      dout << "mac 2: ";
      print_mac_address(h->addr2, true);
      dout << "mac 3: ";
      print_mac_address(h->addr3, true);
    }


    void
    parse_mac_impl::parse_data(char *buf, int length) {
      mac_header* h = (mac_header*)buf;
      if(length < 24) {
        dout << "too short for a data frame" << std::endl;
        return;
      }

      dout << "Subtype: ";
      switch(((h->frame_control) >> 4) & 0xf) {
        case 0:
          dout << "Data";
          break;
        case 1:
          dout << "Data + CF-ACK";
          break;
        case 2:
          dout << "Data + CR-Poll";
          break;
        case 3:
          dout << "Data + CF-ACK + CF-Poll";
          break;
        case 4:
          dout << "Null";
          break;
        case 5:
          dout << "CF-ACK";
          break;
        case 6:
          dout << "CF-Poll";
          break;
        case 7:
          dout << "CF-ACK + CF-Poll";
          break;
        case 8:
          dout << "QoS Data";
          break;
        case 9:
          dout << "QoS Data + CF-ACK";
          break;
        case 10:
          dout << "QoS Data + CF-Poll";
          break;
        case 11:
          dout << "QoS Data + CF-ACK + CF-Poll";
          break;
        case 12:
          dout << "QoS Null";
          break;
        case 13:
          dout << "Reserved";
          break;
        case 14:
          dout << "QoS CF-Poll";
          break;
        case 15:
          dout << "QoS CF-ACK + CF-Poll";
          break;
        default:
          break;
      }
      dout << std::endl;

      int seq_no = int(h->seq_nr >> 4);
      dout << "seq nr: " << seq_no << std::endl;
      dout << "My mac: ";
      print_mac_address(d_src_mac, true);
      dout << "mac 1: ";
      print_mac_address(h->addr1, true);
      dout << "mac 2: ";
      print_mac_address(h->addr2, true);
      dout << "mac 3: ";
      print_mac_address(h->addr3, true);

      d_100_rx_packets += seq_no - d_last_seq_no;
      d_lost_packets += seq_no - d_last_seq_no - 1;
      float per = d_lost_packets / float(d_100_rx_packets);
      dout << "instantaneous PER: " << per << std::endl;

      pmt::pmt_t pdu = pmt::make_f32vector(1, per * 100);
      message_port_pub(pmt::mp("per"), pmt::cons( pmt::PMT_NIL, pdu ));

      d_last_seq_no = seq_no;
      if (d_100_rx_packets >= 100 ||  d_100_rx_packets < 0) {
        d_lost_packets = 0;
        d_100_rx_packets = 0;
      }
    }

    void
    parse_mac_impl::process_ack() {
      //dout << ": ACK received";
      //d_mac_and_parse->setAckReceived(true);
      if (d_mac_and_parse->d_debug_ack) {
        dout << "\n";
        timeval time_now;
        gettimeofday(&time_now, NULL);
        std::cout << "\t\tPARSE: ACK received: " <<
        time_now.tv_sec << " sec " << time_now.tv_usec << " usec\n";
      }

      // Measuring delay
      /*timeval time_now;
      gettimeofday(&time_now, NULL);
      std::cerr << "\t\t\t\tPARSE: ACK message: " << time_now.tv_sec << " seg " << time_now.tv_usec << " us\n";*/
    }

    void
    parse_mac_impl::parse_control(char *buf, int length) {
      mac_header* h = (mac_header*)buf;

      dout << "Subtype: ";
      switch(((h->frame_control) >> 4) & 0xf) {
        case 7:
          dout << "Control Wrapper";
          break;
        case 8:
          dout << "Block ACK Requrest";
          break;
        case 9:
          dout << "Block ACK";
          break;
        case 10:
          dout << "PS Poll";
          break;
        case 11:
          dout << "RTS";
          break;
        case 12:
          dout << "CTS";
          break;
        case 13:
          dout << "ACK";
          process_ack();
          break;
        case 14:
          dout << "CF-End";
          break;
        case 15:
          dout << "CF-End + CF-ACK";
          break;
        default:
          dout << "Reserved";
          break;
      }
      dout << std::endl;

      dout << "RA: ";
      print_mac_address(h->addr1, true);
    }

    void
    parse_mac_impl::parse_body(char* frame, mac_header *h, int data_len){
      // DATA
      if((((h->frame_control) >> 2) & 63) == 2) {
        print_ascii(frame + 24, data_len - 24);
        send_data(frame + 24, data_len - 24);
      // QoS Data
      } else if((((h->frame_control) >> 2) & 63) == 34) {
        print_ascii(frame + 26, data_len - 26);
        send_data(frame + 26, data_len - 26);
      }
    }

    void
    parse_mac_impl::print_mac_address(uint8_t *addr, bool new_line) {
      if(!d_debug) {
        return;
      }

      dout << std::setfill('0') << std::hex << std::setw(2);
      for(int i = 0; i < 6; i++) {
        dout << (int)addr[i];
        if(i != 5) {
          dout << ":";
        }
      }
      dout << std::dec;
      if(new_line) {
        dout << std::endl;
      }
    }

    bool
    parse_mac_impl::equal_mac(uint8_t *addr1, uint8_t *addr2) {
      for(int i = 0; i < 6; i++){
        if(addr1[i] != addr2[i]){
          return false;
        }
      }
      return true;
    }

    void
    parse_mac_impl::print_ascii(char* buf, int length) {
      for(int i = 0; i < length; i++) {
        if((buf[i] > 31) && (buf[i] < 127)) {
          dout << buf[i];
        } else {
          dout << ".";
        }
      }
      dout << std::endl;
    }

    void
    parse_mac_impl::send_data(char* buf, int length){
      uint8_t* data = (uint8_t*) buf;
      pmt::pmt_t pdu = pmt::init_u8vector(length, data);
      message_port_pub(pmt::mp("app out"), pmt::cons( pmt::PMT_NIL, pdu ));
    }

    ofdm_param
    parse_mac_impl::unifyEncoding(std::vector<int> enc, std::vector<int> punct,
                                  std::map<int, int> n_enc, std::map<int, int> n_punct){
      float eff_1_2 = 0;
      float eff_3_4 = 0;

      if (n_enc[QAM64] == N_RB) {
        return ofdm_param(enc, P_2_3);
      }

      std::vector<int> enc_aux(enc);
      for (int i = 0; i < N_RB; i++) {
        if (enc[i] == BPSK && punct[i] == P_1_2) {
          return ofdm_param(enc, P_1_2);
        } else if (punct[i] == P_1_2) {
          enc_aux[i]--;
        }
        eff_1_2 += 0.25 * get_spectral_eff((Encoding)enc[i], P_1_2);
        eff_3_4 += 0.25 * get_spectral_eff((Encoding)enc_aux[i], P_3_4);
      }

      std::cerr << "\tPARSE_MAC: eff 1/2: " << eff_1_2 << " eff 3/4 " << eff_3_4 <<"\n";

      if (eff_1_2 > eff_3_4) {
        return ofdm_param(enc, P_1_2);
      } else {
        return ofdm_param(enc_aux, P_3_4);
      }
    }

    void
    parse_mac_impl::decide_encoding(){
      std::vector<int> encoding(N_RB, BPSK);
      std::vector<int> vect_pun(N_RB, NULL_P);
      ofdm_param ofdm(encoding, P_1_2);

      std::map<int, int> n_enc;
      n_enc[BPSK] = 0;
      n_enc[QPSK] = 0;
      n_enc[QAM16] = 0;
      n_enc[QAM64] = 0;

      std::map<int, int> n_pun;
      n_pun[P_1_2] = 0;
      n_pun[P_2_3] = 0;
      n_pun[P_3_4] = 0;

      dout << std::endl;
      for (int i = 0; i < 4; i++) {
        dout << "SNR estimated rb " << i << ": " << d_snr[i] << std::endl;

        if (d_snr[i] >= MIN_SNR_64QAM_3_4) {
          encoding[i] = QAM64;
          vect_pun[i] = P_3_4;
          n_enc[QAM64]++;
          n_pun[P_3_4]++;
        } else if (d_snr[i] >= MIN_SNR_64QAM_2_3) {
          encoding[i] = QAM64;
          vect_pun[i] = P_2_3;
          n_enc[QAM64]++;
          n_pun[P_2_3]++;
        } else if (d_snr[i] >= MIN_SNR_16QAM_3_4) {
          encoding[i] = QAM16;
          vect_pun[i] = P_3_4;
          n_enc[QAM16]++;
          n_pun[P_3_4]++;
        } else if (d_snr[i] >= MIN_SNR_16QAM_1_2) {
          encoding[i] = QAM16;
          vect_pun[i] = P_1_2;
          n_enc[QAM16]++;
          n_pun[P_1_2]++;
        } else if (d_snr[i] >= MIN_SNR_QPSK_3_4) {
          encoding[i] = QPSK;
          vect_pun[i] = P_3_4;
          n_enc[QPSK]++;
          n_pun[P_3_4]++;
        } else if (d_snr[i] >= MIN_SNR_QPSK_1_2) {
          encoding[i] = QPSK;
          vect_pun[i] = P_1_2;
          n_enc[QPSK]++;
          n_pun[P_1_2]++;
        } else if (d_snr[i] >= MIN_SNR_QPSK_3_4) {
          encoding[i] = BPSK;
          vect_pun[i] = P_3_4;
          n_enc[BPSK]++;
          n_pun[P_3_4]++;
        } else {
          encoding[i] = BPSK;
          vect_pun[i] = P_1_2;
          n_enc[BPSK]++;
          n_pun[P_1_2]++;
        }
      }
      dout << std::endl;

      if (n_pun[P_1_2] == N_RB) {
        ofdm = ofdm_param (encoding, P_1_2);
      } else if(n_pun[P_2_3] == N_RB) {
        ofdm = ofdm_param (encoding, P_2_3);
      } else if(n_pun[P_3_4] == N_RB){
        ofdm = ofdm_param (encoding, P_3_4);
      } else {
        ofdm = unifyEncoding(encoding, vect_pun, n_enc, n_pun);
      }
      ofdm.print();
      d_mac_and_parse->setEncoding(ofdm.resource_blocks_e, ofdm.punct);
    }
  } /* namespace frequencyAdaptiveOFDM */
} /* namespace gr */
