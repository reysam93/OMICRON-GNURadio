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

class EncodingF(Enum):
	_BPSK_1_2 = 0
	_BPSK_3_4 = 1
	_QPSK_1_2 = 2
	_QPSK_3_4 = 3
	_16QAM_1_2 = 4
	_16QAM_3_4 = 5
	_64QAM_1_2 = 6
	_64QAM_3_4 = 7
	_64QAM_2_3 = 8


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

def display_encoding(encoding_f,verbose):
	encodings = encoding_f.read()
	encodings = encodings.split("\n")[:-1]

	if "," in encodings[0]:
		encodings =translate_data(encodings)
		EncoNames = EncodingF.__members__.items()
		n_encoding = [0, 0, 0, 0, 0, 0, 0, 0, 0]
	else:
		EncoNames = Encoding.__members__.items()
		n_encoding = [0, 0, 0, 0, 0, 0, 0, 0]

	if len(encodings) == 0:
		print("ERROR: encoding file is not correct")
		return

	
	for encoding in encodings:
		n_encoding[int(encoding)]+=1

	print("Transmited Encoding")
	i= 0
	for name, _ in EncoNames:
		if n_encoding[i] != 0:
			encoding_percent = 100 * n_encoding[i]/float(len(encodings))
			print ("   {0}: {1:.2f}%".format(name[1:],encoding_percent))
		i+=1


if __name__ == "__main__":
	if len(sys.argv) < 2:
		print("usage: ./display_encoding.py encoding_file")
		sys.exit(-1)
	try:
		encoding_f = open(sys.argv[1])
	except IOError as e:
		print ("I/O error: {0}").format(e)
		sys.exit(-1)
	display_encoding(encoding_f,True)
	encoding_f.close()
