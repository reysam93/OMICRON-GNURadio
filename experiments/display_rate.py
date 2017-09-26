#!/usr/bin/env python

import sys
import os


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
	print("Mean rate (b/s): {0:.2f}".format(mean_rate))


if __name__ == "__main__":
	if len(sys.argv) < 2:
		print("usage: ./display_rate.py rate_file")
		sys.exit(-1)

	try:	
		rate_f = open(sys.argv[1])
	except IOError as e:
		print ("I/O error: {0}").format(e)
		sys.exit(-1)

	display_mean_rate(rate_f)
	rate_f.close()