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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "rb_const_demux_impl.h"

namespace gr {
  namespace frequencyAdaptiveOFDM {

    rb_const_demux::sptr
    rb_const_demux::make()
    {
      return gnuradio::get_initial_sptr
        (new rb_const_demux_impl());
    }

    /*
     * The private constructor
     */
    rb_const_demux_impl::rb_const_demux_impl()
      : gr::block("rb_const_demux",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0))
    {
      message_port_register_in(pmt::mp("symbols_in"));
      message_port_register_out(pmt::mp("rb1"));
      message_port_register_out(pmt::mp("rb2"));
      message_port_register_out(pmt::mp("rb3"));
      message_port_register_out(pmt::mp("rb4"));

      set_msg_handler(pmt::mp("symbols_in"), 
            boost::bind(&rb_const_demux_impl::symbols_in, this, _1));
    }

    //rb_const_demux_impl::~rb_const_demux_impl() {}

    void
    rb_const_demux_impl::symbols_in(pmt::pmt_t msg){
      std::vector<gr_complex> symbols;

      symbols = pmt::c32vector_elements(pmt::cdr(msg));
      std::vector<gr_complex>::iterator it = symbols.begin();
      std::vector<gr_complex> rb1(it, it+12);
      std::vector<gr_complex> rb2(it+12, it+24);
      std::vector<gr_complex> rb3(it+24, it+36);
      std::vector<gr_complex> rb4(it+36, it+48);

      message_port_pub(pmt::mp("rb1"), pmt::cons(pmt::make_dict(),
                          pmt::init_c32vector(12, rb1)));
      message_port_pub(pmt::mp("rb2"), pmt::cons(pmt::make_dict(),
                          pmt::init_c32vector(12, rb2)));
      message_port_pub(pmt::mp("rb3"), pmt::cons(pmt::make_dict(),
                          pmt::init_c32vector(12, rb3)));
      message_port_pub(pmt::mp("rb4"), pmt::cons(pmt::make_dict(),
                          pmt::init_c32vector(12, rb4)));
    }

  } /* namespace frequencyAdaptiveOFDM */
} /* namespace gr */

