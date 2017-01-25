#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Files Loopback
# Generated: Wed Jan 25 13:21:48 2017
##################################################

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"

import os
import sys
sys.path.append(os.environ.get('GRC_HIER_PATH', os.path.expanduser('~/.grc_gnuradio')))

from PyQt4 import Qt
from PyQt4.QtCore import QObject, pyqtSlot
from gnuradio import blocks
from gnuradio import channels
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import qtgui
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from gnuradio.filter import pfb
from gnuradio.qtgui import Range, RangeWidget
from optparse import OptionParser
from wifi_freq_adap_phy_hier import wifi_freq_adap_phy_hier  # grc-generated hier_block
import adaptiveOFDM
import foo
import frequencyAdaptiveOFDM
import sip
import threading
import time
from gnuradio import qtgui


class files_loopback(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Files Loopback")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Files Loopback")
        qtgui.util.check_set_qss()
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "files_loopback")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())

        ##################################################
        # Variables
        ##################################################
        self.snr = snr = 15
        self.pdu_length = pdu_length = 500
        self.out_buf_size = out_buf_size = 96000
        self.interval = interval = 300
        self.epsilon = epsilon = 0
        self.encoding = encoding = [1,2,2,1]
        self.chan_est = chan_est = 0

        ##################################################
        # Blocks
        ##################################################
        self.frequencyAdaptiveOFDM_mac_and_parse_0 = frequencyAdaptiveOFDM.mac_and_parse(([0x42, 0x42, 0x42, 0x42, 0x42, 0x42]), ([0x42, 0x42, 0x42, 0x42, 0x42, 0x42]), ([0xff, 0xff, 0xff, 0xff, 0xff, 0xff]), False, False)
        self._snr_range = Range(-15, 30, 0.1, 15, 200)
        self._snr_win = RangeWidget(self._snr_range, self.set_snr, "snr", "counter_slider", float)
        self.top_layout.addWidget(self._snr_win)
        self._pdu_length_range = Range(0, 1500, 1, 500, 200)
        self._pdu_length_win = RangeWidget(self._pdu_length_range, self.set_pdu_length, "pdu_length", "counter_slider", int)
        self.top_layout.addWidget(self._pdu_length_win)
        self._epsilon_range = Range(-20e-6, 20e-6, 1e-6, 0, 200)
        self._epsilon_win = RangeWidget(self._epsilon_range, self.set_epsilon, "epsilon", "counter_slider", float)
        self.top_layout.addWidget(self._epsilon_win)
        
        def _encoding_probe():
            while True:
                val = self.frequencyAdaptiveOFDM_mac_and_parse_0.get_encoding()
                try:
                    self.set_encoding(val)
                except AttributeError:
                    pass
                time.sleep(1.0 / (10))
        _encoding_thread = threading.Thread(target=_encoding_probe)
        _encoding_thread.daemon = True
        _encoding_thread.start()
            
        self._chan_est_options = [0,1,3,2]
        self._chan_est_labels = ["LS", "LMS", "STA", "Linear Comb"]
        self._chan_est_group_box = Qt.QGroupBox("chan_est")
        self._chan_est_box = Qt.QHBoxLayout()
        class variable_chooser_button_group(Qt.QButtonGroup):
            def __init__(self, parent=None):
                Qt.QButtonGroup.__init__(self, parent)
            @pyqtSlot(int)
            def updateButtonChecked(self, button_id):
                self.button(button_id).setChecked(True)
        self._chan_est_button_group = variable_chooser_button_group()
        self._chan_est_group_box.setLayout(self._chan_est_box)
        for i, label in enumerate(self._chan_est_labels):
        	radio_button = Qt.QRadioButton(label)
        	self._chan_est_box.addWidget(radio_button)
        	self._chan_est_button_group.addButton(radio_button, i)
        self._chan_est_callback = lambda i: Qt.QMetaObject.invokeMethod(self._chan_est_button_group, "updateButtonChecked", Qt.Q_ARG("int", self._chan_est_options.index(i)))
        self._chan_est_callback(self.chan_est)
        self._chan_est_button_group.buttonClicked[int].connect(
        	lambda i: self.set_chan_est(self._chan_est_options[i]))
        self.top_layout.addWidget(self._chan_est_group_box)
        self.wifi_freq_adap_phy_hier_0 = wifi_freq_adap_phy_hier(
            bandwidth=10e6,
            chan_est=0,
            encoding=encoding,
            frequency=5.89e9,
            sensitivity=0.56,
        )
        self.qtgui_time_sink_x_0 = qtgui.time_sink_f(
        	1024, #size
        	300000, #samp_rate
        	"Data Received", #name
        	1 #number of inputs
        )
        self.qtgui_time_sink_x_0.set_update_time(0.10)
        self.qtgui_time_sink_x_0.set_y_axis(-1, 1)
        
        self.qtgui_time_sink_x_0.set_y_label('Amplitude', "")
        
        self.qtgui_time_sink_x_0.enable_tags(-1, True)
        self.qtgui_time_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, qtgui.TRIG_SLOPE_POS, 0.0, 0, 0, "")
        self.qtgui_time_sink_x_0.enable_autoscale(True)
        self.qtgui_time_sink_x_0.enable_grid(False)
        self.qtgui_time_sink_x_0.enable_axis_labels(True)
        self.qtgui_time_sink_x_0.enable_control_panel(False)
        
        if not True:
          self.qtgui_time_sink_x_0.disable_legend()
        
        labels = ['', '', '', '', '',
                  '', '', '', '', '']
        widths = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
                  "magenta", "yellow", "dark red", "dark green", "blue"]
        styles = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        markers = [-1, -1, -1, -1, -1,
                   -1, -1, -1, -1, -1]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
                  1.0, 1.0, 1.0, 1.0, 1.0]
        
        for i in xrange(1):
            if len(labels[i]) == 0:
                self.qtgui_time_sink_x_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_time_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_time_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_time_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_time_sink_x_0.set_line_style(i, styles[i])
            self.qtgui_time_sink_x_0.set_line_marker(i, markers[i])
            self.qtgui_time_sink_x_0.set_line_alpha(i, alphas[i])
        
        self._qtgui_time_sink_x_0_win = sip.wrapinstance(self.qtgui_time_sink_x_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_time_sink_x_0_win)
        self.qtgui_number_sink_0 = qtgui.number_sink(
            gr.sizeof_float,
            0,
            qtgui.NUM_GRAPH_NONE,
            1
        )
        self.qtgui_number_sink_0.set_update_time(0.10)
        self.qtgui_number_sink_0.set_title("")
        
        labels = ['Error Rate', '', '', '', '',
                  '', '', '', '', '']
        units = ['Percent', '', '', '', '',
                 '', '', '', '', '']
        colors = [("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"),
                  ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black")]
        factor = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        for i in xrange(1):
            self.qtgui_number_sink_0.set_min(i, 0)
            self.qtgui_number_sink_0.set_max(i, 100)
            self.qtgui_number_sink_0.set_color(i, colors[i][0], colors[i][1])
            if len(labels[i]) == 0:
                self.qtgui_number_sink_0.set_label(i, "Data {0}".format(i))
            else:
                self.qtgui_number_sink_0.set_label(i, labels[i])
            self.qtgui_number_sink_0.set_unit(i, units[i])
            self.qtgui_number_sink_0.set_factor(i, factor[i])
        
        self.qtgui_number_sink_0.enable_autoscale(False)
        self._qtgui_number_sink_0_win = sip.wrapinstance(self.qtgui_number_sink_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_number_sink_0_win)
        self.qtgui_const_sink_x_0 = qtgui.const_sink_c(
        	48*10, #size
        	"", #name
        	1 #number of inputs
        )
        self.qtgui_const_sink_x_0.set_update_time(0.10)
        self.qtgui_const_sink_x_0.set_y_axis(-2, 2)
        self.qtgui_const_sink_x_0.set_x_axis(-2, 2)
        self.qtgui_const_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, qtgui.TRIG_SLOPE_POS, 0.0, 0, "")
        self.qtgui_const_sink_x_0.enable_autoscale(False)
        self.qtgui_const_sink_x_0.enable_grid(False)
        self.qtgui_const_sink_x_0.enable_axis_labels(True)
        
        if not True:
          self.qtgui_const_sink_x_0.disable_legend()
        
        labels = ['', '', '', '', '',
                  '', '', '', '', '']
        widths = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        colors = ["blue", "red", "red", "red", "red",
                  "red", "red", "red", "red", "red"]
        styles = [0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0]
        markers = [0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
                  1.0, 1.0, 1.0, 1.0, 1.0]
        for i in xrange(1):
            if len(labels[i]) == 0:
                self.qtgui_const_sink_x_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_const_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_const_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_const_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_const_sink_x_0.set_line_style(i, styles[i])
            self.qtgui_const_sink_x_0.set_line_marker(i, markers[i])
            self.qtgui_const_sink_x_0.set_line_alpha(i, alphas[i])
        
        self._qtgui_const_sink_x_0_win = sip.wrapinstance(self.qtgui_const_sink_x_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_const_sink_x_0_win)
        self.pfb_arb_resampler_xxx_0 = pfb.arb_resampler_ccf(
        	  1+epsilon,
                  taps=None,
        	  flt_size=32)
        self.pfb_arb_resampler_xxx_0.declare_sample_delay(0)
        	
        self._interval_range = Range(10, 1000, 1, 300, 200)
        self._interval_win = RangeWidget(self._interval_range, self.set_interval, "interval", "counter_slider", int)
        self.top_layout.addWidget(self._interval_win)
        self.foo_packet_pad2_0 = foo.packet_pad2(False, False, 0.001, 500, 0)
        (self.foo_packet_pad2_0).set_min_output_buffer(96000)
        self.channels_channel_model_0 = channels.channel_model(
        	noise_voltage=1,
        	frequency_offset=epsilon * 5.89e9 / 10e6,
        	epsilon=1.0,
        	taps=(1.0, ),
        	noise_seed=0,
        	block_tags=False
        )
        self.blocks_vector_source_x_0 = blocks.vector_source_b(range(100), True, 1, [])
        self.blocks_uchar_to_float_0 = blocks.uchar_to_float()
        self.blocks_tagged_stream_to_pdu_0 = blocks.tagged_stream_to_pdu(blocks.byte_t, 'packet_len')
        self.blocks_stream_to_tagged_stream_0 = blocks.stream_to_tagged_stream(gr.sizeof_char, 1, pdu_length, 'packet_len')
        self.blocks_socket_pdu_0 = blocks.socket_pdu("UDP_SERVER", '', '52001', 10000, False)
        self.blocks_pdu_to_tagged_stream_0_1 = blocks.pdu_to_tagged_stream(blocks.byte_t, 'packet_len')
        self.blocks_pdu_to_tagged_stream_0_0 = blocks.pdu_to_tagged_stream(blocks.complex_t, 'packet_len')
        self.blocks_pdu_to_tagged_stream_0 = blocks.pdu_to_tagged_stream(blocks.float_t, 'packet_len')
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vcc(((10**(snr/10.0))**.5, ))
        self.adaptiveOFDM_stream_spacer_0 = adaptiveOFDM.stream_spacer(blocks.byte_t, 1000)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.blocks_socket_pdu_0, 'pdus'), (self.frequencyAdaptiveOFDM_mac_and_parse_0, 'app in'))    
        self.msg_connect((self.blocks_tagged_stream_to_pdu_0, 'pdus'), (self.frequencyAdaptiveOFDM_mac_and_parse_0, 'app in'))    
        self.msg_connect((self.frequencyAdaptiveOFDM_mac_and_parse_0, 'fer'), (self.blocks_pdu_to_tagged_stream_0, 'pdus'))    
        self.msg_connect((self.frequencyAdaptiveOFDM_mac_and_parse_0, 'app out'), (self.blocks_pdu_to_tagged_stream_0_1, 'pdus'))    
        self.msg_connect((self.frequencyAdaptiveOFDM_mac_and_parse_0, 'phy out'), (self.wifi_freq_adap_phy_hier_0, 'mac_in'))    
        self.msg_connect((self.wifi_freq_adap_phy_hier_0, 'carrier'), (self.blocks_pdu_to_tagged_stream_0_0, 'pdus'))    
        self.msg_connect((self.wifi_freq_adap_phy_hier_0, 'mac_out'), (self.frequencyAdaptiveOFDM_mac_and_parse_0, 'phy in'))    
        self.connect((self.adaptiveOFDM_stream_spacer_0, 0), (self.blocks_tagged_stream_to_pdu_0, 0))    
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.channels_channel_model_0, 0))    
        self.connect((self.blocks_pdu_to_tagged_stream_0, 0), (self.qtgui_number_sink_0, 0))    
        self.connect((self.blocks_pdu_to_tagged_stream_0_0, 0), (self.qtgui_const_sink_x_0, 0))    
        self.connect((self.blocks_pdu_to_tagged_stream_0_1, 0), (self.blocks_uchar_to_float_0, 0))    
        self.connect((self.blocks_stream_to_tagged_stream_0, 0), (self.adaptiveOFDM_stream_spacer_0, 0))    
        self.connect((self.blocks_uchar_to_float_0, 0), (self.qtgui_time_sink_x_0, 0))    
        self.connect((self.blocks_vector_source_x_0, 0), (self.blocks_stream_to_tagged_stream_0, 0))    
        self.connect((self.channels_channel_model_0, 0), (self.pfb_arb_resampler_xxx_0, 0))    
        self.connect((self.foo_packet_pad2_0, 0), (self.blocks_multiply_const_vxx_0, 0))    
        self.connect((self.pfb_arb_resampler_xxx_0, 0), (self.wifi_freq_adap_phy_hier_0, 0))    
        self.connect((self.wifi_freq_adap_phy_hier_0, 0), (self.foo_packet_pad2_0, 0))    

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "files_loopback")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_snr(self):
        return self.snr

    def set_snr(self, snr):
        self.snr = snr
        self.blocks_multiply_const_vxx_0.set_k(((10**(self.snr/10.0))**.5, ))

    def get_pdu_length(self):
        return self.pdu_length

    def set_pdu_length(self, pdu_length):
        self.pdu_length = pdu_length
        self.blocks_stream_to_tagged_stream_0.set_packet_len(self.pdu_length)
        self.blocks_stream_to_tagged_stream_0.set_packet_len_pmt(self.pdu_length)

    def get_out_buf_size(self):
        return self.out_buf_size

    def set_out_buf_size(self, out_buf_size):
        self.out_buf_size = out_buf_size

    def get_interval(self):
        return self.interval

    def set_interval(self, interval):
        self.interval = interval

    def get_epsilon(self):
        return self.epsilon

    def set_epsilon(self, epsilon):
        self.epsilon = epsilon
        self.pfb_arb_resampler_xxx_0.set_rate(1+self.epsilon)
        self.channels_channel_model_0.set_frequency_offset(self.epsilon * 5.89e9 / 10e6)

    def get_encoding(self):
        return self.encoding

    def set_encoding(self, encoding):
        self.encoding = encoding
        self.wifi_freq_adap_phy_hier_0.set_encoding(self.encoding)

    def get_chan_est(self):
        return self.chan_est

    def set_chan_est(self, chan_est):
        self.chan_est = chan_est
        self._chan_est_callback(self.chan_est)


def main(top_block_cls=files_loopback, options=None):

    from distutils.version import StrictVersion
    if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()


if __name__ == '__main__':
    main()
