#!/usr/bin/python
#
# Filename: pclient.py
# Author: Thor Smith
# Purpose: Create a python client to retrieve content
#          and send commands
#
from struct import *
import socket

from guiNetInterface import *

GETBROADCAST = 1
GETHEARTBEAT = 2

helpStr = """Enter a number to test a function:
0  GETBROADCASTIDS _sendGoBack(int fd);
1  GETNUMBROADCAST _sendAccept(int fd);
2  GETBROADCASTLOC _sendStart(int fd);
3  GETPOSITION     _sendEnd(int fd);
4  GETNUMALIVE     _sendFile(int fd);
5  GETALIVE        _sendUpdateObjs(int fd);
6  SENDUPDATEOBJS  _getAlive(int fd);
7  SENDFILE   		 _getNumAlive(int fd);
8  SENDEND         _getPosition(int fd);
10 SENDSTART       _getBroadcastLoc(int fd);
11 SENDACCEPT      _getNumBroadcast(int fd);
12 SENDGOBACK      _getBroadCastIDs(int fd);
"""
while True:
	input = raw_input(helpStr)
	num = int(input)
	if (num == 0):
		getBroadCastIDs()
	elif (num == 1):
		getNumBroadCast()
