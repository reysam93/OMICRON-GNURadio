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


#ifndef INCLUDED_FREQUENCYADAPTIVEOFDM_PARSE_MAC_H
#define INCLUDED_FREQUENCYADAPTIVEOFDM_PARSE_MAC_H

#include <frequencyAdaptiveOFDM/api.h>
#include <gnuradio/block.h>
#include <unistd.h>

namespace gr {
  namespace frequencyAdaptiveOFDM {

    class FREQUENCYADAPTIVEOFDM_API parse_mac : virtual public block
    {
     public:
        typedef boost::shared_ptr<parse_mac> sptr;

        static sptr make(void*  m_and_p,
        					std::vector<uint8_t> src_mac,
        					bool debug,
        					char* rx_packets_f);      
      };

  } // namespace frequencyAdaptiveOFDM
} // namespace gr

#endif /* INCLUDED_FREQUENCYADAPTIVEOFDM_PARSE_MAC_H */

