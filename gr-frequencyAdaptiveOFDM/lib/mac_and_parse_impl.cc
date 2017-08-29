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
#include "mac_and_parse_impl.h"
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

    mac_and_parse::sptr
    mac_and_parse::make(std::vector<uint8_t> src_mac, std::vector<uint8_t> dst_mac, std::vector<uint8_t> bss_mac,
                          bool log, bool debug, char* tx_packets_f, char* rx_packets_f) {
      return gnuradio::get_initial_sptr(new mac_and_parse_impl(src_mac, dst_mac, bss_mac, log, debug, tx_packets_f, rx_packets_f));
    }


    mac_and_parse_impl::mac_and_parse_impl(std::vector<uint8_t> src_mac, std::vector<uint8_t> dst_mac, std::vector<uint8_t> bss_mac,
                                            bool log, bool debug, char* tx_packets_f, char* rx_packets_f) :
        block("mac_and_parse",
          gr::io_signature::make(0, 0, 0),
          gr::io_signature::make(0, 0, 0)),
        d_seq_nr(0), d_last_seq_no(-1),
        d_ofdm(std::vector<int>(4, BPSK), P_1_2),
        d_debug(debug), d_log(log), d_snr(std::vector<double>(4,0)) {

      message_port_register_out(pmt::mp("phy out"));
      message_port_register_out(pmt::mp("fer"));
      message_port_register_out(pmt::mp("frame data"));
      message_port_register_out(pmt::mp("app out"));
      message_port_register_in(pmt::mp("app in"));
      message_port_register_in(pmt::mp("phy in"));
      set_msg_handler(pmt::mp("phy in"), boost::bind(&mac_and_parse_impl::phy_in, this, _1));
      set_msg_handler(pmt::mp("app in"), boost::bind(&mac_and_parse_impl::app_in, this, _1));

      if(!check_mac(src_mac)) throw std::invalid_argument("wrong mac address size");
      if(!check_mac(dst_mac)) throw std::invalid_argument("wrong mac address size");
      if(!check_mac(bss_mac)) throw std::invalid_argument("wrong mac address size");

      for(int i = 0; i < 6; i++) {
        d_src_mac[i] = src_mac[i];
        d_dst_mac[i] = dst_mac[i];
        d_bss_mac[i] = bss_mac[i];
      }

      n_tx_packets = 0;
      n_rx_packets = 0;
      tx_packets_fn = tx_packets_f;
      rx_packets_fn = rx_packets_f;

      if (d_debug) {
        d_ofdm.print();
      }

      pthread_mutex_init(&d_mutex, NULL);
      std::vector<int> initial_e(4, BPSK);
      set_encoding(initial_e, P_1_2);
    }

    mac_and_parse_impl::~mac_and_parse_impl() {
      pthread_mutex_destroy(&d_mutex);
    }

    /*** MAC implementation ***/
    void 
    mac_and_parse_impl::ack_timeout(sigval_t sigval) {
      mac_and_parse_impl* self = (mac_and_parse_impl*) sigval.sival_ptr;
      timer_t timerid = NULL;

      pthread_mutex_lock(&self->d_mutex);
      if (!self->d_timerid_queue.empty()) {
        timerid = self->d_timerid_queue.front();
        self->d_timerid_queue.pop();
      }
      pthread_mutex_unlock(&self->d_mutex);

      if (timerid == NULL) {
        throw std::runtime_error("ack timeout with null timerid");
      }

      if (self->d_debug) 
        std::cout << "\t\t\tMAC_&_PARSE: TIMEOUT: no ACK received.\n" << std::endl;
      
      std::vector<int> encoding(4, BPSK);
      self->set_encoding(encoding, P_1_2);
      
      if (timer_delete(timerid) < 0) {
        throw std::runtime_error("error deleting timer");
      }
    }

    void
    mac_and_parse_impl::set_timeout() {
      struct sigevent sev;
      struct itimerspec its;

      d_timerid = malloc(sizeof(timer_t));

      /*if (d_timerid == NULL) {
        std:: cerr << "MAC_&_PARSE: WARNING: null id before timer_create\n";
      }*/

      // Create timer
      memset(&sev, 0, sizeof (struct sigevent));
      sev.sigev_notify = SIGEV_THREAD;
      sev.sigev_notify_function = &mac_and_parse_impl::ack_timeout;
      sev.sigev_value.sival_ptr = (void*) this;
      sev.sigev_signo = 0;

      if (timer_create(CLOCK_REALTIME, &sev, &d_timerid) < 0) {
        throw std::runtime_error("error creating timeout timer");
      }

      // start timer
      its.it_value.tv_sec = 0;
      its.it_value.tv_nsec = TIMEOUT * 1000; //TIMEOUT in usec
      its.it_interval.tv_sec = 0;
      its.it_interval.tv_nsec = 0;
      if (timer_settime(d_timerid, 0, &its, NULL) < 0){
        throw std::runtime_error("error starting timeout timer");
      }

      /*if (d_timerid == NULL) {
        std:: cerr << "MAC_&_PARSE: WARNING: saving null id\n";
      }*/

      pthread_mutex_lock(&d_mutex);
      d_timerid_queue.push(d_timerid);
      pthread_mutex_unlock(&d_mutex);
    }

    void 
    mac_and_parse_impl::app_in (pmt::pmt_t msg) {
      size_t       msg_len;
      const char   *msdu;
      std::string  str;

      dout << "MAC_&_PARSE: new message in app_in\n";
      if(pmt::is_symbol(msg)) {

        str = pmt::symbol_to_string(msg);
        msg_len = str.length();
        msdu = str.data();

      } else if(pmt::is_pair(msg)) {

        msg_len = pmt::blob_length(pmt::cdr(msg));
        msdu = reinterpret_cast<const char *>(pmt::blob_data(pmt::cdr(msg)));

      } else {
        throw std::runtime_error("MAC expects PDUs or strings");
        return;
      }

      if(msg_len > MAX_PAYLOAD_SIZE) {
        throw std::runtime_error("Frame too large (> 1500)");
      }

      // make MAC frame
      int    psdu_length;
      generate_mac_data_frame(msdu, msg_len, &psdu_length);
      pthread_mutex_lock(&d_mutex);
      send_message(psdu_length, d_ofdm);
      pthread_mutex_unlock(&d_mutex);
      set_timeout();

      if(tx_packets_fn != ""){
        n_tx_packets++;
        std::fstream tx_packets_fs(tx_packets_fn, std::ofstream::out);
        tx_packets_fs << n_tx_packets << std::endl;
        tx_packets_fs.close();
      }
    }

    void
    mac_and_parse_impl::send_message(int psdu_length, ofdm_param ofdm) {
      // dict
      pmt::pmt_t dict = pmt::make_dict();
      dict = pmt::dict_add(dict, pmt::mp("crc_included"), pmt::PMT_T);
      pmt::pmt_t encoding = pmt::init_s32vector(4, ofdm.resource_blocks_e);
      dict = pmt::dict_add(dict, pmt::mp("encoding"), encoding);
      dict = pmt::dict_add(dict, pmt::mp("puncturing"), pmt::from_long(ofdm.punct));

      // blob
      pmt::pmt_t mac = pmt::make_blob(d_psdu, psdu_length);

      // pdu
      message_port_pub(pmt::mp("phy out"), pmt::cons(dict, mac));
    }

    void
    mac_and_parse_impl::generate_mac_data_frame(const char *msdu, int msdu_size, int *psdu_size) {

      // mac header
      mac_header header;
      header.frame_control = 0x0008;
      header.duration = 0x0000;

      for(int i = 0; i < 6; i++) {
        header.addr1[i] = d_dst_mac[i];
        header.addr2[i] = d_src_mac[i];
        header.addr3[i] = d_bss_mac[i];
      }

      header.seq_nr = 0;
      for (int i = 0; i < 12; i++) {
        if(d_seq_nr & (1 << i)) {
          header.seq_nr |=  (1 << (i + 4));
        }
      }
      header.seq_nr = htole16(header.seq_nr);
      d_seq_nr++;

      //header size is 24, plus 4 for FCS means 28 bytes
      *psdu_size = 28 + msdu_size;

      //copy mac header into psdu
      std::memcpy(d_psdu, &header, 24);
      //copy msdu into psdu
      memcpy(d_psdu + 24, msdu, msdu_size);
      //compute and store fcs
      boost::crc_32_type result;
      result.process_bytes(d_psdu, msdu_size + 24);

      uint32_t fcs = result.checksum();
      memcpy(d_psdu + msdu_size + 24, &fcs, sizeof(uint32_t));
    }

    void
    mac_and_parse_impl::generate_mac_ack_frame(uint8_t ra[], int *psdu_size){
      mac_ack_header header;
      header.frame_control = 0x00D4;
      header.duration = 0x0000;

      for(int i = 0; i < 6; i++) {
        header.ra[i] = ra[i];
      }
      *psdu_size = 10;
      std::memcpy(d_psdu, &header, *psdu_size);
      boost::crc_32_type result;
      result.process_bytes(d_psdu, *psdu_size);

      uint32_t fcs = result.checksum();
      memcpy(d_psdu + *psdu_size, &fcs, sizeof(uint32_t));

      // Plus 4bytes of FCS
      *psdu_size += 4;
    }


    /*** Parse MAC Implementation ***/

    void
    mac_and_parse_impl::phy_in (pmt::pmt_t msg) {
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
      msg = pmt::cdr(msg);

      int data_len = pmt::blob_length(msg);
      mac_header *h = (mac_header*)pmt::blob_data(msg);

      if (!equal_mac(d_src_mac, h->addr1)){
        dout << std::endl << std::endl << "Message not for me. Ignoring it." << std::endl;
        return;
      }
      
      decide_encoding();
      send_frame_data();

      mylog(boost::format("length: %1%") % data_len );

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
          generate_mac_ack_frame(h->addr2, &psdu_length);
          //needs to wait 10 usecs before sending ack.
          usleep(SIFS);
          send_message(psdu_length, ofdm_param(std::vector<int>(4, BPSK), P_1_2));
          
          if(rx_packets_fn != ""){
            n_rx_packets++;
            std::fstream rx_packets_fs(rx_packets_fn, std::ofstream::out);
            rx_packets_fs << n_rx_packets << std::endl;
            rx_packets_fs.close();
          }
          break;
        default:
          dout << " (unknown)" << std::endl;
          break;
      }
    }

    void
    mac_and_parse_impl::send_frame_data() {
      pmt::pmt_t dict = pmt::make_dict();
      dict = pmt::dict_add(dict, pmt::mp("snr"), pmt::init_f64vector(4, d_snr));
      pthread_mutex_lock(&d_mutex);
      dict = pmt::dict_add(dict, pmt::mp("encoding"), pmt::init_s32vector(4, d_ofdm.resource_blocks_e));
      dict = pmt::dict_add(dict, pmt::mp("puncturing"), pmt::from_long(d_ofdm.punct));
      pthread_mutex_unlock(&d_mutex);
      message_port_pub(pmt::mp("frame data"), dict);
    }

    void
    mac_and_parse_impl::parse_management(char *buf, int length) {
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
    mac_and_parse_impl::parse_data(char *buf, int length) {
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

      float lost_frames = seq_no - d_last_seq_no - 1;
      if(lost_frames  < 0)
        lost_frames += 1 << 12;

      // calculate frame error rate
      float fer = lost_frames / (lost_frames + 1);
      dout << "instantaneous fer: " << fer << std::endl;

      // keep track of values
      d_last_seq_no = seq_no;

      // publish FER estimate
      pmt::pmt_t pdu = pmt::make_f32vector(lost_frames + 1, fer * 100);
      message_port_pub(pmt::mp("fer"), pmt::cons( pmt::PMT_NIL, pdu ));
    }

    void
    mac_and_parse_impl::process_ack() {
      timer_t timerid = NULL;

      pthread_mutex_lock(&d_mutex);
      if (!d_timerid_queue.empty()) {
        timerid = d_timerid_queue.front();
        d_timerid_queue.pop();
      }
      pthread_mutex_unlock(&d_mutex);

      if (timerid == NULL) {
        dout << "MAC_&_PARSE: WARNING: ACK received after TIMEOUT\n";
        return;
      }

      if (timer_delete(timerid) < 0) {
        throw std::runtime_error("error deleting timer");
      }
      dout << ": Timer deleted.\n";
    }

    void
    mac_and_parse_impl::parse_control(char *buf, int length) {
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
    mac_and_parse_impl::parse_body(char* frame, mac_header *h, int data_len){
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
    mac_and_parse_impl::print_mac_address(uint8_t *addr, bool new_line) {
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
    mac_and_parse_impl::equal_mac(uint8_t *addr1, uint8_t *addr2) {
      for(int i = 0; i < 6; i++){
        if(addr1[i] != addr2[i]){
          return false;
        }
      }
      return true;
    }

    void
    mac_and_parse_impl::print_ascii(char* buf, int length) {
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
    mac_and_parse_impl::send_data(char* buf, int length){
      uint8_t* data = (uint8_t*) buf;
      pmt::pmt_t pdu = pmt::init_u8vector(length, data);
      message_port_pub(pmt::mp("app out"), pmt::cons( pmt::PMT_NIL, pdu ));
    }

    bool
    mac_and_parse_impl::check_mac(std::vector<uint8_t> mac) {
      if(mac.size() != 6) return false;
      return true;
    }

    void
    mac_and_parse_impl::decide_encoding(){
      std::vector<int> encoding(4, BPSK);
      bool punct_3_4 = true;
      bool same_mod;
      int puncturing = P_1_2;

      dout << std::endl;
      for (int i = 0; i < 4; i++) {
        dout << "SNR resource block " << i << ": " << d_snr[i] << std::endl;

        if (d_snr[i] >= MIN_SNR_64QAM_1_2) {      
          encoding[i] = QAM64;
          if (d_snr[i] < MIN_SNR_64QAM_3_4) {
            punct_3_4 = false;
          }
        } else if (d_snr[i] >= MIN_SNR_16QAM_1_2) {
          encoding[i] = QAM16;
          if (d_snr[i] < MIN_SNR_16QAM_3_4) {
            punct_3_4 = false;
          }
        }else if (d_snr[i] >= MIN_SNR_QPSK_1_2) {
          encoding[i] = QPSK;
          if (d_snr[i] < MIN_SNR_QPSK_3_4) {
            punct_3_4 = false;
          }
        } else {
          encoding[i] = BPSK;
          if (d_snr[i] < MIN_SNR_BPSK_3_4) {
            punct_3_4 = false;
          }
        }
      }
      dout << std::endl;

      if (punct_3_4) {
        same_mod = true;
        for (int i = 1; i < 4; i++) {
          if (encoding[0] != encoding[i]){
            same_mod = false;
            break;
          }
        }
        if (same_mod) {
          puncturing = punct_3_4;
        }
      }

      set_encoding(encoding, puncturing);
    }

    void 
    mac_and_parse_impl::set_encoding(std::vector<int> encoding, int punct) {
      ofdm_param ofdm(encoding, punct);
      if (d_debug) {
        std::cout << "MAC_&_PARSE: set encoding\n";
        ofdm.print_encoding();
      }

      pthread_mutex_lock(&d_mutex);
      d_ofdm = ofdm;
      pthread_mutex_unlock(&d_mutex);
    }

  } /* namespace frequencyAdaptiveOFDM */
} /* namespace gr */

