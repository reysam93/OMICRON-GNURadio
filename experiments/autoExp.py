#!/usr/bin/env python

import sys
import os
import subprocess
import time
import display_eff

from enum 			  import Enum
from threading		  import Thread
from optparse		  import OptionParser

from display_per      import display_PER
from display_snr      import display_mean_SNR
from display_delay    import display_mean_delay
from display_encoding import display_encoding
from display_eff      import display_eff
from display_per_eff  import display_PER_eff

#destination = '/home/'+user+'/GNURadio/OMICRON-GNURadio/experiments/auto/'



class Type(Enum):
	TIME = 1
	FREQ = 2


class AutoRemoteSimulator:

	def __init__(self,type, options):
		self.user = options.user
		self.userR = options.remote
		self.ip = options.ip
		self.direction = options.remote+'@'+options.ip

		self.maxGain = options.maxGain
		self.minGain = options.minGain
		self.step = int(options.step)
		self.timeOfExp = int(options.timeOfExp)
		self.iteration=1

		#Path local y remoto a los scripts a ejecutar
		pathLocal =  ['/home/'+self.user+'/GNURadio/OMICRON-GNURadio/experiments/FreqAdapt', '/home/'+self.user+'/GNURadio/OMICRON-GNURadio/experiments/TimeAdapt']
		pathRemote =  ['/home/'+self.userR+'/GNURadio/OMICRON-GNURadio/experiments/FreqAdapt', '/home/'+self.userR+'/GNURadio/OMICRON-GNURadio/experiments/TimeAdapt']

		if type == Type.TIME:
			self.pathL = pathLocal[1]
			self.pathR = pathRemote[1]
			self.pathTMP = '/tmp/time_*'
			self.scriptName = 'time_adapt'
			self.output = 'timeResults.txt'
			self.prefix = 'time'
		else:
			self.pathL = pathLocal[0]
			self.pathR = pathRemote[0]
			self.pathTMP = '/tmp/freq_*'
			self.scriptName = 'freq_adapt'
			self.output = 'freqResults.txt'
			self.prefix = 'freq'
		self.destination = options.destination

	def remoteGRC(self):
		subprocess.call('ssh '+self.direction+' "cd '+self.pathR+'; export DISPLAY=:0 ; ./'+self.scriptName+'_rx.py"' , shell=True)
		time.sleep(self.timeOfExp)
		pid = subprocess.check_output('ssh '+self.direction+' "ps -ef | grep '+self.scriptName+' | awk \'{print $2}\'"', shell=True)
		pid = pid.split()
		pid = pid[16]
		subprocess.call('ssh '+self.direction+' kill '+pid, shell=True)

	#Arranque de GRC
	def start(self):
		remote = Thread(target=self.remoteGRC)
		remote.start()
		time.sleep(10)
		subprocess.call(self.pathL+'/'+self.scriptName+'_tx.py &',shell=True)
		return remote

	#Kill de GRC local y tiempo de espera al kill remoto
	def kill(self):
		time.sleep(self.timeOfExp)
		subprocess.call('ps -ef | grep adapt | awk \'{print $2}\' | xargs kill',shell=True)

	#Scp y mv de resultados
	def move(self):
		subprocess.call('scp '+self.direction+':'+self.pathTMP+' /tmp',shell=True)
		subprocess.call('mv '+self.pathTMP+' '+self.destination+'/p'+str(self.iteration),shell=True)

	def writeAnalysis(self):

		file = open(self.destination+'/p'+str(self.iteration)+'/'+self.output,"w")
		path = self.destination+'/p'+str(self.iteration)+'/'+self.prefix

		mean_eff = self.analizeEff(path)
		file.write("Spectral eff: {0}\n".format(mean_eff))
		file.write("\n")

		meanSNR,simSNR = self.analizeSNR(path)
		if isinstance(meanSNR, list):
			for i in range(len(mean_SNR)):
				file.write("Mean SNR RB {0} (dB): {1:.2f}\n".format(i, meanSNR[i]))
			file.write("Equivalent SNR: {0:.2f}\n".format(simSNR))
		else:
			file.write("Mean SNR (dB): {0:.2f}\n".format(meanSNR))
		file.write("\n")

		n_encoding,EncoNames,total = self.analizeEncoding(path)
		i= 0
		for name, _ in EncoNames:
			if n_encoding[i] != 0:
				encoding_percent = 100 * n_encoding[i]/float(total)
				file.write("   {0}: {1:.2f}%\n".format(name[1:],encoding_percent))
			i+=1
		file.write("\n")

		PER = self.analizePER(path)
		file.write("PER (%): {0:.2f}\n".format(PER))
		file.write('\n')

		EncoNames,rx,tx,total= self.analizePER_EFF(path)
		i = 0
		file.write("PER (PER normalized): \n")
		for name, _ in EncoNames:
			per = 0
			if tx[i] != 0:
				per = 100*(tx[i] - rx[i])/float(tx[i])

			if per != 0:
				prob = tx[i]/float(total)
				nper = per*prob
				file.write("   {0}: {1:.2f}%   ({2:.2f}%)\n".format(name[1:], per, nper))
			i+=1
		file.write('\n')

		meanDelay = self.analizeDelay(path)
		file.write("Mean WiFi frame delay (ms): {0:.2f}\n".format(meanDelay))

	def analizeEff(self,path):
		f = openDataFile(path+"_transmited_encoding_tx.csv")
		f2 = openDataFile(path+"_received_encoding_rx.csv")
		return display_eff(f,f2)

	def analizeSNR(self,path):
		f = openDataFile(path+"_snr_rx.csv")
		return display_mean_SNR(f,False)

	def analizeEncoding(self,path):
		f = openDataFile(path+"_transmited_encoding_tx.csv")
		return display_encoding(f,False)

	def analizePER(self,path):
		f = openDataFile(path+"_transmited_encoding_tx.csv")
		f2 = openDataFile(path+"_received_encoding_rx.csv")
		return display_PER(f,f2)

	def analizePER_EFF(self,path):
		f = openDataFile(path+"_transmited_encoding_tx.csv")
		f2 = openDataFile(path+"_received_encoding_rx.csv")
		return display_PER_eff(f,f2)

	def analizeDelay(self,path):
		f = openDataFile(path+"_wifi_frame_delay_rx.csv")
		return display_mean_delay(f,False)

	def run(self):
		#Creacion de directorio
		subprocess.call('mkdir -p '+self.destination+'/p'+str(self.iteration), shell=True)
		remote= self.start()
		self.kill()
		remote.join()
		self.move()
		self.writeAnalysis()
		self.iteration +=1

