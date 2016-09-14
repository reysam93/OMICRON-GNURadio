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
from gnuradio import fft
from gnuradio import analog
from gnuradio import filter
from gnuradio import blocks
from gnuradio.filter import firdes


class adaptive_ofdm_rx_cb(gr.hier_block2):
    """
    docstring for block adaptive_ofdm_rx_cb
    """
    def __init__(self, samp_rate, len_tag_key, fft_len, cp_len, occupied_carriers, pilot_carriers, pilot_symbols, sync_words):
        gr.hier_block2.__init__(self,
            "adaptive_ofdm_rx_cb",
            gr.io_signature(1, 1, gr.sizeof_gr_complex),
            gr.io_signature(1, 1, gr.sizeof_char))

        self.samp_rate = samp_rate
        self.len_tag_key = len_tag_key
        self.fft_len = fft_len
        self.cp_len = cp_len
        self.occupied_carriers = occupied_carriers
        self.pilot_carriers = pilot_carriers
        self.pilot_symbols = pilot_symbols
        self.sync_words = sync_words
        self.used_carriers = len(occupied_carriers[0]) + len(pilot_carriers[0])
        self.rx_length_tag_key = "frame_len"

        self.select_adaptive_mod()
        self.create_demux_input_blocks()
        self.create_header_demod_blocks()
        self.create_payload_demod_blocks()
        self.connect_blocks()


    def select_adaptive_mod(self):
        # TODO: Do real adaptive modulation
        self.header_const = digital.constellation_bpsk()
        self.payload_const = digital.constellation_qpsk()


    def create_demux_input_blocks(self):
        bw = (float(self.used_carriers)/float(self.fft_len))/2.0
        tbw = bw * 0.08
        filter_taps = firdes.low_pass(1.0, 1, bw+tbw, tbw, firdes.WIN_HAMMING, 6.76)
        self.fft_filter = filter.fft_filter_ccc(1, filter_taps, 1)

        self.delay = blocks.delay(gr.sizeof_gr_complex, self.fft_len+self.cp_len)
        self.ofdm_sync = digital.ofdm_sync_sc_cfb(self.fft_len, self.cp_len, False)
        self.freq_modulator = analog.frequency_modulator_fc(-2.0/self.fft_len)
        self.multiply = blocks.multiply_vcc(1)

        header_len = 3 #In OFDM Symbols
        self.header_payload_demux = digital.header_payload_demux(header_len,
                                                                 self.fft_len,
                                                                 self.cp_len,
                                                                 self.rx_length_tag_key,
                                                                 "",
                                                                 True,
                                                                 gr.sizeof_gr_complex,
                                                                 "rx_time",
                                                                 self.samp_rate,
                                                                 (),
                                                                 0,)

    def create_header_demod_blocks(self):
        self.hdr_fft = fft.fft_vcc(self.fft_len, True, (), True, 1)
        self.chanest = digital.ofdm_chanest_vcvc(self.sync_words[0], self.sync_words[1], 1, 0, 3, False)
        hdr_equalizer = digital.ofdm_equalizer_simpledfe(self.fft_len, self.header_const.base(), self.occupied_carriers,
                                                            self.pilot_carriers, self.pilot_symbols)
        self.hdr_frame_eq = digital.ofdm_frame_equalizer_vcvc(hdr_equalizer.base(), self.cp_len, self.rx_length_tag_key,
                                                                    True, 1)
        self.hdr_serializer = digital.ofdm_serializer_vcc(self.fft_len, self.occupied_carriers, self.rx_length_tag_key,
                                                                    "", 0, "", True)
        self.hdr_const_decoder = digital.constellation_decoder_cb(self.header_const.base())
        header_formatter = digital.packet_header_ofdm(self.occupied_carriers, 1, self.len_tag_key, frame_len_tag_key=self.rx_length_tag_key,
                                                        bits_per_header_sym=self.header_const.bits_per_symbol(),
                                                        bits_per_payload_sym=self.payload_const.bits_per_symbol(), scramble_header=False)
        self.hdr_parser = digital.packet_headerparser_b(header_formatter.base())


    def create_payload_demod_blocks(self):
        self.payload_fft = fft.fft_vcc(self.fft_len, True, (), True, 1)
        payload_equalizer = digital.ofdm_equalizer_simpledfe(self.fft_len, self.payload_const.base(), self.occupied_carriers,
                                                            self.pilot_carriers, self.pilot_symbols, 1)
        self.payload_frame_eq = digital.ofdm_frame_equalizer_vcvc(payload_equalizer.base(), self.cp_len, self.rx_length_tag_key,
                                                                    True, 0)
        self.payload_serializer = digital.ofdm_serializer_vcc(self.fft_len, self.occupied_carriers, self.rx_length_tag_key,
                                                                self.len_tag_key, 0, "", True)
        self.payload_const_decoder = digital.constellation_decoder_cb(self.payload_const.base())
        self.repack_bits = blocks.repack_bits_bb(self.payload_const.bits_per_symbol(), 8, self.len_tag_key, True,
                                                    gr.GR_LSB_FIRST)


    def connect_blocks(self):
        self.connect(self, self.fft_filter)
        self.connect(self.fft_filter, self.delay)
        self.connect((self.delay, 0), (self.multiply, 0))
        self.connect(self.fft_filter, self.ofdm_sync)
        self.connect((self.ofdm_sync, 0), (self.freq_modulator, 0))
        self.connect((self.freq_modulator, 0), (self.multiply, 1))
        self.connect((self.multiply, 0), (self.header_payload_demux, 0))
        self.connect((self.ofdm_sync, 1), (self.header_payload_demux, 1))
        self.connect((self.header_payload_demux, 0), (self.hdr_fft, 0))
        self.connect(self.hdr_fft, self.chanest, self.hdr_frame_eq, self.hdr_serializer)
        self.connect(self.hdr_serializer, self.hdr_const_decoder, self.hdr_parser)
        self.msg_connect((self.hdr_parser, 'header_data'), (self.header_payload_demux, 'header_data'))
        self.connect((self.header_payload_demux, 1), (self.payload_fft, 0))
        self.connect(self.payload_fft, self.payload_frame_eq, self.payload_serializer)
        self.connect(self.payload_serializer, self.payload_const_decoder, self.repack_bits)
        self.connect(self.repack_bits, self)
