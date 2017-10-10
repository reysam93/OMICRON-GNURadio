#!/usr/bin/env python

import sys
import os
from numpy import var

def display_mean_SNR(snr_f,verbose):
	SNRs = snr_f.read().split("\n")[:-1]

	if len(SNRs) == 0:
		print("ERROR: SNR file is not correct")
		return

	if SNRs[0].find(",") > 0:
		mean_SNR = [0, 0, 0, 0]
		max_SNR = [0, 0, 0, 0]
		min_SNR = [100, 100 ,100 ,100]

		for SNR in SNRs:
			SNR_rbs = SNR.split(", ")
			for i in range(len(SNR_rbs)):
				fSNR = float(SNR_rbs[i])
				if fSNR>=max_SNR[i]:
					max_SNR[i] = fSNR
				if fSNR<=min_SNR[i]:
					min_SNR[i] = fSNR	
				mean_SNR[i] += fSNR

		for i in range(len(mean_SNR)):
			mean_SNR[i] /= len(SNRs)
			print("Mean SNR RB {0} (dB): {1:.2f}".format(i, mean_SNR[i]))
			if verbose:
				print("Max  SNR RB {0} (dB): {1:.2f}".format(i, max_SNR[i]))
				print("Min  SNR RB {0} (dB): {1:.2f} \n".format(i, min_SNR[i]))
		
		whole_mean_SNR = 0

		for i in range(len(mean_SNR)):
			whole_mean_SNR += mean_SNR[i]
		whole_mean_SNR /= len(mean_SNR)
		print("Whole Mean SNR (dB): {0:.2f}".format(whole_mean_SNR))
		if verbose:
			print("Whole Max  SNR (dB): {0:.2f}".format(max(max_SNR)))
			print("Whole Min  SNR (dB): {0:.2f}".format(min(min_SNR)))
	else:
		mean_SNR = 0
		min_SNR=100
		max_SNR=0
		for SNR in SNRs:
			fSNR = float(SNR)
			if fSNR>=max_SNR:
				max_SNR = fSNR
			if fSNR<=min_SNR:
				min_SNR = fSNR	
			mean_SNR += fSNR

		mean_SNR /= len(SNRs)
		print("Mean SNR (dB): {0:.2f}".format(mean_SNR))
		if verbose:
			print("Max  SNR (dB): {0:.2f}".format(max_SNR))
			print("Min  SNR (dB): {0:.2f}".format(min_SNR))


if __name__ == "__main__":
	if len(sys.argv) < 2:
		print("usage: ./display_info.py snr_file")
		sys.exit(-1)

	try:
		snr_f = open(sys.argv[1])
	except IOError as e:
		print ("I/O error: {0}").format(e)
		sys.exit(-1)

	display_mean_SNR(snr_f,True)
	snr_f.close()
