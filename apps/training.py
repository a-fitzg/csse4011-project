import sys
import tempfile
import subprocess
from PyQt5 import QtWidgets
from PyQt5.QtCore import QThread, pyqtSignal
import serial
from PIL import Image
import binascii
from serial.tools import list_ports
import numpy as np
import csv

try:
    serialPort = serial.Serial(
        port="COM9", baudrate=115200, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)
except:
    print("We were not able to connect to COM9")

dataCount = 0

coordinate = input("Press Enter to continue...")
with open('ASS.csv', 'w', newline='') as csvfile:
    spamwriter = csv.writer(csvfile, delimiter=',',
                            quotechar='|', quoting=csv.QUOTE_MINIMAL)
    while True:
        if coordinate == 'exit':
            break
        if(serialPort.in_waiting > 0):
            serial = serialPort.readline()
            
            try:
                if dataCount < 100:
                    spamwriter.writerow([f'{beaconData.aiden_rssi}?{beaconData.george_rssi}?{beaconData.thingy_rssi}?{beaconData.dongle_rssi}?{beaconData.xander_rssi}?{beaconData.desmond_rssi}?{beaconData.thingy2_rssi}?{beaconData.dongle2_rssi}', coordinate])
                    dataCount = dataCount + 1
                else:
                    dataCount = 0
                    coordinate = input("Press Enter to continue...")
            except Exception as e:
                print(e)
                print(serial)
                continue
