/*
 * Copyright (C) 2013, 2017 Samuel Rey Escudero <samuel.rey.escudero@gmail.com>
 *                          Bastian Bloessl <bloessl@ccs-labs.org>
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
#ifndef INCLUDED_ADAPTIVEOFD_MAC_AND_PARSE_H
#define INCLUDED_ADAPTIVEOFD_MAC_AND_PARSE_H

#include <adaptiveOFDM/api.h>
#include <gnuradio/hier_block2.h>
#include "utils.h"

/*
  Possible useful debugs:
    - ACK debug
    - Delay debug


  Current debug (under d_debgug):
    - Rx message headers
    - Rx mssage content
    - Estimated SNR
    - Encoding used for Tx
    - ACKs info
*/

namespace gr {
namespace adaptiveOFDM {

  /* 
   * Min SNR has been calculated considering a maximum bit error rate probability
   * of 0.001 and using the approximation:
   *
   *    Pbmax = 0.2*e(-1'5*SNR_MIN/(M-1))
   *
   * where M is the bits per simbol in the modulation.
   */
  // static const float MIN_SNR_BPSK = 3.532211577698691;
  // static const float MIN_SNR_QPSK = 10.596634733096073;
  // static const float MIN_SNR_16QAM = 52.983173665480365;
  // static const float MIN_SNR_64QAM = 222.52932939501753;

  // This limits have been selected experimentally for PER < 5%
  static const float MIN_SNR_BPSK_3_4 = 4.5;
  static const float MIN_SNR_QPSK_1_2 = 6;
  static const float MIN_SNR_QPSK_3_4 = 8.75;
  static const float MIN_SNR_16QAM_1_2 = 13.75;
  static const float MIN_SNR_16QAM_3_4 = 15.75;
  static const float MIN_SNR_64QAM_2_3 = 18;
  static const float MIN_SNR_64QAM_3_4 = 20;

  // Time in usecs
  // SLOT_TIME value may be 9 or 20 usecs
  static const unsigned int SLOT_TIME = 20;
  static const unsigned int SIFS = 10;
  static const unsigned int TIMEOUT = 50*600;

class ADAPTIVEOFDM_API mac_and_parse : virtual public gr::hier_block2
{
public:
  typedef boost::shared_ptr<mac_and_parse> sptr;

    static sptr make(std::vector<uint8_t> src_mac,
                          std::vector<uint8_t> dst_mac,
                          std::vector<uint8_t> bss_mac,
                          bool debug,
                          char* tx_packets_f,
                          char* rx_packets_f);

    virtual Encoding getEncoding() = 0;
    virtual void setEncoding(Encoding enc) = 0;
    virtual bool getAckReceived() = 0;
    virtual void setAckReceived(bool received) = 0;

    virtual void sendAck(uint8_t ra[], int *psdu_size) = 0;
    virtual void decrease_encoding() = 0;

     bool check_mac(std::vector<uint8_t> mac);
};

}  // namespace adaptiveOFDM
}  // namespace gr

#endif /* INCLUDED_ADAPTIVEOFD_MAC_AND_PARSE_H */
