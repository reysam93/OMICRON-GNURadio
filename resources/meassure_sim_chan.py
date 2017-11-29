#!/usr/bin/env python3

import os
import time
from threading import Thread
import subprocess
import sys
from optparse import OptionParser

FREQ_FILE = "../gr-frequencyAdaptiveOFDM/examples/files_loopback.py"
TIME_FILE = "../gr-adaptiveOFDM/examples/files_loopback.py"

MAX_SNR = 31
MIN_SNR = 10
STEP = 1
EXP_TIME = 3*60	# time in seconds

def replaceSNR(file, snr):
	f = open(file, "r")
	lines = f.readlines()
	f.close()

	cont = 0
	for line in lines:
		if line.find("self.snr = snr =") == -1:
			cont += 1
			continue

		newLine = "        self.snr = snr = " + str(snr) + "\n"
		break

	lines[cont] = newLine

	f = open(file, "w")
	f.writelines(lines)
	f.close()


def moveFiles(path):
	os.mkdir(path)
	subprocess.call("mv /tmp/time_* " + path, shell=True)
	subprocess.call("mv /tmp/freq_* " + path, shell=True)


def makeMeassure(cmd, fileName):
	try:
		file = open(fileName, "w") 
		subprocess.call(cmd, timeout=EXP_TIME, stdout=file, stderr=file)
	except subprocess.TimeoutExpired:
		print("\t\t" + cmd + " done")
	

def meassures(prefix, path):
	fileName = path + prefix + "_" + time.strftime("%d_%m")
	for snr in range(MIN_SNR, MAX_SNR, STEP):
		print("\tSNR: " + str(snr))
		replaceSNR(FREQ_FILE, snr)
		replaceSNR(TIME_FILE, snr)
		
		fileNameSNR = fileName + "_snr" + str(snr)
		t1 = Thread(target=makeMeassure, args=(TIME_FILE, "/tmp/time_log.log"))
		t2 = Thread(target=makeMeassure, args=(FREQ_FILE, "/tmp/freq_log.log"))
		t1.start()
		t2.start()
		t1.join()
		t2.join()

		if os.path.isdir(fileNameSNR):
			print("ERROR: file " +  fileName + "already exists")
			return -1

		moveFiles(fileNameSNR)

	return 0	


if __name__ == "__main__":
	parser = OptionParser()
	parser.add_option("-r", "--root_path", help="Root path for saving the files")
	parser.add_option("-p", "--prefix", help="Prefix for the file's name")

	(options, args) = parser.parse_args()

	if (not options.prefix):
		print ("You must indicate the prefix of the file's name")
		sys.exit(-1)

	if (not options.root_path):
		options.root_path = "."

	multiTapPath = options.root_path + "/MultiTap/"
	if not os.path.isdir(multiTapPath):
			os.mkdir(multiTapPath)

	print("MultiTap experiments:")
	if meassures(options.prefix, multiTapPath) < 0:
		sys.exit(-1)

	singleTapPath = options.root_path + "/SingleTap/"
	if not os.path.isdir(singleTapPath):
			os.mkdir(singleTapPath)

	print("SingleTap experiments:")
	if meassures(options.prefix, singleTapPath) < 0:
		sys.exit(-1)

	sys.exit(0)
