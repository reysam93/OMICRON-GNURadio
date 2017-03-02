/* -*- c++ -*- */
/* 
 * Copyright 2017 Samuel Rey Escudero <samuel.rey.escudero@gmail.com>
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


#ifndef INCLUDED_FREQUENCYADAPTIVEOFDM_RB_CONST_DEMUX_H
#define INCLUDED_FREQUENCYADAPTIVEOFDM_RB_CONST_DEMUX_H

#include <frequencyAdaptiveOFDM/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace frequencyAdaptiveOFDM {

    /*
     *  Take an OFDM frame and separate the symbols from the
     *  different resource blocks.
     */
    class FREQUENCYADAPTIVEOFDM_API rb_const_demux : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<rb_const_demux> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of frequencyAdaptiveOFDM::rb_const_demux.
       *
       * To avoid accidental use of raw pointers, frequencyAdaptiveOFDM::rb_const_demux's
       * constructor is in a private implementation
       * class. frequencyAdaptiveOFDM::rb_const_demux::make is the public interface for
       * creating new instances.
       */
      static sptr make();
    };

  } // namespace frequencyAdaptiveOFDM
} // namespace gr

#endif /* INCLUDED_FREQUENCYADAPTIVEOFDM_RB_CONST_DEMUX_H */

