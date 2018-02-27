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

    /*std::vector<int>
    parse_mac_impl::change64QAM(std::vector<int> enc, int punct) {
      for (int i = 0; i < N_RB; i++) {
        if (enc[i] == QAM64 && punct == )
      }
    }*/

    void
    parse_mac_impl::decide_encoding(){
      std::vector<int> encoding(4, BPSK);
      bool punct_3_4 = true;
      bool all_mod_64QAM = true;
      bool punct3_4possible = true;
      int puncturing = P_1_2;

      float min_snr = 5000;
      int min_enc = 0;
      int min_punct = 0;
      int punct_aux = 0;

      //int min_coding = 64QAM;
      float eff1 = 0;
      float eff2 = 0;

      dout << std::endl;
      for (int i = 0; i < 4; i++) {
        dout << "SNR estimated rb " << i << ": " << d_snr[i] << std::endl;

        if (d_snr[i] >= MIN_SNR_64QAM_2_3) {
          punct_aux = P_3_4;
          encoding[i] = QAM64;
          if (d_snr[i] < MIN_SNR_64QAM_3_4) {
            punct_aux = P_2_3;
            punct_3_4 = false;
          }

        } else if (d_snr[i] >= MIN_SNR_16QAM_1_2) {
          punct_aux = P_3_4;
          all_mod_64QAM = false;
          encoding[i] = QAM16;
          if (d_snr[i] < MIN_SNR_16QAM_3_4) {
            punct_aux = P_1_2;
            punct_3_4 = false;
            punct3_4possible = false;
          }

        }else if (d_snr[i] >= MIN_SNR_QPSK_1_2) {
          punct_aux = P_3_4;
          all_mod_64QAM = false;
          encoding[i] = QPSK;
          if (d_snr[i] < MIN_SNR_QPSK_3_4) {
            punct_aux = P_1_2;
            punct_3_4 = false;
            punct3_4possible = false;
          }
        } else {
          punct_aux = P_3_4;
          all_mod_64QAM = false;
          encoding[i] = BPSK;
          if (d_snr[i] < MIN_SNR_BPSK_3_4) {
            punct_aux = P_1_2;
            punct_3_4 = false;
            punct3_4possible = false;
          }
        }

        if (d_snr[i] < min_snr) {
          min_snr = d_snr[i];
          min_enc = encoding[i];
          min_punct = punct_aux;
        }
      }
      dout << std::endl;

      if (punct_3_4) {
        puncturing = P_3_4;
      } else if (all_mod_64QAM) {
        puncturing = P_2_3;
      } else if (punct3_4possible){
        for(int i = 0; i < 4; i++){
          if (encoding[i] == QAM64){
            encoding[i] = QAM16;
          }
        }
        puncturing = P_3_4;
      }



      for (int i = 0; i <4;i++){
        eff1 += 0.25*get_spectral_eff((Encoding)encoding[i],(Puncturing)puncturing);
        //std::cerr << "\t\tPARSE_MAC: ENC: " << encoding[i] << " PUNCT: " << puncturing << "\n";
      }
      eff2 = get_spectral_eff((Encoding)min_enc,(Puncturing)min_punct);

      //std::cerr << "PARSE_MAC: eficiencia1: " << eff1 << "PARSE_MAC: eficiencia2: " << eff2 << "\n";

      if (eff2 >= eff1) {
        std::vector<int> encoding(4, BPSK);
        d_mac_and_parse->setEncoding(std::vector<int>(4,min_enc), min_punct);
      } else {
        d_mac_and_parse->setEncoding(encoding, puncturing);
      }

      //d_mac_and_parse->setEncoding(encoding, puncturing);
    }
  } /* namespace frequencyAdaptiveOFDM */
} /* namespace gr */
