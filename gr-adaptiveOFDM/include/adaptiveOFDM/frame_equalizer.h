/* -*- c++ -*- */
/* 
 * Copyright 2017   Samuel Rey <samuel.rey.escudero@gmail.com>
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


#ifndef INCLUDED_ADAPTIVEOFDM_FRAME_EQUALIZER_H
#define INCLUDED_ADAPTIVEOFDM_FRAME_EQUALIZER_H

#include <adaptiveOFDM/api.h>
#include <gnuradio/block.h>

enum Equalizer {
  LS   = 0,
  LMS  = 1,
  COMB = 2,
  STA  = 3,
};


namespace gr {
  namespace adaptiveOFDM {

    class ADAPTIVEOFDM_API frame_equalizer : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<frame_equalizer> sptr;
      static sptr make(Equalizer algo, double freq, double bw, bool log,
                        bool debug, bool debug_parity, char* delay_file);
      virtual void set_algorithm(Equalizer algo) = 0;
      virtual void set_bandwidth(double bw) = 0;
      virtual void set_frequency(double freq) = 0;
    };

  } // namespace adaptiveOFDM
} // namespace gr

#endif /* INCLUDED_ADAPTIVEOFDM_FRAME_EQUALIZER_H */

