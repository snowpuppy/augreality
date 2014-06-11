#!/usr/bin/python2
#
# Filename: treeSim.py
# Author: Thor Smith
# Purpose: Create a sample python application to run the tree simulation.
#

from __future__ import division
import sys
import os
import socket
import time
import copy
import subprocess
from math import sqrt
os.chdir(sys.path[0])
sys.path.append(os.path.realpath('../gui_src/'))
from guiNetInterface import *
from gpuPyInterface import *


# hardcoded id of headset.
subprocess.Popen(["../gpu_src/render/render", "simulations/trees/trees", "simulations/trees/models/"])
time.sleep(1.0)
output = getRunning();
#print "Running", output

# reset origin on game start.
resetGPSOrigin()

# Read in the information for each
# object from the config file.
configFile = open("tmp.cfg");
myObjs = readObjsFromConfig(configFile)
time.sleep(3.0)

#	def __init__(self, instId, typeShow, x2, y2, x3, y3, z3, roll, pitch, yaw, scale, name):
gameEnd = False
while (not gameEnd):
	if gameEnd:
		break
	try:
		output = getRunning();
	except:
		print "Rendering exited... Quitting simulation."
		exit()
	time.sleep(.1)
		
time.sleep(3.0)
gpuQuit()
