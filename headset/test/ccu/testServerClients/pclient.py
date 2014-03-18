#!/usr/bin/python
#
# Filename: pclient.py
# Author: Thor Smith
# Purpose: Create a python client to retrieve content
#          and send commands
#
from struct import *
import socket

GETBROADCAST = 1
GETHEARTBEAT = 2

HOST = ''   # Symbolic name meaning the local host
PORT = 7777 # Arbitrary non-privileged port


while True:
    raw_input('Press enter to request heartbeat data: ')
    command = '\x02'
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST,PORT))
    s.send(command)
    reply = s.recv(calcsize('Hfffff'));
    data = unpack('Hfffff',reply);
    print data
    s.close()
