#!/usr/bin/python
#
# Filename: testApplication.py
# Author: Thor Smith
# Purpose: Create a sample python application to retrieve content
#          and send commands
#

import sys
import os
import socket
import time
sys.path.append(os.path.realpath('../gui_src/'))
from guiNetInterface import *

# hardcoded id of headset.
nid = '40A66DAE\r13A200\r'

while (1):
	output = getUserPosition()
	numSat = output[0]
	lat = output[1]
	lon = output[2]
	x = output[3]
	y = output[4]
	roll = output[5]
	pitch = output[6]
	yaw = output[7]
	sys.stdout.write("\rNumSat: %d Lat: %2.2f Lon: %2.2f X: %2.2f Y: %2.2f Roll: %2.2f Pitch: %2.2f Yaw: %2.2f" % (numSat, lat, lon, x,y,roll,pitch,yaw) )
	time.sleep(0.02)
