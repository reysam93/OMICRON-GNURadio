/*
 * Copyright (C) 2013, 2016 Bastian Bloessl <bloessl@ccs-labs.org>
 *                          Samuel Rey Escudero <samuel.rey.escudero@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <adaptiveOFDM/mac_and_parse.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/block_detail.h>

#include "utils.h"

#if defined(__APPLE__)
#include <architecture/byte_order.h>
#define htole16(x) OSSwapHostToLittleInt16(x)
#else
#include <endian.h>
#endif

#include <boost/crc.hpp>
#include <iostream>
#include <stdexcept>

using namespace gr::adaptiveOFDM;

class mac_and_parse_impl : public mac_and_parse {

public:
  mac_and_parse_impl(std::vector<uint8_t> src_mac, std::vector<uint8_t> dst_mac, std::vector<uint8_t> bss_mac, bool log, bool debug) :
      block("mac_and_parse",
        gr::io_signature::make(0, 0, 0),
        gr::io_signature::make(0, 0, 0)),
      d_seq_nr(0), d_last_seq_no(-1),
      d_debug(debug), d_log(log), d_snr(0) {

    ack_received = false;
    message_port_register_out(pmt::mp("phy out"));
    message_port_register_out(pmt::mp("fer"));
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

    pthread_mutex_init(&d_mutex, NULL);
    set_encoding(0);
  }

  ~mac_and_parse_impl() {
    pthread_mutex_destroy(&d_mutex);
  }

  /*** MAC implementation ***/

  void app_in (pmt::pmt_t msg) {
    size_t       msg_len;
    const char   *msdu;
    std::string  str;

    if(pmt::is_symbol(msg)) {

      str = pmt::symbol_to_string(msg);
      msg_len = str.length();
      msdu = str.data();

    } else if(pmt::is_pair(msg)) {

      msg_len = pmt::blob_length(pmt::cdr(msg));
      msdu = reinterpret_cast<const char *>(pmt::blob_data(pmt::cdr(msg)));

    } else {
      throw std::invalid_argument("MAC expects PDUs or strings");
      return;
    }

    if(msg_len > MAX_PAYLOAD_SIZE) {
      throw std::invalid_argument("Frame too large (> 1500)");
    }

    // make MAC frame
    int    psdu_length;
    generate_mac_data_frame(msdu, msg_len, &psdu_length);
    send_message(psdu_length);
    usleep(TIME_OUT);

    bool reset_coding = false;
    pthread_mutex_lock(&d_mutex);
    if (!ack_received){
      reset_coding = true;
      dout << "NO ACK RECEIVED. CODING SET TO 0." << std::endl;
    }
    ack_received = false;
    pthread_mutex_unlock(&d_mutex);
    if (reset_coding){
      set_encoding(0);
    } 
  }

  void send_message(int psdu_length) {
    // dict
    pmt::pmt_t dict = pmt::make_dict();
    dict = pmt::dict_add(dict, pmt::mp("crc_included"), pmt::PMT_T);

    // blob
    pmt::pmt_t mac = pmt::make_blob(d_psdu, psdu_length);

    // pdu
    message_port_pub(pmt::mp("phy out"), pmt::cons(dict, mac));
  }

  void generate_mac_data_frame(const char *msdu, int msdu_size, int *psdu_size) {

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

  void generate_mac_ack_frame(uint8_t ra[], int *psdu_size){
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

  void phy_in (pmt::pmt_t msg) {
    std::cerr << "PHY IN" << std::endl;
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

    std::cerr << "CHECK PASSED" << std::endl;
    pmt::pmt_t dict = pmt::car(msg);
    d_snr = pmt::to_double(pmt::dict_ref(dict, pmt::mp("snr"), pmt::from_double(0)));
    decide_modulation();
    msg = pmt::cdr(msg);

    int data_len = pmt::blob_length(msg);
    mac_header *h = (mac_header*)pmt::blob_data(msg);

    if (!equal_mac(d_src_mac, h->addr1)){
      dout << std::endl << std::endl << "Message not for me. Ignoring it." << std::endl;
      return;
    }

    mylog(boost::format("length: %1%") % data_len );

    dout << std::endl << "new mac frame  (length " << data_len << ")" << std::endl;
    dout << "=========================================" << std::endl;

    #define HEX(a) std::hex << std::setfill('0') << std::setw(2) << int(a) << std::dec
    dout << "duration: " << HEX(h->duration >> 8) << " " << HEX(h->duration  & 0xff) << std::endl;
    dout << "frame control: " << HEX(h->frame_control >> 8) << " " << HEX(h->frame_control & 0xff);

    std::cerr << "BEFORE PARSING" << std::endl;
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
        send_message(psdu_length);
        break;

      default:
        dout << " (unknown)" << std::endl;
        break;
    }
    std::cerr << "DONE PARSING" << std::endl;
  }

  void parse_management(char *buf, int length) {
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


  void parse_data(char *buf, int length) {
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

  void parse_control(char *buf, int length) {
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
        pthread_mutex_lock(&d_mutex);
        ack_received = true;
        pthread_mutex_unlock(&d_mutex);
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

  void parse_body(char* frame, mac_header *h, int data_len){
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

  void print_mac_address(uint8_t *addr, bool new_line = false) {
    if(!d_debug) {
      return;
    }

    std::cout << std::setfill('0') << std::hex << std::setw(2);
    for(int i = 0; i < 6; i++) {
      std::cout << (int)addr[i];
      if(i != 5) {
        std::cout << ":";
      }
    }
    std::cout << std::dec;
    if(new_line) {
      std::cout << std::endl;
    }
  }

  bool equal_mac(uint8_t *addr1, uint8_t *addr2) {
    for(int i = 0; i < 6; i++){
      if(addr1[i] != addr2[i]){
        return false;
      }
    }
    return true;
  }

  void print_ascii(char* buf, int length) {
    for(int i = 0; i < length; i++) {
      if((buf[i] > 31) && (buf[i] < 127)) {
        dout << buf[i];
      } else {
        dout << ".";
      }
    }
    dout << std::endl;
  }

  void send_data(char* buf, int length){
    uint8_t* data = (uint8_t*) buf;
    pmt::pmt_t pdu = pmt::init_u8vector(length, data);
    message_port_pub(pmt::mp("app out"), pmt::cons( pmt::PMT_NIL, pdu ));
  }

  bool check_mac(std::vector<uint8_t> mac) {
    if(mac.size() != 6) return false;
    return true;
  }

  void decide_modulation(){
    std::cout << std::endl << "SNR: " << d_snr << std::endl;
    if (d_snr >= MIN_SNR_64QAM) {
      std::cout << "64QAM. Min SNR: " << MIN_SNR_64QAM << std::endl;
      set_encoding(QAM64_2_3);
    } else if (d_snr >= MIN_SNR_16QAM) {
      std::cout << "16QAM. Min SNR: " << MIN_SNR_16QAM << std::endl;
      set_encoding(QAM16_1_2);
    } else if (d_snr >= MIN_SNR_QPSK) {
      std::cout << "QPSK. Min SNR: " << MIN_SNR_QPSK << std::endl;
      set_encoding(QPSK_1_2);
    } else if (d_snr >= MIN_SNR_BPSK) {
      std::cout << "BPSK. Min SNR: " << MIN_SNR_BPSK << std::endl;
      set_encoding(BPSK_1_2);
    } else {
      std::cout << "SNR IS TO LOW. SHOWLD NOT TRANSMIT." << std::endl;
    }
  }

  void set_encoding(int encoding) {
    pthread_mutex_lock(&d_mutex);
    d_encoding = encoding;
    pthread_mutex_unlock(&d_mutex);
  }

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

int 
mac_and_parse::get_encoding(){
  pthread_mutex_lock(&d_mutex);
  int tmp = d_encoding;
  pthread_mutex_unlock(&d_mutex);
  return tmp;
}

mac_and_parse::sptr
mac_and_parse::make(std::vector<uint8_t> src_mac, std::vector<uint8_t> dst_mac, std::vector<uint8_t> bss_mac, bool log, bool debug) {
  return gnuradio::get_initial_sptr(new mac_and_parse_impl(src_mac, dst_mac, bss_mac, log, debug));
}