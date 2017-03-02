/* -*- c++ -*- */
/* 
 * Copyright 2017 <+YOU OR YOUR COMPANY+>.
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

#ifndef INCLUDED_FREQUENCYADAPTIVEOFDM_RB_CONST_DEMUX_IMPL_H
#define INCLUDED_FREQUENCYADAPTIVEOFDM_RB_CONST_DEMUX_IMPL_H

#include <frequencyAdaptiveOFDM/rb_const_demux.h>

namespace gr {
  namespace frequencyAdaptiveOFDM {

    class rb_const_demux_impl : public rb_const_demux
    {
     public:
      rb_const_demux_impl();
      //~rb_const_demux_impl();

      void symbols_in(pmt::pmt_t msg);
    };

  } // namespace frequencyAdaptiveOFDM
} // namespace gr

#endif /* INCLUDED_FREQUENCYADAPTIVEOFDM_RB_CONST_DEMUX_IMPL_H */

