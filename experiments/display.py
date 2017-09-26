#!/usr/bin/env python


import sys
import os

from optparse         import OptionParser
from display_rate     import display_mean_rate
from display_per      import display_PER
from display_snr      import display_mean_SNR
from display_delay    import display_mean_delay
from display_encoding import display_encoding


def openDataFile(file):
	try:	
		return open(file)
	except IOError as e:
		print ("I/O error: {0}").format(e)
		return None


if __name__ == "__main__":
	parser = OptionParser()
	parser.add_option("-t", "--time",action="store_true",help="Prefix of data files")
	parser.add_option("-f","--freq",action="store_true",help="Prefix of data files")
	parser.add_option("-q","--quiet",action="store_true",help="Print less data")
	parser.add_option("--path",help="Path of the data files")
	parser.add_option("--tx",action="store_true",help="Transmision files")
	parser.add_option("--rx",action="store_true",help="Recepction files")

	parser.add_option("-r","--rate",action="store_true",help="Print rate info")
	parser.add_option("-s","--snr",action="store_true",help="Print SNR info")
	parser.add_option("-e","--encoding",action="store_true",help="Print encoding info")
	parser.add_option("-p","--per",action="store_true",help="Print PER info")
	parser.add_option("-d","--delay",action="store_true",help="Print frame delay info")

	(options, args) = parser.parse_args()

	if (options.time and options.freq) or (not options.time and not options.freq):
		print ("You must indicate the prefix of data files")
		sys.exit(-1)

	if (not options.tx and not options.rx) or (options.tx and options.rx):
		print("Tx files choosen by default")
		filetype = "tx"

	if options.tx:
		filetype = "tx"
	else:
		filetype = "rx"


	if options.time:
		prefix = "/time"
	else:
		prefix = "/freq"

	verbose = True
	if options.quiet:
		verbose = False

	if options.rate:
		f = openDataFile(options.path+prefix+"_rate.csv")
		if f != None:
			display_mean_rate(f)
		print("")

	if options.snr:
		f = openDataFile(options.path+prefix+"_snr_"+fileType+".csv")
		if f != None:
			display_mean_SNR(f,verbose)
		print("")

	if options.encoding:	
		f = openDataFile(options.path+prefix+"_encoding_"+filetype+".csv")
		if f != None:
			display_encoding(f)
		print("")

	if options.per:
		if options.time:
			f = openDataFile(options.path+prefix+"_packets_.csv")
			f2 = openDataFile(options.path+prefix+"_packets_.csv")
			if f != None and f2!= None:
				display_PER(f,f2)
		print("")

	if options.delay:
		if options.time:
			f = openDataFile(options.path+prefix+"_wifi_frame_delay_"+filetype+".csv")
			if f != None:
				display_mean_delay(f,verbose)
		print("")


	#If all flags are false, show all
	if not options.rate and not options.snr and not options.encoding and not options.per and not options.delay:
		f = openDataFile(options.path+prefix+"_rate.csv")
		if f != None:
			display_mean_rate(f)
		print("")

		f = openDataFile(options.path+prefix+"_snr_"+filetype+".csv")
		if f != None:
			display_mean_SNR(f,verbose)
		print("")

		f = openDataFile(options.path+prefix+"_encoding_"+filetype+".csv")
		if f != None:
			display_encoding(f)
		print("")

		f = openDataFile(options.path+prefix+"_packets_tx.csv")
		f2 = openDataFile(options.path+prefix+"_packets_rx.csv")
		if f != None and f2!= None:
			display_PER(f,f2)
		print("")

		f = openDataFile(options.path+prefix+"_wifi_frame_delay_"+filetype+".csv")
		if f != None:
			display_mean_delay(f,verbose)
