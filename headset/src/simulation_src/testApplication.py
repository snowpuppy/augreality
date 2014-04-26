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
sys.path.append(os.path.realpath('../gui_src/'))
from guiNetInterface import *
from gpuPyInterface import *

ghost1State = 0
ghost2State = 0
ghost3State = 0
ghost4State = 0

def initGhosts(objs):
	for i in objs['ghost']:
		i.xvel = 0;
		i.yvel = 0;

def moveGhosts(objs):
	# move ghost by .2 meters (2 decimeters)
	ghosts = objs['ghost']
	global ghost1State
	global ghost2State
	global ghost3State
	global ghost4State
	# Ghost1
	if (ghost1State == 0):
		ghosts[0].yaw = 180
		if (ghosts[0].x3 > -2):
			ghosts[0].x3 -= .4;
		else:
			ghost1State = 1
	if (ghost1State == 1):
		ghosts[0].yaw = -90
		if (ghosts[0].y3 > -21):
			ghosts[0].y3 -= .4;
		else:
			ghost1State = 2
	if (ghost1State == 2):
		ghosts[0].yaw = 0
		if (ghosts[0].x3 < 22):
			ghosts[0].x3 += .4
		else:
			ghost1State = 3
	if (ghost1State == 3):
		ghosts[0].yaw = 90
		if (ghosts[0].y3 < -7):
			ghosts[0].y3 += .4;
		else:
			ghost1State = 4
	if (ghost1State == 4):
		ghosts[0].yaw = 180
		if (ghosts[0].x3 > -2):
			ghosts[0].x3 -= .4;
		else:
			ghost1State = 1
	print "GhostState:", ghost1State, "x3:",x3,"y3:",y3
	# Ghost2
	# Ghost3
	# Ghost4

def collideWithPellet(objs):
	# pass in users position
	# and return the pellet that
	# a user collides with so that
	# it can be made hidden. Don't
	# collide with hidden pellets.
	pos = getUserPosition()
	print pos
	x = pos[3]
	y = pos[4]
	# coordinate adjustments
	x = -pos[4]
	y = pos[3]
	print pos
	pellets = objs['pellet']
	# Find distance player is from pellet.
	# Return any pellet that the player collides
	# with.
	for i in pellets:
		d = sqrt((i.x3 -x)**2 + (i.y3 - y)**2)
		if d < 2:
			if i.typeShow == 0:
				return pellet
	# Return nothing if no collision.
	return None

# hardcoded id of headset.
subprocess.Popen(["../gpu_src/render/render", "simulations/pacman/pacman", "simulations/pacman/models/"])
time.sleep(1.0)
output = getRunning();
print "Running", output

resetGPSOrigin()

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
#	def __init__(self, instId, typeShow, x2, y2, x3, y3, z3, roll, pitch, yaw, scale, name):
initGhosts(myObjs)
while (1):
	moveGhosts(myObjs);
	sendUpdateObjsGpu(len(myObjs['ghost']), myObjs['ghost'])
	pellet = collideWithPellet(myObjs)
	if pellet and pellet.typeShow == 1:
		pellet.typeShow = 0
	time.sleep(.1)
	sendUpdateObjsGpu(len(myObjs['pellet']), myObjs['pellet'])
	time.sleep(.1)
time.sleep(3.0)
gpuQuit()

