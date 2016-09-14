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

from gnuradio import gr
from gnuradio import digital
from gnuradio import blocks
from gnuradio import fft

import math


class adaptive_ofdm_tx_bc(gr.hier_block2):
    """
    Hierarchical block for sending packets using an adaptive OFDM modulation

        The output is a modulated signal at baseband

        Args:

    """
    def __init__(self, len_tag_key, fft_len, cp_len, occupied_carriers, pilot_symbols, pilot_carriers, sync_words):
        gr.hier_block2.__init__(self,
            "adaptive_ofdm_tx_bc",
            gr.io_signature(1, 1, gr.sizeof_char),
            gr.io_signature(1, 1, gr.sizeof_gr_complex))

        self.len_tag_key = len_tag_key
        self.fft_len = fft_len
        self.cp_len = cp_len
        self.occupied_carriers = occupied_carriers
        self.pilot_carriers = pilot_carriers
        self.pilot_symbols = pilot_symbols
        self.sync_words = sync_words


        self.select_adaptive_mod()
        self.create_header_blocks()
        self.create_payload_blocks()
        self.tagged_stream_mux = blocks.tagged_stream_mux(gr.sizeof_gr_complex, self.len_tag_key, 0)
        self.create_ofdm_blocks()
        self.multiply = blocks.multiply_const_vcc((1/math.sqrt(self.fft_len), ))
        self.connect_blocks()

    def select_adaptive_mod(self):
        # TODO: Do real adaptive modulation
        self.header_const = digital.constellation_bpsk()
        self.payload_const = digital.constellation_qpsk()


    def create_header_blocks(self):
        hdr_format = digital.header_format_ofdm(self.occupied_carriers, 1, self.len_tag_key)
        self.protocol_formatter = digital.protocol_formatter_bb(hdr_format, self.len_tag_key)

        self.repack_bits_hdr = blocks.repack_bits_bb(8, self.header_const.bits_per_symbol(),
                                                     self.len_tag_key, True, gr.GR_LSB_FIRST)
        self.hdr_modulation = digital.chunks_to_symbols_bc(self.header_const.points(), 1)


    def create_payload_blocks(self):
        self.repack_bits_payload = blocks.repack_bits_bb(8, self.payload_const.bits_per_symbol(),
                                                        self.len_tag_key, True, gr.GR_LSB_FIRST)
        self.payload_modulation = digital.chunks_to_symbols_bc(self.payload_const.points(), 1)


    def create_ofdm_blocks(self):
        self.carrier_allocator = digital.ofdm_carrier_allocator_cvc(self.fft_len, self.occupied_carriers,
                                        self.pilot_carriers, self.pilot_symbols, self.sync_words, self.len_tag_key)
        self.ifft = fft.fft_vcc(self.fft_len, False, (), True, 1)
        ofdm_len = self.fft_len + self.cp_len
        self.ofdm_cyclic_prefixer = digital.ofdm_cyclic_prefixer(self.fft_len, ofdm_len, 0, self.len_tag_key)


    def connect_blocks(self):
        self.connect(self, self.protocol_formatter)
        self.connect(self.protocol_formatter, self.repack_bits_hdr)
        self.connect(self.repack_bits_hdr, self.hdr_modulation)
        self.connect(self, self.repack_bits_payload)
        self.connect(self.repack_bits_payload, self.payload_modulation)
        self.connect((self.hdr_modulation, 0), (self.tagged_stream_mux, 0))
        self.connect((self.payload_modulation, 0), (self.tagged_stream_mux, 1))
        self.connect(self.tagged_stream_mux, self.carrier_allocator)
        self.connect(self.carrier_allocator, self.ifft, self.ofdm_cyclic_prefixer)
        self.connect(self.ofdm_cyclic_prefixer, self.multiply)
        self.connect(self.multiply, self)
