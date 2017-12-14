#!/usr/bin/env python3

import os
import time
from threading import Thread
import subprocess
import sys
from optparse import OptionParser

from display_eff  import display_eff
from display_per import display_PER 


class AutoSimulator:
	FREQ_FILE = "../gr-frequencyAdaptiveOFDM/examples/files_loopback.py"
	TIME_FILE = "../gr-adaptiveOFDM/examples/files_loopback.py"
	
	REPLACED_STR_TAP = "taps=("
	REPLACED_STR_SNR = "self.snr = snr ="
	REPLACING_STR_TAP = "            taps=({0}),\n"
	REPLACING_STR_SNR = "        self.snr = snr = {0}\n"


	def __init__(self, options):
		self.multiTapPath = options.root_path + "/MultiTap/"
		self.singleTapPath = options.root_path + "/SingleTap/"
		self.fileName = options.prefix + "_" + time.strftime("%d_%m")
		self.data = {"SNR": [], "TimeEff": [], "FreqEff": [], "Gain": [],
					"TimePer": [], "FreqPer": []}

		self.maxSnr = int(options.max_snr)
		self.minSnr = int(options.min_snr)
		self.step = int(options.step)
		self.exp_time = int(options.time)

		if not os.path.isdir(self.multiTapPath):
			os.mkdir(multiTapPath)

		if not os.path.isdir(self.singleTapPath):
			os.mkdir(singleTapPath)


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


	def meassures(self, path):
		file = path + self.fileName 

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

		return 0	


	def multiTapExp(self):
		print("MultiTap experiments:")
		self.replaceString(self.TIME_FILE, self.REPLACED_STR_TAP, 
							self.REPLACING_STR_TAP.format("taps"))
		self.replaceString(self.FREQ_FILE, self.REPLACED_STR_TAP,
							self.REPLACING_STR_TAP.format("taps"))

		return self.meassures(self.multiTapPath)


	def singleTapExp(self):
		print("SingleTap experiments:")
		self.replaceString(self.TIME_FILE, self.REPLACED_STR_TAP, 
							self.REPLACING_STR_TAP.format("1,"))
		self.replaceString(self.FREQ_FILE, self.REPLACED_STR_TAP,
							self.REPLACING_STR_TAP.format("1,"))

		return self.meassures(self.singleTapPath)


	def toCSV(self, file):
		csv_file = open(file, "w")

		data_str = ""
		csv_file.write(str(self.data.keys()) + "\n")
		for i in range(len(self.data["SNR"])):
			data_str += str(self.data["SNR"][i])
			data_str += ", " + str(self.data["TimeEff"][i])
			data_str += ", " + str(self.data["FreqEff"][i])
			data_str += ", " + str(self.data["Gain"][i])
			data_str += ", " + str(self.data["TimePer"][i])
			data_str += ", " + str(self.data["FreqPer"][i]) + "\n"

		csv_file.write(data_str)
		csv_file.close()


	def multiTapToCSV(self):
		file = self.multiTapPath + self.fileName + "_multitap.csv"
		self.toCSV(file)


	def singleTapToCSV(self):
		file = self.singleTapPath + self.fileName + "_singletap.csv"
		self.toCSV(file)


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
	(options, args) = parseArgs()
	autoSim = AutoSimulator(options)

	if autoSim.multiTapExp() < 0:
		sys.exit(-1)
	autoSim.multiTapToCSV()

	if autoSim.singleTapExp() < 0:
		sys.exit(-1)
	autoSim.singleTapToCSV()

	sys.exit(0)
