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

def display_encoding_adaptative(encodings):
	encodingPunct = [[],[]]
	for encoding in encodings:
		encodingAux = encoding.split(",")
		encodingPunct[int(encoding[-1])].append(encodingAux[:-1])

	encodingTotal = [[0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0]]

	for encoding in encodingPunct[0]:
		for i, encodingrb in enumerate(encoding):
			encodingrb = int(encodingrb)
			if encodingrb == 0:
				encodingTotal[i][0]+=1;
			elif encodingrb == 1:
				encodingTotal[i][2]+=1
			elif encodingrb == 2:
				encodingTotal[i][4]+=1
			else:
				encodingTotal[i][6]+=1

	for encoding in encodingPunct[1]:
		for i, encodingrb in enumerate(encoding):
			encodingrb = int(encodingrb)
			if encodingrb == 0:
				encodingTotal[i][1]+=1;
			elif encodingrb == 1:
				encodingTotal[i][3]+=1
			elif encodingrb == 2:
				encodingTotal[i][5]+=1
			else:
				encodingTotal[i][7]+=1

	for index,rb in enumerate(encodingTotal):
		print("Resource Block {0}".format(index))
		i= 0
		for name, _ in Encoding.__members__.items():
			if rb[i] != 0:
				encoding_percent = 100 * rb[i]/float(len(encodings))
				print ("   {0}: {1:.2f}%".format(name[1:],encoding_percent))
			i+=1
		print("")


def display_encoding(encoding_f,verbose):
	encodings = encoding_f.read()
	encodings = encodings.split("\n")[:-1]

	if "," in encodings[0]:
		if verbose :
			display_encoding_adaptative(encodings)
		encodings =translate_data(encodings)

	if len(encodings) == 0:
		print("ERROR: encoding file is not correct")
		return

	n_encoding = [0, 0, 0, 0, 0, 0, 0, 0]
	for encoding in encodings:
		n_encoding[int(encoding)]+=1

	print("Transmited Encoding")
	i= 0
	for name, _ in Encoding.__members__.items():
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
