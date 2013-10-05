#!/usr/bin/python

# Author: Thor Smith
# Date: 10/4/2013
#
#
# Purpose: The purpose of this program is to get
# an idea of how accurate GPS decimal places are
# and to run some sample calculations based on
# relevant data.
#
# The points for this test are mapped roughly
# as follows.
#
# The GPS coordinates and distances were sampled from google
# maps. Note that the distances are approximate and could vary
# by 5-7 meters. They also represent combined x and y distances
# between points (not exactly straight lines).
#
#              362.116 m
#           D-----------C
#           |           |
# 383.928 m |           | 395.472
#           |           |
#           |           |
#           A-----------B
#             350.934 m
#
#

LATMETERSPERDEG = 111320
LONMETERSPERDEG = 78710

class GPS:
  def __init__(self,lattitude, longitude):
    self.lat = float(lattitude)
    self.lon = float(longitude)
  def __sub__(self, other):
    newGPS = GPS(self.lat - other.lat, self.lon - other.lon)
    return newGPS


A = GPS(40.427569, -86.930385)
B = GPS(40.427602, -86.926008)
C = GPS(40.43113,  -86.925965)
D = GPS(40.43113,  -86.930417)


Origin = A

AminB = A - B

print "Latitude: %f, Longitude %f" % (AminB.lat, AminB.lon)
print "dX: %f, dY %f" % (AminB.lat*LATMETERSPERDEG, AminB.lon*LONMETERSPERDEG)
