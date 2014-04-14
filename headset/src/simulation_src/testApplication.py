#!/usr/bin/python2
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
import subprocess
sys.path.append(os.path.realpath('../gui_src/'))
from guiNetInterface import *
from gpuPyInterface import *

# hardcoded id of headset.
subprocess.Popen(["../gpu_src/render/render", "simulations/pacman/pacman", "simulations/pacman/models/"])
time.sleep(1.0)
output = getRunning();
print "Running", output

#while (1):
#	output = getUserPosition()
#	numSat = output[0]
#	lat = output[1]
#	lon = output[2]
#	x = output[3]
#	y = output[4]
#	roll = output[5]
#	pitch = output[6]
#	yaw = output[7]
#	sys.stdout.write("\rNumSat: %d Lat: %2.2f Lon: %2.2f X: %2.2f Y: %2.2f Roll: %2.2f Pitch: %2.2f Yaw: %2.2f" % (numSat, lat, lon, x,y,roll,pitch,yaw) )
#	time.sleep(0.02)

# Read in the information for each
# object from the config file.
configFile = open("simulations/pacman/config.txt");
myObjs = readObjsFromConfig(configFile)
time.sleep(3.0)
x3 = 0
y3 = 5
roll = 0
pitch = 0
yaw = 0
# 74 and 75 are ghosts
ghost1 = myObjs['ghost'][0]
ghost2 = myObjs['ghost'][1]
ghost1.x3 = 2
ghost1.y3 = 3
ghost2.x3 = 2
ghost2.y3 = 2
#	def __init__(self, instId, typeShow, x2, y2, x3, y3, z3, roll, pitch, yaw, scale, name):
while (1):
	sendUpdateObjsGpu(2, [ghost1, ghost2])
	ghost1.yaw = (ghost1.yaw + 5)  % 270
	ghost2.yaw = (ghost2.yaw + 5) % 270
	ghost1.x3 = (ghost1.x3 + .2) % 5
	ghost2.y3 = (ghost2.y3 + .2) % 5
	time.sleep(.2)
time.sleep(3.0)
gpuQuit()

