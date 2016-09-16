#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import sys
import math
import random

if len(sys.argv) > 3:
	print "USAGE: preamble_maker.py [fft_len] [occupied_tones]"
	sys.exit(1)

if (len(sys.argv) > 1):
	fft_len = int(sys.argv[1])
else:
	fft_len = 512

if (len(sys.argv) > 2):
	occupied_tones = int(sys.argv[2])
else:
	occupied_tones = 200

print "FFT_LEN:", fft_len, "OCCUPIED_TONES:", occupied_tones

zeros_left = int(math.ceil((fft_len - occupied_tones)/2.0))
ksfreq = [2*random.randint(0,1)-1 for i in range(occupied_tones)]

for i in range(len(ksfreq)):
	if((zeros_left + i) & 1):
		ksfreq[i] = 0

preamble = fft_len*[0]
preamble[zeros_left:zeros_left + occupied_tones] = ksfreq

print "PREAMBLE:", preamble