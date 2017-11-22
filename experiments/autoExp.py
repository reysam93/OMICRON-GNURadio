#!/usr/bin/env python

import subprocess
import time
from threading import Thread


timeOfExp = 5*60
timeOfExp = 30
numOfExp = 10
numOfExp = 2

user = 'lab0'
userR = 'lab1'
ip = '10.1.135.167'
direction = userR+'@'+ip

#Path local y remoto a los scripts a ejecutar
pathL = ['/home/'+user+'/GNURadio/OMICRON-GNURadio/experiments/FreqAdapt','/home/'+user+'/GNURadio/OMICRON-GNURadio/experiments/TimeAdapt']
pathR = ['/home/'+userR+'/GNURadio/OMICRON-GNURadio/experiments/FreqAdapt','/home/'+userR+'/GNURadio/OMICRON-GNURadio/experiments/TimeAdapt']

destination = '/home/'+user+'/GNURadio/OMICRON-GNURadio/experiments/auto/'



def remoteGRC(timeBool):
	if timeBool: 
		subprocess.call('ssh '+direction+' "cd '+pathR[0]+'; export DISPLAY=:0 ; ./time_adapt_rx.py"' ,shell=True)
		time.sleep(timeOfExp+10)
		pid = subprocess.check_output('ssh '+direction+' "ps -ef | grep time_adapt | awk \'{print $2}\'"',shell=True)
	else:
		subprocess.call('ssh '+direction+' "cd '+pathR[1]+'; export DISPLAY=:0 ; ./time_adapt_rx.py"' ,shell=True)
		time.sleep(timeOfExp+10)
		pid = subprocess.check_output('ssh '+direction+' "ps -ef | grep freq_adapt | awk \'{print $2}\'"',shell=True)
	pid = pid.split()
	pid = pid[16]
	subprocess.call('ssh '+direction+' kill '+pid, shell=True)


if __name__ == "__main__":

	for i in range(numOfExp):

		#Creacion de directorio
		subprocess.call('mkdir -p '+destination+'p'+str(i), shell=True)

		#Tiempo
		#Arranque de GRC
		remote = Thread(target=remoteGRC, args=(True,))
		remote.start()
		time.sleep(5)
		subprocess.call(pathL[0]+'/time_adapt_tx.py &',shell=True)

		#Kill de GRC
		time.sleep(timeOfExp)
		subprocess.call('ps -ef | grep adapt | awk \'{print $2}\' | xargs kill',shell=True)
		remote.join()

		#Scp y mv de resultados
		subprocess.call('scp '+direction+':/tmp/time_*')
		subprocess.call('mv /tmp/time_* '+destination+'p'+str(i),shell=True)

		break

		#Frecuencia
		remote = Thread(target=remoteGRC, args=(False,))
		remote.start()
		time.sleep(5)
		subprocess.call(pathL[1]+'/freq_adapt_tx.py &',shell=True)
		time.sleep(timeOfExp)
		subprocess.call('ps -ef | grep adapt | awk \'{print $2}\' | xargs kill',shell=True)
		remote.join()
		subprocess.call('scp '+direction+':/tmp/freq_*')
		subprocess.call('mv /tmp/freq_* '+destination+'p'+str(i),shell=True)

