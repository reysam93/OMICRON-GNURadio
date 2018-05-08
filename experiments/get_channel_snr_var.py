#!/usr/bin/env python

import sys
from optparse         import OptionParser
from numpy            import log10


def get_channel_snr_var(file):
    SNRs = file.read().split("\n")[:-1]

    if len(SNRs) == 0:
      print("ERROR: SNR file is not correct")
      return

    chan_var = 0
    for SNR in SNRs:
        max_snr = float(SNR.split(", ")[0])
        min_snr = float(SNR.split(", ")[1])
        chan_var += 10**(max_snr/10) - 10**(min_snr/10)

    chan_var /= len(SNRs)
    return 10*log10(chan_var)


if __name__ == "__main__":
    parser = OptionParser()
    parser.add_option("-t", "--time",action="store_true",help="Prefix of data files")
    parser.add_option("-f", "--freq",action="store_true",help="Prefix of data files")
    parser.add_option("--path",help="Path of the data files")
    (options, args) = parser.parse_args()

    if options.path:
        f_name = options.path
    else:
        f_name = "/tmp"
    if options.time:
        f_name += "/time_snr_var_rx.csv"
    elif options.freq:
        f_name += "/freq_snr_var_rx.csv"
    else:
        print("Must select argument -t or -f")
        sys.exit(-1)

    print(f_name)
    f = open(f_name, 'r')
    chan_var = get_channel_snr_var(f)
    print("Channel SNR var:", chan_var)
