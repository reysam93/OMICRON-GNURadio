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
    frequencyAdaptiveOFDM_mapper.xml
    frequencyAdaptiveOFDM_chunks_to_symbols.xml
    frequencyAdaptiveOFDM_frame_equalizer.xml
    frequencyAdaptiveOFDM_decode_mac.xml
    frequencyAdaptiveOFDM_mac_and_parse.xml
    frequencyAdaptiveOFDM_rb_const_demux.xml
    frequencyAdaptiveOFDM_rb_const_demux_stream.xml
    frequencyAdaptiveOFDM_display_rate_file.xml
    frequencyAdaptiveOFDM_write_frame_data.xml
    DESTINATION share/gnuradio/grc/blocks
)
