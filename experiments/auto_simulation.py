#!/usr/bin/env python3

import os
import time
from threading import Thread
import subprocess
import sys
from optparse import OptionParser

from display_eff  import display_eff
from display_per import display_PER
from snr_class import SNRdata
from get_channel_snr_var import  get_channel_snr_var


class AutoSimulator:
	FREQ_FILE = "../gr-frequencyAdaptiveOFDM/examples/files_loopback.py"
	TIME_FILE = "../gr-adaptiveOFDM/examples/files_loopback.py"

	REPLACED_STR_TAP = "taps=("
	REPLACED_STR_SNR = "self.snr = snr ="
	REPLACING_STR_TAP = "            taps=({0}),\n"
	REPLACING_STR_SNR = "        self.snr = snr = {0}\n"


	def __init__(self, options, taps):
		self.experiment_path = options.root_path + "/taps_" + taps + "/"
		self.fileName = options.prefix + "_" + time.strftime("%d_%m")
		self.data = {"SNR": [], "TimeEff": [], "FreqEff": [], "Gain": [],
					"TimePer": [], "FreqPer": [], "TimeSNR": [], "FreqSNR": [],
					 "TimeChanVar": [], "FreqChanVar": [], "RBVar": []}

		self.maxSnr = int(options.max_snr)
		self.minSnr = int(options.min_snr)
		self.step = int(options.step)
		self.exp_time = int(options.time)

		if not os.path.isdir(self.experiment_path):
			os.mkdir(self.experiment_path)


	def replaceString(self, file, oldStr, newStr):
		fd = open(file, "r")
		lines = fd.readlines()
		fd.close()

		cont = 0
		for line in lines:
			if line.find(oldStr) == -1:
				cont += 1
				continue

			newLine = newStr
			break

		lines[cont] = newLine

		fd = open(file, "w")
		fd.writelines(lines)
		fd.close()


	def makeMeassure(self, cmd, logFile):
		try:
			file = open(logFile, "w")
			subprocess.call(cmd, timeout=self.exp_time, stdout=file, stderr=file)
		except subprocess.TimeoutExpired:
			print("\t\t" + cmd + " done")


	def moveFiles(self, path):
		os.mkdir(path)
		subprocess.call("mv /tmp/time_* " + path, shell=True)
		subprocess.call("mv /tmp/freq_* " + path, shell=True)


	def getEff(self, path):
		f = open(path + "_transmited_encoding_tx.csv")
		f2 = open(path + "_received_encoding_rx.csv")
		return display_eff(f,f2)


	def getPER(self, path):
		f = open(path + "_transmited_encoding_tx.csv")
		f2 = open(path + "_received_encoding_rx.csv")
		return display_PER(f,f2)


	def getSNR(self, path):
		f = open(path + "_snr_rx.csv")
		SNR_data = SNRdata(f, False)
		(mean_SNR, sim_SNR) = SNR_data.get_mean_SNR()
		return sim_SNR


	def getChanVar(self, path):
		f = open(path + "_snr_var_rx.csv")
		return get_channel_snr_var(f)


	def getRBVar(self, path):
		f = open(path + "_snr_rx.csv")
		SNR_data = SNRdata(f, False)
		return SNR_data.get_rb_snr_var()


	def meassures(self):
		file = self.experiment_path + self.fileName

		for snr in range(self.minSnr, self.maxSnr, self.step):
			print("\tSNR: " + str(snr))
			self.replaceString(self.FREQ_FILE, self.REPLACED_STR_SNR,
							self.REPLACING_STR_SNR.format(str(snr)))
			self.replaceString(self.TIME_FILE, self.REPLACED_STR_SNR,
							self.REPLACING_STR_SNR.format(str(snr)))

			fileSNR = file + "_snr" + str(snr)
			t1 = Thread(target=self.makeMeassure, args=(self.TIME_FILE, "/tmp/time_log.log"))
			t2 = Thread(target=self.makeMeassure, args=(self.FREQ_FILE, "/tmp/freq_log.log"))
			t1.start()
			t2.start()
			t1.join()
			t2.join()

			if os.path.isdir(fileSNR):
				print("ERROR: file " +  fileSNR + "already exists")
				return -1

			self.moveFiles(fileSNR)

			time_eff = self.getEff(fileSNR + "/time")
			freq_eff = self.getEff(fileSNR + "/freq")
			self.data["SNR"].append(snr)
			self.data["TimeEff"].append(time_eff)
			self.data["FreqEff"].append(freq_eff)
			self.data["Gain"].append((freq_eff-time_eff)/time_eff)
			self.data["TimePer"].append(self.getPER(fileSNR + "/time"))
			self.data["FreqPer"].append(self.getPER(fileSNR + "/freq"))
			self.data["TimeSNR"].append(self.getSNR(fileSNR + "/time"))
			self.data["FreqSNR"].append(self.getSNR(fileSNR + "/freq"))
			self.data["TimeChanVar"].append(self.getChanVar(fileSNR + "/time"))
			self.data["FreqChanVar"].append(self.getChanVar(fileSNR + "/freq"))
			self.data["RBVar"].append(self.getRBVar(fileSNR + "/freq"))

		return 0


	def prepate_files(self, taps):
		self.replaceString(self.TIME_FILE, self.REPLACED_STR_TAP,
							self.REPLACING_STR_TAP.format(taps))
		self.replaceString(self.FREQ_FILE, self.REPLACED_STR_TAP,
							self.REPLACING_STR_TAP.format(taps))


	def toCSV(self):
		file_name = self.experiment_path + self.fileName + ".csv"
		csv_file = open(file_name, "w")
		data_str = "SNR, TimeEff, FreqEff, Gain, TimePer, FreqPer, TimeSNR, \
					FreqSNR, TimeChanVar, FreqChanVar, RBVar\n"

		for i in range(len(self.data["SNR"])):
			data_str += str(self.data["SNR"][i])
			data_str += ", " + str(self.data["TimeEff"][i])
			data_str += ", " + str(self.data["FreqEff"][i])
			data_str += ", " + str(self.data["Gain"][i])
			data_str += ", " + str(self.data["TimePer"][i])
			data_str += ", " + str(self.data["FreqPer"][i])
			data_str += ", " + str(self.data["TimeSNR"][i])
			data_str += ", " + str(self.data["FreqSNR"][i])
			data_str += ", " + str(self.data["TimeChanVar"][i])
			data_str += ", " + str(self.data["FreqChanVar"][i])
			data_str += ", " + str(self.data["RBVar"][i]) + "\n"

		csv_file.write(data_str)
		csv_file.close()


def parseArgs():
	parser = OptionParser()
	parser.add_option("-r", "--root_path", help="Root path for saving the files")
	parser.add_option("-p", "--prefix", help="Prefix for the file's name")
	parser.add_option("--min_snr", help="Min snr value for the swept", default=10)
	parser.add_option("--max_snr", help="Min snr value for the swept", default=31)
	parser.add_option("-s", "--step", help="Step value for the snr swept", default=1)
	parser.add_option("-t","--time", help="Experiment time in seconds", default=2*60)
	(options, args) = parser.parse_args()

	if (not options.prefix):
		print ("You must indicate the prefix of the file's name")
		sys.exit(-1)

	if (not options.root_path):
		options.root_path = "."

	return (options, args)

if __name__ == "__main__":
	CHANNELS_TAPS = ["1,","taps1","taps2"]

	(options, args) = parseArgs()

	print("OPTIONS:",options)

	for taps in CHANNELS_TAPS:
		print("Experiment taps: " + taps)
		autoSim = AutoSimulator(options, taps)
		autoSim.prepate_files(taps)
		if autoSim.meassures() < 0:
			sys.exit(-1)
		autoSim.toCSV()

	sys.exit(0)
