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
import copy
import subprocess
from math import sqrt
print sys.path[0]
os.chdir(sys.path[0])
sys.path.append(os.path.realpath('../gui_src/'))
from guiNetInterface import *
from gpuPyInterface import *


# hardcoded id of headset.
subprocess.Popen(["/home/cornell/augreality/headset/src/gpu_src/render/render", "simulations/pacman/pacman", "simulations/pacman/models/"])
time.sleep(1.0)
output = getRunning();
print "Running", output

# Read in the information for each
# object from the config file.
configFile = open("simulations/pacman/config.txt");
myObjs = readObjsFromConfig(configFile)
time.sleep(3.0)

# 74 and 75 are ghosts
#	def __init__(self, instId, typeShow, x2, y2, x3, y3, z3, roll, pitch, yaw, scale, name):
gameEnd = False
while (not gameEnd):
	pos = getUserPosition()
	objs = getUpdateObjs()
	if (len(objs) > 0):
		print "Received objs!"
		sendUpdateObjsGpu(len(objs), objs)
	time.sleep(.01)
	#gameEnd = getEnd()
		
time.sleep(3.0)
gpuQuit()
