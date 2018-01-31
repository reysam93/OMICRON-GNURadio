#!/usr/bin/env python

import sys
import os


def display_mean_delay(delay_f,verbose):
	delays = delay_f.read().split("\n")[2:-1]

	if len(delays) == 0:
		print("ERROR: delays file is not correct")
		return

	mean_delay = 0
	min_delay = 1000
	max_delay = 0
	for delay in delays:
		mean_delay += float(delay)
		if float(delay)>=max_delay:
			max_delay = float(delay)
		if float(delay)<=min_delay:
			min_delay = float(delay)
	mean_delay /= len(delays)

	print("Mean WiFi frame delay (ms): {0:.2f}".format(mean_delay))
	if verbose:
		print("Max  WiFi frame delay (ms): {0:.2f}".format(max_delay))
		print("Min  WiFi frame delay (ms): {0:.2f}".format(min_delay))

	return mean_delay

if __name__ == "__main__":
	if len(sys.argv) < 2:
		print("usage: ./display_delay.py delay_file")
		sys.exit(-1)

	try:
		frame_delay_f = open(sys.argv[1])
	except IOError as e:
		print ("I/O error: {0}").format(e)
		sys.exit(-1)

	display_mean_delay(frame_delay_f,True)
	frame_delay_f.close()
