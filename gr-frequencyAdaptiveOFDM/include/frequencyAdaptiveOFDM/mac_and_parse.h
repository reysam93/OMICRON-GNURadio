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


#ifndef INCLUDED_FREQUENCYADAPTIVEOFDM_MAC_AND_PARSE_H
#define INCLUDED_FREQUENCYADAPTIVEOFDM_MAC_AND_PARSE_H

#include <frequencyAdaptiveOFDM/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace frequencyAdaptiveOFDM {

    class FREQUENCYADAPTIVEOFDM_API mac_and_parse : virtual public block
    {
     public:
       /* 
        * Min SNR has been calculated considering a maximum bit error rate probability
        * of 0.001 and using the approximation:
        *
        *    Pbmax = 0.2*e(-1'5*SNR_MIN/(M-1))
        *
        * where M is the bits per simbol in the modulation.
        */
        static const float MIN_SNR_BPSK_1_2 = 3.532211577698691;
        // static const float MIN_SNR_BPSK = 3.532211577698691;
        // static const float MIN_SNR_QPSK = 10.596634733096073;
        // static const float MIN_SNR_16QAM = 52.983173665480365;
        // static const float MIN_SNR_64QAM = 222.52932939501753;

        // This limits have been selected experimentally
        static const float MIN_SNR_BPSK_3_4 = 6.0;
        static const float MIN_SNR_QPSK_1_2 = 7.5;
        static const float MIN_SNR_QPSK_3_4 = 9.5;
        static const float MIN_SNR_16QAM_1_2 = 15;
        static const float MIN_SNR_16QAM_3_4 = 18;
        static const float MIN_SNR_64QAM_1_2 = 25;
        static const float MIN_SNR_64QAM_3_4 = 30;

        // Time in usecs
        static const unsigned int SIFS = 10;
        static const unsigned int SLOT_TIME = 9;
        static const unsigned int TIME_OUT = 50;

        std::vector<int> get_encoding();

        typedef boost::shared_ptr<mac_and_parse> sptr;

        static sptr make(std::vector<uint8_t> src_mac,
                          std::vector<uint8_t> dst_mac,
                          std::vector<uint8_t> bss_mac,
                          bool log, 
                          bool debug);
      
        std::vector<int> d_encoding;
        int d_punct;
        pthread_mutex_t d_mutex;
      };

  } // namespace frequencyAdaptiveOFDM
} // namespace gr

#endif /* INCLUDED_FREQUENCYADAPTIVEOFDM_MAC_AND_PARSE_H */

