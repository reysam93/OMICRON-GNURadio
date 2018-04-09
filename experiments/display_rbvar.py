#!/usr/bin/env python


import sys
import os

from numpy import log10


def display_rbvar(snr_f):

    SNRs = snr_f.read().split("\n")[:-1]

    if len(SNRs) == 0:
      print("ERROR: SNR file is not correct")
      return

    min_SNR=100
    max_SNR=0
    if SNRs[0].find(",") > 0:
        for SNR in SNRs:
            SNRrbs = SNR.split(", ")
            for SNRrb in SNRrbs:
                fSNR = float(SNRrb)
                if fSNR>=max_SNR:
                    max_SNR = fSNR
                if fSNR<=min_SNR:
                    min_SNR = fSNR
    else:
        for SNR in SNRs:
            fSNR = float(SNR)
            if fSNR>=max_SNR:
                max_SNR = fSNR
            if fSNR<=min_SNR:
                min_SNR = fSNR

    rbvar = max_SNR-min_SNR
    dbvar = 10*log10(rbvar)
    return (rbvar,dbvar)


if __name__ == "__main__":
    if len(sys.argv) < 2:
       print("usage: ./display_info.py snr_file")
       sys.exit(-1)

    try:
       snr_f = open(sys.argv[1])
    except IOError as e:
       print ("I/O error: {0}").format(e)
       sys.exit(-1)
    (rbvar,dbvar) = display_rbvar(snr_f)
    print("Varianza SNR transmision: {0:.2f}/{1:.2f}\n".format(rbvar,dbvar))
    snr_f.close()
