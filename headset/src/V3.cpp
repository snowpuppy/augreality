#include "V3.h"
#include <math.h>

//constructors
V3::V3() {
	v[0] = v[1] = v[2] = 0.0;
}

V3::V3(float x, float y, float z) {
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

//destructor
V3::~V3() {}

//accessors
float V3::getX() {
    return v[0];
}

float V3::getY() {
    return v[1];
}

float V3::getZ() {
    return v[2];
}

//useful functions
float V3::magnitude() {
	return sqrt( (v[0]*v[0]) + (v[1]*v[1]) + (v[2]*v[2]) );
}

V3 V3::cross(const V3 &v2) {
    return V3(v[1]*v2[2] - v[2]*v2[1], v[2]*v2[0] - v[0]*v2[2], v[0]*v2[1]-v[1]*v2[0]);
}

//operators

//indexing
float & V3::operator[](const int index) {
    return v[index];
}

const float & V3::operator[](const int index) const {
    return v[index];
}

//add vector to vector
V3 V3::operator+(const V3 &rhs) {
    return V3(v[0] + rhs[0], v[1] + rhs[1], v[2] + rhs[2]);
}

//add float to vector
V3 V3::operator+(const float rhs) {
    return V3(v[0]+rhs, v[1]+rhs, v[2]+rhs);
}

//subtract vector from vector
V3 V3::operator-(const V3 &rhs) {
    return V3(v[0] - rhs[0], v[1] - rhs[1], v[2] - rhs[2]);
}

//subtract float from vector
V3 V3::operator-(const float rhs) {
    return V3(v[0]-rhs, v[1]-rhs, v[2]-rhs);
}

//dot product
float V3::operator*(const V3 &rhs) {
    return v[0]*rhs[0] + v[1]*rhs[1] + v[2]*rhs[2];
}

//scale vector by float
V3 V3::operator*(const float rhs) {
    return V3(v[0]*rhs, v[1]*rhs, v[2]*rhs);
}

//divide vector by float
V3 V3::operator/(const float rhs) {
    return V3(v[0]/rhs, v[1]/rhs, v[2]/rhs);
}

//assignment
V3 & V3::operator=(const V3 &rhs) {
    //check for self-assignment
    if (this == &rhs)
        return *this;
    v[0] = rhs[0];
    v[1] = rhs[1];
    v[2] = rhs[2];
    return *this;
}
