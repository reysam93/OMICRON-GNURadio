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


#ifndef INCLUDED_FREQUENCYADAPTIVEOFDM_CONSTELLATIONS_H
#define INCLUDED_FREQUENCYADAPTIVEOFDM_CONSTELLATIONS_H

#include <frequencyAdaptiveOFDM/api.h>
#include <gnuradio/digital/constellation.h>

namespace gr {
  namespace frequencyAdaptiveOFDM {

    class FREQUENCYADAPTIVEOFDM_API constellation_bpsk : virtual public digital::constellation
    {
    public:
      typedef boost::shared_ptr<gr::frequencyAdaptiveOFDM::constellation_bpsk> sptr;
      static sptr make();

    protected:
      constellation_bpsk();
    };

    class FREQUENCYADAPTIVEOFDM_API constellation_qpsk : virtual public digital::constellation
    {
    public:
      typedef boost::shared_ptr<gr::frequencyAdaptiveOFDM::constellation_qpsk> sptr;
      static sptr make();

    protected:
      constellation_qpsk();
    };

    class FREQUENCYADAPTIVEOFDM_API constellation_16qam : virtual public digital::constellation
    {
    public:
      typedef boost::shared_ptr<gr::frequencyAdaptiveOFDM::constellation_16qam> sptr;
      static sptr make();

    protected:
      constellation_16qam();
    };

    class FREQUENCYADAPTIVEOFDM_API constellation_64qam : virtual public digital::constellation
    {
    public:
      typedef boost::shared_ptr<gr::frequencyAdaptiveOFDM::constellation_64qam> sptr;
      static sptr make();

    protected:
      constellation_64qam();
    };

  } // namespace frequencyAdaptiveOFDM
} // namespace gr

#endif /* INCLUDED_FREQUENCYADAPTIVEOFDM_CONSTELLATIONS_H */