def parseArgs():
	parser = OptionParser()
	parser.add_option("-u", "--user", help="Local user",default='lab0')
	parser.add_option("-r", "--remote", help="Remote user",default='lab1')
	parser.add_option("--ip", help="Remote ip",default='10.1.144.254')
	parser.add_option("-d", "--destination", help="Destination of the files")

	parser.add_option("--minGain", help="Min gain value for the swept", default=0.8)
	parser.add_option("--maxGain", help="Min gain value for the swept", default=0.8)
	parser.add_option("-s", "--step", help="Step value for the gain swept", default=0.05)

	parser.add_option("-t","--timeOfExp", help="Experiment time in seconds", default=20)
	parser.add_option("-n","--numOfExp", help="Number of experiment for each step", default=1)

	(options, args) = parser.parse_args()

	if (not options.destination):
		options.destination = "./autoExp"
	return (options, args)

def openDataFile(file):
	try:
		return open(file)
	except IOError as e:
		print ("I/O error: {0}").format(e)
		return None

if __name__ == "__main__":

	(options, args) = parseArgs()
	autoRemoteSimTime = AutoRemoteSimulator(Type.TIME,options)
	autoRemoteSimFreq = AutoRemoteSimulator(Type.FREQ,options)

	for i in  xrange(0,options.numOfExp):
		autoRemoteSimTime.run()
		autoRemoteSimFreq.run()
