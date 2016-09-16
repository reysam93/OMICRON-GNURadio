#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2016 <+YOU OR YOUR COMPANY+>.
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

import numpy
import pmt
from gnuradio import gr

class qpsk_tagged_demod_cb_py(gr.sync_block):
    """
    docstring for block qpsk_tagged_demod_cb_py
    """
    def __init__(self, gray_code):
        self.gray_code = gray_code
        self.low_ampl_state = False
        gr.sync_block.__init__(self,
            name="qpsk_demod_py",
            in_sig=[numpy.complex64],
            out_sig=[numpy.uint8])


    def get_minimum_distances(self, sample):
        if self.gray_code == 1:
            if (sample.imag >= 0 and sample.real >= 0):
                return 0 # 1+1j
            elif (sample.imag >= 0 and sample.real < 0):
                return 2 # -1+1j
            elif (sample.imag < 0 and sample.real < 0):
                return 3 # -1-1j
            elif (sample.imag < 0 and sample.real >= 0):
                return 1 # 1-1j
        else:
            if (sample.imag >= 0 and sample.real >= 0):
                return 0 # 1+1j
            elif (sample.imag >= 0 and sample.real < 0):
                return 3 # -1+1j
            elif (sample.imag < 0 and sample.real < 0):
                return 2 # -1-1j
            elif (sample.imag < 0 and sample.real >= 0):
                return 1 # 1-1j


    def check_ampl(self, sample, i):
        if abs(sample) < 0.01 and not self.low_ampl_state:
            self.add_item_tag(0,                                # Port number
                    self.nitems_written(0) + i,                 # Offset
                    pmt.intern("amplitude_warning"),            # Key
                    pmt.from_double(numpy.double(abs(sample)))  # Value
                    # Note: is necessary to explicity create a double because
                    # sample is an explicit 32-bit float
            )
            self.low_ampl_state = True
        elif abs(sample) > 0.01 and self.low_ampl_state:
            self.add_item_tag(0, self.nitems_written(0) + i, 
                    pmt.intern("amplitude_recovered"), 
                    pmt.from_double(numpy.double(abs(sample)))
            )
            self.low_ampl_state = False


    def work(self, input_items, output_items):
        in0 = input_items[0]
        out = output_items[0]

        for i in range(0, len(in0)):
            sample = in0[i]
            self.check_ampl(sample, i)
            out[i] = self.get_minimum_distances(sample)

        return len(output_items[0])