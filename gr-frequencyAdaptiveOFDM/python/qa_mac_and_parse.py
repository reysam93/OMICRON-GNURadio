#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2017 samuel.rey.escudero@gmail.com.
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
import pmt
from gnuradio import blocks
import frequencyAdaptiveOFDM_swig as frAdapOFDM
import random

class qa_mac_and_parse (gr_unittest.TestCase):
    def setUp (self):
        src_mac = ([0x42, 0x42, 0x42, 0x42, 0x42, 0x42])
        dst_mac = src_mac
        bss_mac = ([0xff, 0xff, 0xff, 0xff, 0xff, 0xff])
        debug = debug_acks = debug_delay = False
        tx_pkg_f = rx_pkg_f = ""
        self.mac_and_parse = frAdapOFDM.mac_and_parse(src_mac, dst_mac, bss_mac,
                            debug, debug_acks, debug_delay, tx_pkg_f, rx_pkg_f)

    def tearDown (self):
        self.mac_and_parse = None

    def rand_snr(self, min_snr, max_snr):
        snr = []
        if type(min_snr) is not list:
            min_snr = [min_snr] * frAdapOFDM.N_RB

        if type(max_snr) is not list:
            max_snr = [max_snr] * frAdapOFDM.N_RB

        for i in range(frAdapOFDM.N_RB):
            snr.append(random.uniform(min_snr[i], max_snr[i] - 1e-3))
        return snr


    def test_001_decide_encoding(self):
        enc = self.rand_snr(frAdapOFDM.MIN_SNR_BPSK_1_2, frAdapOFDM.MIN_SNR_BPSK_3_4)
        self.mac_and_parse.decide_encoding(enc);
        self.assertEqual((0, 0, 0, 0), self.mac_and_parse.getEncoding())
        self.assertEqual(frAdapOFDM.P_1_2, self.mac_and_parse.getPuncturing())

    def test_002_decide_encoding(self):
        enc = self.rand_snr(frAdapOFDM.MIN_SNR_BPSK_3_4, frAdapOFDM.MIN_SNR_QPSK_1_2)
        self.mac_and_parse.decide_encoding(enc);
        self.assertEqual((0, 0, 0, 0), self.mac_and_parse.getEncoding())
        self.assertEqual(frAdapOFDM.P_3_4, self.mac_and_parse.getPuncturing())

    def test_003_decide_encoding(self):
        enc = self.rand_snr(frAdapOFDM.MIN_SNR_QPSK_1_2, frAdapOFDM.MIN_SNR_QPSK_3_4)
        self.mac_and_parse.decide_encoding(enc);
        self.assertEqual((1, 1, 1, 1), self.mac_and_parse.getEncoding())
        self.assertEqual(frAdapOFDM.P_1_2, self.mac_and_parse.getPuncturing())

    def test_004_decide_encoding(self):
        enc = self.rand_snr(frAdapOFDM.MIN_SNR_QPSK_3_4, frAdapOFDM.MIN_SNR_16QAM_1_2)
        self.mac_and_parse.decide_encoding(enc);
        self.assertEqual((1, 1, 1, 1), self.mac_and_parse.getEncoding())
        self.assertEqual(frAdapOFDM.P_3_4, self.mac_and_parse.getPuncturing())

    def test_005_decide_encoding(self):
        enc = self.rand_snr(frAdapOFDM.MIN_SNR_16QAM_1_2, frAdapOFDM.MIN_SNR_16QAM_3_4)
        self.mac_and_parse.decide_encoding(enc);
        self.assertEqual((2, 2, 2, 2), self.mac_and_parse.getEncoding())
        self.assertEqual(frAdapOFDM.P_1_2, self.mac_and_parse.getPuncturing())

    def test_006_decide_encoding(self):
        enc = self.rand_snr(frAdapOFDM.MIN_SNR_16QAM_3_4, frAdapOFDM.MIN_SNR_64QAM_2_3)
        self.mac_and_parse.decide_encoding(enc);
        self.assertEqual((2, 2, 2, 2), self.mac_and_parse.getEncoding())
        self.assertEqual(frAdapOFDM.P_3_4, self.mac_and_parse.getPuncturing())

    def test_007_decide_encoding(self):
        enc = self.rand_snr(frAdapOFDM.MIN_SNR_64QAM_2_3, frAdapOFDM.MIN_SNR_64QAM_3_4)
        self.mac_and_parse.decide_encoding(enc);
        self.assertEqual((3, 3, 3, 3), self.mac_and_parse.getEncoding())
        self.assertEqual(frAdapOFDM.P_2_3, self.mac_and_parse.getPuncturing())

    def test_008_decide_encoding(self):
        enc = self.rand_snr(frAdapOFDM.MIN_SNR_64QAM_3_4, 50)
        self.mac_and_parse.decide_encoding(enc);
        self.assertEqual((3, 3, 3, 3), self.mac_and_parse.getEncoding())
        self.assertEqual(frAdapOFDM.P_3_4, self.mac_and_parse.getPuncturing())

    def test_009_decide_encoding(self):
        min_snr = [frAdapOFDM.MIN_SNR_64QAM_2_3, frAdapOFDM.MIN_SNR_16QAM_3_4,
                    frAdapOFDM.MIN_SNR_16QAM_1_2, frAdapOFDM.MIN_SNR_64QAM_3_4];
        max_snr = [frAdapOFDM.MIN_SNR_64QAM_3_4, frAdapOFDM.MIN_SNR_64QAM_2_3,
                    frAdapOFDM.MIN_SNR_16QAM_3_4, 50];

        enc = self.rand_snr(min_snr, max_snr)
        self.mac_and_parse.decide_encoding(enc);
        self.assertEqual((2, 2, 1, 3), self.mac_and_parse.getEncoding())
        self.assertEqual(frAdapOFDM.P_3_4, self.mac_and_parse.getPuncturing())

    def test_010_decide_encoding(self):
        min_snr = [frAdapOFDM.MIN_SNR_64QAM_2_3, frAdapOFDM.MIN_SNR_16QAM_1_2,
                    frAdapOFDM.MIN_SNR_16QAM_1_2, frAdapOFDM.MIN_SNR_BPSK_3_4];
        max_snr = [frAdapOFDM.MIN_SNR_64QAM_3_4, frAdapOFDM.MIN_SNR_16QAM_3_4,
                    frAdapOFDM.MIN_SNR_16QAM_3_4, frAdapOFDM.MIN_SNR_QPSK_1_2];

        enc = self.rand_snr(min_snr, max_snr)
        self.mac_and_parse.decide_encoding(enc);
        self.assertEqual((3, 2, 2, 0), self.mac_and_parse.getEncoding())
        self.assertEqual(frAdapOFDM.P_1_2, self.mac_and_parse.getPuncturing())

    def test_011_decide_encoding(self):
        min_snr = [frAdapOFDM.MIN_SNR_64QAM_3_4, frAdapOFDM.MIN_SNR_16QAM_3_4,
                    frAdapOFDM.MIN_SNR_QPSK_3_4, frAdapOFDM.MIN_SNR_BPSK_1_2];
        max_snr = [50, frAdapOFDM.MIN_SNR_64QAM_2_3,
                    frAdapOFDM.MIN_SNR_16QAM_1_2, frAdapOFDM.MIN_SNR_BPSK_3_4];

        enc = self.rand_snr(min_snr, max_snr)
        self.mac_and_parse.decide_encoding(enc);
        self.assertEqual((3, 2, 1, 0), self.mac_and_parse.getEncoding())
        self.assertEqual(frAdapOFDM.P_1_2, self.mac_and_parse.getPuncturing())

    def test_012_decide_encoding(self):
        min_snr = [frAdapOFDM.MIN_SNR_16QAM_3_4, frAdapOFDM.MIN_SNR_16QAM_1_2,
                    frAdapOFDM.MIN_SNR_QPSK_3_4, frAdapOFDM.MIN_SNR_16QAM_1_2];
        max_snr = [frAdapOFDM.MIN_SNR_64QAM_2_3, frAdapOFDM.MIN_SNR_16QAM_3_4,
                    frAdapOFDM.MIN_SNR_16QAM_1_2, frAdapOFDM.MIN_SNR_16QAM_3_4];

        enc = self.rand_snr(min_snr, max_snr)
        self.mac_and_parse.decide_encoding(enc);
        self.assertEqual((2, 1, 1, 1), self.mac_and_parse.getEncoding())
        self.assertEqual(frAdapOFDM.P_3_4, self.mac_and_parse.getPuncturing())

    def test_013_decide_encoding(self):
        min_snr = [frAdapOFDM.MIN_SNR_16QAM_3_4, frAdapOFDM.MIN_SNR_16QAM_1_2,
                    frAdapOFDM.MIN_SNR_QPSK_1_2, frAdapOFDM.MIN_SNR_16QAM_1_2];
        max_snr = [frAdapOFDM.MIN_SNR_64QAM_2_3, frAdapOFDM.MIN_SNR_16QAM_3_4,
                    frAdapOFDM.MIN_SNR_QPSK_3_4, frAdapOFDM.MIN_SNR_16QAM_3_4];

        enc = self.rand_snr(min_snr, max_snr)
        self.mac_and_parse.decide_encoding(enc);
        self.assertEqual((2, 2, 1, 2), self.mac_and_parse.getEncoding())
        self.assertEqual(frAdapOFDM.P_1_2, self.mac_and_parse.getPuncturing())

    def test_014_decide_encoding(self):
        min_snr = [frAdapOFDM.MIN_SNR_QPSK_1_2, frAdapOFDM.MIN_SNR_QPSK_1_2,
                    frAdapOFDM.MIN_SNR_16QAM_1_2, frAdapOFDM.MIN_SNR_16QAM_1_2];
        max_snr = [frAdapOFDM.MIN_SNR_QPSK_3_4, frAdapOFDM.MIN_SNR_QPSK_3_4,
                    frAdapOFDM.MIN_SNR_16QAM_3_4, frAdapOFDM.MIN_SNR_16QAM_3_4];

        enc = self.rand_snr(min_snr, max_snr)
        self.mac_and_parse.decide_encoding(enc);
        self.assertEqual((1, 1, 2, 2), self.mac_and_parse.getEncoding())
        self.assertEqual(frAdapOFDM.P_1_2, self.mac_and_parse.getPuncturing())

    def test_015_decide_encoding(self):
        min_snr = [frAdapOFDM.MIN_SNR_16QAM_3_4, frAdapOFDM.MIN_SNR_QPSK_3_4,
                    frAdapOFDM.MIN_SNR_QPSK_3_4, frAdapOFDM.MIN_SNR_BPSK_3_4];
        max_snr = [frAdapOFDM.MIN_SNR_64QAM_2_3, frAdapOFDM.MIN_SNR_16QAM_1_2,
                    frAdapOFDM.MIN_SNR_16QAM_1_2, frAdapOFDM.MIN_SNR_QPSK_1_2];

        enc = self.rand_snr(min_snr, max_snr)
        self.mac_and_parse.decide_encoding(enc);
        self.assertEqual((2, 1, 1, 0), self.mac_and_parse.getEncoding())
        self.assertEqual(frAdapOFDM.P_3_4, self.mac_and_parse.getPuncturing())

if __name__ == '__main__':
    gr_unittest.run(qa_mac_and_parse, "qa_mac_and_parse.xml")
