#!/usr/bin/env python

import sys
import os

def display_rx_data(data_f):
	stats = os.stat(data_f)
	print("File size (bytes): {0}".format(stats.st_size))


def display_mean_rate(rate_f):
	# First 2 lines and last line are not interesting
	rates = rate_f.read().split("\n")[2:-1]

	if len(rates) == 0:
		print("ERROR: rate file is not correct")
		return

	# Count first 0's
	n_first_ceros = 0
	while rates[n_first_ceros] == "0":
		n_first_ceros += 1

	# Count last 0's
	n_last_ceros = 0
	while rates[len(rates) - n_last_ceros - 1] == "0":
		n_last_ceros += 1

	# Remove 0's
	rates = rates[n_first_ceros:-n_last_ceros]

	mean_rate = 0
	for rate in rates:
		mean_rate += float(rate)
	mean_rate /= len(rates)
	print("Mean rate (b/s): {0}".format(mean_rate))


def display_PER(tx_p_f, rx_p_f):
	n_tx_p = float(tx_p_f.readline())
	n_rx_p = float(rx_p_f.readline())
	PER = (1 - (n_rx_p / n_tx_p)) * 100
	print("PER (%): {0}".format(PER))


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


def display_mean_deay(delay_f):
	delays = delay_f.read().split("\n")[2:-1]
	
	if len(delays) == 0:
		print("ERROR: delays file is not correct")
		return

	mean_delay = 0
	for delay in delays:
		mean_delay += float(delay)
	mean_delay /= len(delays)
	print("Mean WiFi frame delay (ms): {0}".format(mean_delay))


if __name__ == "__main__":
	if len(sys.argv) != 7:
		print("usage: ./display_info.py rx_data rate tx_packets rx_packets snr frame_delay")
		sys.exit(-1)

	rx_data_f = sys.argv[1]
	try:	
		rate_f = open(sys.argv[2])
		tx_packets_f = open(sys.argv[3])
		rx_packets_f = open(sys.argv[4])
		snr_f = open(sys.argv[5])
		frame_delay_f = open(sys.argv[6])
	except IOError as e:
		print ("I/O error: {0}").format(e.strerror)
#		sys.exit(-1)

	display_rx_data(rx_data_f)
	display_mean_rate(rate_f)
	display_PER(tx_packets_f, rx_packets_f)
	display_mean_SNR(snr_f)
	display_mean_deay(frame_delay_f)

	# Close files!!
	rate_f.close()
	tx_packets_f.close()
	rx_packets_f.close()
	snr_f.close()
	frame_delay_f.close()