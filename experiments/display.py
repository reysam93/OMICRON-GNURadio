#!/usr/bin/env python


import sys
import os

from optparse         import OptionParser
from display_rate     import display_mean_rate
from display_per      import display_PER
from display_snr      import display_mean_SNR
from display_delay    import display_mean_delay
from display_encoding import display_encoding
from display_eff      import display_eff
from display_per_eff  import display_PER_eff
from display_per_eff  import show_results


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
	parser.add_option("-s","--simple",action="store_true",help="Prefix of data files")
	parser.add_option("-v","--verbose",action="store_true",help="Print more data")
	parser.add_option("--path",help="Path of the data files")
	parser.add_option("--tx",action="store_true",help="Transmision files")
	parser.add_option("--rx",action="store_true",help="Recepction files")

	parser.add_option("-r","--rate",action="store_true",help="Print rate info")
	parser.add_option("--snr",action="store_true",help="Print SNR info")
	parser.add_option("-e","--encoding",action="store_true",help="Print encoding info")
	parser.add_option("-p","--per",action="store_true",help="Print PER info")
	parser.add_option("-d","--delay",action="store_true",help="Print frame delay info")

	(options, args) = parser.parse_args()

	if (not options.time and not options.freq and not options.simple):
		print ("You must indicate the prefix of data files")
		sys.exit(-1)

	if (not options.tx and not options.rx) or (options.tx and options.rx):
		print("Tx files choosen by default")
		filetype = "tx"

	if options.tx:
		filetype = "tx"
	if options.rx:
		filetype = "rx"


	if options.time:
		prefix = "/time"
	elif options.freq:
		prefix = "/freq"
	else:
		prefix = "/simple"

	verbose = False
	if options.verbose:
		verbose = True

	#If all flags are false, show all
	if not options.rate and not options.snr and not options.encoding and not options.per and not options.delay:
		options.rate = True
		options.snr = True
		options.encoding = True
		options.per = True
		options.delay = True

	if options.rate:
		f = openDataFile(options.path+prefix+"_transmited_encoding_tx.csv")
		f2 = openDataFile(options.path+prefix+"_received_encoding_rx.csv")
		if f != None and f2!= None:
			mean_eff = display_eff(f,f2)
			print("Spectral eff: {0}".format(mean_eff))
		print("")

	if options.snr:
		f = openDataFile(options.path+prefix+"_snr_"+filetype+".csv")
		if f != None:
			display_mean_SNR(f,verbose)
		print("")

	if options.encoding:
		f = openDataFile(options.path+prefix+"_transmited_encoding_tx.csv")
		if f != None:
			display_encoding(f,verbose)
		print("")

	if options.per:
		f = openDataFile(options.path+prefix+"_transmited_encoding_tx.csv")
		f2 = openDataFile(options.path+prefix+"_received_encoding_rx.csv")
		if f!= None and f2!= None:
			PER = display_PER(f,f2)
			print("PER (%): {0:.2f}".format(PER))
		print("")

		f = openDataFile(options.path+prefix+"_transmited_encoding_tx.csv")
		f2 = openDataFile(options.path+prefix+"_received_encoding_rx.csv")
		if f != None and f2!= None:
			EncoNames,rx,tx,total= display_PER_eff(f,f2)
			show_results(EncoNames,rx, tx,total)
		print("")

	if options.delay:
		f = openDataFile(options.path+prefix+"_wifi_frame_delay_"+filetype+".csv")
		if f != None:
			display_mean_delay(f,verbose)
		print("")
