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
sys.path.append(os.path.realpath('../src/gui/'))
from guiNetInterface import *

# hardcoded id of headset.
nid = '40A66DAE\r13A200\r'

while (1):
	output = getPosition(nid)
	x = output[0]
	y = output[1]
	pitch = output[2]
	yaw = output[3]
	roll = output[4]
	sys.stdout.write("\rX: %2.2f Y: %2.2f Roll: %2.2f Pitch: %2.2f Yaw: %2.2f" % (x,y,roll,pitch,yaw) )
	time.sleep(0.02)
