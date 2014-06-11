/*
 * File: genLevel.cpp
 * Original Author: Steven Ellis
 * Purpose: file for functions that read a configuration file.
 *
 */
#include <stdio.h>
#include "genLevel.h"
#include <cstring>

#define WIDTH 10
#define HEIGHT 25
#define SCALE 2
#define ORIGINX  0
#define ORIGINY -10

void writeConfigFile(char *filename, char *path)
{
	FILE *ifp = fopen(filename, "r");
	FILE *ofp = fopen("tmp.cfg", "w");
	std::map<char,objConfigSettings_t> letterMapping;
	simConfigSettings_t config = {0};
	readGlobalSettings(ifp, config);
	readLetterMapping(ifp, letterMapping);
	writeMap(ifp, ofp, path, config, letterMapping);
}

int readGlobalSettings(FILE *ifp, simConfigSettings_t &config)
{
	char buf[256];
	fscanf(ifp,"%255s", buf);
	if (strcmp(buf,"[GlobalSettings]") != 0)
	{
		return -1;
	}
	// check error codes!
	fscanf(ifp, "%255s%*[ ]%d", buf, &config.width);
	fscanf(ifp, "%255s%*[ ]%d", buf, &config.height);
	fscanf(ifp, "%255s%*[ ]%d", buf, &config.scale);
	fscanf(ifp, "%255s%*[ ]%d", buf, &config.originx);
	fscanf(ifp, "%255s%*[ ]%d", buf, &config.originy);
	printf("Config Settings\n"
				 "Width\t%d\n"
				 "Height\t%d\n"
				 "Scale\t%d\n"
				 "OriginX\t%d\n"
				 "OriginY\t%d\n",
				 config.width,
				 config.height,
				 config.scale,
				 config.originx,
				 config.originy
				 );
}

int readLetterMapping(FILE *ifp, std::map<char,objConfigSettings_t> &letterMapping)
{
	char buf[256];
	char c = 0;
	float scale = 0.0f;
	int is3d = 0;
	int ret = 0;
	//std::map<char,objConfigSettings_t>::iterator it;
	objConfigSettings_t obj;
	fscanf(ifp,"%255s", buf);
	if (strcmp(buf,"[LetterMapping]") != 0)
	{
		printf("Error: got %s, expected [LetterMapping]!\n", buf);
		return -1;
	}
	// check error codes!
	// Read in all matching lines. The non-matching
	// line will be lost. In this case it will be the
	// line "[AsciiMap]", but this isn't a big deal.
	while (fscanf(ifp, " %c%*[ ]%255s%*[ ]%*[a-zA-Z0-9]=%f%*[ ]%*[a-zA-Z0-9]=%d", &c, buf, &scale, &is3d) == 4)
	{
		// create the mapping of characters to strings!
		obj.name = std::string(buf);
		obj.scale = scale;
		obj.is3d = is3d;
		letterMapping[c] = obj;
		printf("C = %c, name=%s, scale=%2.2f,is3d=%d\n", c, obj.name.c_str(), obj.scale, obj.is3d);
	}
}

int writeMap(FILE *ifp, FILE *ofp, char *path, simConfigSettings_t &config, std::map<char,objConfigSettings_t> &letterMapping)
{
	int y = 0, x = -1;
	int idx = 0;
	char obj;
	char buf[256];

	// Validate Map Area
	fscanf(ifp,"%255s", buf);
	// Note the leading bracket was lost from the last scan operation!
	if (strcmp(buf,"AsciiMap]") != 0)
	{
		printf("Error: got %s, expected AsciiMap]!\n", buf);
		return -1;
	}

	// Read in the entire map!
	for(obj = fgetc(ifp); !feof(ifp); obj = fgetc(ifp))
	{
		switch(obj)
		{
			case '\n':
				x++;
				y=0;
				break;
			case ' ':
				break;
			case '\t':
				break;
			default:
				fprintf(ofp,
						/*<id> <is3d> <x3> <y3> <z3> <roll> <pitch> <yaw> <objName> <visible> <scale> */
						"%d\n %d %d %d 0 0 0 0 %s%s 1 %f\n",
						idx, letterMapping[obj].is3d, x*config.scale - config.originx,
						y*config.scale - config.originy, path,
						letterMapping[obj].name.c_str(),letterMapping[obj].scale);
				fflush(ofp);
				break;
		}
		if(obj!='\n')
		{
			y++;
			idx++;
		}
	}
}
