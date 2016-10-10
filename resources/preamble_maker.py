#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import sys
import math
import random

#NOTE: The occupied tones must include the pilots carriers also

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
ksfreq1 = [1.41421356*(2.0*random.randint(0,1)-1) for i in range(occupied_tones)]
ksfreq2 = [2*random.randint(0,1)-1 for i in range(occupied_tones)]

for i in range(len(ksfreq1)):
	if(not((zeros_left + i) & 1)):
		ksfreq1[i] = 0.

preamble1 = fft_len*[0.]
preamble1[zeros_left:zeros_left + occupied_tones] = ksfreq1

preamble2 = fft_len*[0]
preamble2[zeros_left:zeros_left + occupied_tones] = ksfreq2
preamble2.insert(fft_len/2, 0)
preamble2.pop(-1)

print "PREAMBLE 1:", preamble1
print
print "PREAMBLE 2:", preamble2