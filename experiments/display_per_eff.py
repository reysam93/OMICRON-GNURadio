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


def show_results(total,errors):

	per = [0, 0, 0, 0, 0, 0, 0, 0]
	for i in range(8):
		if total[i]!=0:
			per[i]= (errors[i]/float(total[i]))
			per[i]= per[i]*100
	i=0
	print "PER: "
	for name, _ in Encoding.__members__.items():	
			if per[i] != 0:
				print ("   {0}: {1:.2f}%".format(name[1:],per[i]))
			i+=1

def display_PER_eff(encodings_tx_f,encodings_rx_f):
	encodings_tx = encodings_tx_f.read()
	encodings_rx = encodings_rx_f.read()
	encodings_tx = encodings_tx.split("\n")[2:-1]
	encodings_rx = encodings_rx.split("\n")[2:-1]


	if "," in encodings_tx[0]:
		encodings_tx = translate_data(encodings_tx)
		encodings_rx = translate_data(encodings_rx)

	total = [0, 0, 0, 0, 0, 0, 0, 0]
	errors = [0, 0, 0, 0, 0, 0, 0, 0]
	index_tx = 0
	index_rx = 0
	

	while index_tx < len(encodings_tx):
		total[int(encodings_tx[index_tx])]+=1
		if index_rx < len(encodings_rx):
			if encodings_tx[index_tx] != encodings_rx[index_rx]:
				errors[int(encodings_tx[index_tx])]+=1
			else:
				index_rx+=1
		else:
			errors[int(encodings_tx[index_tx])]+=1
		index_tx+=1
	show_results(total,errors)


	

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