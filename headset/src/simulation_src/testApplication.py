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


def initGhosts(objs):
	for i in objs['ghost']:
		i.xvel = 0;
		i.yvel = 0;

def moveGhost(ghost, xmin, xmax, ymin, ymax):
	if (ghost.state == 0):
		ghost.yaw = 180
		if (ghost.x3 > xmin):
			ghost.x3 -= .4;
		else:
			ghost.state = 1
	if (ghost.state == 1):
		ghost.yaw = -90
		if (ghost.y3 > ymin):
			ghost.y3 -= .4;
		else:
			ghost.state = 2
	if (ghost.state == 2):
		ghost.yaw = 0
		if (ghost.x3 < xmax):
			ghost.x3 += .4
		else:
			ghost.state = 3
	if (ghost.state == 3):
		ghost.yaw = 90
		if (ghost.y3 < ymax):
			ghost.y3 += .4;
		else:
			ghost.state = 4
	if (ghost.state == 4):
		ghost.yaw = 180
		if (ghost.x3 > xmin):
			ghost.x3 -= .4;
		else:
			ghost.state = 1
	print "GhostState:", ghost.state, "x3:",x3,"y3:",y3

def moveGhost1(ghost):
	moveGhost(ghost, -10, 50, 0, 10)
	
def moveGhost2(ghost):
	moveGhost(ghost, -2, 2, -2, 2)
	
def moveGhost3(ghost):
	moveGhost(ghost, -5, 10, -30, 0)
	
def moveGhost4(ghost):
	moveGhost(ghost, -1, 1, -1, 1)

def moveGhosts(objs):
	# move ghost by .2 meters (2 decimeters)
	ghosts = objs['ghost']
	moveGhost1(ghosts[0])
	moveGhost2(ghosts[1])
	moveGhost3(ghosts[2])
	moveGhost4(ghosts[3])

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
			if i.typeShow != 0:
				return pellet
	# Return nothing if no collision.
	return None

def checkGameEnd(objs):
	pellets = objs['pellet']
	gameEnd = True
	for pellet in pellets:
		if pellet.typeShow != 0:
			gameEnd = False
	return gameEnd

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
gameEnd = False
while (!gameEnd):
	moveGhosts(myObjs);
	sendUpdateObjsGpu(len(myObjs['ghost']), myObjs['ghost'])
	pellet = collideWithPellet(myObjs)
	if pellet and pellet.typeShow == 1:
		pellet.typeShow = 0
	time.sleep(.1)
	sendUpdateObjsGpu(len(myObjs['pellet']), myObjs['pellet'])
	time.sleep(.1)
	gameEnd = checkGameEnd(myObjs)
		
time.sleep(3.0)
gpuQuit()

