/* -*- c++ -*- */
/* 
 * Copyright 2016   Bastian Bloessl <bloessl@ccs-labs.org>
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

#ifndef INCLUDED_FREQUENCYADAPTIVEOFDM_CONSTELLATIONS_IMPL_H
#define INCLUDED_FREQUENCYADAPTIVEOFDM_CONSTELLATIONS_IMPL_H

#include <frequencyAdaptiveOFDM/constellations.h>

namespace gr {
  namespace frequencyAdaptiveOFDM {

    class constellation_bpsk_impl : public constellation_bpsk
    {
    public:
      constellation_bpsk_impl();
      ~constellation_bpsk_impl();

      unsigned int decision_maker(const gr_complex *sample);
    };


    class constellation_qpsk_impl : public constellation_qpsk
    {
    public:
      constellation_qpsk_impl();
      ~constellation_qpsk_impl();

      unsigned int decision_maker(const gr_complex *sample);
    };



    class constellation_16qam_impl : public constellation_16qam
    {
    public:
      constellation_16qam_impl();
      ~constellation_16qam_impl();

      unsigned int decision_maker(const gr_complex *sample);
    };




    class constellation_64qam_impl : public constellation_64qam
    {
    public:
      constellation_64qam_impl();
      ~constellation_64qam_impl();

      unsigned int decision_maker(const gr_complex *sample);
    };
    

  } // namespace frequencyAdaptiveOFDM
} // namespace gr

#endif /* INCLUDED_FREQUENCYADAPTIVEOFDM_CONSTELLATIONS_IMPL_H */

