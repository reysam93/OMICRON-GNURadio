#!/usr/bin/env python

import sys
import os


def display_PER(encodings_tx_f,encodings_rx_f):
	encodings_tx = encodings_tx_f.read()
	encodings_rx = encodings_rx_f.read()
	encodings_tx = encodings_tx.split("\n")[:-1]
	encodings_rx = encodings_rx.split("\n")[:-1]
	PER = (1 - (len(encodings_rx) / float(len(encodings_tx)))) * 100
	print("PER (%): {0:.2f}".format(PER))


if __name__ == "__main__":
	if len(sys.argv) < 3:
		print("usage: ./display_per.py usedEncoding_tx_f usedEncoding_rx_f")
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