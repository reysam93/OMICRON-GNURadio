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

from gnuradio import gr, gr_unittest
from gnuradio import blocks
from qpsk_tagged_demod_cb_py import qpsk_tagged_demod_cb_py

class qa_qpsk_tagged_demod_cb_py (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        src_data = ((-1.2-2j), (-0.5+2j), (1.3+3j), (4-2j))
        expected_result = (2, 3, 0, 1)
        gray_code = False

        src = blocks.vector_source_c(src_data)
        qpsk = qpsk_tagged_demod_cb_py(gray_code)
        snk = blocks.vector_sink_b()

        self.tb.connect (src, qpsk)
        self.tb.connect (qpsk, snk)
        self.tb.run ()

        result_data = snk.data ()
        self.assertFloatTuplesAlmostEqual (expected_result, result_data, 6)


    def test_002_t (self):
        src_data = ((-1.2-2j), (-0.5+2j), (1.3+3j), (4-2j))
        expected_result = (3, 2, 0, 1)
        gray_code = True

        src = blocks.vector_source_c(src_data)
        qpsk = qpsk_tagged_demod_cb_py(gray_code)
        snk = blocks.vector_sink_b()

        self.tb.connect (src, qpsk)
        self.tb.connect (qpsk, snk)
        self.tb.run ()

        result_data = snk.data ()
        self.assertFloatTuplesAlmostEqual (expected_result, result_data, 6)


if __name__ == '__main__':
    gr_unittest.run(qa_qpsk_tagged_demod_cb_py, "qa_qpsk_tagged_demod_cb_py.xml")
