/* -*- c++ -*- */
/* 
 * Copyright 2016 <+YOU OR YOUR COMPANY+>.
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


#ifndef INCLUDED_CPP_TUTORIAL_MY_QPSK_DEMOD_CB_H
#define INCLUDED_CPP_TUTORIAL_MY_QPSK_DEMOD_CB_H

#include <cpp_tutorial/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace cpp_tutorial {

    /*!
     * \brief <+description of block+>
     * \ingroup cpp_tutorial
     *
     */
    class CPP_TUTORIAL_API my_qpsk_demod_cb : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<my_qpsk_demod_cb> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of cpp_tutorial::my_qpsk_demod_cb.
       *
       * To avoid accidental use of raw pointers, cpp_tutorial::my_qpsk_demod_cb's
       * constructor is in a private implementation
       * class. cpp_tutorial::my_qpsk_demod_cb::make is the public interface for
       * creating new instances.
       */
      static sptr make(bool gray_code);
    };

  } // namespace cpp_tutorial
} // namespace gr

#endif /* INCLUDED_CPP_TUTORIAL_MY_QPSK_DEMOD_CB_H */

