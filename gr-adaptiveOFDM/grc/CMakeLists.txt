# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

install(FILES
    adaptiveOFDM_adaptive_ofdm_tx_bc.xml
    adaptiveOFDM_adaptive_ofdm_rx_cb.xml
    #adaptiveOFDM_parse_mac.xml
    adaptiveOFDM_stream_spacer.xml
    #adaptiveOFDM_mac.xml
    adaptiveOFDM_mac_and_parse.xml
    adaptiveOFDM_write_frame_data.xml
    adaptiveOFDM_frame_equalizer.xml 
    adaptiveOFDM_decode_mac.xml
    adaptiveOFDM_mapper.xml DESTINATION share/gnuradio/grc/blocks
)
