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

import numpy
import os
import pmt
from gnuradio import gr

class write_frame_data(gr.sync_block):
    """
    Receive the SNR and Encoding used in each frame and stores it in the files given
    as arguments.

        - snr_file: path for the file for the snr data
        - enc_file: path for the file for the coding scheme used
        - debug: if true, the information writen in the files will be displayed 
    """
    def __init__(self, snr_file, enc_file, debug):
        gr.sync_block.__init__(self,
            "sink",
            None,
            None)

        self.snr_file = snr_file
        self.enc_file = enc_file
        self.debug = debug

        # Reset the files
        open(self.snr_file, 'w').close()
        open(self.enc_file, 'w').close()

        self.message_port_register_in(pmt.intern("frame data"))
        self.set_msg_handler(pmt.intern("frame data"), self.write_data)


    def write_data(self, msg):
        snr = pmt.f64vector_elements(pmt.dict_ref(msg, pmt.intern("snr"), 
                                    pmt.make_vector(0, pmt.from_long(0))))
        encoding = pmt.s32vector_elements(pmt.dict_ref(msg, pmt.intern("encoding"),
                                    pmt.make_vector(0, pmt.from_long(0))))

        snr_str = ""
        enc_str = ""
        for i in range(len(snr)):
            snr_str += str(snr[i])
            enc_str += str(encoding[i])
            if i != len(snr) - 1:
                snr_str += ", "
                enc_str += ", "

        f_snr = open(self.snr_file, 'a')
        f_enc = open(self.enc_file, 'a')
        f_snr.write(snr_str + '\n')
        f_enc.write(enc_str + '\n')
        f_snr.close()
        f_enc.close()

        if self.debug:
            print("SNR:", snr_str)
            print("Encoding:", enc_str)