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


#ifndef INCLUDED_FREQUENCYADAPTIVEOFDM_MAC_AND_PARSE_H
#define INCLUDED_FREQUENCYADAPTIVEOFDM_MAC_AND_PARSE_H

#include <frequencyAdaptiveOFDM/api.h>
#include <gnuradio/hier_block2.h>
#include "utils.h"

/*
  Possible useful debugs:
    - ACK debug -> with delay
    - Delay for all (main) blocks!


  Current debug (under d_debgug):
    - Rx message headers
    - Rx mssage content
    - Estimated SNR
    - Encoding used for Tx
    - ACKs info
*/

namespace gr {
  namespace frequencyAdaptiveOFDM {

    /*
        * Min SNR has been calculated considering a maximum bit error rate probability
        * of 0.001 and using the approximation:
        *
        *    Pbmax = 0.2*e(-1'5*SNR_MIN/(M-1))
        *
        * where M is the bits per simbol in the modulation.
        */
        //static const float MIN_SNR_BPSK_1_2 = 3.532211577698691;
        // static const float MIN_SNR_BPSK = 3.532211577698691;
        // static const float MIN_SNR_QPSK = 10.596634733096073;
        // static const float MIN_SNR_16QAM = 52.983173665480365;
        // static const float MIN_SNR_64QAM = 222.52932939501753;

        /*// This limits have been selected experimentally
        static const float MIN_SNR_BPSK_3_4 = 6.0;
        static const float MIN_SNR_QPSK_1_2 = 7.5;
        static const float MIN_SNR_QPSK_3_4 = 9.5;
        static const float MIN_SNR_16QAM_1_2 = 16;
        static const float MIN_SNR_16QAM_3_4 = 19;
        static const float MIN_SNR_64QAM_1_2 = 26;
        static const float MIN_SNR_64QAM_3_4 = 30;*/

        // Simulated limits - 20/10 - LoS
        static const float MIN_SNR_BPSK_1_2 = 1;
        static const float MIN_SNR_BPSK_3_4 = 3.5;  //0.75;
        static const float MIN_SNR_QPSK_1_2 = 6.5;  //4.8;
        static const float MIN_SNR_QPSK_3_4 = 7.5;
        static const float MIN_SNR_16QAM_1_2 = 13;
        static const float MIN_SNR_16QAM_3_4 = 15.5;
        static const float MIN_SNR_64QAM_2_3 = 21.5;
        static const float MIN_SNR_64QAM_3_4 = 22.5;
/*
        // Simulated limits - 18/10
        static const float MIN_SNR_BPSK_1_2 = -2;
        static const float MIN_SNR_BPSK_3_4 = 3;
        static const float MIN_SNR_QPSK_1_2 = 6;
        static const float MIN_SNR_QPSK_3_4 = 7.5;
        static const float MIN_SNR_16QAM_1_2 = 13.5;
        static const float MIN_SNR_16QAM_3_4 = 16;
        static const float MIN_SNR_64QAM_2_3 = 23;
        static const float MIN_SNR_64QAM_3_4 = 25;

/*
        // Experimentally - 16/10
        static const float MIN_SNR_BPSK_1_2 = -2;
        static const float MIN_SNR_BPSK_3_4 = 4.5;
        static const float MIN_SNR_QPSK_1_2 = 6;
        static const float MIN_SNR_QPSK_3_4 = 7.5;
        static const float MIN_SNR_16QAM_1_2 = 14.5;
        static const float MIN_SNR_16QAM_3_4 = 17.5;
        static const float MIN_SNR_64QAM_2_3 = 25;
        static const float MIN_SNR_64QAM_3_4 = 27;

        //OLD
        // This limits have been selected experimentally for PER < 5%
        static const float MIN_SNR_BPSK_3_4 = 4.5;
        static const float MIN_SNR_QPSK_1_2 = 6.5;
        static const float MIN_SNR_QPSK_3_4 = 10;
        static const float MIN_SNR_16QAM_1_2 = 17.5;
        static const float MIN_SNR_16QAM_3_4 = 20;
        static const float MIN_SNR_64QAM_1_2 = 23+6;
        static const float MIN_SNR_64QAM_3_4 = 26+6;
*/
        // Time in usecs
        // SLOT_TIME value may be 9 or 20 usecs
        static const unsigned int SLOT_TIME = 20;
        static const unsigned int SIFS = 10;
        static const unsigned int TIMEOUT = 50*600;

        static const int N_RB = 4;

    class FREQUENCYADAPTIVEOFDM_API mac_and_parse : virtual public gr::hier_block2
    {
     public:
      typedef boost::shared_ptr<mac_and_parse> sptr;

      static sptr make(std::vector<uint8_t> src_mac,
                          std::vector<uint8_t> dst_mac,
                          std::vector<uint8_t> bss_mac,
                          bool debug,
                          bool debug_ack,
                          bool debug_delay,
                          char* tx_packets_f,
                          char* rx_packets_f);

      virtual std::vector<int> getEncoding() = 0;
      virtual int getPuncturing() = 0;
      virtual void setEncoding(std::vector<int> pilots_enc, int puncturing) = 0;
      virtual bool getAckReceived() = 0;
      virtual void setAckReceived(bool received) = 0;

      virtual void sendAck(uint8_t ra[], int *psdu_size) = 0;
      virtual void decrease_encoding() = 0;

      bool check_mac(std::vector<uint8_t> mac);

      // Debug
      bool d_debug;
      bool d_debug_ack;
      bool d_debug_delay;
    };

  } // namespace frequencyAdaptiveOFDM
} // namespace gr

#endif /* INCLUDED_FREQUENCYADAPTIVEOFDM_MAC_AND_PARSE_H */
