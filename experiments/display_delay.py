#!/usr/bin/env python

import sys
import os


def display_mean_deay(delay_f):
	delays = delay_f.read().split("\n")[2:-1]
	
	if len(delays) == 0:
		print("ERROR: delays file is not correct")
		return

	mean_delay = 0
	for delay in delays:
		mean_delay += float(delay)
	mean_delay /= len(delays)
	print("Mean WiFi frame delay (ms): {0}".format(mean_delay))


if __name__ == "__main__":
	if len(sys.argv) < 2:
		print("usage: ./display_info.py rx_data rate tx_packets rx_packets snr frame_delay")
		sys.exit(-1)

	try:	
		frame_delay_f = open(sys.argv[1])
	except IOError as e:
		print ("I/O error: {0}").format(e)
		sys.exit(-1)

	display_mean_deay(frame_delay_f)
	frame_delay_f.close()