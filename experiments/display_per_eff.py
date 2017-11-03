#!/usr/bin/env python

import sys
import os
from enum import Enum

class Encoding(Enum):
	_BPSK_1_2 = 0
	_BPSK_3_4 = 1
	_QPSK_1_2 = 2
	_QPSK_3_4 = 3
	_16QAM_1_2 = 4
	_16QAM_3_4 = 5
	_64QAM_2_3 = 6
	_64QAM_3_4 = 7

def translate_data(encodings):
	translated_encoding = []
	for encoding in encodings:
		encoding = encoding.split(",")
		if int(encoding[4]) ==0:
			for i in range(4):
				if int(encoding[i])==0:
					translated_encoding.append(0)
				elif int(encoding[i])==1:
					translated_encoding.append(2)
				elif int(encoding[i])==2:
					translated_encoding.append(4)
				else:
					translated_encoding.append(6)
		else:
			for i in range(4):
				if int(encoding[i])==0:
					translated_encoding.append(1)
				elif int(encoding[i])==1:
					translated_encoding.append(3)
				elif int(encoding[i])==2:
					translated_encoding.append(5)
				else:
					translated_encoding.append(7)
	return translated_encoding


def show_results(rx, tx, total):
	i = 0
	print "PER (PER normalized): "
	for name, _ in Encoding.__members__.items():
			per = 0
			if tx[i] != 0:
				per = 100*(tx[i] - rx[i])/float(tx[i])

			if per != 0:
				prob = tx[i]/float(total)
				nper = per*prob
				print ("   {0}: {1:.2f}%   ({2:.2f}%)".format(name[1:], per, nper))
			i+=1


def display_PER_eff(encodings_tx_f,encodings_rx_f):
	encodings_tx = encodings_tx_f.read()
	encodings_rx = encodings_rx_f.read()
	encodings_tx = encodings_tx.split("\n")[:-1]
	encodings_rx = encodings_rx.split("\n")[:-1]

	if "," in encodings_tx[0]:
		encodings_tx = translate_data(encodings_tx)
		encodings_rx = translate_data(encodings_rx)

	tx = [0, 0, 0, 0, 0, 0, 0, 0]
	rx = [0, 0, 0, 0, 0, 0, 0, 0]
	i = 0
	while i < len(encodings_tx):
		tx[int(encodings_tx[i])] += 1
		if i < len(encodings_rx):
			rx[int(encodings_rx[i])] += 1
		i += 1

	show_results(rx, tx, len(encodings_tx))


if __name__ == "__main__":
	if len(sys.argv) < 3:
		print("usage: ./display_per_eff.py usedEncoding_tx_f usedEncoding_rx_f")
		sys.exit(-1)

	try:
		encodings_tx_f = open(sys.argv[1])
		encodings_rx_f = open(sys.argv[2])
	except IOError as e:
		print ("I/O error: {0:}").format(e)
		sys.exit(-1)

	display_PER_eff(encodings_tx_f, encodings_rx_f)

	encodings_tx_f.close()
	encodings_rx_f.close()
