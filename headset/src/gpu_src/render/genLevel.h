/*
 * File: genLevel.h
 * Author: Thor Smith
 * Purpose: header file for functions that read a configuration file.
 *
 */

/*
This program generates a configuration file for the application to read 
from a simplified format
consisting of * for walls, o for pellets and g for ghosts.
Each line is printed in this format:
<id> <is3d> <x3> <y3> <z3> <roll> <pitch> <yaw> <objName> <visible> <scale>
 0		1			2		3			4			5			6				7			8						9				10
*/

#ifndef GEN_LEVEL_H
#define GEN_LEVEL_H
#include <map>
#include <string>

typedef struct simConfigSettings
{
	int width;
	int height;
	int scale;
	int originx;
	int originy;
} simConfigSettings_t;

typedef struct objConfigSettings
{
	std::string name;
	float scale;
	int is3d;
} objConfigSettings_t;

void writeConfigFile(char *filename, char *path);
int readGlobalSettings(FILE *ifp, simConfigSettings_t &config);
int readLetterMapping(FILE *ifp, std::map<char,objConfigSettings_t> &letterMapping);
int writeMap(FILE *ifp, FILE *ofp, char *path, simConfigSettings_t &config, std::map<char,objConfigSettings_t> &letterMapping);
#endif
