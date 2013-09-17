#!/usr/bin/python

"""
Returns corners of buildings within specified region in lat/lng coordinates:
  (lat1, lng1), (lat2, lng2), ..., (latN, lngN)   # bldg 1
  (lat1, lng1), ...                               # bldg 2

To acquire lat/lng:
  [1] maps.google.com
  [2] right-click -> "What's Here?" -> lat/lng appear in search bar
"""

import sys
import urllib2
import Image

# CONSTANTS
USAGE = "Usage: %s <lat1> <lng1> <lat2> <lng2>" % sys.argv[0]
ZOOM = 21
TILE_URL = "http://mt0.google.com/vt/lyrs=m@129&hl=en"
# MATCH_RGB = [0xF4, 0xF3, 0xEC] # m@0
# MATCH_RGB = [0xF2, 0xEE, 0xE6] # m@129
MATCH_RGB = [0xF0, 0xEC, 0xE2] # Purdue campus
NON_BLDG_RGB = [0x00, 0x00, 0x00]
BLDG_RGB = [0xFF, 0xFF, 0xFF]
NON_BLDG_IDX = 254
BLDG_IDX = 255

def saveTile(x, y, z, fmt=".png"):
    x_tag = "&x=" + str(x)
    y_tag = "&y=" + str(y)
    z_tag = "&z=" + str(z)  
    url = TILE_URL + x_tag + y_tag + z_tag
    fname = str(x) + "_" + str(y) + "_" + str(z) + fmt
    f = open(fname, 'wb')
    f.write(urllib2.urlopen(url).read())
    f.close()
        
def segmentTile(x, y, z, rgb, fmt=".png", x_pix=256, y_pix=256):
    fname = str(x) + "_" + str(y) + "_" + str(z) + fmt
    img = Image.open(fname)
    pix = img.load()
    pal = groupsOf(3, img.getpalette())
    pal[NON_BLDG_IDX] = NON_BLDG_RGB
    pal[BLDG_IDX] = BLDG_RGB
    img.putpalette([item for sublist in pal for item in sublist])
    for x in range(x_pix):
        for y in range(y_pix):
            if ((pal[pix[x,y]][0] != MATCH_RGB[0]) or 
                (pal[pix[x,y]][1] != MATCH_RGB[1]) or
                (pal[pix[x,y]][2] != MATCH_RGB[2])):
                pix[x,y] = NON_BLDG_IDX
            else:
                pix[x,y] = BLDG_IDX
    img.save(fname)

def groupsOf(size, iter):
    # flatten any previous list:
    if len(iter) % size != 0:
        raise ValueError("Expected iterable to be multiple of " + size)
    out = []
    for group_num in range(len(iter) / size):
        out.append([])
        for offset in range(size):
            out[group_num].append(iter[(size * group_num) + offset])
    return out
    
# main
rgb = BLDG_RGB
z = ZOOM
x_rng = range(542221, 542306+1) # Purdue z=21
y_rng = range(790634, 790708+1)
# x_rng = range(135564-20, 135564+20+1) # Purdue z=19
# y_rng = range(197670-20, 197670+20+1)
# x_rng = range(84704-20, 84704+20+1) # Bikini z=19
# y_rng = range(203480-20, 203480+20+1)
cat = Image.new('P', (len(x_rng)*256,len(y_rng)*256))
pal = groupsOf(3, cat.getpalette())
pal[NON_BLDG_IDX] = NON_BLDG_RGB
pal[BLDG_IDX] = BLDG_RGB
cat.putpalette([item for sublist in pal for item in sublist])
for x_idx, x in enumerate(x_rng):
    for y_idx, y in enumerate(y_rng):
        saveTile(x, y, z)
        segmentTile(x, y, z, rgb)
        #imageCleanup(x, y, z)
        fname = str(x) + "_" + str(y) + "_" + str(z) + ".png"
        cat.paste(Image.open(fname), (256*x_idx,256*y_idx))
cat.save(x_rng[0] + "_" + x_rng[-1] + "_" + y_rng[0] + "_" + y_rng[-1] + ".png")