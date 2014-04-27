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
os.chdir(sys.path[0])
sys.path.append(os.path.realpath('../gui_src/'))
from guiNetInterface import *
from gpuPyInterface import *

# Read in the information for each
# object from the config file.
configFile = open("simulations/pacman/config.txt");
myObjs = readObjsFromConfig(configFile)
time.sleep(3.0)
# 74 and 75 are ghosts
#	def __init__(self, instId, typeShow, x2, y2, x3, y3, z3, roll, pitch, yaw, scale, name):
gameEnd = False
while (not gameEnd):
	sendUpdateObjs(len(myObjs['ghost']), myObjs['ghost'])
	time.sleep(.1)
	objs = getUpdateObjs()
	# print what we received!
	for i in objs:
		print i
	time.sleep(.1)
	gameEnd = getEnd()
		
time.sleep(3.0)
gpuQuit()
