/* -*- c++ -*- */
/* 
 * Copyright 2016   Samuel Rey <samuel.rey.escudero@gmail.com>
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


#ifndef INCLUDED_FREQUENCYADAPTIVEOFDM_SIGNAL_FIELD_H
#define INCLUDED_FREQUENCYADAPTIVEOFDM_SIGNAL_FIELD_H

#include <frequencyAdaptiveOFDM/api.h>
#include <gnuradio/digital/packet_header_default.h>


/*
import frequencyAdaptiveOFDM
help(frequencyAdaptiveOFDM)
*/
namespace gr {
  namespace frequencyAdaptiveOFDM {

    class FREQUENCYADAPTIVEOFDM_API signal_field : virtual public digital::packet_header_default
    {
    public:
      typedef boost::shared_ptr<signal_field> sptr;
      static sptr make();

    protected:
      signal_field();
    };

  } // namespace frequencyAdaptiveOFDM
} // namespace gr

#endif /* INCLUDED_FREQUENCYADAPTIVEOFDM_SIGNAL_FIELD_H */

