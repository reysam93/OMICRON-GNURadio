#!/usr/bin/env python

import sys
import os
from numpy import var
from numpy import log10
import matplotlib.pyplot as plt

class SNRdata:
    def __init__(self,SNRfile,verbose):
        SNRs = SNRfile.read().split("\n")[:-1]
        self.verbose = verbose
        if len(SNRs) == 0:
            print("ERROR: SNR file is not correct")
            self.SNRs = None
        else:
            self.SNRs = SNRs


    def get_mean_SNR(self):

    	if self.SNRs[0].find(",") > 0:
    		mean_SNR = [0, 0, 0, 0]
    		max_SNR = [0, 0, 0, 0]
    		min_SNR = [100, 100 ,100 ,100]
    		sim_SNR = 0
    		min_Rb = 0

    		for SNR in self.SNRs:
    			SNR_rbs = SNR.split(", ")
    			min_Rb = float(SNR_rbs[0])
    			for i in range(len(SNR_rbs)):
    				fSNR = float(SNR_rbs[i])
    				if fSNR>=max_SNR[i]:
    					max_SNR[i] = fSNR
    				if fSNR<=min_SNR[i]:
    					min_SNR[i] = fSNR
    				if fSNR<=min_Rb:
    					min_Rb = fSNR
    				mean_SNR[i] += fSNR
    			sim_SNR+= min_Rb

    		for i in range(len(mean_SNR)):
    			mean_SNR[i] /= len(self.SNRs)
    			print("Mean SNR RB {0} (dB): {1:.2f}".format(i, 10*log10(mean_SNR[i])))
    			if self.verbose:
    				print("Max  SNR RB {0} (dB): {1:.2f}".format(i, 10*log10(max_SNR[i])))
    				print("Min  SNR RB {0} (dB): {1:.2f} \n".format(i, 10*log10(min_SNR[i])))

    		sim_SNR /= len(self.SNRs)
    		whole_mean_SNR = 0

    		for i in range(len(mean_SNR)):
    			whole_mean_SNR += mean_SNR[i]
    		whole_mean_SNR /= len(mean_SNR)
    		print("Whole Mean SNR (dB): {0:.2f}".format(10*log10(whole_mean_SNR)))
    		if self.verbose:
    			print("Whole Max  SNR (dB): {0:.2f}".format(10*log10(max(max_SNR))))
    			print("Whole Min  SNR (dB): {0:.2f}".format(10*log10(min(min_SNR))))
    			plotSNR(meanRBs(SNRs))
    		print("Equivalent SNR: {0:.2f}".format(sim_SNR))
    		return (mean_SNR,sim_SNR)
    	else:
    		mean_SNR = 0
    		min_SNR=100
    		max_SNR=0
    		for SNR in self.SNRs:
    			fSNR = float(SNR)
    			if fSNR>=max_SNR:
    				max_SNR = fSNR
    			if fSNR<=min_SNR:
    				min_SNR = fSNR
    			mean_SNR += fSNR
    		mean_SNR /= len(self.SNRs)
    		print("Mean SNR (dB): {0:.2f}".format(10*log10(mean_SNR)))
    		if self.verbose:
    			print("Max  SNR (dB): {0:.2f}".format(10*log10(max_SNR)))
    			print("Min  SNR (dB): {0:.2f}".format(10*log10(min_SNR)))

    			plotSNR(self.SNRs)
    		return (mean_SNR,mean_SNR)

    def meanRBs(self,SNRs):
    	meanSNR = []
    	for snr in SNRs:
    		SNR_rbs = snr.split(", ")
    		tot = 0.0
    		for rb in SNR_rbs:
    			tot+= float(rb)
    		meanSNR.append(tot/len(snr))
    	return meanSNR

    def plotSNR(self,snr):
    	plt.plot(snr)
    	plt.ylabel('dB')
    	plt.xlabel('packet')
    	plt.show()

    def get_rb_snr_var(self):

        rbvar = 0
        if self.SNRs[0].find(",") > 0:
            for SNR in self.SNRs:
                min_SNR=100
                max_SNR=0
                SNRrbs = SNR.split(", ")
                for SNRrb in SNRrbs:
                    fSNR = float(SNRrb)
                    if fSNR>=max_SNR:
                        max_SNR = fSNR
                    if fSNR<=min_SNR:
                        min_SNR = fSNR
                rbvar += max_SNR-min_SNR
        else:
            print("Wrong format")
            return 0
        rbvar /= len(self.SNRs)
        return  10*log10(rbvar)



if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("usage: ./display_info.py snr_file")
        sys.exit(-1)
    try:
        snr_f = open(sys.argv[1])
    except IOError as e:
        print ("I/O error: {0}").format(e)
        sys.exit(-1)

    SNRdata = SNRdata(snr_f,False)
    SNRdata.get_mean_SNR()
    snr_f.close()
