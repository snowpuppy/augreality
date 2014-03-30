#!/usr/bin/python
#
# Filename: testPacketsPclient.py
# Author: Thor Smith
# Purpose: Create a python client to retrieve content
#          and send commands
#
from struct import *
import socket
import sys
import os

sys.path.append(os.path.realpath('../../src/gui_src/'))
from guiNetInterface import *

GETBROADCAST = 1
GETHEARTBEAT = 2

helpStr = """Enter a number to test a function:
0  GETBROADCASTIDS _getBroadCastIDs(int fd);
1  GETNUMBROADCAST _getNumBroadcast(int fd);
2  GETBROADCASTLOC _getBroadcastLoc(int fd);
3  GETPOSITION     _getPosition(int fd);
4  GETNUMALIVE     _getNumAlive(int fd);
5  GETALIVE        _getAlive(int fd);
6  SENDUPDATEOBJS  _sendUpdateObjs(int fd);
7  SENDFILE   		 _sendFile(int fd);
8  SENDEND         _sendEnd(int fd);
10 SENDSTART       _sendStart(int fd);
11 SENDACCEPT      _sendAccept(int fd);
12 SENDGOBACK      _sendGoBack(int fd);
13 exit
"""
while True:
	inpu = raw_input(helpStr)
	args = inpu.split()
	num = int(args[0])
	lat, lon = 0,0
	if (len(args) > 1):
		nid = args[1]
	else:
		#nid = "bad1bad1bad1bad1"
		nid = '40A66DAE\r13A200\r'
	if (num == 0):
		output = getBroadCastIDs()
		print output
	elif (num == 1):
		output = getNumBroadCast()
		print output
	elif (num == 2):
		output = getBroadcastLoc(nid)
		lat = output[0]
		lon = output[1]
		print output
	elif (num == 3):
		output = getPosition(nid)
		print output
	elif (num == 4):
		output = getNumAlive()
		print output
	elif (num == 5):
		output = getAlive(nid)
		print output
	elif (num == 6):
		sendUpdateObjs(1,1,2,0,0,1.5,2.5,90.0,10.0,10.0)
	elif (num == 7):
		sendFile(nid)
	elif (num == 8):
		sendEnd(nid)
	elif (num == 10):
		sendStart()
	elif (num == 11):
		sendAccept(nid,lat,lon)
	elif (num == 12):
		sendGoBack(nid)
	elif (num == 13):
		exit()
