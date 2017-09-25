#!/usr/bin/env python

import sys
import os


def display_mean_SNR(snr_f):
	SNRs = snr_f.read().split("\n")[:-1]

	if len(SNRs) == 0:
		print("ERROR: SNR file is not correct")
		return

	if SNRs[0].find(",") > 0:
		mean_SNR = [0, 0, 0, 0]

		for SNR in SNRs:
			SNR_rbs = SNR.split(", ")
			for i in range(len(SNR_rbs)):
				mean_SNR[i] += float(SNR_rbs[i])

		for i in range(len(mean_SNR)):
			mean_SNR[i] /= len(SNRs)
			print("Mean SNR RB {0} (dB): {1}".format(i, mean_SNR[i]))
		
		whole_mean_SNR = 0
		for i in range(len(mean_SNR)):
			whole_mean_SNR += mean_SNR[i]
		whole_mean_SNR /= len(mean_SNR)
		print("Whole Mean SNR (dB): {0}".format(whole_mean_SNR))
	else:
		mean_SNR = 0
		for SNR in SNRs:
			mean_SNR += float(SNR)

		mean_SNR /= len(SNRs)
		print("Mean SNR (dB): {0}".format(mean_SNR))


if __name__ == "__main__":
	if len(sys.argv) < 2:
		print("usage: ./display_info.py snr_file")
		sys.exit(-1)

	try:
		snr_f = open(sys.argv[1])
	except IOError as e:
		print ("I/O error: {0}").format(e)
		sys.exit(-1)

	display_mean_SNR(snr_f)
	snr_f.close()
