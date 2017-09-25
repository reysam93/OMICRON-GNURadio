#!/usr/bin/env python

import sys
import os


def display_PER(tx_p_f, rx_p_f):
	n_tx_p = float(tx_p_f.readline())
	n_rx_p = float(rx_p_f.readline())
	PER = (1 - (n_rx_p / n_tx_p)) * 100
	print("PER (%): {0:.2f}".format(PER))


if __name__ == "__main__":
	if len(sys.argv) < 3:
		print("usage: ./display_per.py tx_packets_f rx_packets_f")
		sys.exit(-1)

	try:	
		tx_packets_f = open(sys.argv[1])
		rx_packets_f = open(sys.argv[2])
	except IOError as e:
		print ("I/O error: {0:}").format(e)
		sys.exit(-1)

	display_PER(tx_packets_f, rx_packets_f)

	tx_packets_f.close()
	rx_packets_f.close()