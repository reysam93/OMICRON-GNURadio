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
import time
from gnuradio import gr

class stream_spacer(gr.sync_block):
    """
    docstring for block stream_spacer
    """
    def __init__(self, dataType, interval):
        
        dtype = self.getDataType(dataType)
        gr.sync_block.__init__(self,
            name="stream_spacer",
            in_sig=[dtype],
            out_sig=[dtype])

        self.interval = interval/1000

    def getDataType(self, data):
        if data == 0:
            return numpy.uint8
        elif data == 1:
            return numpy.complex
        elif data == 2:
            return numpy.float32
        elif data == 3:
            return numpy.float16
        else:
            return None


    def work(self, input_items, output_items):
        in0 = input_items[0]
        out = output_items[0]

        out[:] = in0
        if self.interval > 0:
            time.sleep(self.interval)
        return len(output_items[0])

