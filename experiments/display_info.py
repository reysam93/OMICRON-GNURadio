#!/usr/bin/env python

import sys
import os
from optparse import OptionParser

from display_rate import display_mean_rate
from display_per import display_PER
from display_snr import display_mean_SNR
from display_delay import display_mean_deay


def display_rx_data(data_f):
	stats = os.stat(data_f)
	print("File size (bytes): {0}".format(stats.st_size))	


if __name__ == "__main__":
	if len(sys.argv) != 7:
		print("usage: ./display_info.py rx_data rate tx_packets rx_packets snr frame_delay")
		sys.exit(-1)

	rate_f = None
	tx_packets_f = None
	rx_packets_f = None
	snr_f = None
	frame_delay_f = None

	rx_data_f = sys.argv[1]
	try:
		rate_f = open(sys.argv[2])
		tx_packets_f = open(sys.argv[3])
		rx_packets_f = open(sys.argv[4])
		snr_f = open(sys.argv[5])
		frame_delay_f = open(sys.argv[6])
	except IOError as e:
		print ("I/O error: {0}").format(e)

	if rx_data_f:
		display_rx_data(rx_data_f)

	if rate_f:
		display_mean_rate(rate_f)
		rate_f.close()

	if tx_packets_f and rx_packets_f:
		display_PER(tx_packets_f, rx_packets_f)
		tx_packets_f.close()
		rx_packets_f.close()

	if snr_f:
		display_mean_SNR(snr_f)
		snr_f.close()

	if frame_delay_f:
		display_mean_deay(frame_delay_f)
		frame_delay_f.close()