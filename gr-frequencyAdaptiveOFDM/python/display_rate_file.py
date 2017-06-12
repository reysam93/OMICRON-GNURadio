#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2017 Samuel Rey <samuel.rey.escudero@gmail.com>
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from time import sleep
from threading import Thread
from threading import Lock
from datetime import datetime
import os
import sys


class display_rate_file:
    """
    This block calculate the received data rate.
    The received data must be stored in the file "read_file".
    This script check its size every second, being the rate the different
    between the file's size.
    Parameters:
        - read_file: path to the file where the data is stored
        - display_file: if different than "", it will be he path for saving 
                        the rate information
        - debug: indicates if the debug inforamtion should be displayed in the terminal
    
    Write exit in the terminal for stopping it
    """

    def __init__(self, read_file, display_file, debug):
        self.debug = debug
        self.loop = True
        self.lock = Lock()

        if os.path.isfile(read_file):
            self.read_file = read_file
        else:
            print "ERROR: wrong read file"
            return

        if display_file is "":
            self.display_file = None
        else:
            self.display_file = open(display_file, 'w')

        self.data_size = 0

        t_checkRate = Thread(target=self.checkRate)
        t_exit = Thread(target=self.exit)
        t_checkRate.start()
        t_exit.start()

    def exit(self):
        cmd = ""
        while cmd != "exit\n":
            print("Enter 'exit' for exiting")
            cmd = sys.stdin.readline()
        self.lock.acquire()
        self.loop = False
        self.lock.release()

    def checkRate(self):
        while self.loop:
            new_size = os.stat(self.read_file).st_size
            rate = new_size - self.data_size
            self.data_size = new_size

            if self.debug:
                print datetime.now(), "Rate:", rate
            if self.display_file != None:
                #text = str(datetime.now()) + ", " + str(rate) + "\n"
                self.display_file.write(str(rate) + "\n")

            sleep(1)

        if self.display_file != None:
            self.display_file.close()