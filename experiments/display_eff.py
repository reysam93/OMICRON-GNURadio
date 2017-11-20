#!/usr/bin/env python

import sys
import os
from enum import Enum

# Efficencies for Time Adaptive
EFF  = [0.5, 0.75, 1, 1.5, 2, 3, 4, 4.5] #1.884
# Efficencies for Frequency Adaptive
EFFF = [0.5, 0.75, 1, 1.5, 2, 3, 3, 4.5, 4] #2.81


def translate_data(encodings):
	translated_encoding = []
	for encoding in encodings:
		encoding = encoding.split(",")
		offset = int(encoding[4])
		for i in range(4):
			if int(encoding[i]) == 0:
				translated_encoding.append(0+offset)
			elif int(encoding[i]) == 1:
				translated_encoding.append(2+offset)
			elif int(encoding[i]) == 2:
				translated_encoding.append(4+offset)
			else:
				translated_encoding.append(6+offset)
	return translated_encoding




def display_eff(encodings_tx_f,encodings_rx_f):
	encodings_tx = encodings_tx_f.read()
	encodings_rx = encodings_rx_f.read()
	encodings_tx = encodings_tx.split("\n")[:-1]
	encodings_rx = encodings_rx.split("\n")[:-1]

	mods_rx = [0, 0, 0, 0, 0, 0, 0, 0]
	efficencies = EFF

	if "," in encodings_tx[0]:
		encodings_tx = translate_data(encodings_tx)
		encodings_rx = translate_data(encodings_rx)
		efficencies = EFFF


	for encoding in encodings_rx:
		mods_rx[int(encoding)]+=1

	total = len(encodings_tx)

	probs =  [0, 0, 0, 0, 0, 0, 0, 0]
	mean_eff = 0
	for index,_ in enumerate(probs):
		probs[index] = mods_rx[index]/float(total)
		mean_eff += probs[index]*efficencies[index]
		
	print("Spectral eff: {0}".format(mean_eff))

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

	display_eff(encodings_tx_f, encodings_rx_f)

	encodings_tx_f.close()
	encodings_rx_f.close()
