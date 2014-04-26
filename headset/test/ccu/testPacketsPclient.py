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
0  GETUSERPOSITION _getUserPosition(int fd);
1  GETBROADCASTIDS _getBroadCastIDs(int fd);
2  GETNUMBROADCAST _getNumBroadCast(int fd);
3  GETBROADCASTLOC _getBroadCastLoc(int fd);
4  GETPOSITION     _getPosition(int fd);
5  GETNUMALIVE     _getNumAlive(int fd);
6  GETALIVE        _getAlive(int fd);
7  SENDUPDATEOBJS  _sendUpdateObjs(int fd);
8  SENDFILE        _sendFile(int fd);
9  SENDEND         _sendEnd(int fd);
10 SENDSTART       _sendStart(int fd);
11 SENDACCEPT      _sendAccept(int fd);
12 RESETORIGIN     _resetGPSOrigin(int fd);
14 SETGPSORIGIN    _getGPSOrigin(int fd);
15 GETGPSORIGIN    _getGPSOrigin(int fd);
16 GETACCEPTEDIDS  _getAcceptIds(int fd);
17 GETRECEIVEDFILE _getReceivedFile(int fd);
18 GETEND          _getEnd(int fd);
19 GETSTART        _getStart(int fd);
20 GETDROP         _getDrop(int fd);
21 SENDDROP        _sendDrop(int fd);
22 GETACCEPT       _getAccept(int fd);
23 SETHOSTHEADSET  _getAccept(int fd);
24 exit
"""
while True:
	inpu = raw_input(helpStr)
	args = inpu.split()
	num = int(args[0])
	lat, lon = 0,0
	if (len(args) > 1):
		nid = args[1]

	if (num == 0):
		output = getUserPosition()
		print output
	elif (num == 1):
		output = getBroadCastIDs()
		nid = output[1]
		print output
		print nid
	elif (num == 2):
		output = getNumBroadCast()
		print output
	elif (num == 3):
		output = getBroadcastLoc()
		lat = output[0]
		lon = output[1]
		print output
	elif (num == 4):
		output = getPosition(nid)
		print output
	elif (num == 5):
		output = getNumAlive()
		print output
	elif (num == 6):
		output = getAlive(nid)
		print output
	elif (num == 7):
		sendUpdateObjs(1,1,2,0,0,1.5,2.5,90.0,10.0,10.0)
	elif (num == 8):
		sendFile(nid)
	elif (num == 9):
		sendEnd(nid)
	elif (num == 10):
		sendStart()
	elif (num == 11):
		sendAccept(nid)
	elif (num == 12):
		resetGPSOrigin()
	elif (num == 14):
		setGPSOrigin(40,100)
	elif (num == 15):
		output = getGPSOrigin()
		print output
	elif (num == 16):
		output = getAcceptIds()
		print output
	elif (num == 17):
		output = getReceivedFile()
		print output
	elif (num == 18):
		output = getEnd()
		print output
	elif (num == 19):
		output = getStart()
		print output
	elif (num == 20):
		output = getDrop()
		print output
	elif (num == 21):
		sendDrop(nid)
	elif (num == 22):
		output = getAccept()
		print output
	elif (num == 23):
		output = setHostHeadset(1)
		print output
	elif (num == 24):
		exit()
