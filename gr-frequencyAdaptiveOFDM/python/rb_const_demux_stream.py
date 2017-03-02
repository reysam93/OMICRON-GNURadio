#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2017 Samuel Rey Escudero <samuel.rey.escudero@gmail.com>
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

from gnuradio import gr
from gnuradio import blocks
import pmt
from frequencyAdaptiveOFDM import rb_const_demux


class rb_const_demux_stream(gr.hier_block2):
    """
    This block take care of the multiplexation of the 
    symbols of the resource block using the rb_const_demux.
    Then, it connects each of the outputs with a pdu_to_tagged_stream
    block, for directly sending stream of symbols.
    """
    def __init__(self, tag):
        gr.hier_block2.__init__(self,
            "rb_const_demux_stream",
            gr.io_signature(0, 0, 0),  # Input signature
            gr.io_signature(4, 4, gr.sizeof_gr_complex)) # Output signature

        self.tag = tag
        self.message_port_register_hier_in("symbols_in")
        self.createBlocks()
        self.connect_blocks()


    def createBlocks(self):
        self.rb_demux = rb_const_demux()
        self.rb_stream1 = blocks.pdu_to_tagged_stream(blocks.complex_t, self.tag)
        self.rb_stream2 = blocks.pdu_to_tagged_stream(blocks.complex_t, self.tag)
        self.rb_stream3 = blocks.pdu_to_tagged_stream(blocks.complex_t, self.tag)
        self.rb_stream4 = blocks.pdu_to_tagged_stream(blocks.complex_t, self.tag)


    def connect_blocks(self):
        self.msg_connect(self, "symbols_in", self.rb_demux, "symbols_in")
        self.msg_connect((self.rb_demux, "rb1"), (self.rb_stream1, "pdus"))
        self.msg_connect((self.rb_demux, "rb2"), (self.rb_stream2, "pdus"))
        self.msg_connect((self.rb_demux, "rb3"), (self.rb_stream3, "pdus"))
        self.msg_connect((self.rb_demux, "rb4"), (self.rb_stream4, "pdus"))
        self.connect(self.rb_stream1, (self, 0))
        self.connect(self.rb_stream2, (self, 1))
        self.connect(self.rb_stream3, (self, 2))
        self.connect(self.rb_stream4, (self, 3))